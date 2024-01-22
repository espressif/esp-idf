set(srcs "i2s_pdm_example_main.c")

if(CONFIG_SOC_I2S_SUPPORTS_PDM_TX AND CONFIG_EXAMPLE_PDM_TX)
    list(APPEND srcs "i2s_pdm_tx.c")
endif()

if(CONFIG_SOC_I2S_SUPPORTS_PDM_RX AND CONFIG_EXAMPLE_PDM_RX)
    list(APPEND srcs "i2s_pdm_rx.c")
endif()

idf_component_register(SRCS "${srcs}"
                    PRIV_REQUIRES esp_driver_i2s esp_driver_gpio
                    INCLUDE_DIRS "." "$ENV{IDF_PATH}/examples/peripherals/i2s/common")
