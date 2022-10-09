
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
