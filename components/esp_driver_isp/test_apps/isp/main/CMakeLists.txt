set(srcs "test_app_main.c"
         "test_isp_driver.c")

if(CONFIG_SOC_ISP_SHARE_CSI_BRG)
    list(APPEND srcs "test_isp_csi.c")
endif()

set(priv_requires
        unity
        esp_driver_isp
        esp_psram
)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES ${priv_requires}
                       WHOLE_ARCHIVE TRUE)
