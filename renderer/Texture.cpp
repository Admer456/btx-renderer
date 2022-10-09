// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#include "Precompiled.hpp"
#include "Texture.hpp"

TextureDesc& Texture::GetDesc()
{
	return desc;
}

const TextureDesc& Texture::GetDesc() const
{
	return desc;
}
