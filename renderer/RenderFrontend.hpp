// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#pragma once

#include "Model.hpp"

class RenderFrontend : public IRenderFrontend
{
public: // Data structures for binding sets
	struct ViewFrameData
	{
		Mat4 viewMatrix;
		Mat4 projectionMatrix;
		float time;
	};

	struct EntityData
	{
		Mat4 modelMatrix;
		Vec4 shaderParametersA;
		Vec4 shaderParametersB;
	};

public: // Plugin API
	bool					Init( const EngineAPI& api ) override;
	void					Shutdown() override;

	const char*				GetPluginName() const override
	{
		return "BTX Test Renderer";
	}

public: // Render frontend API
	bool 					PostInit( RenderBackend* renderBackend, IWindow* mainWindow ) override;
	void					Update() override;
	IBackend*				GetBackend() const override;

	void					BeginFrame() override;
	void					EndFrameAndPresent( const IView* view ) override;

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

	// RenderFrontend.Init.cpp
	bool					CreateCommandLists();
	bool					CreateMainFramebuffer();
	bool					CreateScreenVertexBuffer(); // screen quad

	// RenderFrontend.Model.cpp
	bool					ValidateModelAsset( const Assets::IModel* modelAsset );
	nvrhi::BufferHandle		CreateIndexBuffer( Vector<uint32_t> indices );
	nvrhi::BufferHandle		CreateVertexBuffer( const Vector<float>& rawVertexData );
	bool					CreateVertexBuffer( uint32_t face, const Assets::RenderData::VertexDataSegment& segment, VertexBufferMap& outVertexBuffers );
	bool					CreateBuffersFromVertexData( uint32_t face, const Assets::RenderData::VertexData& data, Vector<nvrhi::BufferHandle>& outIndexBuffers, VertexBufferMap& outVertexBuffers );
	IModel*					BuildModelFromAsset( const Assets::IModel* modelAsset );

	// RenderFrontend.Pipeline.cpp
	Path					BuildShaderPath( nvrhi::ShaderType type, StringView shaderPath );
	nvrhi::ShaderHandle		CreateShader( nvrhi::ShaderType type, StringView shaderPath );
	bool					CreateShaderPair( StringView shaderPath, nvrhi::ShaderHandle& outVertexShader, nvrhi::ShaderHandle& outPixelShader );
	nvrhi::ShaderHandle		CreateComputeShader( StringView shaderPath );
	bool					CreateMainShaders();
	bool					CreateMainGraphicsPipelines();
	
	// RenderFrontend.Render.cpp
	bool					IsEntityVisible( const IView* view, const IEntity* entity );
	void					RenderEntity( const IView* view, const IEntity* entity );

	// RenderFrontend.Texture.cpp
	std::pair<nvrhi::TextureHandle, nvrhi::TextureHandle> CreateFramebufferImagesForView( const ViewDesc& desc );
	nvrhi::FramebufferHandle CreateFramebufferFromImages( nvrhi::ITexture* colourTexture, nvrhi::ITexture* depthTexture );
	nvrhi::BindingSetHandle CreateBindingSetForView( nvrhi::TextureHandle colourTexture, nvrhi::TextureHandle depthTexture );

private:
	Vector<UniquePtr<IBatch>>	batches{};
	Vector<UniquePtr<IEntity>>	entities{};
	Vector<UniquePtr<ILight>>	lights{};
	Vector<UniquePtr<ITexture>>	textures{};
	Vector<UniquePtr<IView>>	views{};
	Vector<UniquePtr<IVolume>>	volumes{};
	Vector<UniquePtr<IModel>>	models{};

	IWindow*				window{ nullptr };
	RenderBackend*			backendManager{ nullptr };
	IBackend*				backend{ nullptr };

	nvrhi::SamplerHandle	screenSampler{ nullptr };

	nvrhi::CommandListHandle transferCommands{};
	// Later we will have multiple render commandlists,
	// so we can do multithreaded rendering
	nvrhi::CommandListHandle renderCommands{};

	// Todo: replace with a map of vertex layouts
	// E.g. something that could be used like GetVertexLayoutForCombo( { VA::Position, VA::Normal } );
	// In latter iterations, when we have a material system, base materials will demand vertex layout
	// specifications, and this is crucial for that
	nvrhi::InputLayoutHandle entityVertexLayout{};
	// This will also be changed once we have a material system.
	// Each material base will have its own pipeline which uses its own shader
	nvrhi::BindingLayoutHandle viewFrameBindingLayout{};
	nvrhi::BindingLayoutHandle entityBindingLayout{};
	// Look at ViewFrameData and EntityData
	nvrhi::BufferHandle		viewDataBuffer{};
	ViewFrameData			currentViewData;
	nvrhi::BufferHandle		entityDataBuffer{};
	EntityData				currentEntityData;
	// This binding set contains the projection & view matrix. It changes per frame and per view
	// Later on there will be a per-surface binding set too, once we have texturing and all
	nvrhi::BindingSetHandle viewFrameBindingSet{};
	// This binding set contains transformation data for the entity and its parameters
	nvrhi::BindingSetHandle entityBindingSet{};
	nvrhi::ShaderHandle entityVertexShader{};
	nvrhi::ShaderHandle entityPixelShader{};
	nvrhi::GraphicsPipelineHandle entityPipeline{};
	
	// The minimum needed to render a basic fullscreen quad
	// 2D vector for positions, and another 2D vector for texture coords
	nvrhi::InputLayoutHandle screenVertexLayout{};
	nvrhi::BufferHandle		screenVertexBuffer{};
	nvrhi::BufferHandle		screenIndexBuffer{};
	// 1 texture sampler, 1 colour attachment, 1 depth attachment
	nvrhi::BindingLayoutHandle screenBindingLayout{};
	nvrhi::ShaderHandle		screenVertexShader{};
	nvrhi::ShaderHandle		screenPixelShader{};
	nvrhi::GraphicsPipelineHandle screenPipeline{};
};
