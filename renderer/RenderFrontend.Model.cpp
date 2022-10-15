// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#include "Precompiled.hpp"
#include "RenderFrontend.hpp"

bool RenderFrontend::ValidateModelAsset( const Assets::IModel* modelAsset )
{
	const auto& data = modelAsset->GetModelData();
	StringView name = modelAsset->GetName();

	if ( data.meshes.empty() )
	{
		Console->Error( format( "RenderFrontend: model '%s' has no meshes",
			name.data() ) );
		return false;
	}

	bool modelInvalid = false;

	for ( const auto& mesh : data.meshes )
	{
		if ( mesh.faces.empty() )
		{
			Console->Error( format( "RenderFrontend: model '%s', mesh '%s' has no faces",
				name.data(), mesh.name.data() ) );
			modelInvalid = true;
			continue;
		}

		int faceId = -1;
		for ( const auto& face : mesh.faces )
		{
			faceId++;

			if ( face.data.vertexIndices.empty() || face.data.vertexData.empty() )
			{
				Console->Error( format( "RenderFrontend: model '%s', mesh '%s' has face with no data",
					name.data(), mesh.name.data() ) );
				modelInvalid = true;
				continue;
			}

			if ( face.data.vertexIndices.size() % 3 )
			{
				Console->Error( format( "RenderFrontend: model '%s', mesh '%s' has face with isolated vertices or edges (vertex indices should be a multiple of 3)",
					name.data(), mesh.name.data() ) );
				modelInvalid = true;
				continue;
			}
		}
	}

	return !modelInvalid;
}

nvrhi::BufferHandle RenderFrontend::CreateIndexBuffer( Vector<uint32_t> indices )
{
	nvrhi::BufferHandle indexBuffer;

	auto desc = nvrhi::BufferDesc()
		.setByteSize( indices.size() * sizeof( uint32_t ) )
		.setFormat( nvrhi::Format::R32_UINT )
		.setIsIndexBuffer( true )
		.setInitialState( nvrhi::ResourceStates::IndexBuffer );


	// TODO: Create the index buffer and then figure out **when** to populate it
	// Should we:
	// 1) Open a commandlist here, record everything that's needed and execute immediately?
	// 2) Open a commandlist here, record, execute later?
	// 3) Submit some sorta request into a queue, to do #1 sometime later?
	// Anyway, I must sleep now, will think about this tomorrow
	//return indexBuffer;
	return nullptr;
}

bool RenderFrontend::CreateBuffersFromVertexData( uint32_t face, const Assets::RenderData::VertexData& data, Vector<nvrhi::BufferHandle>& outIndexBuffers, VertexBufferMap& outVertexBuffers )
{
	nvrhi::BufferHandle indexBufferHandle = CreateIndexBuffer( data.vertexIndices );
	// The render backend will report errors in this situation
	if ( nullptr == indexBufferHandle )
	{
		return false;
	}
	outIndexBuffers.push_back( std::move( indexBufferHandle ) );

	// TODO: create vertex buffers
	return false;
}

IModel* RenderFrontend::BuildModelFromAsset( const Assets::IModel* modelAsset )
{
	Vector<nvrhi::BufferHandle> indexBuffers;
	VertexBufferMap vertexBuffers;

	auto& data = modelAsset->GetModelData();
	for ( const auto& mesh : data.meshes )
	{
		int faceId = -1;
		for ( const auto& face : mesh.faces )
		{
			faceId++;

			if ( !CreateBuffersFromVertexData( uint32_t( faceId ), face.data, indexBuffers, vertexBuffers ) )
			{
				Console->Warning( format( "RenderFrontend: failed to build vertex buffers for model '%', mesh '%s', face %i. Part(s) of the model will not be visible!",
					modelAsset->GetName().data(), mesh.name.data() ) );
			}
		}
	}

	if ( indexBuffers.empty() || vertexBuffers.empty() )
	{
		Console->Error( format( "RenderFrontend: could not upload any model data to the GPU for model '%s'", modelAsset->GetName().data() ) );
		return nullptr;
	}

	return new Model( modelAsset, std::move( indexBuffers ), std::move( vertexBuffers ) );
}
