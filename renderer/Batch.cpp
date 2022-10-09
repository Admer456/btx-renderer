// SPDX-FileCopyrightText: 2022 Admer �uko
// SPDX-License-Identifier: MIT

#include "Precompiled.hpp"
#include "Batch.hpp"

BatchDesc& Batch::GetDesc()
{
	return desc;
}

const BatchDesc& Batch::GetDesc() const
{
	return desc;
}
