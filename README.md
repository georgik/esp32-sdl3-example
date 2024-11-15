# Example of SDL3 for ESP32

![Test Status](https://github.com/georgik/esp32-sdl3-example/actions/workflows/test.yml/badge.svg)

Fork of SDL3 to ESP32 ESP-IDF v5.3+

## On-line Demo Simulation

[![ESP32-P4 SDL3 Demo Simulation](docs/img/esp32-p4-sdl3.webp)](https://wokwi.com/experimental/viewer?diagram=https%3A%2F%2Fraw.githubusercontent.com%2Fgeorgik%2Fesp32-sdl3-example%2Fmain%2Fboards%2Fesp32_p4_function_ev_board%2Fdiagram.json&firmware=https%3A%2F%2Fgithub.com%2Fgeorgik%2Fesp32-sdl3-example%2Freleases%2Fdownload%2Fv0.2.0%2Fesp32-sdl3-example-esp32_p4_function_ev_board.bin)

[Run the ESP32-P4 simulation with Wokwi.com](https://wokwi.com/experimental/viewer?diagram=https%3A%2F%2Fraw.githubusercontent.com%2Fgeorgik%2Fesp32-sdl3-example%2Fmain%2Fboards%2Fesp32_p4_function_ev_board%2Fdiagram.json&firmware=https%3A%2F%2Fgithub.com%2Fgeorgik%2Fesp32-sdl3-example%2Freleases%2Fdownload%2Fv0.2.0%2Fesp32-sdl3-example-esp32_p4_function_ev_board.bin)

Working parts:
- minimalistic SDL3 build
- SDL3 - littlefs integration
- SDL3\_timer
- SDL3\_events
- SDL\_image - BMP
- SDL\_ttf
- Lua 5.5
- touch

## Requirements

`idf_component_manager` 2.x - install manually

## Build

```shell
git clone git@github.com:georgik/esp32-sdl3-test.git
cd esp32-sdl3-test

idf.py @boards/esp-box-3.cfg build
```

### Other boards

- ESP32-S3-BOX-3
```shell
idf.py @boards/esp-box-3.cfg build
```

- ESP32-S3-BOX (prior Dec. 2023)
```shell
idf.py @boards/esp-box.cfg build
```

- ESP32-P4
```shell
idf.py @boards/esp32_p4_function_ev_board.cfg build
```

- M5Stack-CoreS3
```shell
idf.py @boards/m5stack_core_s3.cfg build
```

### Build for ESP32-C6-DevKit or other custom DevKit

![ESP32-C6-DevKit](docs/img/esp32-c6-devkit.webp)

The configuration of this board is based on [ESP-BSP Generic](https://developer.espressif.com/blog/using-esp-bsp-with-devkits/) which allows configuration using menuconfig.

SPI Display configuration:

```ini
CONFIG_BSP_DISPLAY_ENABLED=y
CONFIG_BSP_DISPLAY_SCLK_GPIO=6
CONFIG_BSP_DISPLAY_MOSI_GPIO=7
CONFIG_BSP_DISPLAY_MISO_GPIO=-1
CONFIG_BSP_DISPLAY_CS_GPIO=20
CONFIG_BSP_DISPLAY_DC_GPIO=21
CONFIG_BSP_DISPLAY_RST_GPIO=3
CONFIG_BSP_DISPLAY_DRIVER_ILI9341=y
```

You can change the configuration by running:

```shell
idf.py @boards/esp32_c6_devkit.cfg menuconfig
```

Build, flash and monitor the application:

```shell
idf.py @boards/esp32_c6_devkit.cfg flash monitor
```

### Run simulation in VS Code

- Build the project, to get binaries for simulation.
- Install [Wokwi for VS Code](https://docs.wokwi.com/vscode/getting-started/).
- Open file `boards/esp32_.../diagram.json`.
- Click Play button to start simulation.
- Click Pause button to freeze simulation and display states of GPIOs.

## Notes

- screen resolution bigger than 320x100 requires enabled PSRAM
- SDL surface for ESP-IDF framebuffer is defined for RGB565
- drawing surface requires `SDL_FRect` as destination, not `SDL_Rect`, in `SDL_Rect` case nothing draws

## Credits

- FreeSans.ttf - https://github.com/opensourcedesign/fonts/blob/master/gnu-freefont_freesans/FreeSans.ttf
