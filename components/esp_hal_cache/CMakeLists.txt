idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(includes "include" "${target}/include")

# The cache HAL is always built for normal apps, where the flash cache is enabled.
# Pure RAM apps do not use the flash cache, except on chips where internal memory
# accesses also go through the L1 cache (e.g. ESP32-P4): cache synchronization is
# still required around internal RAM DMA operations on such chips.
if(NOT CONFIG_APP_BUILD_TYPE_PURE_RAM_APP OR CONFIG_SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE)
    if(${target} STREQUAL "esp32")
        list(APPEND srcs "esp32/cache_hal_esp32.c")
    else()
        list(APPEND srcs "cache_hal.c")
    endif()
endif()

# The MMU HAL implements external memory (flash/PSRAM) address translation.
# Pure RAM apps do not map any external memory, so it is not built for them.
if(NOT CONFIG_APP_BUILD_TYPE_PURE_RAM_APP AND CONFIG_SOC_MMU_PERIPH_NUM)
    list(APPEND srcs "mmu_hal.c")
endif()

# Target-specific cache peripheral data
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/cache_periph.c")
    list(APPEND srcs "${target}/cache_periph.c")
endif()

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS ${includes}
    REQUIRES soc hal
    LDFRAGMENTS linker.lf
)
