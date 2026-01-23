idf_build_get_property(target IDF_TARGET)

set(srcs)
set(public_include "include")
if(CONFIG_SOC_PARLIO_SUPPORTED)
    list(APPEND srcs "src/parlio_common.c"
                     "src/parlio_tx.c"
                     "src/parlio_rx.c")
endif()

if(CONFIG_SOC_BITSCRAMBLER_SUPPORTED)
    list(APPEND srcs "src/parlio_bitscrambler.c")
endif()

if(${target} STREQUAL "linux")
    set(priv_requires "")
else()
    set(priv_requires esp_pm esp_driver_gpio esp_mm esp_driver_bitscrambler esp_driver_dma)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES "${priv_requires}"
                       REQUIRES esp_hal_parlio
                       LDFRAGMENTS "linker.lf"
                      )
