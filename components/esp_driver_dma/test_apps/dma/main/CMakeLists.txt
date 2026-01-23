set(srcs "test_app_main.c")

if(CONFIG_SOC_ASYNC_MEMCPY_SUPPORTED)
    list(APPEND srcs "test_async_memcpy.c")
endif()

if(CONFIG_SOC_GDMA_SUPPORTED)
    list(APPEND srcs "test_gdma.c" "gdma_test_utils.c")

    if(CONFIG_SOC_ETM_SUPPORTED AND CONFIG_SOC_GDMA_SUPPORT_ETM)
        list(APPEND srcs "test_gdma_etm.c")
    endif()

    if(CONFIG_SOC_GDMA_SUPPORT_CRC)
        list(APPEND srcs "test_gdma_crc.c")
    endif()
endif()

if(CONFIG_SOC_DW_GDMA_SUPPORTED)
    list(APPEND srcs "test_dw_gdma.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity esp_mm esp_driver_gpio esp_psram esp_driver_dma
                       WHOLE_ARCHIVE)

idf_component_get_property(lib_name esp_hal_dma COMPONENT_LIB)
# Test GDMA retention correctness with software retention feature
target_compile_definitions(${lib_name} PRIVATE "CI_TEST_SW_RETENTION=1")
