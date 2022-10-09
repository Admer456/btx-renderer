// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#pragma once

class Light final : public ILight
{
public:
	LightDesc& GetDesc() override;
	const LightDesc& GetDesc() const override;
private:
	LightDesc desc;
};
