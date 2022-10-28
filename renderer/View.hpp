// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#pragma once

class View final : public IView
{
public:
	View( const ViewDesc& desc,
		nvrhi::TextureHandle colour,
		nvrhi::TextureHandle depth,
		nvrhi::FramebufferHandle framebuffer,
		nvrhi::BindingSetHandle bindingSet );

	nvrhi::IFramebuffer* GetFramebuffer() const override;
	nvrhi::ITexture* GetColourTexture() const override;
	nvrhi::ITexture* GetDepthTexture() const override;
	nvrhi::IBindingSet* GetBindingSet() const override;

	ViewDesc& GetDesc() override;
	const ViewDesc& GetDesc() const override;
private:
	ViewDesc desc{};

	nvrhi::TextureHandle colourTexture{};
	nvrhi::TextureHandle depthTexture{};
	nvrhi::FramebufferHandle framebuffer{};
	nvrhi::BindingSetHandle bindingSet{};
};
