add_lib(${PROJECT_NAME}_LIBHEXEX)

target_include_directories(${PROJECT_NAME}_LIBHEXEX
        PRIVATE
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/deps"
)

# configure sources
file(GLOB LIBHEXEX_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/*.c
)

# add sources to the library
target_sources(${PROJECT_NAME}_LIBHEXEX
        PRIVATE
        ${LIBHEXEX_SOURCE}
)