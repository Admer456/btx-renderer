// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#include "Precompiled.hpp"
#include "RenderFrontend.hpp"
#include "Entity.hpp"
#include "View.hpp"
#include <nvrhi/utils.h>

static PluginRegistry Registry( EngineVersion );

ADM_API PluginRegistry* GetPluginRegistry()
{
	return &Registry.Register<RenderFrontend>();
}

bool RenderFrontend::Init( const EngineAPI& api )
{
	Core = api.core;
	Console = api.console;
	FileSystem = api.fileSystem;
	ModelManager = api.modelManager;
	MaterialManager = api.materialManager;

	Console->Print( "RenderFrontend::Init" );

	return true;
}

void RenderFrontend::Shutdown()
{
	Console->Print( "RenderFrontend::Shutdown" );

	batches.clear();
	entities.clear();
	lights.clear();
	textures.clear();
	views.clear();
	volumes.clear();
	models.clear();

	backend = nullptr;

	Core = nullptr;
	Console = nullptr;
	FileSystem = nullptr;
	ModelManager = nullptr;
	MaterialManager = nullptr;
}

void RenderFrontend::Update()
{

}

IBackend* RenderFrontend::GetBackend() const
{
	return backend;
}

void RenderFrontend::BeginFrame()
{
	backendManager->BeginFrame();
}

void RenderFrontend::EndFrameAndPresent()
{
	const nvrhi::Viewport windowViewport = { window->GetSize().x, window->GetSize().y };
	auto graphicsState = nvrhi::GraphicsState()
		.addBindingSet( screenBindingSet )
		.addVertexBuffer( { screenVertexBuffer, 0, 0 } )
		.setIndexBuffer( { screenIndexBuffer, nvrhi::Format::R32_UINT, 0 } )
		.setFramebuffer( backendManager->GetCurrentFramebuffer() )
		.setPipeline( screenPipeline );
	graphicsState.viewport.addViewportAndScissorRect( windowViewport );

	// Vertex count is actually index count in this case
	const auto drawArgs = nvrhi::DrawArguments().setVertexCount( 6 );

	renderCommands->open();
	renderCommands->setGraphicsState( graphicsState );

	renderCommands->drawIndexed( drawArgs );

	renderCommands->close();
	backend->executeCommandList( renderCommands );

	backendManager->Present();
	backend->runGarbageCollection();
}

void RenderFrontend::RenderView( const IView* view )
{
	const Vec4 c = view->GetDesc().clearColour;
	const nvrhi::Color clearColour = { c.m.x, c.m.y, c.m.z, c.m.w };

	const nvrhi::Viewport windowViewport = { window->GetSize().x, window->GetSize().y };
	auto graphicsState = nvrhi::GraphicsState()
		.addBindingSet( screenBindingSet )
		.addVertexBuffer( { screenVertexBuffer, 0, 0 } )
		.setIndexBuffer( { screenIndexBuffer, nvrhi::Format::R32_UINT, 0 } )
		.setFramebuffer( backendManager->GetCurrentFramebuffer() )
		.setPipeline( screenPipeline );
	graphicsState.viewport.addViewportAndScissorRect( windowViewport );

	renderCommands->open();
	renderCommands->setGraphicsState( graphicsState );
	nvrhi::utils::ClearColorAttachment( renderCommands, mainFramebuffer, 0, clearColour );
	renderCommands->clearDepthStencilTexture( mainFramebufferDepth, nvrhi::AllSubresources, true, 0.0f, false, 0 );
	//nvrhi::utils::ClearDepthStencilAttachment( renderCommands, mainFramebuffer, 0.0f, 0 );
	renderCommands->close();
}

void RenderFrontend::DebugLine( adm::Vec3 start, adm::Vec3 end, adm::Vec3 colour, float life, bool depthTest )
{

}

void RenderFrontend::DebugRay( adm::Vec3 start, adm::Vec3 direction, float length, bool withArrowhead, adm::Vec3 colour, float life, bool depthTest )
{

}

void RenderFrontend::DebugBox( adm::Vec3 min, adm::Vec3 max, adm::Vec3 colour, float life, bool depthTest )
{

}

void RenderFrontend::DebugCube( adm::Vec3 position, float extents, adm::Vec3 colour, float life, bool depthTest )
{
}

void RenderFrontend::DebugSphere( adm::Vec3 position, float extents, adm::Vec3 colour, float life, bool depthTest )
{
}

IBatch* RenderFrontend::CreateBatch( const BatchDesc& desc )
{
	return nullptr;
}

bool RenderFrontend::DestroyBatch( IBatch* batch )
{
	return false;
}

size_t RenderFrontend::GetNumBatches() const
{
	return batches.size();
}

IBatch* RenderFrontend::GetBatch( uint32_t index )
{
	if ( index >= GetNumBatches() )
	{
		return nullptr;
	}

	return batches.at( index ).get();
}

