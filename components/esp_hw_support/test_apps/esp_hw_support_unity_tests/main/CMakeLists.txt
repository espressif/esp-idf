set(srcs "test_app_main.c"
          "test_dport.c"
          "test_fp.c"
          "test_dport_xt_highint5.S"
          "test_random.c"
           )

if(CONFIG_SOC_GP_LDO_SUPPORTED)
    list(APPEND srcs "test_ldo.c")
endif()

if(CONFIG_SOC_GPIO_CLOCKOUT_BY_GPIO_MATRIX OR CONFIG_SOC_GPIO_CLOCKOUT_BY_IO_MUX)
    list(APPEND srcs "test_esp_clock_output.c")
endif()

if(CONFIG_SOC_DEBUG_PROBE_SUPPORTED)
    list(APPEND srcs "test_debug_probe.c")
endif()

if(CONFIG_SOC_ETM_SUPPORTED)
    list(APPEND srcs "test_etm_core.c")
endif()

if(CONFIG_SOC_GPTIMER_SUPPORTED)
    list(APPEND SRC "test_intr_alloc.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       REQUIRES unity esp_driver_gpio esp_driver_gptimer esp_driver_uart test_utils efuse spi_flash
                       WHOLE_ARCHIVE)
