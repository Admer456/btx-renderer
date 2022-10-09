
#include "Precompiled.hpp"
#include "Entity.hpp"

EntityDesc& Entity::GetDesc()
{
	return desc;
}

const EntityDesc& Entity::GetDesc() const
{
	return desc;
}
