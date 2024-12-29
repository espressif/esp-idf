set(srcs "pretty_effect.c"
    "lcd_tjpgd_example_main.c"
    "decode_image.c"
    )

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_lcd
                    EMBED_FILES image.jpg)
