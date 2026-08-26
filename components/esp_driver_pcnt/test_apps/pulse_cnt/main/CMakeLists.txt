set(srcs "test_app_main.c"
         "test_pulse_cnt_simulator.c"
         "test_pulse_cnt.c")

if(CONFIG_PCNT_ISR_IRAM_SAFE)
    list(APPEND srcs "test_pulse_cnt_iram.c")
endif()

if(CONFIG_SOC_LIGHT_SLEEP_SUPPORTED AND CONFIG_PM_ENABLE)
    list(APPEND srcs "test_pulse_cnt_sleep.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity esp_driver_pcnt esp_driver_gpio spi_flash esp_psram
                       WHOLE_ARCHIVE)
