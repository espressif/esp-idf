idf_build_get_property(target IDF_TARGET)

set(srcs)

set(public_include "include")

# SDMMC related source files
if(CONFIG_SOC_SDMMC_HOST_SUPPORTED)
    list(APPEND srcs "src/sdmmc_transaction.c"
                     "src/sdmmc_host.c")
                     "src/sd_host_sdmmc.c"
                     "src/sd_trans_sdmmc.c")
endif()

if(${target} STREQUAL "linux")
    set(requires "")
    set(priv_requires esp_timer)
else()
    set(requires esp_driver_sd_intf sdmmc esp_driver_gpio)
    set(priv_requires esp_timer esp_pm esp_mm)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES "${requires}"
                       PRIV_REQUIRES "${priv_requires}"
                      )
