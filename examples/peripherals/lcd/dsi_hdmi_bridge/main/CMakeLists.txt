idf_component_register(SRCS "dsi_hdmi_bridge_example_main.c" "lvgl_port.c" "lvgl_demo_ui.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_driver_i2c esp_lcd esp_timer)
