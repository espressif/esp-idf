idf_build_get_property(target IDF_TARGET)
idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

set(srcs "hal_utils.c")
set(includes "platform_port/include")
set(requires)

# target specific include must be added before the generic one
# because of the "include_next" directive used by the efuse_hal.h
list(APPEND includes "${target}/include")

list(APPEND includes "include")

if(CONFIG_SOC_EFUSE_SUPPORTED)
    list(APPEND srcs "efuse_hal.c" "${target}/efuse_hal.c")
endif()

if(NOT CONFIG_APP_BUILD_TYPE_PURE_RAM_APP)
    if(CONFIG_SOC_MMU_PERIPH_NUM)
        list(APPEND srcs "mmu_hal.c")
    endif()

    # We wrap Cache ROM APIs as Cache HAL APIs for: 1. internal ram ; 2. unified APIs
    # ESP32 cache structure / ROM APIs are different and we have a patch `cache_hal_esp32.c` for it.
    if(${target} STREQUAL "esp32")
        list(APPEND srcs "esp32/cache_hal_esp32.c")
    elseif(NOT ${target} STREQUAL "linux")
        list(APPEND srcs "cache_hal.c")
    endif()
else()
    if(CONFIG_SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE)
        list(APPEND srcs "cache_hal.c")
    endif()
endif()

if(NOT esp_tee_build AND NOT BOOTLOADER_BUILD)
    list(APPEND srcs "color_hal.c")

    if(CONFIG_SOC_SYSTIMER_SUPPORTED AND NOT CONFIG_HAL_SYSTIMER_USE_ROM_IMPL)
        list(APPEND srcs "systimer_hal.c")
    endif()

    if(CONFIG_SOC_SDMMC_HOST_SUPPORTED)
        list(APPEND srcs "sdmmc_hal.c")
    endif()

    if(CONFIG_SOC_ETM_SUPPORTED)
        list(APPEND srcs "etm_hal.c" "${target}/etm_periph.c")
    endif()

    if(CONFIG_SOC_MODEM_CLOCK_IS_INDEPENDENT AND CONFIG_SOC_MODEM_CLOCK_SUPPORTED)
        list(APPEND srcs "${target}/modem_clock_hal.c")
    endif()

    if(CONFIG_SOC_SDIO_SLAVE_SUPPORTED)
        list(APPEND srcs "sdio_slave_hal.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_INCLUDE_DIRS ${priv_include}
                       REQUIRES ${requires}
                       LDFRAGMENTS linker.lf)

if(CONFIG_HAL_DEFAULT_ASSERTION_LEVEL EQUAL 1)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u abort")
elseif(CONFIG_HAL_DEFAULT_ASSERTION_LEVEL EQUAL 2)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u __assert_func")
endif()
