set(srcs "test_app_main.c"
          "test_dport.c"
          "test_fp.c"
          "test_intr_alloc.c"
          "test_dport_xt_highint5.S"
          "test_ds.c"
          "test_hmac.c"
          "test_random.c"
          "test_key_mgr.c"
           )

if(CONFIG_SOC_GP_LDO_SUPPORTED)
    list(APPEND srcs "test_ldo.c")
endif()

if(CONFIG_SOC_GPIO_CLOCKOUT_BY_GPIO_MATRIX OR CONFIG_SOC_GPIO_CLOCKOUT_BY_IO_MUX)
    list(APPEND srcs "test_esp_clock_output.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       REQUIRES unity esp_driver_gpio esp_driver_gptimer esp_driver_uart test_utils efuse spi_flash
                       WHOLE_ARCHIVE)
