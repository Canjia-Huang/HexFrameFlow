cmake_minimum_required(VERSION 3.30)
project(HexExtraction)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

IF(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
ENDIF()
if(APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
    add_compile_options(-O2 -DNDEBUG)
endif()

add_definitions(-D_USE_MATH_DEFINES)

# CMake module path
list(PREPEND CMAKE_MODULE_PATH
        ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(add_lib)

# set cmake output dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 3rdParty
set(USE_GUROBI OFF CACHE BOOL "Use Gurobi for solving MIPs")
set(USE_COMISO ON CACHE BOOL "Use Comiso for solving MIPs")
include(Dependencies)

add_subdirectory(src)
add_subdirectory(app)