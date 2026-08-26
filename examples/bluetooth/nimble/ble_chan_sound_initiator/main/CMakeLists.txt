set(srcs "main.c")

idf_component_register(SRCS "gatt_svr.c" "${srcs}"
                       INCLUDE_DIRS ".")
