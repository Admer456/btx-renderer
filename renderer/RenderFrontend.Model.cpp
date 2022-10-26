// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#include "Precompiled.hpp"
#include "RenderFrontend.hpp"

bool RenderFrontend::ValidateModelAsset( const Assets::IModel* modelAsset )
{
	const auto& data = modelAsset->GetModelData();
	const StringView name = modelAsset->GetName();

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
		//.setFormat( nvrhi::Format::R32_UINT )
		.setIsIndexBuffer( true )
		.setInitialState( nvrhi::ResourceStates::CopyDest );

	indexBuffer = backend->createBuffer( desc );
	if ( nullptr == indexBuffer )
	{
		return nullptr;
	}

	transferCommands->open();
	transferCommands->beginTrackingBufferState( indexBuffer, nvrhi::ResourceStates::CopyDest );
	transferCommands->writeBuffer( indexBuffer, indices.data(), indices.size() * sizeof( uint32_t ) );
	transferCommands->setPermanentBufferState( indexBuffer, nvrhi::ResourceStates::IndexBuffer );
	transferCommands->close();

	backend->executeCommandList( transferCommands, nvrhi::CommandQueue::Graphics );
	
	// TODO: Figure out **when** to populate the buffers
	// Should we:
	// 1) Open a commandlist here, record everything that's needed and execute immediately?
	// 2) Open a commandlist here, record, execute later?
	// 3) Submit some sorta request into a queue, to do #1 sometime later?
	// Right now, we're doing #1 because it's the most naive & simple one
	return indexBuffer;
}

static const char* VertexSegmentToString( Assets::RenderData::VertexAttributeType segmentType )
{
	using Assets::RenderData::VertexAttributeType;

	switch ( segmentType )
	{
	case VertexAttributeType::Position: return "Position";
	case VertexAttributeType::Normal: return "Normal";
	case VertexAttributeType::TangentAndBitangent: return "TangentAndBitangent";
	case VertexAttributeType::Uv1: return "Uv1";
	case VertexAttributeType::Uv2: return "Uv2";
	case VertexAttributeType::Uv3: return "Uv3";
	case VertexAttributeType::Uv4: return "Uv4";
	case VertexAttributeType::Colour1: return "Colour1";
	case VertexAttributeType::Colour2: return "Colour2";
	case VertexAttributeType::Colour3: return "Colour3";
	case VertexAttributeType::Colour4: return "Colour4";
	case VertexAttributeType::BoneWeights: return "BoneWeights";
	case VertexAttributeType::BoneIndices: return "BoneIndices";
	}

	return "UNKNOWN";
}

bool RenderFrontend::CreateVertexBuffer( uint32_t face, const Assets::RenderData::VertexDataSegment& segment, VertexBufferMap& outVertexBuffers )
{
	const VertexMapKey key = { face, segment.type };

	auto desc = nvrhi::BufferDesc()
		.setByteSize( segment.rawData.size() )
		.setIsVertexBuffer( true )
		.setInitialState( nvrhi::ResourceStates::CopyDest );

	nvrhi::BufferHandle vertexBuffer = backend->createBuffer( desc );

	if ( nullptr == vertexBuffer )
	{
		return false;
	}

	transferCommands->open();
	transferCommands->beginTrackingBufferState( vertexBuffer, nvrhi::ResourceStates::CopyDest );
	transferCommands->writeBuffer( vertexBuffer, segment.rawData.data(), desc.byteSize );
	transferCommands->setPermanentBufferState( vertexBuffer, nvrhi::ResourceStates::VertexBuffer );
	transferCommands->close();

	backend->executeCommandList( transferCommands, nvrhi::CommandQueue::Graphics );
	
	// Finally insert the thing
	outVertexBuffers[key] = vertexBuffer;

	return true;
}

bool RenderFrontend::CreateBuffersFromVertexData( uint32_t face, const Assets::RenderData::VertexData& data,
	Vector<nvrhi::BufferHandle>& outIndexBuffers, VertexBufferMap& outVertexBuffers )
{
	// The render backend will report errors in this situation
	nvrhi::BufferHandle indexBufferHandle = CreateIndexBuffer( data.vertexIndices );
	if ( nullptr == indexBufferHandle )
	{
		Console->Error( format( "Failed to create index buffer (face %u)", face ) );
		return false;
	}

	outIndexBuffers.push_back( std::move( indexBufferHandle ) );

	// Build different vertex buffer segments, e.g. one buffer for positions, one for normals etc.
	bool failedCreatingVertexBuffers = false;
	for ( const auto& segment : data.vertexData )
	{
		if ( !CreateVertexBuffer( face, segment, outVertexBuffers ) )
		{
			Console->Error( format( "Failed to create vertex buffer (face %u, segment '%s')",
				face, VertexSegmentToString( segment.type ) ) );
			failedCreatingVertexBuffers = true;
		}
	}

	return !failedCreatingVertexBuffers;
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
					modelAsset->GetName().data(), mesh.name.data(), faceId ) );
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
