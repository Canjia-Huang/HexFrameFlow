add_executable(LIBHEXEX_CMDLINE_TOOL
        main.cpp
)

target_include_directories(LIBHEXEX_CMDLINE_TOOL
        PRIVATE
        "${CMAKE_SOURCE_DIR}/deps"
        "${CMAKE_SOURCE_DIR}/src/libHexEx"
)

target_link_libraries(LIBHEXEX_CMDLINE_TOOL
        PRIVATE
        ${PROJECT_NAME}_libHexEx
        ${OPENVOLUMEMESH_LIBRARY}
)