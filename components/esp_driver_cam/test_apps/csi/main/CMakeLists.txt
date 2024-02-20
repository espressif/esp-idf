set(srcs "test_app_main.c")

if(CONFIG_SOC_MIPI_CSI_SUPPORTED)
    list(APPEND srcs "test_csi_driver.c")
endif()

set(priv_requires
        unity
        esp_driver_cam
        esp_psram
)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES ${priv_requires}
                       WHOLE_ARCHIVE TRUE)
