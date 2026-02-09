idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(public_include "include")
set(requires esp_hal_dma)
set(srcs "src/esp_dma_utils.c" "src/gdma_link.c")

if(CONFIG_SOC_GDMA_SUPPORTED)
    list(APPEND srcs "src/gdma.c")
    if(CONFIG_SOC_GDMA_SUPPORT_SLEEP_RETENTION AND CONFIG_SOC_PAU_SUPPORTED)
        list(APPEND srcs "src/gdma_sleep_retention.c")
    endif()
    if(CONFIG_SOC_GDMA_SUPPORT_ETM)
        list(APPEND srcs "src/gdma_etm.c")
    endif()
    if(CONFIG_SOC_GDMA_SUPPORT_CRC)
        list(APPEND srcs "src/gdma_crc.c")
    endif()
endif()

if(CONFIG_SOC_ASYNC_MEMCPY_SUPPORTED)
    list(APPEND srcs "src/esp_async_memcpy.c")
    if(CONFIG_SOC_GDMA_SUPPORTED)
        list(APPEND srcs "src/async_memcpy_gdma.c")
    endif() # CONFIG_SOC_GDMA_SUPPORTED
    if(CONFIG_SOC_CP_DMA_SUPPORTED)
        list(APPEND srcs "src/async_memcpy_cp_dma.c")
    endif() # CONFIG_SOC_CP_DMA_SUPPORTED
endif() # CONFIG_SOC_ASYNC_MEMCPY_SUPPORTED

if(CONFIG_SOC_DW_GDMA_SUPPORTED)
    list(APPEND srcs "src/dw_gdma.c")
endif()

if(CONFIG_SOC_DMA2D_SUPPORTED)
    list(APPEND srcs "src/dma2d.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES ${requires}
                       PRIV_REQUIRES esp_mm
                       LDFRAGMENTS "linker.lf"
                      )

if(CONFIG_SPIRAM)
    idf_component_optional_requires(PRIVATE esp_psram)
endif()
