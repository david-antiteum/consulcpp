cmake_minimum_required( VERSION 3.12...3.16 )

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include( Common )
include( Tools )

project( consulcpp VERSION 0.1.1 DESCRIPTION "A C++ library that implements the Consul API" LANGUAGES CXX )

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	include( CTest )
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		include( CodeCoverage )
		append_coverage_compiler_flags()
		setup_target_for_coverage_gcovr( 
			NAME coverage
			EXECUTABLE consulcpp-test
		)
	endif()
endif()

option( BUILD_EXAMPLES "Build examples" ON )

sonarqube_setup()

add_subdirectory( src )
if( BUILD_EXAMPLES )
	add_subdirectory( examples/leader )
endif()
if( BUILD_TESTING )
	add_subdirectory( tests )
endif()
