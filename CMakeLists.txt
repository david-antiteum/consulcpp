cmake_minimum_required( VERSION 3.12...3.16 )

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include( Common )
include( Tools )

project( consulcpp VERSION 0.1.1 DESCRIPTION "A C++ library that implements the Consul API" LANGUAGES CXX )

option( BUILD_EXAMPLES "Build examples" ON )

add_subdirectory( src )
if( BUILD_EXAMPLES )
	add_subdirectory( examples/leader )
endif()
