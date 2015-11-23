
project(voting)

cmake_minimum_required(VERSION 2.8)

# voting package version
set(VOTING_VERSION_MAJOR "0")
set(VOTING_VERSION_MINOR "0")
set(VOTING_VERSION_PATCH "1")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

# No in-source building
include(MacroEnsureOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out-of-source build.  Create a build directory and run 'cmake ${CMAKE_SOURCE_DIR} [options]'.")

include_directories(include)
file(GLOB_RECURSE voting_include include/*.h include/*.hpp)
file(GLOB_RECURSE voting_src src/voting/*.cpp)
file(GLOB voting_scripts src/*.cpp)
file(GLOB voting_R src/*.R)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/results)

# Use pkg-config to find the includes/libs for eris
find_package(PkgConfig REQUIRED)
pkg_check_modules(ERIS REQUIRED liberis)
include_directories(${ERIS_INCLUDE_DIRS})
link_directories(${ERIS_LIBRARY_DIRS})
add_definitions(${ERIS_CFLAGS_OTHER})

set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -Wall -Wextra ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -DERIS_DEBUG")

add_library(voting ${voting_src})

foreach(v_src ${voting_scripts})
    get_filename_component(v "${v_src}" NAME_WE)
    add_executable("${v}" "${v_src}")
    target_link_libraries("${v}" ${ERIS_LIBRARIES})
    target_link_libraries("${v}" voting)
endforeach()

foreach(v_r ${voting_R})
    file(COPY "${v_r}" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

add_subdirectory(doc EXCLUDE_FROM_ALL)

