#   Copyright 2017 James Fong
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

##### PEPPERGRAINS ENGINE CMAKE CONFIG #####

### Basic CMake project and build information ###

# CMake minimum version
# 3.7.2 is the first version that FindVulkan is supported
cmake_minimum_required(VERSION 3.7.2)

# Project name/version
project(PEPPERGRAINS VERSION 0.1.0)
message(STATUS "Peppergrains Engine version ${PEPPERGRAINS_VERSION}")

# Where to find modules
list(APPEND CMAKE_MODULE_PATH "${PEPPERGRAINS_SOURCE_DIR}/cmake")

### Variables starting with "PGLOCAL_" are not CMake built-ins

# Where the .cpp files reside
set(PGLOCAL_SOURCE_DIR "${PEPPERGRAINS_SOURCE_DIR}/src")

# Where the header files reside
set(PGLOCAL_INCLUDE_DIRS ${PGLOCAL_SOURCE_DIR})

# Name of target executable
set(PGLOCAL_ENGINE_TARGET "Pegr")

### User options ###

### Build target configuration ###

# Load the sources list from a file as PGLOCAL_SOURCES_LIST
include("EngineSrcList")

set(PGLOCAL_ALL_REQUIRED_READY TRUE)
message(STATUS "Package status: ")

## Header-Only / Compiled Third-party ##

# STB #
message(STATUS "STB ==================")
find_package(STB)
if(STB_FOUND)
    message(STATUS "\tInclude Dirs: " ${STB_INCLUDE_DIR})
    list(APPEND PGLOCAL_INCLUDE_DIRS ${STB_INCLUDE_DIR})
else()
    message("\tNOT FOUND")
    set(PGLOCAL_ALL_REQUIRED_READY FALSE)
endif()

# JsonCpp #
message(STATUS "JsonCpp ==============")
find_package(JsonCpp)
if(JSONCPP_FOUND)
    message(STATUS "\tInclude Dirs: " ${JSONCPP_INCLUDE_DIR})
    message(STATUS "\tSource Files: " ${JSONCPP_SOURCE_FILES})
    list(APPEND PGLOCAL_INCLUDE_DIRS ${JSONCPP_INCLUDE_DIR})
    list(APPEND PGLOCAL_SOURCES_LIST ${JSONCPP_SOURCE_FILES})
else()
    message("\tNOT FOUND")
    set(PGLOCAL_ALL_REQUIRED_READY FALSE)
endif()

# Add a new build target
add_executable(${PGLOCAL_ENGINE_TARGET} ${PGLOCAL_SOURCES_LIST})

# Add required features
set_property(TARGET ${PGLOCAL_ENGINE_TARGET} PROPERTY CXX_STANDARD 11)

## Linked Third-party ##

# Lua #
message(STATUS "Lua 5.3 ==============")
find_package(Lua)
if(LUA_FOUND)
    if(LUA_VERSION_MAJOR EQUAL 5 AND NOT LUA_VERSION_MINOR LESS 3)
        message(STATUS "\tFOUND VERSION: " ${LUA_VERSION_STRING})
        message(STATUS "\tInclude Dirs: " ${LUA_INCLUDE_DIR})
        message(STATUS "\tLibraries: " ${LUA_LIBRARY})
        list(APPEND PGLOCAL_INCLUDE_DIRS ${LUA_INCLUDE_DIR})
        target_link_libraries(${PGLOCAL_ENGINE_TARGET} ${LUA_LIBRARY})
    else()
        message("\tINCOMPATIBLE VERSION: " ${LUA_VERSION_STRING})
        set(PGLOCAL_ALL_REQUIRED_READY FALSE)
    endif()
else()
    message("\tNOT FOUND")
    set(PGLOCAL_ALL_REQUIRED_READY FALSE)
endif()

# GLFW #
message(STATUS "GLFW 3.2 =============")
find_package(GLFW)
if(GLFW_FOUND)
    if(GLFW_VERSION_MAJOR EQUAL 3 AND NOT GLFW_VERSION_MINOR LESS 2)
        message(STATUS "\tFOUND VERSION: " ${GLFW_VERSION_STRING})
        message(STATUS "\tInclude Dirs: " ${GLFW_INCLUDE_DIR})
        message(STATUS "\tLibraries: " ${GLFW_LIBRARY})
        list(APPEND PGLOCAL_INCLUDE_DIRS ${GLFW_INCLUDE_DIR})
        target_link_libraries(${PGLOCAL_ENGINE_TARGET} ${GLFW_LIBRARY})
        add_definitions("-DPGG_GLFW")
    else()
        message("\tINCOMPATIBLE VERSION: " ${GLFW_VERSION_STRING})
        set(PGLOCAL_ALL_REQUIRED_READY FALSE)
    endif()
