cmake_minimum_required( VERSION 3.12...3.16 )

project( leader )

find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin) 

set(PROTOBUF_IMPORT_DIRS ${Protobuf_INCLUDE_DIRS})

set( ProtoFiles health.proto )
protobuf_generate_cpp(ProtoSources ProtoHeaders ${ProtoFiles})
PROTOBUF_GENERATE_GRPC_CPP(ProtoGRPCSources ProtoGRPCHeaders ${ProtoFiles})

set( SOURCES 
	main.cpp
)
add_executable( ${PROJECT_NAME}
	${SOURCES}
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

target_link_libraries( ${PROJECT_NAME} 
	consulcpp
	protobuf::libprotobuf
	gRPC::gpr gRPC::grpc gRPC::grpc++
)

if( UNIX )
	find_package( Threads REQUIRED )
	target_link_libraries( ${PROJECT_NAME} 
		Threads::Threads
	)
endif()

target_include_directories( ${PROJECT_NAME}
	PUBLIC
		../../include
		${Protobuf_INCLUDE_DIRS}
		${CMAKE_CURRENT_BINARY_DIR}	
)

clang_format_setup( ${SOURCES} )

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
