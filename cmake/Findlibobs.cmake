# FindLibObs - Locate OBS Studio libobs library
find_path(LIBOBS_INCLUDE_DIR
  NAMES obs-module.h
  PATHS
    ${CMAKE_CURRENT_LIST_DIR}/../obs-sdk
  PATH_SUFFIXES
    include
    libobs
    include/obs
)

find_library(LIBOBS_LIB
  NAMES obs libobs
  PATHS
    ${CMAKE_CURRENT_LIST_DIR}/../obs-sdk
  PATH_SUFFIXES
    lib
    lib64
    bin
    bin/64bit
)

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
