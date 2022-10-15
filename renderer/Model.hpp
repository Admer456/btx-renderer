// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#pragma once

struct VertexMapKey
{
	uint32_t face{};
	Assets::RenderData::VertexAttributeType vertexAttribute{ Assets::RenderData::VertexAttributeType::Position };

	// Hashing function, required for VertexMap (std::unordered_map)
	size_t operator()() const noexcept
	{
		return face + (1U << (16U + static_cast<size_t>(vertexAttribute)));
	}
	// Required for hashing
	bool operator==( const VertexMapKey& other ) const noexcept
	{
		return face == other.face && vertexAttribute == other.vertexAttribute;
	}
};

template<>
struct std::hash<VertexMapKey>
{
	std::size_t operator()( const VertexMapKey& other ) const noexcept
	{
		return other();
	}
};

using VertexBufferMap = Map<VertexMapKey, nvrhi::BufferHandle>;

class Model final : public IModel
{
public:

public:
	Model() = default;
	Model( Assets::IModel* asset, Vector<nvrhi::BufferHandle>&& indexBuffers, VertexBufferMap&& vertexBuffers );
	~Model() = default;
	
	StringView GetName() const override;

	size_t GetNumFaces() const override;
	IBuffer* GetVertexBuffer( uint32_t face, Assets::RenderData::VertexAttributeType attribute ) const override;
	IBuffer* GetIndexBuffer( uint32_t face ) const override;

	const Assets::ModelDesc& GetDesc() const override;

private:
	Vector<nvrhi::BufferHandle> indexBuffers{};
	VertexBufferMap vertexBuffers{};
	Assets::IModel* modelAsset{ nullptr };
};
