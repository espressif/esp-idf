set(srcs "test_app_main.c"
    )

if(CONFIG_SOC_JPEG_DECODE_SUPPORTED)
    list(APPEND srcs
                    "test_jpeg_decode.c"
    )
    if(CONFIG_PM_ENABLE)
        list(APPEND srcs "test_jpeg_sleep_retention.c")
    endif()
endif()

if(CONFIG_SOC_JPEG_ENCODE_SUPPORTED)
    list(APPEND srcs
                    "test_jpeg_encode.c"
    )
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES esp_driver_jpeg unity esp_psram test_utils esp_pm
                       WHOLE_ARCHIVE)
