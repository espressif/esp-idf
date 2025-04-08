set(srcs "test_app_main.c")

if(CONFIG_SOC_TWAI_SUPPORT_FD)
    list(APPEND srcs "test_twaifd.c")
endif()

idf_component_register(
    SRCS ${srcs}
    PRIV_REQUIRES esp_driver_twai
    WHOLE_ARCHIVE
)
