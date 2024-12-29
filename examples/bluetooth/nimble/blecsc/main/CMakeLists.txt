set(srcs "main.c"
         "gatt_svr.c")

idf_component_register(SRCS "${srcs}"
                       PRIV_REQUIRES bt nvs_flash
                       INCLUDE_DIRS ".")
