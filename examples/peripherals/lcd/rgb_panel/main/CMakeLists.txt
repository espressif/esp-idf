idf_component_register(SRCS "rgb_lcd_example_main.c" "lvgl_demo_ui.c" "example_rgb_lcd_panel.c"
                       PRIV_REQUIRES esp_lcd esp_timer esp_driver_gpio
                       INCLUDE_DIRS ".")
