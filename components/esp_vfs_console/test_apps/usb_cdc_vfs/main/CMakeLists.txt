set(src "test_app_main.c")

idf_component_register(SRCS ${src}
                       PRIV_INCLUDE_DIRS .
                       PRIV_REQUIRES esp_system esp_vfs_console unity
                       WHOLE_ARCHIVE
                       )
