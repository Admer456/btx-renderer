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

	Console->DPrint( format( "RenderFrontend: Loaded shader '%s'!", fullShaderPathStr.c_str() ), 1);
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

	if ( !CreateShaderPair( "default", entityVertexShader, entityPixelShader ) )
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

	{
		nvrhi::VertexAttributeDesc screenVertexLayoutDesc[] =
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
		screenVertexLayout = backend->createInputLayout( screenVertexLayoutDesc, 2U, screenVertexShader );

		auto screenBindingLayoutDesc = nvrhi::BindingLayoutDesc()
			.setVisibility( nvrhi::ShaderType::Vertex | nvrhi::ShaderType::Pixel )
			.addItem( nvrhi::BindingLayoutItem::Texture_SRV( 0 ) )
			.addItem( nvrhi::BindingLayoutItem::Texture_SRV( 1 ) )
			.addItem( nvrhi::BindingLayoutItem::Sampler( 0 ) );

		screenBindingLayout = backend->createBindingLayout( screenBindingLayoutDesc );
		if ( nullptr == screenBindingLayout )
		{
			Console->Error( "RenderFrontend: Failed to create screen binding layout" );
			return false;
		}

		auto screenRasterState = nvrhi::RasterState()
			.setCullNone()
			.setFillSolid();

		// The "screen pipeline" doesn't have depth testing, it is merely
		// drawing images onto the screen/backbuffer which has no depth attachment
		auto screenDepthStencilState = nvrhi::DepthStencilState()
			.disableDepthTest()
			.disableDepthWrite()
			.disableStencil()
			.setDepthFunc( nvrhi::ComparisonFunc::Less );

		auto screenRenderState = nvrhi::RenderState()
			.setRasterState( screenRasterState )
			.setDepthStencilState( screenDepthStencilState );

		auto screenPipelineDesc = nvrhi::GraphicsPipelineDesc()
			.setVertexShader( screenVertexShader )
			.setPixelShader( screenPixelShader )
			.setInputLayout( screenVertexLayout )
			.setRenderState( screenRenderState )
			.addBindingLayout( screenBindingLayout );

		// If you get errors in DX12 here, you are likely missing dxil.dll. You should have dxc.exe, dxcompiler.dll AND dxil.dll,
		// as the 3rd one will perform shader validation/signature,
		// and DX12 doesn't like unsigned shaders by default (you'd need to modify NVRHI to allow that)
		screenPipeline = backend->createGraphicsPipeline( screenPipelineDesc, backendManager->GetCurrentFramebuffer() );

		if ( nullptr == screenPipeline )
		{
			Console->Error( "RenderFrontend: Failed to create screen pipeline" );
			return false;
		}
	}

	// Workaround: at loading time, we do not have a framebuffer to use to create the entity pipeline
	// (requires a depth attachment!)
	// So we create a temporary renderview that will have the exact framebuffer needed
	ViewDesc temporaryViewDesc;
	temporaryViewDesc.viewportSize = Vec2( -1.0f );
	IView* temporaryRenderView = CreateView( temporaryViewDesc );
	{
		// Todo: Figure out how to do interleaved vertex buffers
		nvrhi::VertexAttributeDesc entityVertexLayoutDesc[] =
		{
			nvrhi::VertexAttributeDesc()
			.setName( "POSITION" )
			.setBufferIndex( 0U )
			.setFormat( nvrhi::Format::RGB32_FLOAT )
			.setElementStride( sizeof( Vec3 ) ),

			nvrhi::VertexAttributeDesc()
			.setName( "NORMAL" )
			.setBufferIndex( 1U )
			.setFormat( nvrhi::Format::RGBA8_SNORM )
			.setElementStride( sizeof( byte ) * 4 ),

			nvrhi::VertexAttributeDesc()
			.setName( "TEXCOORD" )
			.setBufferIndex( 2U )
			.setFormat( nvrhi::Format::RG32_FLOAT )
			.setElementStride( sizeof( Vec2 ) ),

			nvrhi::VertexAttributeDesc()
			.setName( "COLOUR" )
			.setBufferIndex( 3U )
			.setFormat( nvrhi::Format::RGBA8_UNORM )
			.setElementStride( sizeof( byte ) * 4 )
		};
		entityVertexLayout = backend->createInputLayout( entityVertexLayoutDesc, 4U, entityVertexShader );

		auto viewFrameBindingLayoutDesc = nvrhi::BindingLayoutDesc()
			.setVisibility( nvrhi::ShaderType::Vertex | nvrhi::ShaderType::Pixel )
			.addItem( nvrhi::BindingLayoutItem::VolatileConstantBuffer( 0 ) );
		
		viewFrameBindingLayout = backend->createBindingLayout( viewFrameBindingLayoutDesc );
		if ( nullptr == viewFrameBindingLayout )
		{
			Console->Error( "RenderFrontend: Failed to create view binding layout" );
			return false;
		}

		auto entityBindingLayoutDesc = nvrhi::BindingLayoutDesc()
			.setVisibility( nvrhi::ShaderType::Vertex | nvrhi::ShaderType::Pixel )
			.addItem( nvrhi::BindingLayoutItem::VolatileConstantBuffer( 1 ) );

		entityBindingLayout = backend->createBindingLayout( entityBindingLayoutDesc );
		if ( nullptr == entityBindingLayout )
		{
			Console->Error( "RenderFrontend: Failed to create entity binding layout" );
			return false;
		}

		auto viewFrameSetDesc = nvrhi::BindingSetDesc()
			.addItem( nvrhi::BindingSetItem::ConstantBuffer( 0, viewDataBuffer ) );
		viewFrameBindingSet = backend->createBindingSet( viewFrameSetDesc, viewFrameBindingLayout );
		if ( nullptr == viewFrameBindingSet )
		{
			Console->Error( "RenderFrontend: Failed to create view binding set" );
			return false;
		}

		auto entitySetDesc = nvrhi::BindingSetDesc()
			.addItem( nvrhi::BindingSetItem::ConstantBuffer( 1, entityDataBuffer ) );
		entityBindingSet = backend->createBindingSet( entitySetDesc, entityBindingLayout );
		if ( nullptr == entityBindingSet )
		{
			Console->Error( "RenderFrontend: Failed to create entity binding set" );
			return false;
		}


		auto entityRasterState = nvrhi::RasterState()
			.setCullNone() // Change to front after the experiment
			.setFillSolid();

		auto entityDepthStencilState = nvrhi::DepthStencilState()
			.enableDepthTest()
			.enableDepthWrite()
			.disableStencil()
			.setDepthFunc( nvrhi::ComparisonFunc::Less );

		auto entityRenderState = nvrhi::RenderState()
			.setRasterState( entityRasterState )
			.setDepthStencilState( entityDepthStencilState );

		auto entityPipelineDesc = nvrhi::GraphicsPipelineDesc()
			.setVertexShader( entityVertexShader )
			.setPixelShader( entityPixelShader )
			.setInputLayout( entityVertexLayout )
			.setRenderState( entityRenderState )
			.addBindingLayout( viewFrameBindingLayout )
			.addBindingLayout( entityBindingLayout );

		// If you get errors in DX12 here, you are likely missing dxil.dll. You should have dxc.exe, dxcompiler.dll AND dxil.dll,
		// as the 3rd one will perform shader validation/signature,
		// and DX12 doesn't like unsigned shaders by default (you'd need to modify NVRHI to allow that)
		entityPipeline = backend->createGraphicsPipeline( entityPipelineDesc, temporaryRenderView->GetFramebuffer() );

		if ( nullptr == entityPipeline )
		{
			Console->Error( "RenderFrontend: Failed to create entity pipeline" );
			return false;
		}
	}
	DestroyView( temporaryRenderView );

	Console->DPrint( "RenderFrontend: Successfully created core graphics pipelines!", 1 );
	return true;
}