IEntity* RenderFrontend::CreateEntity( const EntityDesc& desc )
{
	if ( nullptr == desc.model )
	{
		Console->Warning( "RenderFrontend::CreateEntity: tried creating an entity with no model" );
		return false;
	}

	if ( desc.numBones > 0 && nullptr == desc.bones )
	{
		Console->Warning( "RenderFrontend::CreateEntity: you can't have numBones without an actual reference to a bone buffer" );
		return false;
	}

	auto modelIt = FindIterator( models, desc.model );
	if ( modelIt == models.end() )
	{
		Console->Warning( "RenderFrontend::CreateEntity: tried creating an entity with an unregistered model" );
		return false;
	}

	return entities.emplace_back( new Entity( desc ) ).get();
}

bool RenderFrontend::DestroyEntity( IEntity* entity )
{
	if ( nullptr == entity )
	{
		Console->Warning( "RenderFrontend::DestroyEntity: tried destroying a non-existing entity" );
		return false;
	}

	auto it = FindIterator( entities, entity );
	if ( it == entities.end() )
	{
		Console->Warning( "RenderFrontend::DestroyEntity: tried destroying an unregistered entity" );
		return false;
	}

	entities.erase( it );

	return true;
}

size_t RenderFrontend::GetNumEntities() const
{
	return entities.size();
}

IEntity* RenderFrontend::GetEntity( uint32_t index )
{
	if ( index >= GetNumEntities() )
	{
		return nullptr;
	}

	return entities.at( index ).get();
}

ILight* RenderFrontend::CreateLight( const LightDesc& desc )
{
	return nullptr;
}

bool RenderFrontend::DestroyLight( ILight* light )
{
	return false;
}

size_t RenderFrontend::GetNumLights() const
{
	return lights.size();
}

ILight* RenderFrontend::GetLight( uint32_t index )
{
	if ( index >= GetNumLights() )
	{
		return nullptr;
	}

	return lights.at( index ).get();
}

ITexture* RenderFrontend::CreateTexture( const TextureDesc& desc )
{
	return nullptr;
}

bool RenderFrontend::DestroyTexture( ITexture* view )
{
	return false;
}

size_t RenderFrontend::GetNumTextures() const
{
	return textures.size();
}

ITexture* RenderFrontend::GetTexture( uint32_t index )
{
	if ( index >= GetNumTextures() )
	{
		return nullptr;
	}

	return textures.at( index ).get();
}

IView* RenderFrontend::CreateView( const ViewDesc& desc )
{
	return views.emplace_back( new View( desc ) ).get();
}

bool RenderFrontend::DestroyView( IView* view )
{
	if ( nullptr == view )
	{
		Console->Warning( "RenderFrontend::DestroyView: tried destroying a non-existing view" );
		return false;
	}

	auto it = FindIterator( views, view );
	if ( it == views.end() )
	{
		Console->Warning( "RenderFrontend::DestroyView: tried destroying an unregistered view" );
		return false;
	}

	views.erase( it );

	return true;
}

size_t RenderFrontend::GetNumViews() const
{
	return views.size();
}

IView* RenderFrontend::GetView( uint32_t index )
{
	if ( index >= GetNumViews() )
	{
		return nullptr;
	}

	return views.at( index ).get();
}

IVolume* RenderFrontend::CreateVolume( const VolumeDesc& desc )
{
	return nullptr;
}

bool RenderFrontend::DestroyVolume( IVolume* volume )
{
	return false;
}

size_t RenderFrontend::GetNumVolumes() const
{
	return volumes.size();
}

IVolume* RenderFrontend::GetVolume( uint32_t index )
{
	if ( index >= GetNumVolumes() )
	{
		return nullptr;
	}

	return volumes.at( index ).get();
}

IModel* RenderFrontend::CreateModel( const Assets::IModel* modelAsset )
{
	if ( nullptr == modelAsset )
	{
		Console->Warning( "RenderFrontend::CreateModel: tried creating a rendermodel from a non-existing model" );
		return nullptr;
	}

	if ( !ValidateModelAsset( modelAsset ) )
	{
		Console->Warning( format( "RenderFrontend::CreateModel: model '%s' has invalid data", modelAsset->GetName().data() ) );
		return nullptr;
	}

	IModel* model = BuildModelFromAsset( modelAsset );
	if ( nullptr != model )
	{
		models.emplace_back( model );
	}
	else
	{
		Console->Warning( format( "RenderFrontend::CreateModel: model '%s' failed to upload to the GPU", modelAsset->GetName().data() ) );
	}

	return model;
}

bool RenderFrontend::DestroyModel( IModel* model )
{
	if ( nullptr == model )
	{
		Console->Warning( "RenderFrontend::DestroyModel: tried destroying a non-existing model" );
		return false;
	}

	auto it = FindIterator( models, model );
	if ( it == models.end() )
	{
		Console->Warning( "RenderFrontend::DestroyModel: tried destroying an unregistered model" );
		return false;
	}

	models.erase( it );

	return true;
}

size_t RenderFrontend::GetNumModels() const
{
	return models.size();
}

IModel* RenderFrontend::GetModel( uint32_t index )
{
	if ( index >= GetNumModels() )
	{
		return nullptr;
	}

	return models.at( index ).get();
}
