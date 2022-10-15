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
	size_t					GetNumBatches() const override;
	IBatch*					GetBatch( uint32_t index ) override;

	IEntity*				CreateEntity( const EntityDesc& desc ) override;
	bool					DestroyEntity( IEntity* entity ) override;
	size_t					GetNumEntities() const override;
	IEntity*				GetEntity( uint32_t index ) override;

	ILight*					CreateLight( const LightDesc& desc ) override;
	bool					DestroyLight( ILight* light ) override;
	size_t					GetNumLights() const override;
	ILight*					GetLight( uint32_t index ) override;

	ITexture*				CreateTexture( const TextureDesc& desc ) override;
	bool 					DestroyTexture( ITexture* view ) override;
	size_t					GetNumTextures() const override;
	ITexture*				GetTexture( uint32_t index ) override;

	IView*					CreateView( const ViewDesc& desc ) override;
	bool					DestroyView( IView* view ) override;
	size_t					GetNumViews() const override;
	IView*					GetView( uint32_t index ) override;

	IVolume*				CreateVolume( const VolumeDesc& desc ) override;
	bool					DestroyVolume( IVolume* volume ) override;
	size_t					GetNumVolumes() const override;
	IVolume*				GetVolume( uint32_t index ) override;

	IModel*					CreateModel( const Assets::IModel* modelAsset ) override;
	bool					DestroyModel( IModel* model ) override;
	size_t					GetNumModels() const override;
	IModel*					GetModel( uint32_t index ) override;

private: // Internals

	bool					CreateMainFramebuffer();

private:
	Vector<UniquePtr<IBatch>>	batches{};
	Vector<UniquePtr<IEntity>>	entities{};
	Vector<UniquePtr<ILight>>	lights{};
	Vector<UniquePtr<ITexture>>	textures{};
	Vector<UniquePtr<IView>>	views{};
	Vector<UniquePtr<IVolume>>	volumes{};
	Vector<UniquePtr<IModel>>	models{};

	IWindow*				window{ nullptr };
	IBackend*				backend{ nullptr };

	nvrhi::ITexture*		mainFramebufferColour{ nullptr };
	nvrhi::ITexture*		mainFramebufferDepth{ nullptr };
	nvrhi::IFramebuffer*	mainFramebuffer{ nullptr };
};
