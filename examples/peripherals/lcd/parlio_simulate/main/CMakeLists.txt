set(embedded_images)

file(GLOB_RECURSE embedded_images images/*.c)

idf_component_register(SRCS "parlio_simulate_example_main.c" "lvgl_demo_ui.c" ${embedded_images}
                       PRIV_REQUIRES esp_driver_gpio esp_driver_parlio esp_lcd
                       INCLUDE_DIRS ".")
