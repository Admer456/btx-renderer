// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#pragma once

class RenderFrontend : public IRenderFrontend
{
public:
	bool Init( const EngineAPI& api ) override;
	void Shutdown() override;

	const char* GetPluginName() const override
	{
		return "BTX Test Renderer";
	}

	Render::IBackend* GetBackend() const override;

	void RenderView( const Render::IView* view ) override;
	
	void DebugLine( adm::Vec3 start, adm::Vec3 end, adm::Vec3 colour, float life, bool depthTest ) override;
	void DebugRay( adm::Vec3 start, adm::Vec3 direction, float length, bool withArrowhead, adm::Vec3 colour, float life, bool depthTest ) override;
	void DebugBox( adm::Vec3 min, adm::Vec3 max, adm::Vec3 colour, float life, bool depthTest ) override;
	void DebugCube( adm::Vec3 position, float extents, adm::Vec3 colour, float life, bool depthTest ) override;
	void DebugSphere( adm::Vec3 position, float extents, adm::Vec3 colour, float life, bool depthTest ) override;

	Render::IBatch* CreateBatch( const Render::BatchDesc& desc ) override;
	bool DestroyBatch( Render::IBatch* batch ) override;
	const Vector<Render::IBatch*>& GetBatches() const override;

	Render::IEntity* CreateEntity( const Render::EntityDesc& desc ) override;
	bool DestroyEntity( Render::IEntity* entity ) override;
	const Vector<Render::IEntity*>& GetEntities() const override;

	Render::ILight* CreateLight( const Render::LightDesc& desc ) override;
	bool DestroyLight( Render::ILight* light ) override;
	const Vector<Render::ILight*>& GetLights() const override;

	Render::IView* CreateView( const Render::ViewDesc& desc ) override;
	bool DestroyView( Render::IView* view ) override;
	const Vector<Render::IView*>& GetViews() const override;

	Render::IVolume* CreateVolume( const Render::VolumeDesc& desc ) override;
	bool DestroyVolume( Render::IVolume* volume ) override;
	const Vector<Render::IVolume*>& GetVolumes() const override;
};
