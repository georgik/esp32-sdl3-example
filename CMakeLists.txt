# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Enable the SDL_ESP_IDF option
set(SDL_ESP_IDF ON CACHE BOOL "Build SDL specifically for the ESP-IDF environment")

# Ensure out-of-source build for SDL
set(EXTERNAL_SDL_DIR ${CMAKE_SOURCE_DIR}/components/SDL)
set(BUILD_SDL_DIR ${CMAKE_BINARY_DIR}/sdl_build)
file(MAKE_DIRECTORY ${BUILD_SDL_DIR})
add_subdirectory(${EXTERNAL_SDL_DIR} ${BUILD_SDL_DIR})

project(sdl-test)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
# find_package(Threads REQUIRED)

list(APPEND EXTRA_COMPONENT_DIRS "components/SDL")

