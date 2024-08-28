#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "SDL3/SDL.h"

void clear_screen(SDL_Renderer *renderer);
void draw_image(SDL_Renderer *renderer, SDL_Texture *texture, float x, float y, float w, float h);
void draw_moving_rectangles(SDL_Renderer *renderer, float rect_x);
void DrawColoredRect(SDL_Renderer *renderer, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, int index);
SDL_Texture *LoadBackgroundImage(SDL_Renderer *renderer, const char *imagePath);

#endif // GRAPHICS_H
