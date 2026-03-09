# FindLibObs - Locate OBS Studio libobs library
# Search for headers recursively
file(GLOB_RECURSE _obs_header_search "${CMAKE_CURRENT_LIST_DIR}/../obs-sdk/**/obs-module.h")
if(_obs_header_search)
  list(GET _obs_header_search 0 _obs_header_path)
  get_filename_component(LIBOBS_INCLUDE_DIR "${_obs_header_path}" DIRECTORY)
  message(STATUS "Found OBS headers at: ${LIBOBS_INCLUDE_DIR}")
endif()

# Search for import library (.lib) first, then DLL
file(GLOB_RECURSE _obs_lib_search "${CMAKE_CURRENT_LIST_DIR}/../obs-sdk/**/obs.lib")
if(_obs_lib_search)
  list(GET _obs_lib_search 0 LIBOBS_IMPLIB)
  message(STATUS "Found OBS import lib at: ${LIBOBS_IMPLIB}")
endif()

file(GLOB_RECURSE _obs_dll_search "${CMAKE_CURRENT_LIST_DIR}/../obs-sdk/**/obs.dll")
if(_obs_dll_search)
  list(GET _obs_dll_search 0 LIBOBS_DLL)
  message(STATUS "Found OBS DLL at: ${LIBOBS_DLL}")
endif()

# Use whichever we found for the standard args check
if(LIBOBS_IMPLIB)
  set(LIBOBS_LIB "${LIBOBS_IMPLIB}")
elseif(LIBOBS_DLL)
  set(LIBOBS_LIB "${LIBOBS_DLL}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libobs DEFAULT_MSG LIBOBS_LIB LIBOBS_INCLUDE_DIR)

if(libobs_FOUND)
  if(NOT TARGET OBS::libobs)
    add_library(OBS::libobs SHARED IMPORTED)
    if(LIBOBS_IMPLIB)
      set_target_properties(OBS::libobs PROPERTIES
        IMPORTED_IMPLIB "${LIBOBS_IMPLIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${LIBOBS_INCLUDE_DIR}"
      )
    else()
      set_target_properties(OBS::libobs PROPERTIES
        IMPORTED_LOCATION "${LIBOBS_DLL}"
        IMPORTED_IMPLIB "${LIBOBS_DLL}"
        INTERFACE_INCLUDE_DIRECTORIES "${LIBOBS_INCLUDE_DIR}"
      )
    endif()
    if(LIBOBS_DLL)
      set_target_properties(OBS::libobs PROPERTIES
        IMPORTED_LOCATION "${LIBOBS_DLL}"
      )
    endif()
  endif()
endif()
