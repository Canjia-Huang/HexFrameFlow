add_executable(hexex
        main.cpp
)

target_include_directories(hexex
        PRIVATE
        "${CMAKE_SOURCE_DIR}/deps"
        "${CMAKE_SOURCE_DIR}/src/libHexEx"
        "${CMAKE_SOURCE_DIR}/src"
)

target_link_libraries(hexex
        PRIVATE
        ${PROJECT_NAME}_LIBHEXEX
        OpenVolumeMesh::OpenVolumeMesh
#        ${OPENVOLUMEMESH_LIBRARY}
)