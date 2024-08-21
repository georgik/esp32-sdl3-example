#include <stdio.h>

#include "SDL3/SDL.h"
void app_main(void)
{
    printf("SDL3 on ESP32\n");
    if (SDL_Init(SDL_INIT_TIMER) != 0) {
        //SDL_Log("Could not initialize SDL: %s\n", SDL_GetError());
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
    } else {
        printf("Initialized");
    }
}
