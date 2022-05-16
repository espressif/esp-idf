set(srcs "main.c"
        "gatt_svr.c"
        "misc.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ".")
