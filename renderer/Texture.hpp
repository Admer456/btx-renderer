
#pragma once

class Texture final : public ITexture
{
public:
	TextureDesc& GetDesc() override;
	const TextureDesc& GetDesc() const override;
private:
	TextureDesc desc;
};
