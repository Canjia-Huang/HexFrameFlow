############################################# Eigen3 #############################################
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIRS})

############################################# Polyscope #############################################
find_package(Polyscope REQUIRED)
include_directories(${POLYSCOPE_INCLUDE_DIR})

############################################# Libigl #############################################
option(LIBIGL_GLFW                "Build target igl::glfw"                OFF)
option(LIBIGL_PNG                 "Build target igl::png"                 OFF)
option(LIBIGL_IMGUI               "Build target igl::imgui"               OFF)
option(LIBIGL_OPENGL              "Build target igl::opengl"              OFF)
option(LIBIGL_PREDICATES          "Build target igl::predicates"          OFF)
option(LIBIGL_COPYLEFT_COMISO     "Build target igl_copyleft::comiso"     ON)
option(LIBIGL_COPYLEFT_CORE       "Build target igl_copyleft::core"       ON)
option(LIBIGL_COPYLEFT_TETGEN     "Build target igl_copyleft::tetgen"     OFF)
option(LIBIGL_COPYLEFT_CGAL       "Build target igl_copyleft::cgal"       OFF)
option(LIBIGL_RESTRICTED_TRIANGLE "Build target igl_restricted::triangle" OFF)

set(LIBIGL_WITH_COMISO $CACHE{USE_COMISO} CACHE BOOL "Build LIBIGL with CoMISo" FORCE)
include(libigl)

############################################# MIP Solvers #############################################
if(USE_GUROBI)
    # use gurobi
    find_package(Gurobi REQUIRED)
    if(NOT GUROBI_FOUND)
        message(SEND_ERROR "Unable to find Gurobi. Make sure that the env GUROBI_HOME is set")
    else()
        include_directories(${GUROBI_INCLUDE_DIR})
    endif()
endif()

############################################# OpenVolumeMesh #############################################
add_subdirectory("${CMAKE_SOURCE_DIR}/deps")
include_directories("${CMAKE_SOURCE_DIR}/deps/OpenVolumeMesh/src")

#find_package(OpenVolumeMesh REQUIRED)
#add_definitions(-DNDEBUG)
#include_directories(${OPENVOLUMEMESH_INCLUDE_DIR})
#link_directories(${OPENVOLUMEMESH_LIBRARY_DIR})
