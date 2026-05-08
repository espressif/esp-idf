idf_component_register(SRCS "test_app_main.c" "test_verify_image.c"
                       INCLUDE_DIRS "."
                       REQUIRES unity bootloader_support esp_partition app_update
                       WHOLE_ARCHIVE)

target_link_libraries(${COMPONENT_LIB} INTERFACE
                      "-Wl,--wrap=bootloader_mmap_get_free_pages"
                      "-Wl,--wrap=bootloader_common_check_efuse_blk_validity")
