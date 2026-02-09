idf_build_get_property(target IDF_TARGET)
idf_build_get_property(non_os_build NON_OS_BUILD)

if(${target} STREQUAL "linux")
    idf_component_register(SRCS "linux/spi_flash_linux.c"
                          "linux/cache_utils.c"
                          "linux/flash_mmap.c"
                          "spi_flash_blockdev.c"
                           INCLUDE_DIRS include
                           REQUIRES esp_hal_mspi esp_blockdev)
    return()
endif()

if(non_os_build OR CONFIG_APP_BUILD_TYPE_PURE_RAM_APP)
    set(srcs "spi_flash_wrap.c")
    set(priv_requires bootloader_support soc esp_hal_gpio)
else()
    set(srcs "flash_brownout_hook.c")

    if(CONFIG_SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE)
        list(APPEND srcs "${target}/spi_flash_oct_flash_init.c")
    endif()

    if(CONFIG_SPI_FLASH_HPM_ON)
        list(APPEND srcs
            "spi_flash_hpm_enable.c")
    endif()

    if(CONFIG_ESP_SLEEP_SET_FLASH_DPD)
        list(APPEND srcs
            "spi_flash_dpd_enable.c")
    endif()

    # New implementation after IDF v4.0
    list(APPEND srcs
        "spi_flash_chip_drivers.c"
        "spi_flash_chip_generic.c"
        "spi_flash_chip_issi.c"
        "spi_flash_chip_mxic.c"
        "spi_flash_chip_gd.c"
        "spi_flash_chip_winbond.c"
        "spi_flash_chip_boya.c"
        "spi_flash_chip_mxic_opi.c"
        "spi_flash_chip_th.c"
        "memspi_host_driver.c"
        "spi_flash_blockdev.c")

    set(cache_srcs
        "cache_utils.c"
        "flash_mmap.c"
        "flash_ops.c"
        "spi_flash_wrap.c"
    )

    list(APPEND cache_srcs
        "esp_flash_api.c"
        "esp_flash_spi_init.c"
        "spi_flash_os_func_app.c"
        "spi_flash_os_func_noos.c")

    list(APPEND srcs ${cache_srcs})
    set(priv_requires bootloader_support soc esp_hal_gpio esp_driver_gpio esp_mm)

    if(${target} STREQUAL "esp32s2")
        list(APPEND priv_requires esp_security)
    endif()
endif()

idf_component_register(SRCS "${srcs}"
                    REQUIRES hal esp_hal_mspi esp_blockdev
                    PRIV_REQUIRES "${priv_requires}"
                    INCLUDE_DIRS include
                    LDFRAGMENTS linker.lf)

# Avoid cache miss by unexpected inlineing when built by -Os
set_source_files_properties(${cache_srcs} PROPERTIES COMPILE_FLAGS "-fno-inline-functions")
if(CMAKE_C_COMPILER_ID MATCHES "GNU")
    # These flags are GCC specific
    set_property(SOURCE ${cache_srcs} APPEND_STRING PROPERTY COMPILE_FLAGS
        " -fno-inline-small-functions -fno-inline-functions-called-once")
endif()

if(NOT non_os_build AND NOT CONFIG_APP_BUILD_TYPE_PURE_RAM_APP)
    if(CONFIG_SPIRAM)
        # [refactor-todo]: requires "esp_psram" for few MMU usages in `flash_mmap.c`
        # will be replaced with MMU requirements
        idf_component_optional_requires(PRIVATE esp_psram)
    endif()
    # Force linking init_flash ESP_SYSTEM_INIT_FN to ensure it's not discarded by linker
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_flash_spi_init_include_func")
endif()
