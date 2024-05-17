idf_build_get_property(target IDF_TARGET)

set(srcs "")
set(priv_requires "")

if(BOOTLOADER_BUILD)
    list(APPEND srcs "log_noos.c")
else()
    list(APPEND srcs "src/os/log_write.c")

    if(${target} STREQUAL "linux")
        list(APPEND srcs "log_linux.c")
    else()
        list(APPEND srcs "log_freertos.c")
        list(APPEND priv_requires soc hal esp_hw_support)
    endif()

    # Buffer APIs call ESP_LOG_LEVEL -> esp_log_write, which can not used in bootloader.
    list(APPEND srcs "src/buffer/log_buffers.c"
                     "src/util.c")

    list(APPEND srcs "src/log_level/log_level.c"
                     "src/log_level/tag_log_level/tag_log_level.c")

    if(CONFIG_LOG_TAG_LEVEL_IMPL_LINKED_LIST OR CONFIG_LOG_TAG_LEVEL_IMPL_CACHE_AND_LINKED_LIST)
        list(APPEND srcs "src/log_level/tag_log_level/linked_list/log_linked_list.c")
    endif()

    if(CONFIG_LOG_TAG_LEVEL_CACHE_ARRAY)
        list(APPEND srcs "src/log_level/tag_log_level/cache/log_array.c")
    elseif(CONFIG_LOG_TAG_LEVEL_CACHE_BINARY_MIN_HEAP)
        list(APPEND srcs "src/log_level/tag_log_level/cache/log_binary_heap.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS "include/esp_private"
                       LDFRAGMENTS linker.lf
                       PRIV_REQUIRES ${priv_requires})
