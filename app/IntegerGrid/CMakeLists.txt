add_executable(${PROJECT_NAME}_INTEGER_GRID
        main_integergrid.cpp
)

target_include_directories(${PROJECT_NAME}_INTEGER_GRID
        PRIVATE
        "${CMAKE_SOURCE_DIR}/deps"
        "${CMAKE_SOURCE_DIR}/src/core"
)

target_link_libraries(${PROJECT_NAME}_INTEGER_GRID
        PRIVATE
        ${PROJECT_NAME}_CORE
)