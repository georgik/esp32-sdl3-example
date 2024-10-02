/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2011 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include "../../events/SDL_touch_c.h"
#include "../SDL_sysvideo.h"
#include "SDL_espidftouch.h"
#include <stdbool.h>

#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#include "bsp/touch.h"
#include "esp_log.h"

#define ESPIDF_TOUCH_ID         1
#define ESPIDF_TOUCH_FINGER     1

esp_lcd_touch_handle_t touch_handle;   // LCD touch handle

void ESPIDF_InitTouch(void)
{
    bsp_i2c_init();

    /* Initialize touch */
    bsp_touch_new(NULL, &touch_handle);

    SDL_AddTouch(ESPIDF_TOUCH_ID, SDL_TOUCH_DEVICE_DIRECT, "Touchscreen");
    ESP_LOGI("SDL", "ESPIDF_InitTouch");
}

void ESPIDF_PumpTouchEvent(void)
{
    SDL_Window *window;
    SDL_VideoDisplay *display;
    static SDL_bool was_pressed = SDL_FALSE;
    SDL_bool pressed;

    uint16_t touchpad_x[1] = {0};
    uint16_t touchpad_y[1] = {0};
    uint8_t touchpad_cnt = 0;

    esp_lcd_touch_read_data(touch_handle);
    bool touchpad_pressed = esp_lcd_touch_get_coordinates(touch_handle, touchpad_x, touchpad_y, NULL, &touchpad_cnt, 1);
    pressed = (touchpad_x[0] != 0 || touchpad_y[0] != 0);

    display = NULL;
    window = display ? display->fullscreen_window : NULL;

    if (pressed != was_pressed) {
        was_pressed = pressed;
        ESP_LOGD("SDL", "touchpad_pressed: %d, [%d, %d]", touchpad_pressed, touchpad_x[0], touchpad_y[0]);
        SDL_SendTouch(0, ESPIDF_TOUCH_ID, ESPIDF_TOUCH_FINGER,
                      window,
                      pressed,
                      touchpad_x[0],
                      touchpad_y[0],
                      pressed ? 1.0f : 0.0f);
    } else if (pressed) {
        SDL_SendTouchMotion(0, ESPIDF_TOUCH_ID, ESPIDF_TOUCH_FINGER,
                            window,
                            touchpad_x[0],
                            touchpad_y[0],
                            1.0f);
    }
}

int ESPIDF_CalibrateTouch(float screenX[], float screenY[], float touchX[], float touchY[])
{
    return 0;
}

void ESPIDF_ChangeTouchMode(int raw)
{
    return;
}

void ESPIDF_ReadTouchRawPosition(float* x, float* y)
{
    return;
}

void ESPIDF_QuitTouch(void)
{
    // ts_close(ts);
}
