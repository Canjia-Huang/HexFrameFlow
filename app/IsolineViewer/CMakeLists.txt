add_executable(ISOLINE_VIEWER
        main.cpp
)

target_sources(ISOLINE_VIEWER
        PRIVATE
        ExtractIsolines.cpp
)

target_include_directories(ISOLINE_VIEWER
        PRIVATE
        "${CMAKE_SOURCE_DIR}/deps"
        "${CMAKE_SOURCE_DIR}/src/CubeCover"
)

target_link_libraries(ISOLINE_VIEWER
        PRIVATE
        ${PROJECT_NAME}_CubeCover
        polyscope
)