set(srcs
    "test_app_main.c"
    "test_twaifd.c"
)

idf_component_register(
    SRCS ${srcs}
    PRIV_REQUIRES esp_driver_twai
    WHOLE_ARCHIVE
)
