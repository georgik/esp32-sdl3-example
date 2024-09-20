cmake_minimum_required(VERSION 3.16)

# Select board configuration based on -DBUILD_BOARD
if(NOT DEFINED BUILD_BOARD)
    message(FATAL_ERROR "BUILD_BOARD CMake variable is not set. Use idf.py @boards/BOARD.cfg reconfigure")
endif()

set(ENV{BUILD_BOARD} ${BUILD_BOARD})

set(COMPONENTS
    main
    ${BUILD_BOARD}
    georgik__sdl
) # "Trim" the build. Include the minimal set of components; main and anything it depends on.

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(esp32-sdl3-example)

get_filename_component(configName "${CMAKE_BINARY_DIR}" NAME)
list(APPEND EXTRA_COMPONENT_DIRS "${CMAKE_SOURCE_DIR}/components/esp_littlefs")
littlefs_create_partition_image(assets assets FLASH_IN_PROJECT)
