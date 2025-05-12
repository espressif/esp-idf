set(srcs "test_app_main.c"
         "test_parlio_rx.c"
         "test_parlio_tx.c")

if(CONFIG_SOC_LIGHT_SLEEP_SUPPORTED AND CONFIG_PM_ENABLE)
    list(APPEND srcs "test_parlio_sleep.c")
endif()

if(CONFIG_SOC_BITSCRAMBLER_SUPPORTED)
    list(APPEND srcs "test_parlio_bitscrambler.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity esp_driver_parlio esp_driver_gpio
                                    esp_driver_i2s esp_driver_spi esp_psram
                                    esp_driver_bitscrambler
                       WHOLE_ARCHIVE)

if(CONFIG_SOC_BITSCRAMBLER_SUPPORTED)
    target_bitscrambler_add_src("test_parlio_tx_LSB_to_MSB.bsasm")
    target_bitscrambler_add_src("test_parlio_tx_multiply.bsasm")
endif()
