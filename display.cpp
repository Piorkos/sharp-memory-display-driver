#include "display.h"

Display::Display(uint16_t width, uint16_t height)
: kScreenWidth_{width}, kScreenHeight_{height}
{
    std::cout << "Display Contrustor" << "\n";
}

Display::~Display() = default;