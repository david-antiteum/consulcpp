cmake_minimum_required( VERSION 3.12...3.16 )

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include( Common )
include( Tools )

project( consulcpp VERSION 0.2.2 DESCRIPTION "A C++ library that implements the Consul API" LANGUAGES CXX )

option( BUILD_EXAMPLES "Build examples" OFF )
option( BUILD_TESTING "Build tests" OFF )
option( ADD_COVERAGE "Add coverage" OFF )

sonarqube_setup()

add_subdirectory( src )
if( BUILD_EXAMPLES )
	add_subdirectory( examples/leader )
endif()
if( BUILD_TESTING )
	add_subdirectory( tests )

	if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
		include( CTest )
		if( ADD_COVERAGE )
			include( CodeCoverage )
			append_coverage_compiler_flags()
			setup_target_for_coverage_gcovr( 
				NAME coverage
				EXECUTABLE consulcpp-test
			)
		endif()
	endif()
endif()
