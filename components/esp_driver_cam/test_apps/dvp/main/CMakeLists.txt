set(srcs "test_app_main.c")

if(CONFIG_SOC_LCDCAM_CAM_SUPPORTED)
    list(APPEND srcs "test_dvp_driver.c")
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
