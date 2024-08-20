set(srcs "test_app_main.c"
         "test_gptimer.c"
         "test_gptimer_sleep.c")

if(CONFIG_GPTIMER_ISR_IRAM_SAFE)
    list(APPEND srcs "test_gptimer_iram.c")
endif()

if(CONFIG_SOC_TIMER_SUPPORT_ETM)
    list(APPEND srcs "test_gptimer_etm.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity esp_driver_gptimer esp_driver_gpio
                       WHOLE_ARCHIVE)
