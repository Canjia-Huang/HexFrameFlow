add_executable(SEAMLESS
        main_seamless.cpp
)

target_include_directories(SEAMLESS
        PRIVATE
        "${CMAKE_SOURCE_DIR}/deps"
        "${CMAKE_SOURCE_DIR}/src/CubeCover"
)

target_link_libraries(SEAMLESS
        PRIVATE
        ${PROJECT_NAME}_CubeCover
)