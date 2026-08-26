idf_component_register(SRCS "button.c" "button_obj.cpp"
                    INCLUDE_DIRS  "." "include"
                    PRIV_REQUIRES esp_driver_gpio esp_timer)
