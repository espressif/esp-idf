set(src "test_app_main.c" "test_vfs_access.c"
        "test_vfs_append.c" "test_vfs_eventfd.c"
        "test_vfs_fd.c" "test_vfs_lwip.c"
        "test_vfs_open.c" "test_vfs_paths.c"
        "test_vfs_select.c"
        )

idf_component_register(SRCS ${src}
                       PRIV_INCLUDE_DIRS .
                       PRIV_REQUIRES test_utils vfs fatfs spiffs unity lwip wear_levelling cmock
                                     esp_driver_gptimer esp_driver_uart
                       WHOLE_ARCHIVE
                       )
