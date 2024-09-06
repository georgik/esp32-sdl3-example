#include <stdio.h>
#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "graphics.h"
#include "filesystem.h"
#include "text.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>


Uint64 TimerCallback(void *param, Uint64 interval)
{
    // printf("Timer callback executed!\n");
    return interval; // Return the interval to keep the timer running
}

void app_main(void) {
    printf("SDL3 on ESP32\n");

    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return;
    }
    printf("SDL initialized successfully\n");

    SDL_Window *window = SDL_CreateWindow("SDL on ESP32", 320, 240, 0);
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

    clear_screen(renderer);

    SDL_InitFS();
    TestFileOpen("/assets/espressif.bmp");

    TTF_Font *font = initialize_font("/assets/FreeSans.ttf", 12);
    if (!font) return;

    // Create a repeating timer with a 1-second interval
    SDL_TimerID timer_id = SDL_AddTimer(1000, TimerCallback, NULL);
    if (timer_id == 0) {
        printf("Failed to create timer: %s\n", SDL_GetError());
    } else {
        printf("Timer created successfully\n");
    }

    SDL_Texture *textTexture = render_text(renderer, font, "Hello ESP32 - SDL3", (SDL_Color){255, 255, 255, 255});
    SDL_Texture *imageTexture = LoadBackgroundImage(renderer, "/assets/espressif.bmp");

    // Animation variables
    float rect_x = 10.0f, speed = 2.0f;
    int direction = 1;

    // Variables for the BMP position and speed
    float bmp_x = 2.0f;
    float bmp_y = 2.0f;
    float bmp_speed_x = 2.0f;
    float bmp_speed_y = 2.0f;

   // Variables for the text position, speed, and size
    float text_x = 30.0f, text_y = 40.0f;
    float text_speed_x = 1.5f, text_speed_y = 1.2f;
    float text_scale = 1.0f, text_scale_speed = 0.01f;
    int text_direction_x = 1, text_direction_y = 1;
    int scale_direction = 1;

    printf("Starting Lua\n");
    lua_State *L = luaL_newstate();
    printf("Opening Lua Libs\n");
    luaL_openlibs(L);

    printf("Calling Lua code: \n");
    lua_pushinteger(L, 42);
    lua_setglobal(L, "answer");

    char * code = "print(answer)";

    if (luaL_dostring(L, code) == LUA_OK) {
        lua_pop(L, lua_gettop(L));
    }

    printf("Closing Lua\n");
    lua_close(L);

    printf("Entering main loop...\n");

    SDL_Event event;

    while (1) {

      while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    // Handle quit event (if needed)
                    break;

                case SDL_EVENT_FINGER_UP:
                    bmp_x = (float)event.tfinger.x;
                    bmp_y = (float)event.tfinger.y;

                    printf("Finger up [%f, %f]\n", bmp_x, bmp_y);
                    break;
            }
        }

        // Move the BMP image
        bmp_x += bmp_speed_x;
        bmp_y += bmp_speed_y;

        // Check for collisions with screen edges and bounce
        if (bmp_x <= 0 || bmp_x + 32 >= 320) bmp_speed_x *= -1;
        if (bmp_y <= 0 || bmp_y + 32 >= 240) bmp_speed_y *= -1;

        // Move the rectangle and bounce it
        rect_x += speed * direction;

        if (rect_x <= 0 || rect_x + 50 >= 320) direction *= -1;

       // Move the text and bounce it
        text_x += text_speed_x * text_direction_x;
        text_y += text_speed_y * text_direction_y;
        if (text_x <= 0 || text_x >= 200) text_direction_x *= -1;
        if (text_y <= 0 || text_y >= 200) text_direction_y *= -1;

        // Scale the text size
        text_scale += text_scale_speed * scale_direction;
        if (text_scale <= 0.5f || text_scale >= 2.0f) scale_direction *= -1;

        // Clear screen and draw
        clear_screen(renderer);
        draw_image(renderer, imageTexture, bmp_x, bmp_y, 32.0f, 32.0f);
        draw_moving_rectangles(renderer, rect_x);
        draw_text(renderer, textTexture, text_x, text_y, 120, 20 * text_scale);

        SDL_RenderPresent(renderer);
        vTaskDelay(pdMS_TO_TICKS(16));
    }
}
