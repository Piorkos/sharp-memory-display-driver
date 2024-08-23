#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdlib.h>
#include <iostream>
#include <string>

class Display
{
public:
    Display(uint16_t width, uint16_t height);
    virtual ~Display();
    virtual void DrawLineOfText(uint16_t x, uint16_t y, const std::string& new_string, const uint8_t font[], bool join_with_existing_pixels = false) = 0;
    virtual void DrawHorizontalLine(uint16_t x) = 0;
    virtual void DrawVerticalLine(uint16_t y) = 0;
    virtual void SetPixel(uint16_t x, uint16_t y) = 0;
    virtual void ResetPixel(uint16_t x, uint16_t y) = 0;
    virtual void RefreshScreen(uint8_t line_start, uint8_t line_end) = 0;
    virtual void ClearScreen() = 0;

protected:
    const uint16_t kScreenWidth_;
    const uint16_t kScreenHeight_;
};

#endif // DISPLAY_H
