// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#include "Precompiled.hpp"
#include "RenderFrontend.hpp"

bool RenderFrontend::IsEntityVisible( const IView* view, const IEntity* entity )
{
	// We could do all kinds of things here, like frustum culling
	return true;
}

void RenderFrontend::RenderEntity( const IView* view, const IEntity* entity )
{
	using VA = Assets::RenderData::VertexAttributeType;

	const nvrhi::Viewport viewport = { view->GetDesc().viewportSize.x, view->GetDesc().viewportSize.y };
	auto graphicsState = nvrhi::GraphicsState()
		.addBindingSet( viewFrameBindingSet )
		.addBindingSet( entityBindingSet )
		.setFramebuffer( view->GetFramebuffer() )
		.setPipeline( entityPipeline );
	graphicsState.viewport.addViewportAndScissorRect( viewport );

	const EntityDesc& desc = entity->GetDesc();

	currentEntityData.modelMatrix = desc.transform;
	currentEntityData.shaderParametersA = desc.shaderParameters[0];
	currentEntityData.shaderParametersB = desc.shaderParameters[1];

	renderCommands->writeBuffer( entityDataBuffer, &currentEntityData, sizeof( currentEntityData ) );

	const IModel* model = entity->GetDesc().model;
	for ( size_t face = 0; face < model->GetNumFaces(); face++ )
	{
		// TODO: Once there's a material system in place, we need to
		// determine which vertex buffers are used, based on the current
		// material's requirements
		graphicsState.vertexBuffers =
		{
			{ model->GetVertexBuffer( face, VA::Position ), 0U, 0U },
			{ model->GetVertexBuffer( face, VA::Normal ),	1U, 0U },
			{ model->GetVertexBuffer( face, VA::Uv1 ),		2U, 0U },
			{ model->GetVertexBuffer( face, VA::Colour1 ),	3U, 0U }
		};
		graphicsState.indexBuffer = { model->GetIndexBuffer( face ), nvrhi::Format::R32_UINT, 0U };

		const auto drawArguments = nvrhi::DrawArguments().setVertexCount( model->GetNumIndices( face ) );

		renderCommands->setGraphicsState( graphicsState );
		renderCommands->drawIndexed( drawArguments );
	}
}
