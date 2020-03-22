set(srcs "ble_mesh_demo_main.c"
        "ble_mesh_demo_init.c"
        "board.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS  ".")
