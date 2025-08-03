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
        ${PROJECT_NAME}_CubeCover
        ${OPENVOLUMEMESH_LIBRARY}
)

#################### MAIN - Singularity Viewer ####################
add_subdirectory(SingularityViewer)

#################### MAIN - Integer Grid ####################
add_subdirectory(IntegerGrid)

#################### MAIN - Volume Param Viewer ####################
add_subdirectory(VolumeParamViewer)

add_subdirectory(Seamless)

add_subdirectory(IsolineViewer)

add_subdirectory(libHexEx_cmdline_tool)