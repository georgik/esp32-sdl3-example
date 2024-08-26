#include <stdio.h>
#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Timer callback function
Uint64 TimerCallback(void *param, Uint64 interval)
{
    printf("Timer callback executed!\n");
    return interval; // Return the interval to keep the timer running
}

void LoadAndDisplayImage(SDL_Renderer *renderer, const char *imagePath)
{
    // Load the image into a surface
    SDL_Surface *imageSurface = SDL_LoadBMP(imagePath);
    if (!imageSurface) {
        printf("Failed to load image: %s\n", SDL_GetError());
        return;
    }

    // Convert the surface to a texture
    SDL_Texture *imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_DestroySurface(imageSurface); // We no longer need the surface
    if (!imageTexture) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        return;
    }

    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 88, 66, 255, 255);
    SDL_RenderClear(renderer);

    // Copy the texture to the renderer
    SDL_RenderTexture(renderer, imageTexture, NULL, NULL);

    // Present the rendered content to the screen
    SDL_RenderPresent(renderer);

    // Clean up
    SDL_DestroyTexture(imageTexture);
}


void app_main(void)
{
    printf("SDL3 on ESP32\n");

    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return;
    }
    printf("SDL initialized successfully\n");

    SDL_Window *window = SDL_CreateWindow("SDL on ESP32", 320, 120, 0);
    if (!window) {
        printf("Failed to create window: %s\n", SDL_GetError());
        return;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        return;
    }

    // Create a repeating timer with a 1-second interval
    SDL_TimerID timer_id = SDL_AddTimer(1000, TimerCallback, NULL);
    if (timer_id == 0) {
        printf("Failed to create timer: %s\n", SDL_GetError());
    } else {
        printf("Timer created successfully\n");
    }

    // Initial position of the rectangle
    float rect_x = 10.0f;
    float rect_y = 20.0f;
    float rect_w = 50.0f;
    float rect_h = 30.0f;

    // Speed of movement
    float speed = 2.0f;
    int direction = 1; // 1 for right, -1 for left

    // Splash screen
    LoadAndDisplayImage(renderer, "image.bmp");
    vTaskDelay(pdMS_TO_TICKS(1000)); // Approximately 60 frames per second

    while (1) {
        // Move the rectangle
        rect_x += speed * direction;

        // Reverse direction if the rectangle hits the window boundaries
        if (rect_x <= 0 || rect_x + rect_w >= 320) {
            direction *= -1;
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 88, 66, 255, 255);
        SDL_RenderClear(renderer);

        // Draw the moving rectangle
        SDL_SetRenderDrawColor(renderer, 0, 66, 0, 255);
        SDL_FRect rect = {rect_x, rect_y, rect_w, rect_h};
        SDL_RenderFillRect(renderer, &rect);

        // Present the rendered content to the screen
        SDL_RenderPresent(renderer);

        // Delay to control the frame rate
        vTaskDelay(pdMS_TO_TICKS(16)); // Approximately 60 frames per second
    }

    // Cleanup
    // SDL_DestroyRenderer(renderer);
    // SDL_DestroyWindow(window);
    // SDL_Quit();
}
