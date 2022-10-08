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
	return true;
}

void RenderFrontend::Shutdown()
{
}

void RenderFrontend::SetBackend( Render::IBackend* renderBackend )
{
	
}

Render::IBackend* RenderFrontend::GetBackend() const
{
	return nullptr;
}

void RenderFrontend::RenderView( const Render::IView* view )
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

Render::IBatch* RenderFrontend::CreateBatch( const Render::BatchDesc& desc )
{
	return nullptr;
}

bool RenderFrontend::DestroyBatch( Render::IBatch* batch )
{
	return false;
}

const Vector<Render::IBatch*>& RenderFrontend::GetBatches() const
{
	// // O: insert return statement here
	return {};
}

Render::IEntity* RenderFrontend::CreateEntity( const Render::EntityDesc& desc )
{
	return nullptr;
}

bool RenderFrontend::DestroyEntity( Render::IEntity* entity )
{
	return false;
}

const Vector<Render::IEntity*>& RenderFrontend::GetEntities() const
{
	// // O: insert return statement here
	return {};
}

Render::ILight* RenderFrontend::CreateLight( const Render::LightDesc& desc )
{
	return nullptr;
}

bool RenderFrontend::DestroyLight( Render::ILight* light )
{
	return false;
}

const Vector<Render::ILight*>& RenderFrontend::GetLights() const
{
	// // O: insert return statement here
	return {};
}

Render::IView* RenderFrontend::CreateView( const Render::ViewDesc& desc )
{
	return nullptr;
}

bool RenderFrontend::DestroyView( Render::IView* view )
{
	return false;
}

const Vector<Render::IView*>& RenderFrontend::GetViews() const
{
	// // O: insert return statement here
	return {};
}

Render::IVolume* RenderFrontend::CreateVolume( const Render::VolumeDesc& desc )
{
	return nullptr;
}

bool RenderFrontend::DestroyVolume( Render::IVolume* volume )
{
	return false;
}

const Vector<Render::IVolume*>& RenderFrontend::GetVolumes() const
{
	// // O: insert return statement here
	return {};
}
