idf_build_get_property(target IDF_TARGET)
idf_build_get_property(non_os_build NON_OS_BUILD)

if(${target} STREQUAL "linux")
    idf_component_register(SRCS "linux/cache_utils.c"
                           INCLUDE_DIRS include
                           REQUIRES esp_hal_mspi)
    return()
endif()

if(non_os_build OR CONFIG_APP_BUILD_TYPE_PURE_RAM_APP)
    # efuse is required by esp_mspi_align.c to query the flash encryption state
    set(priv_requires bootloader_support soc esp_hal_gpio efuse)
    set(requires esp_hal_mspi esp_hal_cache)
    set(srcs "spi_flash_wrap.c" "spi_flash_os_tee_stub.c")
    if(NOT BOOTLOADER_BUILD)
        list(APPEND srcs "esp_mspi_align/esp_mspi_align.c")
    endif()
    if(ESP_TEE_BUILD)
        if(CONFIG_SECURE_TEE_EXT_FLASH_MEMPROT_SPI1)
            list(APPEND srcs "mspi_timing_tuning/mspi_timing_tuning.c")
            if(CONFIG_SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY)
                list(APPEND srcs "mspi_timing_tuning/tuning_scheme_impl/mspi_timing_by_mspi_delay.c")
            endif()
            if(CONFIG_SOC_MEMSPI_TIMING_TUNING_BY_DQS)
                list(APPEND srcs "mspi_timing_tuning/tuning_scheme_impl/mspi_timing_by_dqs.c")
            endif()
            if(CONFIG_SOC_MEMSPI_TIMING_TUNING_BY_FLASH_DELAY)
                list(APPEND srcs "mspi_timing_tuning/tuning_scheme_impl/mspi_timing_by_flash_delay.c")
            endif()
        endif()
    endif()
else()
    # efuse is required by esp_mspi_align.c to query the flash encryption state
    set(priv_requires bootloader_support soc esp_hal_gpio esp_mm efuse)
    set(requires hal esp_hal_mspi)

    set(srcs "flash_brownout_hook.c" "cache_utils.c" "flash_ops.c" "spi_flash_wrap.c"
             "esp_mspi_align/esp_mspi_align.c"
             "mspi_mem_barrier/mspi_mem_barrier.c")

    # MSPI0 Octal flash init
    if(CONFIG_SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE)
        list(APPEND srcs "${target}/spi_flash_oct_flash_init.c")
    endif()

    if(CONFIG_SPI_FLASH_HPM_ON)
        list(APPEND srcs "spi_flash_hpm_enable.c")
    endif()

    if(CONFIG_ESP_SLEEP_SET_FLASH_DPD)
        list(APPEND srcs "spi_flash_dpd_enable.c")
    endif()

    # MSPI timing tuning
    list(APPEND srcs "mspi_timing_tuning/mspi_timing_tuning.c")
    if(CONFIG_SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY AND NOT CONFIG_IDF_TARGET_ESP32S3)
        set(mspi_delay_file
            "${CMAKE_CURRENT_LIST_DIR}/mspi_timing_tuning/tuning_scheme_impl/mspi_timing_by_mspi_delay.c"
        )
        if(EXISTS "${mspi_delay_file}")
            list(APPEND srcs "mspi_timing_tuning/tuning_scheme_impl/mspi_timing_by_mspi_delay.c")
        endif()
    endif()
    if(CONFIG_SOC_MEMSPI_TIMING_TUNING_BY_DQS)
        list(APPEND srcs "mspi_timing_tuning/tuning_scheme_impl/mspi_timing_by_dqs.c")
    endif()
    if(CONFIG_SOC_MEMSPI_TIMING_TUNING_BY_FLASH_DELAY)
        list(APPEND srcs "mspi_timing_tuning/tuning_scheme_impl/mspi_timing_by_flash_delay.c")
    endif()

    # MSPI intr
    list(APPEND srcs "mspi_intr/mspi_intr.c")
endif()

set(include_dirs include
    "mspi_timing_tuning/include"
    "mspi_timing_tuning/tuning_scheme_impl/include"
    "mspi_intr/include"
    "esp_mspi_align/include"
    "mspi_mem_barrier/include")

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND include_dirs "${target}/include")
endif()

set(ldfragments "")
if(NOT CONFIG_APP_BUILD_TYPE_PURE_RAM_APP)
    list(APPEND ldfragments linker.lf)
endif()

idf_component_register(SRCS "${srcs}"
                       REQUIRES ${requires}
                       PRIV_REQUIRES "${priv_requires}"
                       INCLUDE_DIRS ${include_dirs}
                       LDFRAGMENTS ${ldfragments})

idf_define_esp_err_codes(HEADERS include/spi_flash_err.h)

if(NOT non_os_build AND NOT CONFIG_APP_BUILD_TYPE_PURE_RAM_APP)
    if(CONFIG_SPIRAM)
        # esp_mspi_get_io() queries esp_psram_io_get_cs_io() for the PSRAM CS pin
        idf_component_optional_requires(PRIVATE esp_psram)
    endif()
    # Force linking init_flash_dpd ESP_SYSTEM_INIT_FN to ensure it's not discarded by linker
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_mspi_flash_ops_include_func")
endif()

idf_build_get_property(target IDF_TARGET)
if(CONFIG_SOC_SPI_MEM_SUPPORT_TIMING_TUNING)
    add_subdirectory(mspi_timing_tuning/port/${target})
endif()
