#include "graphics.h"
#include <stdio.h>

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

void clear_screen(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 88, 66, 255, 255);
    SDL_RenderClear(renderer);
}

void draw_image(SDL_Renderer *renderer, SDL_Texture *texture, float x, float y, float w, float h) {
    SDL_FRect destRect = {x, y, w, h};
    SDL_RenderTexture(renderer, texture, NULL, &destRect);
}

void draw_moving_rectangles(SDL_Renderer *renderer, float rect_x) {
    DrawColoredRect(renderer, rect_x, 20, 50, 10, 0, 0, 0, 0);
    DrawColoredRect(renderer, rect_x, 20, 50, 10, 255, 0, 0, 1);
    DrawColoredRect(renderer, rect_x, 20, 50, 10, 255, 165, 0, 2);
    DrawColoredRect(renderer, rect_x, 20, 50, 10, 255, 255, 0, 3);
    DrawColoredRect(renderer, rect_x, 20, 50, 10, 0, 255, 0, 4);
    DrawColoredRect(renderer, rect_x, 20, 50, 10, 0, 0, 255, 5);
    DrawColoredRect(renderer, rect_x, 20, 50, 10, 75, 0, 130, 6);
    DrawColoredRect(renderer, rect_x, 20, 50, 10, 238, 130, 238, 7);
    DrawColoredRect(renderer, rect_x, 20, 50, 10, 255, 255, 255, 8);
}

void DrawColoredRect(SDL_Renderer *renderer, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, int index) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_FRect rect = {x, y + index * h, w, h};
    SDL_RenderFillRect(renderer, &rect);
}
