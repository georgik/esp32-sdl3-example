#ifndef SDL_espidfframebuffer_h_
#define SDL_espidfframebuffer_h_

#include "SDL_internal.h"

extern int SDL_ESPIDF_CreateWindowFramebuffer(SDL_VideoDevice *_this, SDL_Window *window, SDL_PixelFormat *format, void **pixels, int *pitch);
extern int SDL_ESPIDF_UpdateWindowFramebuffer(SDL_VideoDevice *_this, SDL_Window *window, const SDL_Rect *rects, int numrects);
extern void SDL_ESPIDF_DestroyWindowFramebuffer(SDL_VideoDevice *_this, SDL_Window *window);

#endif
