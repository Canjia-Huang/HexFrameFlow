add_executable(singularityviewer
        main.cpp
)

target_sources(singularityviewer
        PRIVATE
        FrameFieldVis.cpp
        ReadFancyData.cpp
)

target_include_directories(singularityviewer
        PRIVATE
        "${CMAKE_SOURCE_DIR}/deps"
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/src/CubeCover"
)

target_link_libraries(singularityviewer
        PRIVATE
        ${PROJECT_NAME}_CUBECOVER
        polyscope
)