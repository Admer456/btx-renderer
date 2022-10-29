// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#include "Precompiled.hpp"
#include "Model.hpp"

Model::Model( const Assets::IModel* asset,
	Vector<nvrhi::BufferHandle>&& indexBuffers,
	VertexBufferMap&& vertexBuffers,
	Vector<size_t> indexCountPerFace,
	Vector<size_t> vertexCountPerFace )
{
	modelAsset = asset;
	this->indexBuffers = std::move( indexBuffers );
	this->vertexBuffers = std::move( vertexBuffers );
	indexCounts = indexCountPerFace;
	vertexCounts = vertexCountPerFace;
}

StringView Model::GetName() const
{
	return GetDesc().modelData.name;
}

size_t Model::GetNumFaces() const
{
	return indexBuffers.size();
}

size_t Model::GetNumIndices( uint32_t face ) const
{
	return indexCounts[face];
}

size_t Model::GetNumVertices( uint32_t face ) const
{
	return vertexCounts[face];
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
