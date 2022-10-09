// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#include "Precompiled.hpp"
#include "RenderFrontend.hpp"

static PluginRegistry Registry( EngineVersion );

ADM_API PluginRegistry* GetPluginRegistry()
{
	Registry.Register<RenderFrontend>();
	return &Registry;
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

	Core = nullptr;
	Console = nullptr;
	FileSystem = nullptr;
	ModelManager = nullptr;
	MaterialManager = nullptr;
}

void RenderFrontend::SetBackend( IBackend* renderBackend )
{
	backend = renderBackend;
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

const Vector<IBatch*>& RenderFrontend::GetBatches() const
{
	return batches;
}

IEntity* RenderFrontend::CreateEntity( const EntityDesc& desc )
{
	return nullptr;
}

bool RenderFrontend::DestroyEntity( IEntity* entity )
{
	return false;
}

const Vector<IEntity*>& RenderFrontend::GetEntities() const
{
	return entities;
}

ILight* RenderFrontend::CreateLight( const LightDesc& desc )
{
	return nullptr;
}

bool RenderFrontend::DestroyLight( ILight* light )
{
	return false;
}

const Vector<ILight*>& RenderFrontend::GetLights() const
{
	return lights;
}

ITexture* RenderFrontend::CreateTexture( const TextureDesc& desc )
{
	return nullptr;
}

bool RenderFrontend::DestroyTexture( ITexture* view )
{
	return false;
}

const Vector<ITexture*>& RenderFrontend::GetTextures() const
{
	return textures;
}

IView* RenderFrontend::CreateView( const ViewDesc& desc )
{
	return nullptr;
}

bool RenderFrontend::DestroyView( IView* view )
{
	return false;
}

const Vector<IView*>& RenderFrontend::GetViews() const
{
	return views;
}

IVolume* RenderFrontend::CreateVolume( const VolumeDesc& desc )
{
	return nullptr;
}

bool RenderFrontend::DestroyVolume( IVolume* volume )
{
	return false;
}

const Vector<IVolume*>& RenderFrontend::GetVolumes() const
{
	return volumes;
}
