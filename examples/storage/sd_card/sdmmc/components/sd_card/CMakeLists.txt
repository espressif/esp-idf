idf_component_register(SRCS "sd_test_io.c"
                       INCLUDE_DIRS "."
                       REQUIRES fatfs esp_adc
                       WHOLE_ARCHIVE)
