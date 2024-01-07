cmake_minimum_required(VERSION 3.5)
project(unstickymem VERSION 1.0 LANGUAGES CXX C)

include(GNUInstallDirs)
include(CTest)

set(CMAKE_VERBOSE_MAKEFILE OFF)

# get list of source files
file(GLOB_RECURSE procmap_src relative ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp" "src/*c")
add_library(procmap SHARED ${procmap_src})

set_property(TARGET procmap PROPERTY POSITION_INDEPENDENT_CODE on)

target_compile_features(procmap PRIVATE cxx_std_14)

target_compile_options(procmap PRIVATE -g -Wall -pedantic -Wshadow -Wfatal-errors)

target_include_directories(procmap
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PUBLIC $<INSTALL_INTERFACE:include>
	PRIVATE src)

# 'make install' to the correct locations (provided by GNUInstallDirs)
install(TARGETS procmap
	EXPORT procmap_config
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'
install(EXPORT procmap_config DESTINATION share/procmap/cmake)

# uninstall target
if(NOT TARGET uninstall)
	configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
		IMMEDIATE @ONLY)
	add_custom_target(uninstall
		COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()


# tests/examples
add_executable(simple test/simple.cpp)
target_link_libraries(simple procmap)
add_test(simple simple)
