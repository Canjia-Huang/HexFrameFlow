add_executable(framefield
        main.cpp
)

target_include_directories(framefield
        PRIVATE
        "${CMAKE_SOURCE_DIR}/deps"
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/src/Practical3DFrameFieldGeneration"
)

target_link_libraries(framefield
        PRIVATE
        ${PROJECT_NAME}_FRAMEFIELD
        ${PROJECT_NAME}_CUBECOVER
)