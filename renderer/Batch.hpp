// SPDX-FileCopyrightText: 2022 Admer �uko
// SPDX-License-Identifier: MIT

#pragma once

class Batch final : public IBatch
{
public:
	BatchDesc& GetDesc() override;
	const BatchDesc& GetDesc() const override;
private:
	BatchDesc desc;
};
