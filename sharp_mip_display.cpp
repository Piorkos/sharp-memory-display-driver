#include "sharp_mip_display.h"

SharpMipDisplay::SharpMipDisplay(uint16_t width, uint16_t height, spi_inst_t* spi, uint display_cs_pin)
: Display(width, height), kSPI_{spi}, kDisplaySpiCsPin_{display_cs_pin}
{
    for(std::size_t i = 0; i < (kScreenHeight_ * kScreenWidthInWords_); ++i)
    {
        screen_buffer_[i] = 0xFF;
    }
}

// SharpMipDisplay::~SharpMipDisplay()
// {
//     // kSPI_ = nullptr;
//     // delete kSPI_;
//     // screen_buffer_ = nullptr;
//     // delete screen_buffer_;
//     // for (int i = 0; i < kScreenHeight_; i++) {
//     //     delete[] screen_buffer_[i];
//     // }
//     // delete[] screen_buffer_;
// }

/**
 * @brief Updates screen buffer (array) with given text. The text is put in the screen buffer at given position.
 * 
 * @param x column, position at which the text starts, in BYTES (8 pixels). It is the number of columns (screen_width_in_pixels/8), NOT pixels.
 * @param y row, position at which the text starts, in PIXELS.
 * @param new_string string which needs to be put in screen buffer on given position.
 * @param font Table with font which should be used. 
 * @param join_with_existing_pixels If FALSE it will clear previous pixels and draw text on empty space. 
 *          If TRUE it will keep existing pixels and join new text with them. Default FALSE.
 */
void SharpMipDisplay::DrawLineOfText(uint16_t x, uint16_t y, const std::string& new_string, const uint8_t font[], bool join_with_existing_pixels)
{
    printf("--SharpMipDisplay::DrawLineOfText : new_string = %s \n", new_string.c_str());

    if(join_with_existing_pixels)
    {
        DrawLineOfTextAdd(x, y, new_string, font);
    }
    else
    {
        DrawLineOfTextReplace(x, y, new_string, font);
    }
}

/**
 * @brief Sends new pixel values to the screen. It updates all lines between line_start and line_end.
 * 
 * @param line_start number of the first row which should be updated.
 * @param line_end number of the last row which should be updated.
 */
void SharpMipDisplay::RefreshScreen(uint8_t line_start, uint8_t line_end)
{
    printf("-- SharpMipDisplay::RefreshScreen \n");
    
    gpio_put(kDisplaySpiCsPin_, 1);

    int length_of_buffer = 1 + (line_end - line_start) * (1 + kScreenWidthInWords_ + 1) + 1;
    uint8_t buf[length_of_buffer];
    int buf_iterator{0};
    // buf[buf_iterator] = 0b10000000;    // command
    if(vcom_bool_)
    {
        buf[0] = 0b11000000;
        vcom_bool_ = false;
    }
    else
    {
        buf[0] = 0b10000000;
        vcom_bool_ = true;
    }
    buf_iterator++;

    for (size_t i = line_start; i < line_end; i++)
    {
        uint8_t little_endian_line_address = SwapBigToLittleEndian(i);
        buf[buf_iterator] = little_endian_line_address;    //line address
        buf_iterator++;
        for (size_t j = 0; j < kScreenWidthInWords_; ++j)
        {
            buf[buf_iterator] = screen_buffer_[i * kScreenWidthInWords_ + j];
            buf_iterator++;
        }
        buf[buf_iterator] = 0b00000000;     //end line trailer
        buf_iterator++;
    }

    buf[buf_iterator] = 0b00000000;     //end transmission trailer
    spi_write_blocking(kSPI_, buf, length_of_buffer);
    gpio_put(kDisplaySpiCsPin_, 0);
    sleep_ms(10);
}

/**
 * @brief Clears the screen.
 * 
 */
void SharpMipDisplay::ClearScreen()
{
    printf("-- ClearScreen \n");

    for(int i = 0; i < (kScreenWidthInWords_ * kScreenHeight_); ++i) 
    {
        screen_buffer_[i] = 0b11111111;
    }

    gpio_put(kDisplaySpiCsPin_, 1);
    uint8_t buf[2];
    // buf[0] = 0b01100000;    // command
    if(vcom_bool_)
    {
        buf[0] = 0b01100000;
        vcom_bool_ = false;
    }
    else
    {
        buf[0] = 0b00100000;
        vcom_bool_ = true;
    }
    buf[1] = 0b00000000;
    spi_write_blocking(kSPI_, buf, 2);
    gpio_put(kDisplaySpiCsPin_, 0);
}

