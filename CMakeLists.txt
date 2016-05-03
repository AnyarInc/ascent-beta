cmake_minimum_required(VERSION 2.8.7)
project(ascent)

set (CMAKE_CXX_STANDARD 14)

set(DEBUG_FLAGS "-std=c++14")
set(RELEASE_FLAGS "-std=c++14")

set(CMAKE_CXX_FLAGS ${RELEASE_FLAGS})
set(CMAKE_CXX_FLAGS_DEBUG ${DEBUG_FLAGS})

if(MSVC)
	# for ChaiScript
	add_definitions(/bigobj)
endif()

mark_as_advanced (CMAKE_CONFIGURATION_TYPES)
mark_as_advanced (CMAKE_INSTALL_PREFIX)

file(GLOB_RECURSE headers ascent/*.cpp ascent/*.h)
file(GLOB_RECURSE sources src/*.cpp src/*.h)
set(srcs ${headers} ${sources})

include_directories(../ascent)

# external dependencies:
include_directories(eigen)
include_directories(ChaiScript/include)

add_library(${PROJECT_NAME} STATIC ${srcs})