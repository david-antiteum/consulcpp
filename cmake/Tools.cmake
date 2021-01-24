# Copyright Tomas Zeman 2019.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
# https://raw.githubusercontent.com/zemasoft/clangformat-cmake/master/cmake/ClangFormat.cmake

function(clang_format_setup)
	if(NOT CLANGFORMAT_EXECUTABLE)
		set(CLANGFORMAT_EXECUTABLE clang-format)
	endif()

	if(NOT EXISTS ${CLANGFORMAT_EXECUTABLE})
		if( WIN32 )
			find_program(clangformat_executable_tmp ${CLANGFORMAT_EXECUTABLE} HINTS $ENV{VCINSTALLDIR}/Tools/Llvm/bin )
		else()
			find_program(clangformat_executable_tmp ${CLANGFORMAT_EXECUTABLE})
		endif()
		if(clangformat_executable_tmp)
			set(CLANGFORMAT_EXECUTABLE ${clangformat_executable_tmp})
			unset(clangformat_executable_tmp)
		endif()
	endif()

	if( EXISTS ${CLANGFORMAT_EXECUTABLE} )
		foreach(clangformat_source ${ARGV})
			get_filename_component(clangformat_source ${clangformat_source} ABSOLUTE)
			list(APPEND clangformat_sources ${clangformat_source})
		endforeach()

		add_custom_target(${PROJECT_NAME}_clangformat
			COMMAND
			${CLANGFORMAT_EXECUTABLE}
			-style=file
			-i
			${clangformat_sources}
			COMMENT
			"Formating with ${CLANGFORMAT_EXECUTABLE} ..."
		)

		if(TARGET clangformat)
			add_dependencies(clangformat ${PROJECT_NAME}_clangformat)
		else()
			add_custom_target(clangformat DEPENDS ${PROJECT_NAME}_clangformat)
		endif()
	endif()
endfunction()

function(target_clang_format_setup target)
	get_target_property(target_sources ${target} SOURCES)
	clang_format_setup(${target_sources})
endfunction()

function(sonarqube_setup)
	if( WIN32 )
		find_program( buildwrapper_tmp build-wrapper-win-x86-64.exe )
	elseif( APPLE )
		find_program( buildwrapper_tmp build-wrapper-macosx-x86 )
	else()
		find_program( buildwrapper_tmp build-wrapper-linux-x86-64 )
	endif()
	if( buildwrapper_tmp )
		set( BUILDWRAPPER_EXECUTABLE ${buildwrapper_tmp})
		unset( buildwrapper_tmp )

		add_custom_target( sonarqube-build
			COMMAND
				${CMAKE_MAKE_PROGRAM} clean
			COMMAND
				${BUILDWRAPPER_EXECUTABLE}
				--out-dir bw-output
				${CMAKE_MAKE_PROGRAM}
			COMMENT
				"Run sonarqube build"
			WORKING_DIRECTORY 
				${CMAKE_BINARY_DIR} 
		)

		add_custom_target( sonarqube
			COMMAND
				sonar-scanner
			COMMENT
				"Run sonarqube analysis"
			WORKING_DIRECTORY 
				${CMAKE_SOURCE_DIR} 
		)

		add_dependencies( sonarqube sonarqube-build )
	endif()
endfunction()
###

# Enable clang-tidy either setting the app or using the option
option( ENABLE_CLANG_TIDY "Enable clang-tidy building." OFF )
if( ENABLE_CLANG_TIDY OR CLANG_TIDY )
	if( NOT CLANG_TIDY )
		if( WIN32 )
			find_program( CLANG_TIDY NAMES "clang-tidy" HINTS $ENV{VCINSTALLDIR}/Tools/Llvm/bin )
		else()
			find_program( CLANG_TIDY NAMES "clang-tidy" )
		endif()
	endif()
	if( CLANG_TIDY )
		set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY}" -checks=readability-*,cppcoreguidelines-*)
	endif()
endif()

option( ENABLE_CPP_CHECK "Enable cppcheck building." OFF )
if( ENABLE_CPP_CHECK OR CMAKE_CXX_CPPCHECK )
	if( NOT CMAKE_CXX_CPPCHECK )
		if( WIN32 )
			find_program( CMAKE_CXX_CPPCHECK NAMES "cppcheck" HINTS $ENV{ProgramFiles}/Cppcheck )
		else()
			find_program( CMAKE_CXX_CPPCHECK NAMES "cppcheck" )
		endif()
	endif()
	if( CMAKE_CXX_CPPCHECK )
		list(
			APPEND CMAKE_CXX_CPPCHECK
				"--std=c++17"
				"--enable=all"
				"--inconclusive"
				"--force" 
				"--inline-suppr"
				"--suppressions-list=${CMAKE_SOURCE_DIR}/CppCheckSuppressions.txt"
		)
	endif()
endif()
