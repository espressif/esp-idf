set(COMPONENT_SRCS "ble_mesh_fast_prov_client_model.c"
                   "ble_mesh_fast_prov_server_model.c"
                   "ble_mesh_fast_prov_operation.c")

set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_REQUIRES bt)

register_component()
