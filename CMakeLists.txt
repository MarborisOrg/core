cmake_minimum_required(VERSION 3.20)

project(core C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Set the build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Current source directory: ${CMAKE_CURRENT_SOURCE_DIR}")

# Check if VCPKG_ROOT is defined
if(DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
    message(STATUS "Using VCPKG_ROOT from environment: ${VCPKG_ROOT}")
else()
    message(FATAL_ERROR "VCPKG_ROOT is not defined in the environment.")
endif()

# Include vcpkg build system
include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

# Create the executable from src/core.cc
add_executable(core core/core.c)

find_package(mongoc-1.0 CONFIG REQUIRED)
target_link_libraries(core PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongoc_shared>,mongo::mongoc_shared,mongo::mongoc_static>)
