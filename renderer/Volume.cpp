// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

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
