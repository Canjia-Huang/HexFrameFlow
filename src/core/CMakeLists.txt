add_lib(${PROJECT_NAME}_CORE)

target_include_directories(${PROJECT_NAME}_CORE
        PRIVATE
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/deps"
)

# configure sources
file(GLOB CORE_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# add sources to the library
target_sources(${PROJECT_NAME}_CORE
        PRIVATE
        ${CORE_SOURCE}
)

if(USE_COMISO)
    target_link_libraries(${PROJECT_NAME}_CORE igl_copyleft::comiso)
    target_compile_definitions(${PROJECT_NAME}_CORE PUBLIC HAS_COMISO)
endif()
if(USE_GUROBI)
    target_link_libraries(${PROJECT_NAME}_CORE ${GUROBI_CXX_LIBRARY} ${GUROBI_LIBRARY} )
    target_compile_definitions(${PROJECT_NAME}_CORE PUBLIC HAS_GUROBI)
endif()

target_link_libraries(${PROJECT_NAME}_CORE igl::core)
