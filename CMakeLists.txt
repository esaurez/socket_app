project(socket_app)

cmake_minimum_required(VERSION 3.21)

include(FindPkgConfig)
include(CMakePrintHelpers)

cmake_print_variables(CMAKE_INCLUDE_PATH CMAKE_PREFIX_PATH)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas -Wextra -Wconversion -Wcast-align -Werror -Wshadow -Weffc++")

option(COMPILE_SERVER "Compile server application" ON)
option(COMPILE_CLIENT "Compile client application	" OFF)

if (NOT COMPILE_SERVER AND NOT COMPILE_CLIENT)
	message(FATAL_ERROR "You must choose at least one application to compile")
endif()

if (COMPILE_SERVER)
	add_subdirectory(server)
endif()

if (COMPILE_CLIENT)
	add_subdirectory(client)
endif()