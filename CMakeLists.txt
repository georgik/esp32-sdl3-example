cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(sdl-test)

get_filename_component(configName "${CMAKE_BINARY_DIR}" NAME)
list(APPEND EXTRA_COMPONENT_DIRS "${CMAKE_SOURCE_DIR}/components/esp_littlefs")
littlefs_create_partition_image(assets assets FLASH_IN_PROJECT)

# Include the path to the prebuilt SDL3 library
#set(SDL3_LIB_PATH ${CMAKE_SOURCE_DIR}/lib/libSDL3.a)

# Add an alias for the library to make linking easier
#add_library(SDL3 STATIC IMPORTED)
#set_target_properties(SDL3 PROPERTIES IMPORTED_LOCATION ${SDL3_LIB_PATH})

# Link the SDL3 library to the project
#target_link_libraries(${COMPONENT_TARGET} PRIVATE SDL3)

