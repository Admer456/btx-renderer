
#pragma once

class Volume final : public IVolume
{
public:
	VolumeDesc& GetDesc() override;
	const VolumeDesc& GetDesc() const override;
private:
	VolumeDesc desc;
};
