// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#pragma once

class Entity final : public IEntity
{
public:
	Entity( const EntityDesc& desc );

	EntityDesc& GetDesc() override;
	const EntityDesc& GetDesc() const override;
private:
	EntityDesc desc;
};
