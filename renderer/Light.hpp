
#pragma once

class Light final : public ILight
{
public:
	LightDesc& GetDesc() override;
	const LightDesc& GetDesc() const override;
private:
	LightDesc desc;
};
