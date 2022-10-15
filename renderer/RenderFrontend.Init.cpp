// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#include "Precompiled.hpp"
#include "RenderFrontend.hpp"

bool RenderFrontend::CreateCommandLists()
{
	auto renderParams = nvrhi::CommandListParameters()
		.setQueueType( nvrhi::CommandQueue::Graphics );

	auto transferParams = nvrhi::CommandListParameters()
		.setQueueType( nvrhi::CommandQueue::Copy );

	renderCommands = backend->createCommandList( renderParams );
	transferCommands = backend->createCommandList( transferParams );

	return nullptr != renderCommands && nullptr != transferCommands;
}

bool RenderFrontend::CreateMainFramebuffer()
{
	return false;
}
