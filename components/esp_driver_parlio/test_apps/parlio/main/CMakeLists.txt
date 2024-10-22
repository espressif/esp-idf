set(srcs "test_app_main.c"
         "test_parlio_rx.c"
         "test_parlio_tx.c")

# TODO: IDF-7840, semaphore in 'spi_bus_lock.c' is not IRAM safe
if(CONFIG_PARLIO_ISR_IRAM_SAFE)
    list(REMOVE_ITEM srcs "test_parlio_rx.c")
endif()

if(CONFIG_SOC_LIGHT_SLEEP_SUPPORTED AND CONFIG_PM_ENABLE)
    list(APPEND srcs "test_parlio_sleep.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity esp_driver_parlio esp_driver_gpio
                                    esp_driver_i2s esp_driver_spi
                       WHOLE_ARCHIVE)
