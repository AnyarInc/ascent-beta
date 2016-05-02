cmake_minimum_required(VERSION 3.0)
project(ascent)

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