// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#include "Precompiled.hpp"
#include "Model.hpp"

Model::Model( Assets::IModel* asset, Vector<nvrhi::BufferHandle>&& indexBuffers, VertexBufferMap&& vertexBuffers )
{
	modelAsset = asset;
	this->indexBuffers = std::move( indexBuffers );
	this->vertexBuffers = std::move( vertexBuffers );
}

StringView Model::GetName() const
{
	return GetDesc().modelData.name;
}

size_t Model::GetNumFaces() const
{
	return indexBuffers.size();
}

IBuffer* Model::GetVertexBuffer( uint32_t face, Assets::RenderData::VertexAttributeType attribute ) const
{
	return vertexBuffers.at( { face, attribute } );
}

IBuffer* Model::GetIndexBuffer( uint32_t face ) const
{
	return indexBuffers.at( face );
}

const Assets::ModelDesc& Model::GetDesc() const
{
	return modelAsset->GetDesc();
}
