cmake_minimum_required( VERSION 3.12 )
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project( leader )
add_executable( ${PROJECT_NAME} main.cpp )
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" 
)

add_compile_definitions( ConsulCPP_DLL )

set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17 )

include_directories( . ../../include )

target_link_libraries( ${PROJECT_NAME} 
	ConsulCPP
)

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
