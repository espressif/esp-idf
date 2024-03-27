set(srcs)

set(public_include "include")

# SDMMC related source files
if(CONFIG_SOC_SDMMC_HOST_SUPPORTED)
    list(APPEND srcs "src/sdmmc_transaction.c"
                     "src/sdmmc_host.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES sdmmc esp_driver_gpio
                       PRIV_REQUIRES esp_timer esp_pm esp_mm
                      )