/**
 * @brief Toggles the state of VCOM. Sharp MIP requires to toggle VCOM at least once per second. 
 * 
 */
void SharpMipDisplay::ToggleVCOM()
{
    printf("-- SharpMipDisplay::ToggleVCOM \n");
    gpio_put(kDisplaySpiCsPin_, 1);
    uint8_t buf[22];
    if(vcom_bool_)
    {
        buf[0] = 0b01000000;
        vcom_bool_ = false;
    }
    else
    {
        buf[0] = 0b00000000;
        vcom_bool_ = true;
    }
    buf[1] = 0b00000000;
    spi_write_blocking(kSPI_, buf, 2);
    gpio_put(kDisplaySpiCsPin_, 0);
    sleep_ms(10);
}



/********** PRIVATE **********/

uint8_t SharpMipDisplay::SwapBigToLittleEndian(uint8_t big_endian)
{
    std::string string_be = std::bitset<8>(big_endian).to_string();
    std::reverse(string_be.begin(), string_be.end());
    std::bitset<8> bitset_le(string_be);
    uint8_t little_e = static_cast<uint8_t>(bitset_le.to_ulong());
    
    return little_e;
}

void SharpMipDisplay::DrawLineOfTextReplace(uint16_t x, uint16_t y, const std::string& new_string, const uint8_t font[])
{
    printf("-- SharpMipDisplay::DrawLineOfTextReplace  \n");
    
    uint8_t char_width_in_bytes = font[0];
    uint8_t char_height_in_pixels = font[1];
    uint8_t first_char_in_fonts = font[2];
    uint16_t char_size_in_bytes = char_width_in_bytes * char_height_in_pixels;
    
    uint16_t char_counter{0};

    for(const auto& character : new_string)     // iterate through every char in string
    {
        int char_position{(character - first_char_in_fonts)*char_size_in_bytes + 3};

        for(std::size_t i = 0; i < char_width_in_bytes; ++i)
        {
            for(std::size_t j = 0; j < char_height_in_pixels; ++j)  //iterate vertically through every line in a char
            {
                uint16_t row = (y + j)*kScreenWidthInWords_;
                uint16_t col = x + char_counter*char_width_in_bytes + i;
                screen_buffer_[row + col] = font[char_position + i + j*char_width_in_bytes];
            }
        }

        ++char_counter;
    }
}

void SharpMipDisplay::DrawLineOfTextAdd(uint16_t x, uint16_t y, const std::string& new_string, const uint8_t font[])
{
    printf("-- SharpMipDisplay::DrawLineOfTextAdd  \n");
    uint8_t char_width_in_bytes = font[0];
    uint8_t char_height_in_pixels = font[1];
    uint8_t first_char_in_fonts = font[2];
    uint16_t char_size_in_bytes = char_width_in_bytes * char_height_in_pixels;
    
    uint16_t char_counter{0};

    for(const auto& character : new_string)     // iterate through every char in string
    {
        int char_position{(character - first_char_in_fonts)*char_size_in_bytes + 3};

        for(std::size_t i = 0; i < char_width_in_bytes; ++i)
        {
            for(std::size_t j = 0; j < char_height_in_pixels; ++j)  //iterate vertically through every line in a char
            {
                uint16_t row = y + j*kScreenWidthInWords_;
                uint16_t col = x + char_counter*char_width_in_bytes + i;
                screen_buffer_[row + col] &= font[char_position + i + j*char_width_in_bytes];
            }
        }

        ++char_counter;
    }
}

/**
 * @brief Helper function to print array of pixels in the terminal. Used only during debugging.
 * 
 * @param array_to_print 
 * @param width in BYTES
 * @param heigth in BITS (pixels)
 */
void SharpMipDisplay::PrintBinaryArray(const uint8_t* array_to_print, size_t width, size_t heigth)
{
    printf("--SharpMipDisplay::PrintBinaryArray\n");
    for (size_t i = 0; i < heigth; i++)     // rows
    {
        for (size_t j = 0; j < width; j++)  // cols
        {
            for (size_t k = 0; k < 8; k++) // word size
            {
                uint8_t mask = 0b10000000;
                mask = mask >> k;
                if(array_to_print[(i*width) + j] & mask)
                {
                    printf(".");
                }
                else
                {
                    printf("+");
                }
            }
        }
        printf("\n");
    }
}