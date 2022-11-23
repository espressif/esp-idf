set(srcs "soft_i2c_master_main.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES driver soft_i2c_master)
