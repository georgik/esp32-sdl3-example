#include <stdio.h>
#include <pthread.h>
#include "SDL3/SDL.h"
#include "SDL3/SDL_esp-idf.h"
//#include "SDL3_image/SDL_image.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "bsp/esp-bsp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "graphics.h"
#include "filesystem.h"
#include "text.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

Uint32 SDLCALL TimerCallback(void *param, SDL_TimerID timerID, Uint32 interval)
{
    // printf("Timer callback executed!\n");
    return interval; // Return the interval to keep the timer running
}

void* sdl_thread(void* args) {
    printf("SDL3 on ESP32\n");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == false) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return NULL;
    }
    printf("SDL initialized successfully\n");

    SDL_Window *window = SDL_CreateWindow("SDL on ESP32", BSP_LCD_H_RES, BSP_LCD_V_RES, 0);
    if (!window) {
        printf("Failed to create window: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        return NULL;
    }

    clear_screen(renderer);

    SDL_InitFS();
    TestFileOpen("/assets/espressif.bmp");

    TTF_Font *font = initialize_font("/assets/FreeSans.ttf", 12);
    if (!font) return NULL;

    SDL_TimerID timer_id = SDL_AddTimer(1000, TimerCallback, NULL);
    if (timer_id == 0) {
        printf("Failed to create timer: %s\n", SDL_GetError());
    } else {
        printf("Timer created successfully\n");
    }

    SDL_Texture *textTexture = render_text(renderer, font, "Hello ESP32 - SDL3", (SDL_Color){255, 255, 255, 255});
    SDL_Texture *imageTexture = LoadBackgroundImage(renderer, "/assets/espressif.bmp");

    float rect_x = 10.0f, speed = 2.0f;
    int direction = 1;

    float bmp_x = 2.0f, bmp_y = 2.0f;
    float bmp_speed_x = 2.0f, bmp_speed_y = 2.0f;

    float text_x = 30.0f, text_y = 40.0f;
    float text_speed_x = 1.5f, text_speed_y = 1.2f;
    float text_scale = 1.0f, text_scale_speed = 0.01f;
    int text_direction_x = 1, text_direction_y = 1;
    int scale_direction = 1;

    printf("Starting Lua\n");
    lua_State *L = luaL_newstate();
    printf("Opening Lua Libs\n");
    luaL_openlibs(L);

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
            if (event.type == SDL_EVENT_QUIT) {
                break;
            } else if (event.type == SDL_EVENT_FINGER_UP) {
                bmp_x = (float)event.tfinger.x;
                bmp_y = (float)event.tfinger.y;
                printf("Finger up [%f, %f]\n", bmp_x, bmp_y);
            }
        }

        bmp_x += bmp_speed_x;
        bmp_y += bmp_speed_y;

        if (bmp_x <= 0 || bmp_x + 32 >= BSP_LCD_H_RES) bmp_speed_x *= -1;
        if (bmp_y <= 0 || bmp_y + 32 >= BSP_LCD_V_RES) bmp_speed_y *= -1;

        rect_x += speed * direction;
        if (rect_x <= 0 || rect_x + 50 >= BSP_LCD_H_RES) direction *= -1;

        text_x += text_speed_x * text_direction_x;
        text_y += text_speed_y * text_direction_y;
        if (text_x <= 0 || text_x >= 200) text_direction_x *= -1;
        if (text_y <= 0 || text_y >= 200) text_direction_y *= -1;

        text_scale += text_scale_speed * scale_direction;
        if (text_scale <= 0.5f || text_scale >= 2.0f) scale_direction *= -1;

        clear_screen(renderer);
        draw_image(renderer, imageTexture, bmp_x, bmp_y, 32.0f, 32.0f);
        draw_moving_rectangles(renderer, rect_x);
        draw_text(renderer, textTexture, text_x, text_y, 120, 20 * text_scale);

        SDL_RenderPresent(renderer);
        vTaskDelay(pdMS_TO_TICKS(16));
    }

    return NULL;
}

void app_main(void) {
    pthread_t sdl_pthread;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 32768);  // Set the stack size for the thread

    int ret = pthread_create(&sdl_pthread, &attr, sdl_thread, NULL);
    if (ret != 0) {
        printf("Failed to create SDL thread: %d\n", ret);
        return;
    }

    pthread_detach(sdl_pthread);
}
