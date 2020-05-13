set(srcs "ble_mesh_demo_main.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS  ".")
