idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(includes "include")

# Note: requires spi_flash for cache_utils, will be refactored
set(priv_requires heap spi_flash esp_hal_dma)

set(srcs)

if(NOT CONFIG_APP_BUILD_TYPE_PURE_RAM_APP)
    set(srcs "esp_mmu_map.c"
             "port/${target}/ext_mem_layout.c"
             "esp_cache_msync.c"
             "esp_cache_utils.c")

    if(CONFIG_IDF_TARGET_ESP32)
        list(APPEND srcs "cache_esp32.c")
    endif()
else()
    if(CONFIG_SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE)
        list(APPEND srcs "esp_cache_msync.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_REQUIRES ${priv_requires}
                       LDFRAGMENTS linker.lf)
