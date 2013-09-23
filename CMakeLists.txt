
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

# Add eris
# If we're building in "foo", look for ../eris/foo for eris build dir, then
# fall back to ../eris/build if that doesn't work.
get_filename_component(BUILD_BASENAME "${CMAKE_BINARY_DIR}" NAME)
set(ERIS_INCLUDE ../eris/include)
set(ERIS_BUILD_SAME "../eris/${BUILD_BASENAME}")
set(ERIS_BUILD_BUILD ../eris/build)
include_directories(${ERIS_INCLUDE})
find_library(
    ERIS_LIB
    NAMES eris
    HINTS "${ERIS_BUILD_SAME}" "${ERIS_BUILD_BUILD}")

add_definitions(-std=c++11 -Wall -Werror)

add_library(voting ${voting_src})

foreach(v_src ${voting_scripts})
    get_filename_component(v "${v_src}" NAME_WE)
    add_executable("${v}" "${v_src}")
    target_link_libraries("${v}" ${ERIS_LIB})
    target_link_libraries("${v}" voting)
endforeach()

foreach(v_r ${voting_R})
    file(COPY "${v_r}" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

add_subdirectory(doc EXCLUDE_FROM_ALL)

#add_library(eris SHARED ${eris_src})

#set_target_properties(eris PROPERTIES
#    VERSION "${LIBERIS_CURRENT}.${LIBERIS_AGE}.${LIBERIS_REVISION}"
#    SOVERSION "${LIBERIS_CURRENT}"
#)
#
#add_subdirectory(scratch)
#
#add_subdirectory(doc EXCLUDE_FROM_ALL)
#
#enable_testing()
#
#add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
#add_subdirectory(tests EXCLUDE_FROM_ALL)
#
#add_custom_target(examples)
#add_subdirectory(examples EXCLUDE_FROM_ALL)
