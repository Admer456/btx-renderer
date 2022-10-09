
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
