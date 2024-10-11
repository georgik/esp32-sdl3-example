idf_component_register(
    SRCS
        "esp32-sdl3-example.c"
        "text.c"
        "filesystem.c"
        "graphics.c"
    INCLUDE_DIRS "."
    REQUIRES love esp_lcd
)
