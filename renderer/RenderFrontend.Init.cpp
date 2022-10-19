// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#include "Precompiled.hpp"
#include "RenderFrontend.hpp"

bool RenderFrontend::PostInit( IBackend* renderBackend, IWindow* mainWindow )
{
	backend = renderBackend;
	window = mainWindow;

	if ( !CreateCommandLists() )
	{
		Console->Error( "RenderFrontend::PostInit: Failed to create commandlists" );
		return false;
	}

	if ( !CreateMainFramebuffer() )
	{
		Console->Error( "RenderFrontend::PostInit: Failed to create main framebuffer" );
		return false;
	}

	return true;
}

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
