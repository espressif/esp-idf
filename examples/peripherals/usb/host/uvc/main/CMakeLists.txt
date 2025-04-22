idf_component_register(SRCS "main.c"
                    PRIV_REQUIRES
                        usb
                        esp_psram    # Required for CONFIG_SPIRAM
                    )
