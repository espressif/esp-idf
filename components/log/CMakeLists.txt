idf_build_get_property(target IDF_TARGET)
idf_build_get_property(non_os_build NON_OS_BUILD)

if(non_os_build)
    set(system_target "noos")
else()
    if(${target} STREQUAL "linux")
        set(system_target "linux")
    else()
        set(system_target "os")
    endif()
endif()

set(srcs "src/${system_target}/log_timestamp.c"
         "src/log_timestamp_common.c"
         "src/${system_target}/log_lock.c")
set(priv_requires "")

if(NOT non_os_build)
    list(APPEND priv_requires soc hal esp_hw_support)

    list(APPEND srcs "src/os/log_write.c")

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
