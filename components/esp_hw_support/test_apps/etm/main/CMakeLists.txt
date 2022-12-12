set(srcs "test_app_main.c"
         "test_etm_core.c")

if(CONFIG_SOC_GPIO_SUPPORT_ETM)
    list(APPEND srcs "test_gpio_etm.c")
endif()

if(CONFIG_SOC_TIMER_SUPPORT_ETM)
    list(APPEND srcs "test_gptimer_etm.c")
endif()

if(CONFIG_SOC_SYSTIMER_SUPPORT_ETM)
    list(APPEND srcs "test_systimer_etm.c")
endif()

if(CONFIG_SOC_GDMA_SUPPORT_ETM)
    list(APPEND srcs "test_gdma_etm.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       WHOLE_ARCHIVE)
