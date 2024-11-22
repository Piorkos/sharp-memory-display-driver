add_library(sharp_mip_display sharp_mip_display.cpp)

target_link_libraries(sharp_mip_display
    pico_stdlib
    hardware_spi
)