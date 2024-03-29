cmake_minimum_required( VERSION 3.12...3.16 )

find_package( Boost REQUIRED )
find_package( uriparser CONFIG REQUIRED )
find_package( nlohmann_json CONFIG REQUIRED )
find_package( fmt CONFIG REQUIRED )
find_package( spdlog CONFIG REQUIRED )
find_package( Microsoft.GSL CONFIG REQUIRED )
find_package( tl-expected CONFIG REQUIRED )

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
	Catalog.cpp
	Sessions.cpp
	Leader.cpp
	KV.cpp
	Utils.cpp
)
target_link_libraries( ${PROJECT_NAME} 
	PUBLIC
		nlohmann_json::nlohmann_json
	PRIVATE
		Boost::headers
		uriparser::uriparser
		fmt::fmt
		spdlog::spdlog_header_only
		Microsoft.GSL::GSL
		tl::expected
)

set_target_properties( ${PROJECT_NAME} 
	PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" 
		LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" 
		VERSION ${PROJECT_VERSION}
		SOVERSION 0
)

include_directories( ../include )

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME} 
	EXPORT ${PROJECT_NAME}Targets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY ../include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ${PROJECT_NAME}Targets
	FILE ${PROJECT_NAME}Targets.cmake
	DESTINATION share/${PROJECT_NAME}/cmake
)

install( FILES "${PROJECT_NAME}Config.cmake"
	DESTINATION share/${PROJECT_NAME}/cmake
)

export(TARGETS ${PROJECT_NAME} FILE ${PROJECT_NAME}Config.cmake)

target_clang_format_setup( ${PROJECT_NAME} )
