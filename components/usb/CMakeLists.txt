idf_build_get_property(target IDF_TARGET)

#USB Host is currently only supported on ESP32-S2
if(NOT "${target}" STREQUAL "esp32s2")
    return()
endif()

idf_component_register(SRCS "hcd.c"
                    INCLUDE_DIRS ""
                    PRIV_INCLUDE_DIRS "private_include"
                    PRIV_REQUIRES "hal"
                    REQUIRES "")
