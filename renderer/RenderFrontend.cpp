// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#include "Precompiled.hpp"
#include "RenderFrontend.hpp"

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

bool RenderFrontend::PostInit( IBackend* renderBackend, IWindow* mainWindow )
{
	backend = renderBackend;
	window = mainWindow;

	if ( !CreateMainFramebuffer() )
	{
		Console->Error( "RenderFrontend::PostInit: Failed to create main framebuffer" );
		return false;
	}

	return true;
}

void RenderFrontend::Update()
{

}

IBackend* RenderFrontend::GetBackend() const
{
	return backend;
}

void RenderFrontend::RenderView( const IView* view )
{

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
	return nullptr;
}

bool RenderFrontend::DestroyEntity( IEntity* entity )
{
	return false;
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
	return nullptr;
}

bool RenderFrontend::DestroyView( IView* view )
{
	return false;
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
	return nullptr;
}

bool RenderFrontend::DestroyModel( IModel* Model )
{
	return false;
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
