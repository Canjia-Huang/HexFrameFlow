add_executable(VOLUME_PARAM_VIEWER
        main.cpp
)

target_sources(VOLUME_PARAM_VIEWER
        PRIVATE
        ExtractIsolines.cpp
)

target_include_directories(VOLUME_PARAM_VIEWER
        PRIVATE
        "${CMAKE_SOURCE_DIR}/deps"
        "${CMAKE_SOURCE_DIR}/src/CubeCover"
)

target_link_libraries(VOLUME_PARAM_VIEWER
        PRIVATE
        ${PROJECT_NAME}_CubeCover
        polyscope
)