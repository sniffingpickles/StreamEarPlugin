# FindLibObs - Locate OBS Studio libobs library
# Search in obs-sdk directory (may have nested subdirectories from ZIP extraction)
file(GLOB_RECURSE _obs_header_search "${CMAKE_CURRENT_LIST_DIR}/../obs-sdk/**/obs-module.h")
if(_obs_header_search)
  list(GET _obs_header_search 0 _obs_header_path)
  get_filename_component(LIBOBS_INCLUDE_DIR "${_obs_header_path}" DIRECTORY)
  message(STATUS "Found OBS headers at: ${LIBOBS_INCLUDE_DIR}")
endif()

find_library(LIBOBS_LIB
  NAMES obs libobs
  PATHS
    ${CMAKE_CURRENT_LIST_DIR}/../obs-sdk
  PATH_SUFFIXES
    lib lib64 bin bin/64bit
    lib/x64 bin/x64
)

# If find_library didn't work, search recursively for obs.lib or obs.dll
if(NOT LIBOBS_LIB)
  file(GLOB_RECURSE _obs_lib_search "${CMAKE_CURRENT_LIST_DIR}/../obs-sdk/**/obs.lib")
  if(NOT _obs_lib_search)
    file(GLOB_RECURSE _obs_lib_search "${CMAKE_CURRENT_LIST_DIR}/../obs-sdk/**/obs.dll")
  endif()
  if(_obs_lib_search)
    list(GET _obs_lib_search 0 LIBOBS_LIB)
    message(STATUS "Found OBS library at: ${LIBOBS_LIB}")
  endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libobs DEFAULT_MSG LIBOBS_LIB LIBOBS_INCLUDE_DIR)

if(libobs_FOUND)
  if(NOT TARGET OBS::libobs)
    add_library(OBS::libobs SHARED IMPORTED)
    set_target_properties(OBS::libobs PROPERTIES
      IMPORTED_LOCATION "${LIBOBS_LIB}"
      IMPORTED_IMPLIB "${LIBOBS_LIB}"
      INTERFACE_INCLUDE_DIRECTORIES "${LIBOBS_INCLUDE_DIR}"
    )
  endif()
endif()
