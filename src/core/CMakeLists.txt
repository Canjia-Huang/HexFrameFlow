add_lib(${PROJECT_NAME}_CORE)

target_include_directories(${PROJECT_NAME}_CORE
        PRIVATE
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/deps"
)

# configure sources
file(GLOB CORE_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# add sources to the library
target_sources(${PROJECT_NAME}_CORE
        PRIVATE
        ${CORE_SOURCE}
)