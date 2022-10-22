// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#include "Precompiled.hpp"
#include "RenderFrontend.hpp"
#include <nvrhi/utils.h>

inline static const char* GetEntryNameForType( nvrhi::ShaderType type )
{
	switch ( type )
	{
	default:
	case nvrhi::ShaderType::Vertex: return "main_vs";
	case nvrhi::ShaderType::Pixel: return "main_ps";
	case nvrhi::ShaderType::Compute: return "main_cs";
	case nvrhi::ShaderType::Geometry: return "main_gs";
	}
}

Path RenderFrontend::BuildShaderPath( nvrhi::ShaderType type, StringView shaderPath )
{
	const nvrhi::GraphicsAPI api = backend->getGraphicsAPI();
	
	const Path apiSpecificShaderPath = [&api]()
	{
		switch ( api )
		{
		case nvrhi::GraphicsAPI::D3D11: return "dx11";
		case nvrhi::GraphicsAPI::D3D12: return "dx12";
		default:
		case nvrhi::GraphicsAPI::VULKAN: return "vk";
		}
	}();

	const Path shaderTypePath = GetEntryNameForType( type );

	// shaders/vk/screen_main_ps.bin
	Path path = "shaders" / apiSpecificShaderPath / shaderPath;
	path += "_";
	path += shaderTypePath;
	path += ".bin";

	return path;
}

nvrhi::ShaderHandle RenderFrontend::CreateShader( nvrhi::ShaderType type, StringView shaderPath )
{
	const Path fullShaderPath = BuildShaderPath( type, shaderPath );
	const String fullShaderPathStr = fullShaderPath.string();
	const auto fullPath = FileSystem->GetPathTo( fullShaderPath, IFileSystem::Path_File );

	if ( !fullPath.has_value() )
	{
		Console->Error( format( "RenderFrontend::CreateShader: Shader '%s' does not exist (full expected path: '%s')",
			shaderPath.data(), fullShaderPathStr.c_str() ) );
		return nullptr;
	}

	std::ifstream file = std::ifstream( fullPath.value(), std::ios::binary | std::ios::ate );
	if ( !file )
	{
		Console->Error( format( "RenderFrontend::CreateShader: Shader '%s': cannot open file '%s'",
			shaderPath.data(), fullShaderPathStr.c_str() ) );
		return nullptr;
	}

	// -------------------------------------------------------------------------------------
	// TODO: Move this somewhere into AdmUtils
	// This is mega ugly(tm)
	// -------------------------------------------------------------------------------------
	const size_t fileSize = file.tellg();
	file.seekg( 0U );

	Vector<uint8_t> shaderData;
	shaderData.reserve( fileSize );

	uint8_t* data = new uint8_t[fileSize];
	file.read( reinterpret_cast<char*>(data), fileSize );
	shaderData.insert( shaderData.begin(), data, data + fileSize );
	delete[] data;
	// -------------------------------------------------------------------------------------

	auto desc = nvrhi::ShaderDesc( type );
	desc.entryName = GetEntryNameForType( type );
	desc.shaderType = type;

	nvrhi::ShaderHandle shader = backend->createShader( desc, shaderData.data(), shaderData.size() );
	if ( nullptr == shader )
	{
		Console->Error( format( "RenderFrontend::CreateShader: Shader '%s' appears to be corrupted", shaderPath.data() ) );
		return nullptr;
	}

	return shader;
}

bool RenderFrontend::CreateShaderPair( StringView shaderPath, nvrhi::ShaderHandle& outVertexShader, nvrhi::ShaderHandle& outPixelShader )
{
	bool failed = false;

	if ( nullptr == (outVertexShader = CreateShader( nvrhi::ShaderType::Vertex, shaderPath )) )
	{
		failed = true;
	}

	if ( nullptr == (outPixelShader = CreateShader( nvrhi::ShaderType::Pixel, shaderPath )) )
	{
		failed = true;
	}

	return !failed;
}

nvrhi::ShaderHandle RenderFrontend::CreateComputeShader( StringView shaderPath )
{
	return CreateShader( nvrhi::ShaderType::Compute, shaderPath );
}

bool RenderFrontend::CreateMainShaders()
{
	if ( !CreateShaderPair( "screen", screenVertexShader, screenPixelShader ) )
	{
		return false;
	}

	return true;
}

bool RenderFrontend::CreateMainGraphicsPipelines()
{
	if ( !CreateMainShaders() )
	{
		Console->Error( "         ^^^ these are the core shaders required for the renderer to work, fix that!" );
		return false;
	}

	nvrhi::VertexAttributeDesc vertexLayoutDesc[] =
	{
		nvrhi::VertexAttributeDesc()
		.setName( "POSITION" )
		.setFormat( nvrhi::Format::RG32_FLOAT )
		.setOffset( 0 )
		.setElementStride( sizeof( Vec2 ) * 2 ),

		nvrhi::VertexAttributeDesc()
		.setName( "TEXCOORD" )
		.setFormat( nvrhi::Format::RG32_FLOAT )
		.setOffset( sizeof( Vec2 ) )
		.setElementStride( sizeof( Vec2 ) * 2 )
	};
	screenVertexLayout = backend->createInputLayout( vertexLayoutDesc, 2U, screenVertexShader );

	nvrhi::BindingSetDesc setDesc;
	setDesc.bindings =
	{
		nvrhi::BindingSetItem::Texture_SRV( 0, mainFramebufferColour ),
		nvrhi::BindingSetItem::Texture_SRV( 1, mainFramebufferDepth ),
		nvrhi::BindingSetItem::Sampler( 0, screenSampler )
	};
	nvrhi::utils::CreateBindingSetAndLayout( backend, nvrhi::ShaderType::Vertex | nvrhi::ShaderType::Pixel, 0,
		setDesc, screenBindingLayout, screenBindingSet );

	auto rasterState = nvrhi::RasterState()
		.setCullNone()
		.setFillSolid();

	// The "screen pipeline" doesn't have depth testing, it is merely
	// drawing images onto the screen/backbuffer which has no depth attachment
	auto depthStencilState = nvrhi::DepthStencilState()
		.disableDepthTest()
		.disableDepthWrite()
		.disableStencil()
		.setDepthFunc( nvrhi::ComparisonFunc::Less );

	auto renderState = nvrhi::RenderState()
		.setRasterState( rasterState )
		.setDepthStencilState( depthStencilState );
	
	auto desc = nvrhi::GraphicsPipelineDesc()
		.setVertexShader( screenVertexShader )
		.setPixelShader( screenPixelShader )
		.setInputLayout( screenVertexLayout )
		.setRenderState( renderState )
		.addBindingLayout( screenBindingLayout );

	// If you get errors in DX12 here, you are likely missing dxil.dll. You should have dxc.exe, dxcompiler.dll AND dxil.dll,
	// as the 3rd one will perform shader validation/signature,
	// and DX12 doesn't like unsigned shaders by default (you'd need to modify NVRHI to allow that)
	screenPipeline = backend->createGraphicsPipeline( desc, backendManager->GetCurrentFramebuffer() );

	if ( nullptr == screenPipeline )
	{
		Console->Error( "RenderFrontend: Failed to create screen pipeline" );
		return false;
	}

	Console->DPrint( "RenderFrontend: Successfully created core graphics pipeline!", 1 );
	return true;
}
