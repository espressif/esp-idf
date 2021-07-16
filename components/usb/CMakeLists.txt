idf_build_get_property(target IDF_TARGET)

#USB Host is currently only supported on ESP32-S2, ESP32S3 chips
if(NOT "${target}" MATCHES "^esp32s[2-3]")
    return()
endif()

idf_component_register(SRCS "hcd.c"
                    INCLUDE_DIRS ""
                    PRIV_INCLUDE_DIRS "private_include"
                    PRIV_REQUIRES hal driver)
