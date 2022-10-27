// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#include "Precompiled.hpp"
#include "View.hpp"

View::View( const ViewDesc& desc, nvrhi::TextureHandle colour, nvrhi::TextureHandle depth, nvrhi::FramebufferHandle framebuffer )
	: desc( desc ), colourTexture( colour ), depthTexture( depth ), framebuffer( framebuffer )
{
}

nvrhi::IFramebuffer* View::GetFramebuffer() const
{
	return framebuffer;
}

nvrhi::ITexture* View::GetColourTexture() const
{
	return colourTexture;
}

nvrhi::ITexture* View::GetDepthTexture() const
{
	return depthTexture;
}

ViewDesc& View::GetDesc()
{
	return desc;
}

const ViewDesc& View::GetDesc() const
{
	return desc;
}
