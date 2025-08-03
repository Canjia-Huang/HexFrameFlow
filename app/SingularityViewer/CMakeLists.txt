add_executable(SINGULARITY_VIEWER
        main.cpp
)

target_sources(SINGULARITY_VIEWER
    PRIVATE
    FrameFieldVis.cpp
)

target_include_directories(SINGULARITY_VIEWER
    PRIVATE
    "${CMAKE_SOURCE_DIR}/deps"
    "${CMAKE_SOURCE_DIR}/src/CubeCover"
)

target_link_libraries(SINGULARITY_VIEWER
    PRIVATE
    ${PROJECT_NAME}_CubeCover
    polyscope
)