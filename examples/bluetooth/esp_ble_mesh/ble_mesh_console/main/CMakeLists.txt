set(srcs "ble_mesh_adapter.c"
        "ble_mesh_model.c"
        "ble_mesh_console_lib.c"
        "ble_mesh_console_main.c"
        "ble_mesh_console_system.c"
        "ble_mesh_register_cmd.c"
        "ble_mesh_reg_cfg_client_cmd.c"
        "ble_mesh_register_server_cmd.c"
        "ble_mesh_reg_gen_onoff_client_cmd.c"
        "ble_mesh_reg_test_perf_client_cmd.c"
        "transaction.c"
        "register_bluetooth.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS  ".")
