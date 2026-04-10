idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_include "include")

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND public_include "${target}/include")
endif()

if(CONFIG_SOC_GDMA_SUPPORTED)
    list(APPEND srcs "gdma_hal_top.c" "${target}/gdma_periph.c")

    if(CONFIG_SOC_GDMA_SUPPORT_CRC)
        list(APPEND srcs "gdma_hal_crc_gen.c")
    endif()

    if(CONFIG_SOC_AHB_GDMA_VERSION EQUAL 1)
        list(APPEND srcs "gdma_hal_ahb_v1.c")
    endif()

    if(CONFIG_SOC_AHB_GDMA_VERSION EQUAL 2)
        list(APPEND srcs "gdma_hal_ahb_v2.c")
    endif()

    if(CONFIG_SOC_AXI_GDMA_SUPPORTED)
        list(APPEND srcs "gdma_hal_axi.c")
    endif()
endif()

if(CONFIG_SOC_DW_GDMA_SUPPORTED)
    list(APPEND srcs "dw_gdma_hal.c")
endif()

if(CONFIG_SOC_DMA2D_SUPPORTED)
    list(APPEND srcs "dma2d_hal.c" "${target}/dma2d_periph.c")
endif()

if(CONFIG_SOC_CP_DMA_SUPPORTED)
    list(APPEND srcs "cp_dma_hal.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES soc hal)
