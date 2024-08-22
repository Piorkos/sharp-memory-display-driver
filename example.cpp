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

    // SPI for Sharp MIP display
    spi_init(spi1, 2000000);
    spi_set_format( spi1, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_set_function(SPI_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SCK_PIN, GPIO_FUNC_SPI);

    gpio_init(SPI_CS_PIN);
    gpio_set_dir(SPI_CS_PIN, GPIO_OUT);
    gpio_put(SPI_CS_PIN, 0);  // this display is low on inactive
    sleep_ms(10);

    SharpMipDisplay* display = new SharpMipDisplay(DISPLAY_WIDTH, DISPLAY_HEIGHT, spi1, SPI_CS_PIN);
    sleep_ms(1000);
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