// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#pragma once

class Texture final : public ITexture
{
public:
	TextureDesc& GetDesc() override;
	const TextureDesc& GetDesc() const override;
private:
	TextureDesc desc;
};
