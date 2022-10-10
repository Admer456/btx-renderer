// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#pragma once

class RenderFrontend : public IRenderFrontend
{
public: // Plugin API
	bool					Init( const EngineAPI& api ) override;
	void					Shutdown() override;

	const char*				GetPluginName() const override
	{
		return "BTX Test Renderer";
	}

public: // Render frontend API
	bool 					PostInit( IBackend* renderBackend, IWindow* mainWindow ) override;
	void					Update() override;
	IBackend*				GetBackend() const override;

	void					RenderView( const IView* view ) override;
	
	void					DebugLine( Vec3 start, Vec3 end, Vec3 colour, float life, bool depthTest ) override;
	void					DebugRay( Vec3 start, Vec3 direction, float length, bool withArrowhead, Vec3 colour, float life, bool depthTest ) override;
	void					DebugBox( Vec3 min, Vec3 max, Vec3 colour, float life, bool depthTest ) override;
	void					DebugCube( Vec3 position, float extents, Vec3 colour, float life, bool depthTest ) override;
	void					DebugSphere( Vec3 position, float extents, Vec3 colour, float life, bool depthTest ) override;

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

private: // Internals



private:
	Vector<IBatch*>			batches{};
	Vector<IEntity*>		entities{};
	Vector<ILight*>			lights{};
	Vector<ITexture*>		textures{};
	Vector<IView*>			views{};
	Vector<IVolume*>		volumes{};

	IWindow*				window{ nullptr };
	IBackend*				backend{ nullptr };
};
