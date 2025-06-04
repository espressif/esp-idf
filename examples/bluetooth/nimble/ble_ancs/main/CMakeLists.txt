set(srcs "main.c"
         "ble_ancs.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ".")
