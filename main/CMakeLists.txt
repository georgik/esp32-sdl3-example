idf_component_register(SRCS "sdl-test.c"
                    INCLUDE_DIRS "."
                    REQUIRES SDL)

# Link the application with the SDL library built for ESP-IDF
target_link_libraries(${COMPONENT_LIB} PRIVATE SDL3-static)
