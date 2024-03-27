set(srcs "test_app_main.c"
         "test_adc.c"
         "test_adc_performance.c"
         "test_adc_driver.c"
         "test_adc_driver_iram.c"
         "test_adc_wifi.c"
         "test_adc_tsens.c"
         "test_common_adc.c")

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES esp_driver_gptimer esp_driver_gpio esp_wifi nvs_flash esp_adc test_utils efuse
                                     esp_driver_tsens
                       WHOLE_ARCHIVE)
