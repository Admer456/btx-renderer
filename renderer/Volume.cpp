
#include "Precompiled.hpp"
#include "Volume.hpp"

VolumeDesc& Volume::GetDesc()
{
	return desc;
}

const VolumeDesc& Volume::GetDesc() const
{
	return desc;
}
