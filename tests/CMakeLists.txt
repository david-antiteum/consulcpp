cmake_minimum_required( VERSION 3.12...3.16 )

project( consulcpp-test )

find_package(Catch2 REQUIRED)

set( SOURCES 
	main.cpp
	services-test.cpp
	kv-test.cpp
	leader-test.cpp
)
add_executable( ${PROJECT_NAME}
	${SOURCES}
)

set_target_properties( ${PROJECT_NAME}
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" 
)

if( BUILD_SHARED_LIBS )
	add_compile_definitions( ConsulCPP_DLL )
endif()

target_link_libraries( ${PROJECT_NAME} 
	PRIVATE
		Catch2::Catch2
		consulcpp
)

if( UNIX )
	find_package( Threads REQUIRED )
	target_link_libraries( ${PROJECT_NAME}
		PRIVATE
			Threads::Threads
	)
endif()

target_include_directories( ${PROJECT_NAME}
	PUBLIC
		../include
		${CMAKE_CURRENT_BINARY_DIR}	
)

clang_format_setup( ${SOURCES} )

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# catch_discover_tests( ${PROJECT_NAME} )

add_test(
	NAME ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)
