idf_build_get_property(target IDF_TARGET)

set(srcs)

set(public_include "include" "legacy/include")

# SDMMC related source files
if(CONFIG_SOC_SDMMC_HOST_SUPPORTED)
    list(APPEND srcs "legacy/src/sdmmc_transaction.c"
                     "legacy/src/sdmmc_host.c"
                     "src/sd_host_sdmmc.c"
                     "src/sd_trans_sdmmc.c")
endif()

if(${target} STREQUAL "linux")
    set(requires "")
    set(priv_requires esp_timer)
else()
    set(requires esp_driver_sd_intf sdmmc)
    set(priv_requires esp_timer esp_pm esp_mm esp_driver_gpio esp_hal_sd)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES "${requires}"
                       PRIV_REQUIRES "${priv_requires}"
                      )
