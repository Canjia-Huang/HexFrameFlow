#################### MAIN ####################
add_executable(${PROJECT_NAME}_MAIN
        main.cpp
)

target_include_directories(${PROJECT_NAME}_MAIN
    PRIVATE
    "${CMAKE_SOURCE_DIR}/deps"
    "${CMAKE_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME}_MAIN
    PRIVATE
    ${PROJECT_NAME}_CORE
)

#################### MAIN - Singularity Viewer ####################
add_executable(${PROJECT_NAME}_SINGULARITY_VIEWER
        main_singularityviewer.cpp
)

target_sources(${PROJECT_NAME}_SINGULARITY_VIEWER
        PRIVATE
        FrameFieldVis.cpp
)

target_include_directories(${PROJECT_NAME}_SINGULARITY_VIEWER
        PRIVATE
        "${CMAKE_SOURCE_DIR}/deps"
        "${CMAKE_SOURCE_DIR}/src/core"
)

target_link_libraries(${PROJECT_NAME}_SINGULARITY_VIEWER
        PRIVATE
        ${PROJECT_NAME}_CORE
        polyscope
)