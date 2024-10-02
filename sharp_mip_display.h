#ifndef SHARP_MIP_DISPLAY_H
#define SHARP_MIP_DISPLAY_H


#include <bitset>
#include <algorithm> // for std::reverse
#include "hardware/spi.h"
#include "hardware/gpio.h"

#include "display.h"
#include "font_16x20.h"

class SharpMipDisplay : public Display
{
public:

    SharpMipDisplay(uint16_t width, uint16_t height, spi_inst_t *spi, uint display_cs_pin);

    /**
     * @brief Updates screen buffer (array) with given text. The text is put in the screen buffer at given position.
     * 
     * @param x column, in BYTES (blocks of 8 pixels). Position at which the text starts. It is the number of columns (screen_width_in_pixels/8), NOT pixels.
     * @param y row, in PIXELS. Position at which the text starts.
     * @param new_string string which needs to be put in screen buffer on given position.
     * @param font Table with font which should be used. 
     * @param mode The rendering mode to use when drawing the text. The mode can be one of the following:
     *  - Mode::kReplace: Fully erases the selected rows before drawing the new text, even if the new text does not cover all columns in those rows.
     *  - Mode::kMix: Erases only the columns in the selected rows that are needed to draw the new text, preserving the rest of the existing content in those rows.
     *  - Mode::kAdd: Does not erase any part of the existing content; instead, the new text is merged with the existing text on the display.
     */
    void DrawLineOfText(uint16_t x, uint16_t y, const std::string& new_string, const uint8_t font[], Mode mode = Mode::kReplace) override;

    /**
     * @brief Draws a horizontal line. This method operates on full bytes, not on pixels also requires to redraw only 1 line of the screen, hence has very good performance
     * 
     * @param y row, in PIXELS
     */
    virtual void DrawHorizontalLine(uint16_t x) override;

    /**
     * @brief Draws a vertical line. This method requires to redraw full display, hence is computation expensive.
     * 
     * @param y column, in PIXELS
     */
    virtual void DrawVerticalLine(uint16_t y) override;

    /**
     * @brief Set a single pixle, i.e. make the pixel black
     * 
     * @param x column, in PIXELS
     * @param y row, in PIXELS
     */
    virtual void SetPixel(uint16_t x, uint16_t y) override;

    /**
     * @brief Reset a single pixle, i.e. make the pixel white.
     * 
     * @param x column, in PIXELS
     * @param y row, in PIXELS
     */
    virtual void ResetPixel(uint16_t x, uint16_t y) override;

    /**
     * @brief Sends new pixel values to the screen. It updates all lines between line_start and line_end.
     * 
     * @param line_start number of the first row which should be updated. In PIXELS.
     * @param line_end number of the last row which should be updated. In PIXELS.
     */
    void RefreshScreen(uint8_t line_start, uint8_t line_end) override;

    /**
     * @brief Clears the screen.
     * 
     */
    void ClearScreen() override;

    /**
     * @brief Toggles the state of VCOM. Sharp MIP requires to toggle VCOM at least once per second. 
     * 
     */
    void ToggleVCOM();

private:

    uint8_t SwapBigToLittleEndian(uint8_t big_endian);
    void DrawLineOfTextReplace(uint16_t x, uint16_t y, const std::string& new_string, const uint8_t font[]);
    void DrawLineOfTextMix(uint16_t x, uint16_t y, const std::string& new_string, const uint8_t font[]);
    void DrawLineOfTextAdd(uint16_t x, uint16_t y, const std::string& new_string, const uint8_t font[]);

    /**
     * @brief Helper function to print array of pixels in the terminal. Used only during debugging.
     * 
     * @param array_to_print 
     * @param width 
     * @param heigth 
     */
    void PrintBinaryArray(const uint8_t* array_to_print, size_t width, size_t heigth);
    

    const uint kDisplaySpiCsPin_;
    spi_inst_t *kSPI_;
    bool vcom_bool_{false};
    const uint8_t kScreenWidthInWords_ = kScreenWidth_ / 8;
    uint8_t* screen_buffer_ = new uint8_t[kScreenWidthInWords_ * kScreenHeight_]{};
};




#endif // SHARP_MIP_DISPLAY_H