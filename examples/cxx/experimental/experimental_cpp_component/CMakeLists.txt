idf_component_register(SRCS
                    "esp_exception.cpp"
                    "i2c_cxx.cpp"
                    "esp_event_api.cpp"
                    "esp_event_cxx.cpp"
                    "esp_timer_cxx.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES driver esp_event esp_timer)
