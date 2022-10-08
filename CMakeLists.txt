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
	${BTXR_ROOT}/src/Precompiled.hpp
	${BTXR_ROOT}/src/RenderFrontend.hpp
	${BTXR_ROOT}/src/RenderFrontend.cpp )

add_library( BtxRenderer SHARED ${BTXR_SOURCES} )

target_link_libraries( BtxRenderer BtxCommon )
