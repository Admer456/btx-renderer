// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#pragma once

class Entity final : public IEntity
{
public:
	EntityDesc& GetDesc() override;
	const EntityDesc& GetDesc() const override;
private:
	EntityDesc desc;
};
