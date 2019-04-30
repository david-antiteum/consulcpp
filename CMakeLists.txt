cmake_minimum_required( VERSION 3.12 )

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	set( USING_VCPKG "ON" )
	set( CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "" )
	message( "Using vcpkg settings" )
endif()

project( consulcpp VERSION 0.0.1 DESCRIPTION "A C++ library that implements the Consul API" LANGUAGES CXX )

if( NOT DEFINED USING_VCPKG AND EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
	message( "Using conan settings" )
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup()
endif()

set( CMAKE_CXX_STANDARD 17 )
if(APPLE)
	set(CMAKE_OSX_DEPLOYMENT_TARGET 10.14)
endif()

option( BUILD_EXAMPLES "Build examples" ON )

add_subdirectory( src )
if( BUILD_EXAMPLES )
	add_subdirectory( examples/leader )
endif()
