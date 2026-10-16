set(srcs "test_app_main.c" "test_jpeg_turbo_common.c")

if(CONFIG_SOC_JPEG_DECODE_SUPPORTED)
    list(APPEND srcs "test_jpeg_decode.c" "test_jpeg_turbo_encode.c")
    if(CONFIG_PM_ENABLE AND CONFIG_SOC_LIGHT_SLEEP_SUPPORTED)
        list(APPEND srcs "test_jpeg_sleep_retention.c")
    endif()
endif()

if(CONFIG_SOC_JPEG_ENCODE_SUPPORTED)
    list(APPEND srcs "test_jpeg_encode.c" "test_jpeg_turbo_decode.c")
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES esp_driver_jpeg unity esp_psram test_utils esp_pm
                       WHOLE_ARCHIVE)
