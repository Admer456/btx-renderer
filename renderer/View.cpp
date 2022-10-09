// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#include "Precompiled.hpp"
#include "View.hpp"

ViewDesc& View::GetDesc()
{
	return desc;
}

const ViewDesc& View::GetDesc() const
{
	return desc;
}
