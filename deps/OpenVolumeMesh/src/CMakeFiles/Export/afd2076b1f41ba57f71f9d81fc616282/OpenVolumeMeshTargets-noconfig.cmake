#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "OpenVolumeMesh::OpenVolumeMesh" for configuration ""
set_property(TARGET OpenVolumeMesh::OpenVolumeMesh APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(OpenVolumeMesh::OpenVolumeMesh PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libOpenVolumeMesh.a"
  )

list(APPEND _cmake_import_check_targets OpenVolumeMesh::OpenVolumeMesh )
list(APPEND _cmake_import_check_files_for_OpenVolumeMesh::OpenVolumeMesh "${_IMPORT_PREFIX}/lib/libOpenVolumeMesh.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
