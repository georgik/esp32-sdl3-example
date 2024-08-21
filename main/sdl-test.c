#include <stdio.h>

#include "SDL3/SDL.h"
void app_main(void)
{
    printf("SDL3x");
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Unable to initialize SDL");
    } else {
        printf("Initialized");
    }
}
