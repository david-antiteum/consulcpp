cmake_minimum_required( VERSION 3.12...3.16 )

if( CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE MATCHES "^(Debug|Release)$")
	message( FATAL_ERROR "Invalid value for CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}. Use Debug or Release." )
endif()

if( EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake" )
	message( "Using conan settings: ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake" )
	include( ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake )
	conan_basic_setup()
elseif( DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE )
	message( "Using vcpkg settings: $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" )
	set( CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "" )
endif()

set( CMAKE_CXX_STANDARD 17 )
if(APPLE)
	set(CMAKE_OSX_DEPLOYMENT_TARGET 10.14)
endif()

add_compile_definitions( _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING )
add_compile_definitions( QT_DEPRECATED_WARNINGS QT_DISABLE_DEPRECATED_BEFORE=0x060000 )

if( WIN32 )
	add_compile_definitions( _WIN32_WINNT=0x0A00 )
	add_compile_definitions( UNICODE )
	add_compile_definitions( _UNICODE )
endif()

function(PROTOBUF_GENERATE_GRPC_CPP SRCS HDRS)
	if(NOT ARGN)
		message(SEND_ERROR "Error: PROTOBUF_GENERATE_GRPC_CPP() called without any proto files")
		return()
	endif()

	if(PROTOBUF_GENERATE_CPP_APPEND_PATH) # This variable is common for all types of output.
		# Create an include path for each file specified
		foreach(FIL ${ARGN})
			get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
			get_filename_component(ABS_PATH ${ABS_FIL} PATH)
			list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
			if(${_contains_already} EQUAL -1)
				list(APPEND _protobuf_include_path -I ${ABS_PATH})
			endif()
		endforeach()
	else()
		set(_protobuf_include_path -I ${CMAKE_CURRENT_SOURCE_DIR})
	endif()

	if(DEFINED PROTOBUF_IMPORT_DIRS)
		foreach(DIR ${Protobuf_IMPORT_DIRS})
			get_filename_component(ABS_PATH ${DIR} ABSOLUTE)
			list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
			if(${_contains_already} EQUAL -1)
				list(APPEND _protobuf_include_path -I ${ABS_PATH})
			endif()
		endforeach()
	endif()

	set(${SRCS})
	set(${HDRS})
	foreach(FIL ${ARGN})
		get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
		get_filename_component(FIL_WE ${FIL} NAME_WE)

		list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.cc")
		list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.h")

		add_custom_command(
			OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.cc" "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.h"
			COMMAND ${Protobuf_PROTOC_EXECUTABLE}
			ARGS --grpc_out=${CMAKE_CURRENT_BINARY_DIR} --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN} -I${PROTOBUF_IMPORT_DIRS} ${_protobuf_include_path} ${ABS_FIL}
			DEPENDS ${ABS_FIL} ${Protobuf_PROTOC_EXECUTABLE}
			COMMENT "Running gRPC C++ protocol buffer compiler on ${FIL}"
			VERBATIM
		)
	endforeach()

	set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
	set(${SRCS} ${${SRCS}} PARENT_SCOPE)
	set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()
