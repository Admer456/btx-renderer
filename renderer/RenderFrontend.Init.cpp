// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#include "Precompiled.hpp"
#include "RenderFrontend.hpp"
#include <nvrhi/utils.h>

bool RenderFrontend::PostInit( RenderBackend* renderBackend, IWindow* mainWindow )
{
	backendManager = renderBackend;
	backend = backendManager->GetDevice();
	window = mainWindow;

	if ( !CreateCommandLists() )
	{
		Console->Error( "RenderFrontend::PostInit: Failed to create commandlists" );
		return false;
	}

	if ( !CreateMainFramebuffer() )
	{
		Console->Error( "RenderFrontend::PostInit: Failed to create main framebuffer" );
		return false;
	}

	if ( !CreateMainGraphicsPipelines() )
	{
		Console->Error( "RenderFrontend::PostInit: Failed to create core graphics pipeline" );
		return false;
	}

	if ( !CreateScreenVertexBuffer() )
	{
		Console->Error( "RenderFrontend::PostInit: Failed to create screen quad" );
		return false;
	}

	return true;
}

bool RenderFrontend::CreateCommandLists()
{
	auto renderParams = nvrhi::CommandListParameters()
		.setQueueType( nvrhi::CommandQueue::Graphics );

	auto transferParams = nvrhi::CommandListParameters()
		.setQueueType( nvrhi::CommandQueue::Graphics );

	renderCommands = backend->createCommandList( renderParams );
	transferCommands = backend->createCommandList( transferParams );

	return (nullptr != renderCommands) && (nullptr != transferCommands);
}

static nvrhi::Format WindowFormatToNvrhi( WindowVideoFormat format )
{
	switch ( format )
	{
	case WindowVideoFormat::SRGBA8: return nvrhi::Format::SRGBA8_UNORM;
	case WindowVideoFormat::SBGRA8: return nvrhi::Format::SBGRA8_UNORM;
	case WindowVideoFormat::RGBA8: return nvrhi::Format::RGBA8_UNORM;
	case WindowVideoFormat::BGRA8: return nvrhi::Format::BGRA8_UNORM;
	}

	return nvrhi::Format::UNKNOWN;
}

// Todo: cleanup since we no longer have a main framebuffer
bool RenderFrontend::CreateMainFramebuffer()
{
	// Sampler
	auto& textureSampler = nvrhi::SamplerDesc()
		.setAllFilters( true )
		.setAllAddressModes( nvrhi::SamplerAddressMode::Wrap );

	screenSampler = backend->createSampler( textureSampler );

	const auto printFramebufferInfo = []( const nvrhi::FramebufferInfo& fbInfo, const char* name )
	{
		Console->DPrint( format( "Framebuffer '%s' info: ", name ), 1 );
		Console->DPrint( format( "  * Size:          %ux%u ", fbInfo.width, fbInfo.height ), 1);
		Console->DPrint( format( "  * Colour format: %s ", nvrhi::utils::FormatToString( fbInfo.colorFormats[0] ) ), 1 );
		Console->DPrint( format( "  * Depth format:  %s ", nvrhi::utils::FormatToString( fbInfo.depthFormat ) ), 1 );
	};

	printFramebufferInfo( backendManager->GetCurrentFramebuffer()->getFramebufferInfo(), "Screen backbuffer" );
	
	const auto& viewDataBufferDesc = nvrhi::utils::CreateVolatileConstantBufferDesc( sizeof( currentViewData ), "currentViewData", 16U );
	viewDataBuffer = backend->createBuffer( viewDataBufferDesc );

	const auto& entityDataBufferDesc = nvrhi::utils::CreateVolatileConstantBufferDesc( sizeof( currentEntityData ), "currentEntityData", 16U );
	entityDataBuffer = backend->createBuffer( entityDataBufferDesc );

	return true;
}

bool RenderFrontend::CreateScreenVertexBuffer()
{
	// Format: XY UV
	const Vector<float> ScreenQuadVertexData =
	{
		-1.0f, -1.0f,
		0.0f, 1.0f,

		1.0f, -1.0f,
		1.0f, 1.0f,

		1.0f, 1.0f,
		1.0f, 0.0f,

		-1.0f, 1.0f,
		0.0f, 0.0f
	};

	const Vector<uint32_t> ScreenQuadIndexData =
	{
		0, 1, 2,
		2, 3, 0
	};

	screenIndexBuffer = CreateIndexBuffer( ScreenQuadIndexData );
	screenVertexBuffer = CreateVertexBuffer( ScreenQuadVertexData );

	return (nullptr != screenIndexBuffer) && (nullptr != screenVertexBuffer);
}
