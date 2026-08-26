set(srcs "test_app_main.c"
         "test_esp_flash_freq_limit.c")

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity test_utils spi_flash esp_pm bootloader_support freertos esp_timer
                                     esp_driver_gptimer esp_hw_support esp_psram
                       WHOLE_ARCHIVE)
