// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#include "Precompiled.hpp"
#include "Light.hpp"

LightDesc& Light::GetDesc()
{
	return desc;
}

const LightDesc& Light::GetDesc() const
{
	return desc;
}
