add_executable(display_driver
    example.cpp
    display.cpp
    )

#add_library(display display.cpp sharp_mip_display.cpp)

add_subdirectory(sharp-mip)

target_link_libraries(display_driver
    sharp_mip_display
    pico_stdlib
    hardware_spi
)

# enable usb output, disable uart output
pico_enable_stdio_usb(display_driver 1)
pico_enable_stdio_uart(display_driver 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(display_driver)

# add url via pico_set_program_url
example_auto_set_url(display_driver)
