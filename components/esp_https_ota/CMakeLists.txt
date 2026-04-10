idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

idf_component_register(SRCS "src/esp_https_ota.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_http_client esp_bootloader_format esp_app_format
                             esp_event esp_partition
                    PRIV_REQUIRES log app_update efuse)
