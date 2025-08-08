#################### MAIN ####################
# add_executable(${PROJECT_NAME}_MAIN
#         main.cpp
# )

# target_include_directories(${PROJECT_NAME}_MAIN
#     PRIVATE
#     "${CMAKE_SOURCE_DIR}/deps"
#     "${CMAKE_SOURCE_DIR}/src"
# )

# target_link_libraries(${PROJECT_NAME}_MAIN
#         PRIVATE
#         ${PROJECT_NAME}_CubeCover
#         ${OPENVOLUMEMESH_LIBRARY}
# )

add_subdirectory(FrameFieldGeneration)
add_subdirectory(IntegerGrid)
add_subdirectory(IsolineViewer)
add_subdirectory(libHexEx)
add_subdirectory(SingularityViewer)
add_subdirectory(VolumeParamViewer)
