idf_component_register(SRCS "eventfd_example.c"
                       PRIV_REQUIRES vfs esp_timer esp_driver_gptimer
                       INCLUDE_DIRS ".")
