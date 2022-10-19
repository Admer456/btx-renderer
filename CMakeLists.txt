## Minimum is 3.16 for PCH support
cmake_minimum_required( VERSION 3.16 )

## Project name
project( BtxRenderer )

## Some property stuff
set_property( GLOBAL PROPERTY USE_FOLDERS ON )
## C++17's filesystem and inline static initialisers are pretty nice
set( CMAKE_CXX_STANDARD 17 )

set( BTXR_ROOT ${CMAKE_CURRENT_SOURCE_DIR} )
set( BTXR_BIN_DIR ${BTX_RENDERER_ROOT}/bin )

set( BTXR_SOURCES
	${BTXR_ROOT}/renderer/Batch.hpp
	${BTXR_ROOT}/renderer/Batch.cpp
	${BTXR_ROOT}/renderer/Entity.hpp
	${BTXR_ROOT}/renderer/Entity.cpp
	${BTXR_ROOT}/renderer/Light.hpp
	${BTXR_ROOT}/renderer/Light.cpp
	${BTXR_ROOT}/renderer/Model.hpp
	${BTXR_ROOT}/renderer/Model.cpp
	${BTXR_ROOT}/renderer/Precompiled.hpp
	${BTXR_ROOT}/renderer/RenderFrontend.hpp
	${BTXR_ROOT}/renderer/RenderFrontend.cpp
	${BTXR_ROOT}/renderer/RenderFrontend.Init.cpp
	${BTXR_ROOT}/renderer/RenderFrontend.Model.cpp
	${BTXR_ROOT}/renderer/Texture.hpp
	${BTXR_ROOT}/renderer/Texture.cpp
	${BTXR_ROOT}/renderer/View.hpp
	${BTXR_ROOT}/renderer/View.cpp
	${BTXR_ROOT}/renderer/Volume.hpp
	${BTXR_ROOT}/renderer/Volume.cpp )

source_group( TREE ${BTXR_ROOT} FILES ${BTXR_SOURCES} )

add_library( BtxRenderer SHARED
	${BTXR_SOURCES} )

target_link_libraries( BtxRenderer BtxCommon nvrhi )
