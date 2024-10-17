set(srcs "main.c")

idf_component_register(SRCS "sht21.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES driver soft_i2c_master)
