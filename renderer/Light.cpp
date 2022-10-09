
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
