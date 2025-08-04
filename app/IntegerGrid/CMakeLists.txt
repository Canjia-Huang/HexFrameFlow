add_executable(integergrid
        main.cpp
)

target_include_directories(integergrid
        PRIVATE
        "${CMAKE_SOURCE_DIR}/deps"
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/src/CubeCover"
)

target_link_libraries(integergrid
        PRIVATE
        ${PROJECT_NAME}_CUBECOVER
)