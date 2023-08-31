set(embedded_images)
if(CONFIG_EXAMPLE_LCD_IMAGE_FROM_EMBEDDED_BINARY)
    file(GLOB_RECURSE embedded_images images/*.c)
endif()

idf_component_register(SRCS "i80_controller_example_main.c" "lvgl_demo_ui.c" ${embedded_images}
                       INCLUDE_DIRS ".")

if(CONFIG_EXAMPLE_LCD_IMAGE_FROM_FILE_SYSTEM)
    # Create a partition to store the image resources in the filesystem
    spiffs_create_partition_image(storage ./images/filesystem FLASH_IN_PROJECT)
endif()
