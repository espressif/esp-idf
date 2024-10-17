set(srcs "main.c")

idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES driver soft_i2c_master)
