set(srcs "")
set(priv_requires "")

if(NOT BOOTLOADER_BUILD)
    if(CONFIG_IDF_TARGET_ESP32H2 OR CONFIG_IDF_TARGET_ESP32P4 OR CONFIG_IDF_TARGET_ESP32C5)
        list(APPEND srcs "src/crypto/${IDF_TARGET}/clk.c")
    endif()

    list(APPEND priv_requires "soc")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES ${priv_requires})
