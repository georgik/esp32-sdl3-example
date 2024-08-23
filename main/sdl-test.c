#include <stdio.h>
#include "SDL3/SDL.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Timer callback function
Uint32 TimerCallback(Uint32 interval, void *param)
{
    printf("Timer callback executed!\n");
    return interval; // Return the interval to keep the timer running
}

void app_main(void)
{
    printf("SDL3 on ESP32\n");

    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
    } else {
        printf("SDL initialized successfully\n");

        // Create a window
        SDL_Window *window = SDL_CreateWindow("SDL on ESP32", 100, 100, 0);
        if (!window) {
            printf("Failed to create window: %s\n", SDL_GetError());
            return;
        }

        // Create a renderer
        SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
        if (!renderer) {
            printf("Failed to create renderer: %s\n", SDL_GetError());
            SDL_DestroyWindow(window);
            return;
        }

        // Set the draw color to blue
        SDL_SetRenderDrawColor(renderer, 88, 66, 255, 255);

        // Clear the screen with the draw color
        SDL_RenderClear(renderer);

        // Set the draw color to green for the rectangle
        SDL_SetRenderDrawColor(renderer, 0, 66, 0, 255);

        // Create and draw a rectangle
        SDL_Rect rect = {10, 20, 8, 6}; // x, y, width, height
        SDL_RenderFillRect(renderer, &rect);

        // Present the rendered content to the screen
        SDL_RenderPresent(renderer);

        // Set the draw color to green for the rectangle
        SDL_SetRenderDrawColor(renderer, 77, 12, 0, 255);

        // Create and draw a rectangle
        SDL_Rect rect2 = {4, 40, 10, 10}; // x, y, width, height
        SDL_RenderFillRect(renderer, &rect2);

        // Present the rendered content to the screen
        SDL_RenderPresent(renderer);


        // Create a repeating timer with a 1-second interval (1000 milliseconds)
        SDL_TimerID timer_id = SDL_AddTimer(200, TimerCallback, NULL);
        if (timer_id == 0) {
            printf("Failed to create timer: %s\n", SDL_GetError());
        } else {
            printf("Timer created successfully\n");
        }

        // Main loop
        while (1) {
            vTaskDelay(pdMS_TO_TICKS(1000)); // Delay 1 second to keep the watchdog happy
        }

        // Cleanup
        // SDL_DestroyRenderer(renderer);
        // SDL_DestroyWindow(window);
        // SDL_Quit();
    }
}
