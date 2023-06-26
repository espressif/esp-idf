idf_component_register(SRCS "test_app_main.c" "test_verify_image.c"
                       INCLUDE_DIRS "."
                       REQUIRES unity bootloader_support esp_partition app_update
                       WHOLE_ARCHIVE)
