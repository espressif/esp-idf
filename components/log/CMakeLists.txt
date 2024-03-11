idf_build_get_property(target IDF_TARGET)

set(srcs "")
set(priv_requires "")

if(BOOTLOADER_BUILD)
    list(APPEND srcs "log_noos.c")
else()
    list(APPEND srcs "log.c")

    if(${target} STREQUAL "linux")
        list(APPEND srcs "log_linux.c")
    else()
        list(APPEND srcs "log_freertos.c")
        list(APPEND priv_requires soc hal esp_hw_support)
    endif()

    # Buffer APIs call ESP_LOG_LEVEL -> esp_log_write, which can not used in bootloader.
    list(APPEND srcs "src/buffer/log_buffers.c"
                     "src/util.c")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS "include/esp_private"
                    LDFRAGMENTS linker.lf
                    PRIV_REQUIRES ${priv_requires})
