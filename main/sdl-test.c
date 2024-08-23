#include <stdio.h>
#include "SDL3/SDL.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Timer callback function
Uint32 TimerCallback(Uint32 interval, void *param)
{
    printf("Timer callback executed!\n");
    return interval; // Return the interval to keep the timer running
}

void app_main(void)
{
    printf("SDL3 on ESP32\n");

    if (SDL_Init(SDL_INIT_TIMER) != 0) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
    } else {
        printf("SDL initialized successfully\n");

        // Create a repeating timer with a 1-second interval (1000 milliseconds)
        SDL_TimerID timer_id = SDL_AddTimer(200, TimerCallback, NULL);
        if (timer_id == 0) {
            printf("Failed to create timer: %s\n", SDL_GetError());
        } else {
            printf("Timer created successfully\n");
        }

        // Main loop
        while (1) {
            vTaskDelay(pdMS_TO_TICKS(1000)); // Delay 1 second to keep the watchdog happy
        }
    }

    // SDL_Quit(); // Commented out as it requires additional dependencies
}
