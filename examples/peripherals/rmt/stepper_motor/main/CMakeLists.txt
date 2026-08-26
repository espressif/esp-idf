idf_component_register(SRCS "stepper_motor_example_main.c" "stepper_motor_encoder.c"
                       PRIV_REQUIRES esp_driver_rmt esp_driver_gpio
                       INCLUDE_DIRS ".")
