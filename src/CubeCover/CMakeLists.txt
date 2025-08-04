add_lib(${PROJECT_NAME}_CUBECOVER)

target_include_directories(${PROJECT_NAME}_CUBECOVER
        PRIVATE
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/deps"
)

# configure sources
file(GLOB CUBECOVER_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# add sources to the library
target_sources(${PROJECT_NAME}_CUBECOVER
        PRIVATE
        ${CUBECOVER_SOURCE}
)

if(USE_COMISO)
    target_link_libraries(${PROJECT_NAME}_CUBECOVER igl_copyleft::comiso)
    target_compile_definitions(${PROJECT_NAME}_CUBECOVER PUBLIC HAS_COMISO)
endif()
if(USE_GUROBI)
    target_link_libraries(${PROJECT_NAME}_CUBECOVER ${GUROBI_CXX_LIBRARY} ${GUROBI_LIBRARY} )
    target_compile_definitions(${PROJECT_NAME}_CUBECOVER PUBLIC HAS_GUROBI)
endif()

target_link_libraries(${PROJECT_NAME}_CUBECOVER igl::core)
