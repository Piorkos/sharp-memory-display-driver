# Sharp Memory display driver for Raspberry Pi Pico SDK

This repository contains a custom driver for interfacing Sharp Memory LCD displays with the Raspberry's RP2040 using the Pico SDK. The driver enables text display on the screen and includes support for two fonts, though not all ASCII characters are currently supported. The driver also manages the toggling of the VCOM signal, which is required to prevent screen degradation.

## Usage

### Creating the Display Object
To start using the display, create an instance of the SharpMipDisplay class:
```cpp
#include "sharp_mip_display.h"

SharpMipDisplay* display = new SharpMipDisplay(DISPLAY_WIDTH, DISPLAY_HEIGHT, spi1, SPI_CS_PIN);
```

### Writing Text to the Display
You can display text using the DrawLineOfText() method of the SharpMipDisplay class. The method parameters allow you to specify the position and behavior of the text:
```cpp
#include "font_12x16.h"

display->DrawLineOfText(x, y, "HELLO", kFont_12_16);
```

- x: The horizontal starting position of the text, specified in bytes (1 byte = 8 pixels).
- y: The vertical starting position of the text, specified in pixels.
- string: The text string you want to display on the screen.
- font: table of font which should be use
- join_with_existing_text: If set to TRUE, the new text will be added to any existing content on the same lines. If set to FALSE, the existing content within the text area will be erased and replaced with the new text. This does not affect content outside the area where the new text is placed.
### Toggling VCOM Manually
The VCOM signal must be toggled at least once per second to avoid display degradation. The driver automatically toggles VCOM during any draw operation. If no drawing occurs within a second, you must call the ToggleVCOM() method manually to toggle the VCOM.
```cpp
display->ToggleVCOM();
```

## Example Code
Here’s a simple example of how to create the display object and write text:
```cpp
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "display.h"
#include "sharp_mip_display.h"
#include "font_12x16.h"
#include "font_16x20.h"

// SPI pins
#define SPI_SCK_PIN     26U         // SCLK / SCK
#define SPI_MOSI_PIN    27U         // MOSI / COPI
#define SPI_CS_PIN      28U         // SS / CS
// Display resolution in pixels
#define DISPLAY_WIDTH   144U
#define DISPLAY_HEIGHT   168U


int main() {

    stdio_init_all();
    sleep_ms(1000);

    // Initialize SPI
    spi_init(spi1, 2000000);
    spi_set_format( spi1, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_set_function(SPI_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SCK_PIN, GPIO_FUNC_SPI);

    // CS pin for Sharp MIP display
    gpio_init(SPI_CS_PIN);
    gpio_set_dir(SPI_CS_PIN, GPIO_OUT);
    gpio_put(SPI_CS_PIN, 0);  // this display is low on inactive
    sleep_ms(10);

    // Create a display object
    SharpMipDisplay* display = new SharpMipDisplay(DISPLAY_WIDTH, DISPLAY_HEIGHT, spi1, SPI_CS_PIN);
    sleep_ms(1000);

    // Draw text "HELLO"
    display->ClearScreen();
    display->DrawLineOfText(0, 0, "HELLO", kFont_12_16);
    display->RefreshScreen(0,16);

    int counter{20};
    while (true)
    {
        sleep_ms(1000);
        
        if(counter < 120)
        {
            display->ClearScreen();
            display->DrawLineOfText(0,counter, "CZE", kFont_16_20);
            display->RefreshScreen(20,140);
            ++counter;
        }
        else
        {
            // Sharp Memory in Pixel display requires to toglle VCOM at least once per second.
            // VCOM is toggled in Draw methods. If no draw method is called, then it is required to call ToggleVCOM()
            display->ToggleVCOM();
        }
    }
    
    return 0;
}
```

## Fonts

Currently, the driver supports two fonts, though they do not include all ASCII characters.
