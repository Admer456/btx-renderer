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
	case WindowVideoFormat::BGRA8: return nvrhi::Format::RGBA8_UNORM;
	}

	return nvrhi::Format::UNKNOWN;
}

// 1) create a sampler that will determine how textures are filtered (nearest, bilinear etc.)
// 2) create a colour and depth texture for our framebuffer, so we can render our scene with depth testing
// 2.1) the two textures will also be used as inputs for the ScreenQuad shader, so we can do post-processing
// 3) create the framebuffers
bool RenderFrontend::CreateMainFramebuffer()
{
	// Sampler
	auto& textureSampler = nvrhi::SamplerDesc()
		.setAllFilters( true )
		.setAllAddressModes( nvrhi::SamplerAddressMode::Wrap );

	screenSampler = backend->createSampler( textureSampler );

	using RStates = nvrhi::ResourceStates;
	const RStates ColourBufferStates = RStates::RenderTarget;
	const RStates DepthBufferStates = RStates::DepthWrite;

	// Colour and depth attachment for the framebuffer
	auto colourAttachmentDesc = nvrhi::TextureDesc()
		.setWidth( window->GetSize().x )
		.setHeight( window->GetSize().y )
		.setFormat( WindowFormatToNvrhi( window->GetVideoMode().format ) )
		.setDimension( nvrhi::TextureDimension::Texture2D )
		.setKeepInitialState( true )
		.setInitialState( ColourBufferStates )
		.setIsRenderTarget( true )
		.setDebugName( "Colour attachment image" );

	mainFramebufferColour = backend->createTexture( colourAttachmentDesc );
	if ( nullptr == mainFramebufferColour )
	{
		Console->Error( "Failed to create main framebuffer's colour component" );
		return false;
	}
	
	auto depthAttachmentDesc = colourAttachmentDesc
		.setFormat( (backend->getGraphicsAPI() == nvrhi::GraphicsAPI::D3D11) ? nvrhi::Format::D24S8 : nvrhi::Format::D32)
		.setDimension( nvrhi::TextureDimension::Texture2D )
		.setInitialState( DepthBufferStates )
		.setDebugName( "Depth attachment image" );

	mainFramebufferDepth = backend->createTexture( depthAttachmentDesc );
	if ( nullptr == mainFramebufferDepth )
	{
		Console->Error( "Failed to create main framebuffer's depth component" );
		return false;
	}

	// ==========================================================================================================
	// FRAMEBUFFER CREATION
	// ==========================================================================================================
	auto mainFramebufferDesc = nvrhi::FramebufferDesc()
		.addColorAttachment( mainFramebufferColour )
		.setDepthAttachment( mainFramebufferDepth );

	mainFramebuffer = backend->createFramebuffer( mainFramebufferDesc );
	if ( nullptr == mainFramebuffer )
	{
		Console->Error( "Failed to create main framebuffer" );
		return false;
	}

	const auto printFramebufferInfo = []( const nvrhi::FramebufferInfo& fbInfo, const char* name )
	{
		Console->DPrint( format( "Framebuffer '%s' info: ", name ), 1 );
		Console->DPrint( format( "  * Size:          %ux%u ", fbInfo.width, fbInfo.height ), 1);
		Console->DPrint( format( "  * Colour format: %s ", nvrhi::utils::FormatToString( fbInfo.colorFormats[0] ) ), 1 );
		Console->DPrint( format( "  * Depth format:  %s ", nvrhi::utils::FormatToString( fbInfo.depthFormat ) ), 1 );
	};

	printFramebufferInfo( mainFramebuffer->getFramebufferInfo(), "Main framebuffer" );
	
	return true;
}
