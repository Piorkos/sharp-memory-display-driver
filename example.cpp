#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "display.h"
#include "sharp_mip_display.h"
#include "fonts/font_8x10.h"
#include "fonts/font_16x20.h"
#include "fonts/font_24x30.h"
#include "fonts/font_32x40.h"

// SPI pins
#define SPI_SCK_PIN     26U         // SCLK / SCK
#define SPI_MOSI_PIN    27U         // MOSI / COPI
#define SPI_CS_PIN      28U         // SS / CS
// Display resolution in pixels
#define DISPLAY_WIDTH   144U
#define DISPLAY_HEIGHT  168U


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
    display->DrawLineOfText(0, 0, "HELLO", kFont_24_30);
    display->RefreshScreen(0,30);

    // Array with printable ASCII characters
    uint8_t amount_of_printable_chars{95};
    uint8_t printable_chars[amount_of_printable_chars] = {' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', 
                                    '*', '+', ',', '-', '.', '/', '0', '1', '2', '3',
                                    '4', '5', '6', '7', '8', '9', ':', ';', '<', '=',
                                    '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
                                    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q',
                                    'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[',
                                    '\\', ']', '^', '_', '`', 'a', 'b', 'c', 'd', 'e',
                                    'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
                                    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
                                    'z', '{', '|', '}', '~'};
    int counter{0};
    int font_iterator{0};
    // arrray of pointers to arrays
    const uint8_t* font_array[] = {kFont_32_40, kFont_16_20, kFont_24_30, kFont_8_10};
    auto selected_font = font_array[0];
    int chars_per_line{DISPLAY_WIDTH / (selected_font[0] * 8)};
    int line_height{selected_font[1] + 2};    // +2 to add some space between lines
    int max_lines{DISPLAY_HEIGHT / line_height};
    int max_characters{chars_per_line * max_lines};
    std::string s{""};

    while (true)
    {
        sleep_ms(2000);

        // Print as much characters as the screen can fit
        display->ClearScreen();
        for(int i = 0; i < max_lines; ++i)  // iterate through all lines
        {
            for(int j = 0; j < chars_per_line; ++j) // iterate through all chars in a line
            {
                // Add next character from printable_chars array to the string
                if(i*chars_per_line + j + counter < amount_of_printable_chars)
                {
                    s += printable_chars[i*chars_per_line + j + counter];
                }
            }    
            display->DrawLineOfText(0, i*line_height, s, selected_font);

            // Reset the s string
            s = "";
        }
        display->RefreshScreen(0,DISPLAY_HEIGHT);


        // If all characters already displayed, switch to next font
        if(counter > (amount_of_printable_chars - max_characters))
        {
            counter = 0;
            font_iterator++;
            selected_font = font_array[font_iterator % 4];
            chars_per_line = DISPLAY_WIDTH / (selected_font[0] * 8);
            line_height = selected_font[1] + 2;
            max_lines = DISPLAY_HEIGHT / line_height;
            max_characters = chars_per_line * max_lines;
        }
        else
        {
            counter += chars_per_line;
        }
    }
    
    return 0;
}