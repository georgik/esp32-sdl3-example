#ifndef TEXT_H
#define TEXT_H

#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

TTF_Font* initialize_font(const char *fontPath, int fontSize);
SDL_Texture* render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color);
void draw_text(SDL_Renderer *renderer, SDL_Texture *texture, float x, float y, float w, float h);

#endif // TEXT_H