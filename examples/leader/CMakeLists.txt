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

file( GLOB ProtoFiles health.proto )
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

if( BUILD_SHARED_LIBS )
	add_compile_definitions( ConsulCPP_DLL )
endif()
add_compile_definitions( _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING )

set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17 )

include_directories( . ../../include )

target_link_libraries( ${PROJECT_NAME} 
	consulcpp
	protobuf::libprotobuf
	gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc_cronet
)

if( WIN32 )
	add_compile_definitions( _WIN32_WINNT=0x0A00 )
endif()

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
