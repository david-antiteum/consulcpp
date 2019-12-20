cmake_minimum_required( VERSION 3.12 )
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project( consulcpp VERSION 0.0.1 DESCRIPTION "A C++ library that implements the Consul API" LANGUAGES CXX )

find_package( uriparser CONFIG REQUIRED )
find_package( nlohmann_json CONFIG REQUIRED )
find_package( Boost REQUIRED )
find_package( fmt CONFIG REQUIRED )
find_package( spdlog CONFIG REQUIRED )

link_libraries( uriparser::uriparser )
link_libraries( nlohmann_json::nlohmann_json )
link_libraries( fmt::fmt )
link_libraries( spdlog::spdlog_header_only )

include_directories(${Boost_INCLUDE_DIRS})

if( BUILD_SHARED_LIBS )
	add_compile_definitions( ConsulCPP_DLL )
	add_compile_definitions( ConsulCPP_EXPORTS )
	set( LIB_TYPE SHARED )
else()
	set( LIB_TYPE STATIC )
endif()

add_library( ${PROJECT_NAME} ${LIB_TYPE} 
	Consul.cpp
	Services.cpp
	Service.cpp
	Sessions.cpp
	Leader.cpp
	KV.cpp
)
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" 
)

set_target_properties( ${PROJECT_NAME} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" 
	VERSION ${PROJECT_VERSION}
	SOVERSION 0
)
add_compile_definitions( _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING )

find_program( CLANG_TIDY NAMES "clang-tidy" )
if( CLANG_TIDY )
	set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY}" -checks=-*,readability-*)
endif()

if( WIN32 )
	add_compile_definitions( _WIN32_WINNT=0x0A00 )
endif()

include_directories( . ../include )

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Config
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY ../include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ${PROJECT_NAME}Config DESTINATION share/${PROJECT_NAME}/cmake)

export(TARGETS ${PROJECT_NAME} FILE ${PROJECT_NAME}Config.cmake)
