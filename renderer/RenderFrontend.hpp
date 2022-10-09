// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#pragma once

class RenderFrontend : public IRenderFrontend
{
public:
	bool					Init( const EngineAPI& api ) override;
	void					Shutdown() override;

	const char*				GetPluginName() const override
	{
		return "BTX Test Renderer";
	}

	void					SetBackend( IBackend* renderBackend ) override;
	IBackend*				GetBackend() const override;

	void					RenderView( const IView* view ) override;
	
	void					DebugLine( adm::Vec3 start, adm::Vec3 end, adm::Vec3 colour, float life, bool depthTest ) override;
	void					DebugRay( adm::Vec3 start, adm::Vec3 direction, float length, bool withArrowhead, adm::Vec3 colour, float life, bool depthTest ) override;
	void					DebugBox( adm::Vec3 min, adm::Vec3 max, adm::Vec3 colour, float life, bool depthTest ) override;
	void					DebugCube( adm::Vec3 position, float extents, adm::Vec3 colour, float life, bool depthTest ) override;
	void					DebugSphere( adm::Vec3 position, float extents, adm::Vec3 colour, float life, bool depthTest ) override;

	IBatch*					CreateBatch( const BatchDesc& desc ) override;
	bool					DestroyBatch( IBatch* batch ) override;
	const Vector<IBatch*>&	GetBatches() const override;

	IEntity*				CreateEntity( const EntityDesc& desc ) override;
	bool					DestroyEntity( IEntity* entity ) override;
	const Vector<IEntity*>& GetEntities() const override;

	ILight*					CreateLight( const LightDesc& desc ) override;
	bool					DestroyLight( ILight* light ) override;
	const Vector<ILight*>&	GetLights() const override;

	ITexture*				CreateTexture( const TextureDesc& desc ) override;
	bool 					DestroyTexture( ITexture* view ) override;
	const Vector<ITexture*>& GetTextures() const override;

	IView*					CreateView( const ViewDesc& desc ) override;
	bool					DestroyView( IView* view ) override;
	const Vector<IView*>&	GetViews() const override;

	IVolume*				CreateVolume( const VolumeDesc& desc ) override;
	bool					DestroyVolume( IVolume* volume ) override;
	const Vector<IVolume*>& GetVolumes() const override;

private:
	Vector<IBatch*>			batches{};
	Vector<IEntity*>		entities{};
	Vector<ILight*>			lights{};
	Vector<ITexture*>		textures{};
	Vector<IView*>			views{};
	Vector<IVolume*>		volumes{};

	IBackend*				backend{ nullptr };
};
