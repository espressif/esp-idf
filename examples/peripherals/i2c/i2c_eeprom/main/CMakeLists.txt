set(srcs "i2c_eeprom_main.c")

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES esp_driver_i2c i2c_eeprom
                       INCLUDE_DIRS ".")
