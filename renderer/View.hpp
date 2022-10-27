// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#pragma once

class View final : public IView
{
public:
	View( const ViewDesc& desc, nvrhi::TextureHandle colour, nvrhi::TextureHandle depth, nvrhi::FramebufferHandle framebuffer );

	nvrhi::IFramebuffer* GetFramebuffer() const override;
	nvrhi::ITexture* GetColourTexture() const override;
	nvrhi::ITexture* GetDepthTexture() const override;

	ViewDesc& GetDesc() override;
	const ViewDesc& GetDesc() const override;
private:
	ViewDesc desc{};

	nvrhi::FramebufferHandle framebuffer{};
	nvrhi::TextureHandle colourTexture{};
	nvrhi::TextureHandle depthTexture{};
};
