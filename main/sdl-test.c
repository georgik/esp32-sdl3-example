#include <stdio.h>
#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_vfs.h"
#include "esp_littlefs.h"

// Timer callback function
Uint64 TimerCallback(void *param, Uint64 interval)
{
    // printf("Timer callback executed!\n");
    return interval; // Return the interval to keep the timer running
}

SDL_Texture *LoadBackgroundImage(SDL_Renderer *renderer, const char *imagePath)
{
    // Load the image into a surface
    SDL_Surface *imageSurface = SDL_LoadBMP(imagePath);
    if (!imageSurface) {
        printf("Failed to load image: %s\n", SDL_GetError());
        return NULL;
    }

    // Convert the surface to a texture
    SDL_Texture *imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_DestroySurface(imageSurface); // We no longer need the surface
    if (!imageTexture) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        return NULL;
    }
    return imageTexture;
}

// Function to list files in a directory
void listFiles(const char *dirname) {
    DIR *dir;
    struct dirent *entry;

    // Open the directory
    dir = opendir(dirname);
    if (!dir) {
        printf("Failed to open directory: %s\n", dirname);
        return;
    }

    // Read directory entries
    while ((entry = readdir(dir)) != NULL) {
        struct stat entry_stat;
        char path[1024];

        // Build full path for stat
        snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

        // Get entry status
        if (stat(path, &entry_stat) == -1) {
            printf("Failed to stat %s\n", path);
            continue;
        }

        // Check if it's a directory or a file
        if (S_ISDIR(entry_stat.st_mode)) {
            printf("[DIR]  %s\n", entry->d_name);
        } else if (S_ISREG(entry_stat.st_mode)) {
            printf("[FILE] %s (Size: %ld bytes)\n", entry->d_name, entry_stat.st_size);
        }
    }

    // Close the directory
    closedir(dir);
}

void SDL_InitFS(void) {
    printf("Initialising File System\n");

    // Define the LittleFS configuration
    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/assets",
        .partition_label = "assets",
        .format_if_mount_failed = false,
        .dont_mount = false,
    };

    // Use the API to mount and possibly format the file system
    esp_err_t err = esp_vfs_littlefs_register(&conf);
    if (err != ESP_OK) {
        printf("Failed to mount or format filesystem\n");
    } else {
        printf("Filesystem mounted\n");
        printf("Listing files in /assets:\n");
        listFiles("/assets");
    }
}

void TestFileOpen(const char *file)
{
    // Attempt to open the file in binary read mode
    SDL_IOStream *rw = SDL_IOFromFile(file, "rb");
    if (rw == NULL) {
        printf("Failed to open file: %s\n", SDL_GetError());
    } else {
        printf("File opened successfully.\n");
        // TODO Close stream
    }
}

void DrawColoredRect(SDL_Renderer *renderer, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, int index)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_FRect rect = {x, y + index * h, w, h};
    SDL_RenderFillRect(renderer, &rect);
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

    // Clear the screen
    printf("Clear screen\n");
    SDL_SetRenderDrawColor(renderer, 88, 66, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_InitFS();

    TestFileOpen("/assets/espressif.bmp");

    if (TTF_Init() == -1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        return;
    }

    TTF_Font *font = TTF_OpenFont("/assets/FreeSans.ttf", 12);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
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
    float rect_h = 10.0f;

    // Speed of movement
    float speed = 2.0f;
    int direction = 1; // 1 for right, -1 for left

    SDL_Color color = {255, 255, 255, 255};  // White color
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Hello ESP32", color);
    if (!textSurface) {
        printf("Failed to render text: %s\n", TTF_GetError());
        TTF_CloseFont(font);
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_DestroySurface(textSurface);
    if (!textTexture) {
        printf("Failed to create texture from surface: %s\n", SDL_GetError());
        TTF_CloseFont(font);
        return;
    }

    SDL_Rect Message_rect;
    Message_rect.x = 0;
    Message_rect.y = 0;
    Message_rect.w = 100;
    Message_rect.h = 100;

    printf("Rendering text message\n");
    SDL_RenderTexture(renderer, textTexture, NULL, &Message_rect);

    SDL_RenderPresent(renderer);

    printf("Loading image from file\n");
    SDL_Texture *imageTexture = LoadBackgroundImage(renderer, "/assets/espressif.bmp");

    printf("Main loop\n");
    while (1) {
        // Move the rectangle
        rect_x += speed * direction;

        // Reverse direction if the rectangle hits the window boundaries
        if (rect_x <= 0 || rect_x + rect_w >= 320) {
            direction *= -1;
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Copy background image to whole background
        SDL_Rect destRect;
        destRect.x = 2.0f;  // X position on the screen
        destRect.y = 2.0f;  // Y position on the screen
        destRect.w = 20.0f;  // Width of the image
        destRect.h = 20.0f;  // Height of the image

        SDL_RenderTexture(renderer, imageTexture, NULL, NULL);

        // Draw the moving rectangle
        DrawColoredRect(renderer, rect_x, 20, 50, 10, 0, 0, 0, 0);     // Black
        DrawColoredRect(renderer, rect_x, 20, 50, 10, 255, 0, 0, 1);   // Red
        DrawColoredRect(renderer, rect_x, 20, 50, 10, 255, 165, 0, 2); // Orange
        DrawColoredRect(renderer, rect_x, 20, 50, 10, 255, 255, 0, 3); // Yellow
        DrawColoredRect(renderer, rect_x, 20, 50, 10, 0, 255, 0, 4);   // Green
        DrawColoredRect(renderer, rect_x, 20, 50, 10, 0, 0, 255, 5);   // Blue
        DrawColoredRect(renderer, rect_x, 20, 50, 10, 75, 0, 130, 6);  // Indigo
        DrawColoredRect(renderer, rect_x, 20, 50, 10, 238, 130, 238, 7); // Violet
        DrawColoredRect(renderer, rect_x, 20, 50, 10, 255, 255, 255, 8); // White

        SDL_RenderTexture(renderer, textTexture, NULL, NULL);

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