else()
    message("\tNOT FOUND")
    set(PGLOCAL_ALL_REQUIRED_READY FALSE)
endif()

# BOOST #
message(STATUS "Boost 1.59 ===========")
set(Boost_USE_STATIC_LIBS TRUE)
find_package(Boost COMPONENTS system filesystem)
if(Boost_FOUND)
    if(Boost_MAJOR_VERSION EQUAL 1 AND NOT Boost_MINOR_VERSION LESS 59)
        message(STATUS "\tFOUND VERSION: " ${Boost_VERSION})
        message(STATUS "\tInclude Dirs: " ${Boost_INCLUDE_DIRS})
        message(STATUS "\tLibraries: " ${Boost_LIBRARIES})
        list(APPEND PGLOCAL_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
        target_link_libraries(${PGLOCAL_ENGINE_TARGET} ${Boost_LIBRARIES})
    else()
        message("\tINCOMPATIBLE VERSION: " ${Boost_VERSION})
        set(PGLOCAL_ALL_REQUIRED_READY FALSE)
    endif()
else()
    message("\tNOT FOUND")
    set(PGLOCAL_ALL_REQUIRED_READY FALSE)
endif()

# Bullet Physics #
message(STATUS "Bullet Physics =======")
find_package(Bullet)
if(BULLET_FOUND)
    message(STATUS "\tInclude Dirs: " ${BULLET_INCLUDE_DIRS})
    message(STATUS "\tLibraries: " ${BULLET_LIBRARIES})
    list(APPEND PGLOCAL_INCLUDE_DIRS ${BULLET_INCLUDE_DIRS})
    target_link_libraries(${PGLOCAL_ENGINE_TARGET} ${BULLET_LIBRARIES})
else()
    message("\tNOT FOUND")
    set(PGLOCAL_ALL_REQUIRED_READY FALSE)
endif()

# GLM #
message(STATUS "GLM ==================")
find_package(GLM)
if(GLM_FOUND)
    if(GLM_VERSION_MAJOR EQUAL 0 AND NOT GLM_VERSION_MINOR LESS 9)
        message(STATUS "\tFOUND VERSION: " ${GLM_VERSION_STRING})
        message(STATUS "\tInclude Dirs: " ${GLM_INCLUDE_DIR})
        list(APPEND PGLOCAL_INCLUDE_DIRS ${GLM_INCLUDE_DIR})
    else()
        message("\tINCOMPATIBLE VERSION: " ${GLM_VERSION_STRING})
        set(PGLOCAL_ALL_REQUIRED_READY FALSE)
    endif()
else()
    message("\tNOT FOUND")
    set(PGLOCAL_ALL_REQUIRED_READY FALSE)
endif()

# Soundio #
message(STATUS "Soundio ==============")
find_package(Soundio)
if(SOUNDIO_FOUND)
    if(SOUNDIO_VERSION_MAJOR EQUAL 1 AND NOT SOUNDIO_VERSION_MINOR LESS 1)
        message(STATUS "\tFOUND VERSION: " ${SOUNDIO_VERSION_STRING})
        message(STATUS "\tLibraries: " ${SOUNDIO_LIBRARY})
        message(STATUS "\tInclude Dirs: " ${SOUNDIO_INCLUDE_DIR})
        list(APPEND PGLOCAL_INCLUDE_DIRS ${SOUNDIO_INCLUDE_DIR})
        target_link_libraries(${PGLOCAL_ENGINE_TARGET} ${SOUNDIO_LIBRARY})
    else()
        message("\tINCOMPATIBLE VERSION: " ${SOUNDIO_VERSION_STRING})
        set(PGLOCAL_ALL_REQUIRED_READY FALSE)
    endif()
else()
    message("\tNOT FOUND")
    set(PGLOCAL_ALL_REQUIRED_READY FALSE)
endif()

# Vulkan #
message(STATUS "Vulkan ===============")
find_package(Vulkan)
if(Vulkan_FOUND)
    message(STATUS "\tInclude Dirs: " ${Vulkan_INCLUDE_DIRS})
    message(STATUS "\tLibraries: " ${Vulkan_LIBRARIES})
    list(APPEND PGLOCAL_INCLUDE_DIRS ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(${PGLOCAL_ENGINE_TARGET} ${Vulkan_LIBRARIES})
    add_definitions("-DPGG_VULKAN")
else()
    message("\tNOT FOUND")
    set(PGLOCAL_ALL_REQUIRED_READY FALSE)
endif()

# Helpful information
if(PGLOCAL_ALL_REQUIRED_READY)
    message(STATUS "All packages found and are compatible")
else()
    message("One or more packages are missing, build will fail!")
endif()

# Setup include directories
include_directories(${PGLOCAL_INCLUDE_DIRS})
