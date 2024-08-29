#include "text.h"
#include <stdio.h>
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

TTF_Font* initialize_font(const char *fontPath, int fontSize) {
    if (TTF_Init() == -1) {
        printf("TTF_Init: %s\n", SDL_GetError());
        return NULL;
    }

    TTF_Font *font = TTF_OpenFont(fontPath, fontSize);
    if (!font) {
        printf("Failed to load font: %s\n", SDL_GetError());
    }
    return font;
}

SDL_Texture* render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color) {
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color);
    if (!textSurface) {
        printf("Failed to render text: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_DestroySurface(textSurface);
    if (!textTexture) {
        printf("Failed to create texture from surface: %s\n", SDL_GetError());
    }

    return textTexture;
}

void draw_text(SDL_Renderer *renderer, SDL_Texture *texture, float x, float y, float w, float h) {
    SDL_FRect Message_rect = {x, y, w, h};
    SDL_RenderTexture(renderer, texture, NULL, &Message_rect);
}
