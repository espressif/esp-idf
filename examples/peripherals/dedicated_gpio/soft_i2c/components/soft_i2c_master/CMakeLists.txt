idf_component_register(SRCS "soft_i2c_master.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES esp_driver_gpio
                       LDFRAGMENTS linker.lf)
