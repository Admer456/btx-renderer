// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#include "Precompiled.hpp"
#include "RenderFrontend.hpp"

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

std::pair<nvrhi::TextureHandle, nvrhi::TextureHandle> RenderFrontend::CreateFramebufferImagesForView( const ViewDesc& desc )
{
	// This seems to be the optimal set of flags for attachments
	// Works in both Vulkan and DX12
	using RStates = nvrhi::ResourceStates;
	const RStates ColourBufferStates = RStates::RenderTarget;
	const RStates DepthBufferStates = RStates::DepthWrite;

	auto colourAttachmentDesc = nvrhi::TextureDesc()
		.setWidth( window->GetSize().x )
		.setHeight( window->GetSize().y )
		.setFormat( WindowFormatToNvrhi( window->GetVideoMode().format ) )
		.setDimension( nvrhi::TextureDimension::Texture2D )
		.setKeepInitialState( true )
		.setInitialState( ColourBufferStates )
		.setIsRenderTarget( true )
		.setDebugName( "Colour attachment image" );

	nvrhi::TextureHandle colourTexture = backend->createTexture( colourAttachmentDesc );
	if ( nullptr == colourTexture )
	{
		Console->Error( "Failed to create colour component for renderview" );
		return { nullptr, nullptr };
	}

	auto depthAttachmentDesc = colourAttachmentDesc
		.setFormat( nvrhi::Format::D32 ) // we don't use a stencil buffer here
		.setDimension( nvrhi::TextureDimension::Texture2D )
		.setInitialState( DepthBufferStates )
		.setDebugName( "Depth attachment image" );

	nvrhi::TextureHandle depthTexture = backend->createTexture( depthAttachmentDesc );
	if ( nullptr == depthTexture )
	{
		Console->Error( "Failed to create depth component for renderview" );
		return { nullptr, nullptr };
	}

	return { colourTexture, depthTexture };
}

nvrhi::FramebufferHandle RenderFrontend::CreateFramebufferFromImages( nvrhi::ITexture* colourTexture, nvrhi::ITexture* depthTexture )
{
	auto framebufferDesc = nvrhi::FramebufferDesc()
		.addColorAttachment( colourTexture )
		.setDepthAttachment( depthTexture );

	return backend->createFramebuffer( framebufferDesc );
}

nvrhi::BindingSetHandle RenderFrontend::CreateBindingSetForView( nvrhi::TextureHandle colourTexture, nvrhi::TextureHandle depthTexture )
{
	// Todo: the sampler for the framebuffer could potentially be nearest sometimes,
	// might wanna have it controllable per-view
	auto bindingSetDesc = nvrhi::BindingSetDesc()
		.addItem( nvrhi::BindingSetItem::Texture_SRV( 0, colourTexture ) )
		.addItem( nvrhi::BindingSetItem::Texture_SRV( 1, depthTexture ) )
		.addItem( nvrhi::BindingSetItem::Sampler( 0, screenSampler ) );

	return backend->createBindingSet( bindingSetDesc, screenBindingLayout );
}
