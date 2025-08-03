add_executable(isolineviewer
        main.cpp
)

target_sources(isolineviewer
        PRIVATE
        ExtractIsolines.cpp
)

target_include_directories(isolineviewer
        PRIVATE
        "${CMAKE_SOURCE_DIR}/deps"
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/src/CubeCover"
)

target_link_libraries(isolineviewer
        PRIVATE
        ${PROJECT_NAME}_CubeCover
        polyscope
)