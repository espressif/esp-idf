idf_component_register(SRCS "usb_host_lib_main.c" "class_driver.c" "usb_commands.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES fatfs console vfs wear_levelling spi_flash
                    )
