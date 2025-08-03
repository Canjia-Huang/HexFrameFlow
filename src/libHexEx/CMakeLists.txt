add_lib(${PROJECT_NAME}_libHexEx)

target_include_directories(${PROJECT_NAME}_libHexEx
        PRIVATE
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/deps"
)

# configure sources
file(GLOB LIBHEXEX_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# add sources to the library
target_sources(${PROJECT_NAME}_libHexEx
        PRIVATE
        ${LIBHEXEX_SOURCE}
)