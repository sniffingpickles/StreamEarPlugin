# Findobs-frontend-api - Locate OBS Studio frontend API library

# Search for the frontend API header
file(GLOB_RECURSE _frontend_header_search "${CMAKE_CURRENT_LIST_DIR}/../obs-sdk/**/obs-frontend-api.h")
if(_frontend_header_search)
  list(GET _frontend_header_search 0 _frontend_header_path)
  get_filename_component(OBS_FRONTEND_INCLUDE_DIR "${_frontend_header_path}" DIRECTORY)
  message(STATUS "Found OBS frontend API header at: ${OBS_FRONTEND_INCLUDE_DIR}")
endif()

# Search for import library
file(GLOB_RECURSE _frontend_lib_search "${CMAKE_CURRENT_LIST_DIR}/../obs-sdk/**/obs-frontend-api.lib")
if(_frontend_lib_search)
  list(GET _frontend_lib_search 0 OBS_FRONTEND_IMPLIB)
  message(STATUS "Found OBS frontend API lib at: ${OBS_FRONTEND_IMPLIB}")
endif()

# Search for DLL
file(GLOB_RECURSE _frontend_dll_search "${CMAKE_CURRENT_LIST_DIR}/../obs-sdk/**/obs-frontend-api.dll")
if(_frontend_dll_search)
  list(GET _frontend_dll_search 0 OBS_FRONTEND_DLL)
  message(STATUS "Found OBS frontend API DLL at: ${OBS_FRONTEND_DLL}")
endif()

if(OBS_FRONTEND_IMPLIB)
  set(OBS_FRONTEND_LIB "${OBS_FRONTEND_IMPLIB}")
elseif(OBS_FRONTEND_DLL)
  set(OBS_FRONTEND_LIB "${OBS_FRONTEND_DLL}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(obs-frontend-api DEFAULT_MSG OBS_FRONTEND_LIB OBS_FRONTEND_INCLUDE_DIR)

if(obs-frontend-api_FOUND)
  if(NOT TARGET OBS::obs-frontend-api)
    add_library(OBS::obs-frontend-api SHARED IMPORTED)
    if(OBS_FRONTEND_IMPLIB)
      set_target_properties(OBS::obs-frontend-api PROPERTIES
        IMPORTED_IMPLIB "${OBS_FRONTEND_IMPLIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${OBS_FRONTEND_INCLUDE_DIR}"
      )
    else()
      set_target_properties(OBS::obs-frontend-api PROPERTIES
        IMPORTED_LOCATION "${OBS_FRONTEND_DLL}"
        IMPORTED_IMPLIB "${OBS_FRONTEND_DLL}"
        INTERFACE_INCLUDE_DIRECTORIES "${OBS_FRONTEND_INCLUDE_DIR}"
      )
    endif()
    if(OBS_FRONTEND_DLL)
      set_target_properties(OBS::obs-frontend-api PROPERTIES
        IMPORTED_LOCATION "${OBS_FRONTEND_DLL}"
      )
    endif()
  endif()
endif()
