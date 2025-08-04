add_executable(volumeparamviewer
        main.cpp
)

target_sources(volumeparamviewer
        PRIVATE
        ExtractIsolines.cpp
)

target_include_directories(volumeparamviewer
        PRIVATE
        "${CMAKE_SOURCE_DIR}/deps"
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/src/CubeCover"
)

target_link_libraries(volumeparamviewer
        PRIVATE
        ${PROJECT_NAME}_CUBECOVER
        polyscope
)