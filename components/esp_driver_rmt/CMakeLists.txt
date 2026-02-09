idf_build_get_property(target IDF_TARGET)

set(srcs)
set(public_include "include")
if(CONFIG_SOC_RMT_SUPPORTED)
    list(APPEND srcs "src/rmt_common.c"
                     "src/rmt_encoder.c"
                     "src/rmt_encoder_bytes.c"
                     "src/rmt_encoder_copy.c"
                     "src/rmt_encoder_simple.c"
                     "src/rmt_rx.c"
                     "src/rmt_tx.c")
endif()

if(CONFIG_SOC_BITSCRAMBLER_SUPPORTED AND CONFIG_SOC_RMT_SUPPORT_DMA)
    list(APPEND srcs "src/rmt_encoder_bs.c")
endif()

if(${target} STREQUAL "linux")
    set(requires "")
    set(priv_requires "")
else()
    set(requires esp_hal_rmt)
    set(priv_requires esp_pm esp_driver_gpio esp_driver_bitscrambler esp_mm esp_driver_dma)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES "${requires}"
                       PRIV_REQUIRES "${priv_requires}"
                       LDFRAGMENTS "linker.lf"
                      )
