cmake_minimum_required( VERSION 3.12 )
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project( leader )

find_package(Protobuf REQUIRED)

set(PROTOBUF_IMPORT_DIRS ${Protobuf_INCLUDE_DIRS})
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
find_package(gRPC REQUIRED)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin) 

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
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.cc"
             "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.h"
      COMMAND  ${Protobuf_PROTOC_EXECUTABLE}
      ARGS --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
           --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
		   -I${PROTOBUF_IMPORT_DIRS}
           ${_protobuf_include_path} ${ABS_FIL}
      DEPENDS ${ABS_FIL} ${Protobuf_PROTOC_EXECUTABLE}
      COMMENT "Running gRPC C++ protocol buffer compiler on ${FIL}"
      VERBATIM)
  endforeach()

  set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()

file( GLOB ProtoFiles leader.proto health.proto )
protobuf_generate_cpp(ProtoSources ProtoHeaders ${ProtoFiles})
PROTOBUF_GENERATE_GRPC_CPP(ProtoGRPCSources ProtoGRPCHeaders ${ProtoFiles})

add_executable( ${PROJECT_NAME}
	main.cpp 
    ${ProtoSources}
	${ProtoGRPCSources}
)

set_target_properties( ${PROJECT_NAME}
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" 
)

add_compile_definitions( ConsulCPP_DLL )
add_compile_definitions( _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING )

set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17 )

include_directories( . ../../include )

target_link_libraries( ${PROJECT_NAME} 
	ConsulCPP
	protobuf::libprotobuf
	gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc_cronet
)

add_compile_definitions( _WIN32_WINNT=0x0A00 )
if( UNIX )
	find_package( Threads REQUIRED )
	target_link_libraries( ${PROJECT_NAME} 
		Threads::Threads
	)
endif()

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

##
