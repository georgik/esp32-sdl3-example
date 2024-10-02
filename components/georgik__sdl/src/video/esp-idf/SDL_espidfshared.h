#ifndef SDL_espidfshared_h_
#define SDL_espidfshared_h_

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "bsp/display.h"
#include "bsp/touch.h"

// Shared handles
extern esp_lcd_panel_handle_t panel_handle;
extern esp_lcd_panel_io_handle_t panel_io_handle;

extern esp_lcd_touch_handle_t touch_handle;

#endif /* SDL_espidfshared_h_ */
