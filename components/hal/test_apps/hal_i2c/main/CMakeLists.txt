set(srcs "hal_i2c_main.c")

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES hal_i2c)
