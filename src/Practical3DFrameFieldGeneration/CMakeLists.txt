add_lib(${PROJECT_NAME}_FRAMEFIELD)

target_include_directories(${PROJECT_NAME}_FRAMEFIELD
        PRIVATE
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/deps"
)

# configure sources
file(GLOB FRAMEFIELD_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# add sources to the library
target_sources(${PROJECT_NAME}_FRAMEFIELD
        PRIVATE
        ${FRAMEFIELD_SOURCE}
)