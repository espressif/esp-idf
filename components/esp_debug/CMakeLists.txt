idf_build_get_property(target IDF_TARGET)

set(srcs    "src/esp_debug.c")

set(include_dirs
    lwip/port/esp32/include
    )

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS include
                    PRIV_INCLUDE_DIRS
                    REQUIRES esp_event lwip
                    PRIV_REQUIRES "${target}")
