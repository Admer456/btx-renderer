// SPDX-FileCopyrightText: 2022 Admer �uko
// SPDX-License-Identifier: MIT

#pragma once

class View final : public IView
{
public:
	ViewDesc& GetDesc() override;
	const ViewDesc& GetDesc() const override;
private:
	ViewDesc desc;
};
