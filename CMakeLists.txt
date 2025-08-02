cmake_minimum_required(VERSION 3.30)
project(CubeCover)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

IF(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
ENDIF()

if(APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
    add_compile_options(-O2 -DNDEBUG)
endif()

# CMake module path
list(PREPEND CMAKE_MODULE_PATH
        ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(add_lib)

# 3rdParty
include(Dependencies)

add_subdirectory(src)
add_subdirectory(app)