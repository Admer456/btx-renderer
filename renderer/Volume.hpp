// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#pragma once

class Volume final : public IVolume
{
public:
	VolumeDesc& GetDesc() override;
	const VolumeDesc& GetDesc() const override;
private:
	VolumeDesc desc;
};
