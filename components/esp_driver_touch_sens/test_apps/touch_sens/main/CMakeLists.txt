set(srcs "test_app_main.c" "test_touch_sens_common.cpp")

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES unity esp_driver_touch_sens esp_psram
                    WHOLE_ARCHIVE)
