set(srcs "ble_mesh_fast_prov_client_model.c"
        "ble_mesh_fast_prov_server_model.c"
        "ble_mesh_fast_prov_operation.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS  "."
                    REQUIRES bt)
