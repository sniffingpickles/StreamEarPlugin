cmake_minimum_required(VERSION 3.16)
project(obs-streamear VERSION 1.1.0 LANGUAGES C)

# Find OBS
find_package(libobs REQUIRED)
find_package(obs-frontend-api REQUIRED)

# Plugin source files
add_library(obs-streamear MODULE
    src/plugin-main.c
    src/streamear-output.c
    src/ring-buffer.c
    src/tcp-sender.c
)

target_include_directories(obs-streamear PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(obs-streamear PRIVATE
    OBS::libobs
    OBS::obs-frontend-api
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(obs-streamear PRIVATE ws2_32)
endif()

# Install to OBS plugin directory
if(APPLE)
    set_target_properties(obs-streamear PROPERTIES
        BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/cmake/macos-bundle.plist.in
    )
endif()

# Set output name
set_target_properties(obs-streamear PROPERTIES
    PREFIX ""
    OUTPUT_NAME "obs-streamear"
)
