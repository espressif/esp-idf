idf_component_register(SRCS "soft_i2c_master.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES driver
                       LDFRAGMENTS linker.lf)
