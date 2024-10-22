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

#include "love/love.h"

Uint32 SDLCALL TimerCallback(void *param, SDL_TimerID timerID, Uint32 interval)
{
    // printf("Timer callback executed!\n");
    return interval; // Return the interval to keep the timer running
}

void* sdl_thread(void* args) {
    printf("Initializing LOVE engine on ESP32\n");

    // Initialize SDL if not already initialized
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return NULL;
    }
    printf("SDL initialized successfully\n");

    // Create SDL window and renderer
    SDL_Window *window = SDL_CreateWindow("LOVE on ESP32", BSP_LCD_H_RES, BSP_LCD_V_RES, 0);
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

    SDL_InitFS();

    // Initialize Lua state
    lua_State *L = luaL_newstate();
    if (!L) {
        printf("Failed to create Lua state\n");
        return NULL;
    }
    printf("Lua state created successfully\n");

    // Open standard Lua libraries
    luaL_openlibs(L);

    // Set up the Lua package.path to include the /assets directory
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");  // Get current package.path
    const char *currentPath = lua_tostring(L, -1);
    lua_pop(L, 1);  // Pop the current path string

    // Append /assets directory to package.path
    char newPath[1024];
    snprintf(newPath, sizeof(newPath), "%s;/assets/?.lua", currentPath);
    lua_pushstring(L, newPath);
    lua_setfield(L, -2, "path");  // Set package.path to the new path

    lua_pop(L, 1);  // Pop the 'package' table

    // Preload LOVE modules
    luaL_requiref(L, "love", luaopen_love, 1);
    lua_pop(L, 1);  // Remove 'love' module from the stack

    // Set up 'arg' table if necessary
    lua_newtable(L);
    lua_setglobal(L, "arg");

    // Require the LOVE module
    lua_getglobal(L, "require");
    lua_pushstring(L, "love");
    if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
        printf("Error requiring 'love': %s\n", lua_tostring(L, -1));
        lua_close(L);
        return NULL;
    }

    // Pop the LOVE table returned by require
    lua_pop(L, 1);

    // Load the boot script from the /assets directory
    const char *bootScript = "require 'boot'";  // Now it will load boot.lua from /assets
    if (luaL_dostring(L, bootScript) != LUA_OK) {
        printf("Error running boot script: %s\n", lua_tostring(L, -1));
        lua_close(L);
        return NULL;
    }

    // Check whether run is a function
    lua_getglobal(L, "love");
    lua_getfield(L, -1, "run");  // Get love.run
    if (!lua_isfunction(L, -1)) {
        printf("Error: love.run is not a function. It is of type %s\n", luaL_typename(L, -1));
        lua_close(L);
        return NULL;
    }

    int type = lua_type(L, -1);
    printf("love.run type: %s\n", lua_typename(L, type));

    // Create a coroutine for the LOVE game loop
    lua_getglobal(L, "coroutine");
    lua_getfield(L, -1, "create");
    lua_remove(L, -2);  // Remove 'coroutine' table from the stack

    lua_getglobal(L, "love");
    lua_getfield(L, -1, "run");
    lua_remove(L, -2);  // Remove 'love' table from the stack

    // Swap the two elements so that coroutine.create is on top, love.run below
    lua_insert(L, -2);

    // Now call coroutine.create(love.run)
    if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
        printf("Error creating LOVE coroutine: %s\n", lua_tostring(L, -1));
        lua_close(L);
        return NULL;
    }

    int loveCoroutineRef = luaL_ref(L, LUA_REGISTRYINDEX);

    // Main loop
    printf("Entering LOVE main loop...\n");
    SDL_Event event;
    bool running = true;
    while (running) {
        // Handle SDL events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
                break;
            } else if (event.type == SDL_EVENT_FINGER_UP) {
                // Handle touch events if needed
            }
            // You can push events to Lua if necessary
        }

        // Resume LOVE coroutine
        lua_rawgeti(L, LUA_REGISTRYINDEX, loveCoroutineRef);

        // Push debug.traceback function onto the stack
        lua_getglobal(L, "debug");
        lua_getfield(L, -1, "traceback");
        lua_remove(L, -2);  // Remove 'debug' table from the stack

        // Move the coroutine to be after the traceback function
        lua_insert(L, -2);

        int status = lua_resume(L, NULL, 0);
        if (status == LUA_OK) {
            // Coroutine finished execution
            running = false;
        } else if (status != LUA_YIELD) {
            // Error occurred, print the detailed error message with traceback
            if (lua_type(L, -1) == LUA_TSTRING) {
                printf("LOVE error: %s\n", lua_tostring(L, -1));
            } else {
                printf("LOVE error: (unknown type)\n");
            }
            running = false;
        }
        // Delay to control frame rate
        vTaskDelay(pdMS_TO_TICKS(16));  // Approximately 60 FPS
    }

    // Cleanup and exit
    lua_close(L);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

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
