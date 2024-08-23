#include <stdio.h>
#include "SDL3/SDL.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Timer callback function
Uint64 TimerCallback(void *param, Uint64 interval)
{
    printf("Timer callback executed!\n");
    return interval; // Return the interval to keep the timer running
}

void DrawCircle(SDL_Renderer *renderer, int x0, int y0, int radius)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        SDL_RenderPoint(renderer, x0 + x, y0 + y);
        SDL_RenderPoint(renderer, x0 + y, y0 + x);
        SDL_RenderPoint(renderer, x0 - y, y0 + x);
        SDL_RenderPoint(renderer, x0 - x, y0 + y);
        SDL_RenderPoint(renderer, x0 - x, y0 - y);
        SDL_RenderPoint(renderer, x0 - y, y0 - x);
        SDL_RenderPoint(renderer, x0 + y, y0 - x);
        SDL_RenderPoint(renderer, x0 + x, y0 - y);

        if (err <= 0)
        {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2*x + 1;
        }
    }
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
        SDL_RenderClear(renderer);

        // Draw a rectangle
        SDL_SetRenderDrawColor(renderer, 0, 66, 0, 255);
        SDL_FRect rect = {10.0f, 20.0f, 50.0f, 30.0f};
        SDL_RenderFillRect(renderer, &rect);

        // Draw a point
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderPoint(renderer, 80, 50);

        // Draw a line
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderLine(renderer, 10, 50, 20, 60);

        // Draw a circle
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        DrawCircle(renderer, 50, 50, 20);

        // Present the rendered content to the screen
        SDL_RenderPresent(renderer);

        // Create a repeating timer with a 1-second interval
        SDL_TimerID timer_id = SDL_AddTimer(1000, TimerCallback, NULL);
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
