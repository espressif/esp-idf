function(register_ble_mesh_libs)
    if(NOT CONFIG_BLE_MESH)
        return()
    endif()

    if(CONFIG_BLE_MESH_V11_SUPPORT)
        add_prebuilt_library(ble_mesh "esp_ble_mesh/lib/lib/${idf_target}/libble_mesh.a")
        target_link_libraries(${COMPONENT_LIB} PRIVATE ble_mesh)
    endif()
endfunction()

set(ble_mesh_srcs "" PARENT_SCOPE)
set(ble_mesh_include_dirs "" PARENT_SCOPE)

# API headers that are used in the docs are also compiled
# even if CONFIG_BT_ENABLED=n as long as CONFIG_IDF_DOC_BUILD=y
if(CONFIG_IDF_DOC_BUILD)
    set(ble_mesh_include_dirs
        "${CMAKE_CURRENT_LIST_DIR}/common/include"
        "${CMAKE_CURRENT_LIST_DIR}/core"
        "${CMAKE_CURRENT_LIST_DIR}/core/include"
        "${CMAKE_CURRENT_LIST_DIR}/core/storage"
        "${CMAKE_CURRENT_LIST_DIR}/btc/include"
        "${CMAKE_CURRENT_LIST_DIR}/models/common/include"
        "${CMAKE_CURRENT_LIST_DIR}/models/client/include"
        "${CMAKE_CURRENT_LIST_DIR}/models/server/include"
        "${CMAKE_CURRENT_LIST_DIR}/api/core/include"
        "${CMAKE_CURRENT_LIST_DIR}/api/models/include"
        "${CMAKE_CURRENT_LIST_DIR}/api"
        # BLE Mesh v1.1 headers
        "${CMAKE_CURRENT_LIST_DIR}/lib/include"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/api/core/include"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/api/models/include"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/btc/include"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/include"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/dfu"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/mbt"
        PARENT_SCOPE
    )
    return()
endif()

if(NOT CONFIG_BLE_MESH)
    return()
endif()

if(CONFIG_BLE_MESH_USE_UNIFIED_CRYPTO)
    message(WARNING "This configuration path is deprecated and will be removed"
                    "in a future version. Please use the corresponding Kconfig"
                    "options under $IDF_PATH/components/bt/common/Kconfig.")
endif()

list(APPEND ble_mesh_srcs
    "${CMAKE_CURRENT_LIST_DIR}/api/core/esp_ble_mesh_ble_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/core/esp_ble_mesh_common_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/core/esp_ble_mesh_local_data_operation_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/core/esp_ble_mesh_low_power_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/core/esp_ble_mesh_networking_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/core/esp_ble_mesh_provisioning_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/core/esp_ble_mesh_proxy_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/models/esp_ble_mesh_config_model_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/models/esp_ble_mesh_generic_model_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/models/esp_ble_mesh_health_model_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/models/esp_ble_mesh_lighting_model_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/models/esp_ble_mesh_sensor_model_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/api/models/esp_ble_mesh_time_scene_model_api.c"
    "${CMAKE_CURRENT_LIST_DIR}/btc/btc_ble_mesh_ble.c"
    "${CMAKE_CURRENT_LIST_DIR}/btc/btc_ble_mesh_config_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/btc/btc_ble_mesh_generic_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/btc/btc_ble_mesh_health_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/btc/btc_ble_mesh_lighting_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/btc/btc_ble_mesh_prov.c"
    "${CMAKE_CURRENT_LIST_DIR}/btc/btc_ble_mesh_sensor_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/btc/btc_ble_mesh_time_scene_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/common/atomic.c"
    "${CMAKE_CURRENT_LIST_DIR}/common/buf.c"
    "${CMAKE_CURRENT_LIST_DIR}/common/common.c"
    "${CMAKE_CURRENT_LIST_DIR}/common/kernel.c"
    "${CMAKE_CURRENT_LIST_DIR}/common/mutex.c"
    "${CMAKE_CURRENT_LIST_DIR}/common/queue.c"
    "${CMAKE_CURRENT_LIST_DIR}/common/timer.c"
    "${CMAKE_CURRENT_LIST_DIR}/common/utils.c"

    "${CMAKE_CURRENT_LIST_DIR}/core/storage/settings_nvs.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/storage/settings_uid.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/storage/settings.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/access.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/adv_common.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/beacon.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/cfg_cli.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/cfg_srv.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/crypto.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/fast_prov.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/friend.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/health_cli.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/health_srv.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/heartbeat.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/local.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/lpn.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/main.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/net.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/prov_common.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/prov_node.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/prov_pvnr.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/proxy_client.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/proxy_server.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/pvnr_mgmt.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/rpl.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/scan.c"
    "${CMAKE_CURRENT_LIST_DIR}/core/test.c"
    "${CMAKE_CURRENT_LIST_DIR}/models/common/device_property.c"
    "${CMAKE_CURRENT_LIST_DIR}/models/common/model_common.c"
    "${CMAKE_CURRENT_LIST_DIR}/models/client/client_common.c"
    "${CMAKE_CURRENT_LIST_DIR}/models/client/generic_client.c"
    "${CMAKE_CURRENT_LIST_DIR}/models/client/lighting_client.c"
    "${CMAKE_CURRENT_LIST_DIR}/models/client/sensor_client.c"
    "${CMAKE_CURRENT_LIST_DIR}/models/client/time_scene_client.c"
    "${CMAKE_CURRENT_LIST_DIR}/models/server/generic_server.c"
    "${CMAKE_CURRENT_LIST_DIR}/models/server/lighting_server.c"
    "${CMAKE_CURRENT_LIST_DIR}/models/server/sensor_server.c"
    "${CMAKE_CURRENT_LIST_DIR}/models/server/server_common.c"
    "${CMAKE_CURRENT_LIST_DIR}/models/server/state_binding.c"
    "${CMAKE_CURRENT_LIST_DIR}/models/server/state_transition.c"
    "${CMAKE_CURRENT_LIST_DIR}/models/server/time_scene_server.c"
)

if(CONFIG_BLE_MESH_SUPPORT_MULTI_ADV)
    list(APPEND ble_mesh_srcs "${CMAKE_CURRENT_LIST_DIR}/core/ext_adv.c")
else()
    list(APPEND ble_mesh_srcs "${CMAKE_CURRENT_LIST_DIR}/core/adv.c")
endif()

if(CONFIG_BLE_MESH_SUPPORT_BLE_ADV)
    list(APPEND ble_mesh_srcs "${CMAKE_CURRENT_LIST_DIR}/core/ble_adv.c")
endif()

# Select crypto implementation based on config
if(CONFIG_BT_SMP_CRYPTO_STACK_TINYCRYPT)
    list(APPEND ble_mesh_srcs "${CMAKE_CURRENT_LIST_DIR}/common/crypto_tc.c")
elseif(CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS)
    if(CONFIG_MBEDTLS_VER_4_X_SUPPORT)
        list(APPEND ble_mesh_srcs "${CMAKE_CURRENT_LIST_DIR}/common/crypto_psa.c")
    else()
        list(APPEND ble_mesh_srcs "${CMAKE_CURRENT_LIST_DIR}/common/crypto_mbedtls.c")
    endif()
endif()

list(APPEND ble_mesh_include_dirs
    "${CMAKE_CURRENT_LIST_DIR}/common/include"
    "${CMAKE_CURRENT_LIST_DIR}/core"
    "${CMAKE_CURRENT_LIST_DIR}/core/include"
    "${CMAKE_CURRENT_LIST_DIR}/core/storage"
    "${CMAKE_CURRENT_LIST_DIR}/btc/include"
    "${CMAKE_CURRENT_LIST_DIR}/models/common/include"
    "${CMAKE_CURRENT_LIST_DIR}/models/client/include"
    "${CMAKE_CURRENT_LIST_DIR}/models/server/include"
    "${CMAKE_CURRENT_LIST_DIR}/api/core/include"
    "${CMAKE_CURRENT_LIST_DIR}/api/models/include"
    "${CMAKE_CURRENT_LIST_DIR}/api"
)

if(CONFIG_BLE_MESH_V11_SUPPORT)
    list(APPEND ble_mesh_include_dirs
        "${CMAKE_CURRENT_LIST_DIR}/lib/include"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/api/core/include"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/api/models/include"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/btc/include"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/include"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/dfu"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/mbt"
    )

    list(APPEND ble_mesh_srcs
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/api/core/esp_ble_mesh_agg_model_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/api/core/esp_ble_mesh_brc_model_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/api/core/esp_ble_mesh_cm_data_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/api/core/esp_ble_mesh_df_model_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/api/core/esp_ble_mesh_lcd_model_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/api/core/esp_ble_mesh_odp_model_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/api/core/esp_ble_mesh_prb_model_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/api/core/esp_ble_mesh_rpr_model_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/api/core/esp_ble_mesh_sar_model_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/api/core/esp_ble_mesh_srpl_model_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/api/models/esp_ble_mesh_mbt_model_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/api/models/esp_ble_mesh_dfu_model_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/api/models/esp_ble_mesh_dfu_slot_api.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/btc/btc_ble_mesh_agg_model.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/btc/btc_ble_mesh_brc_model.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/btc/btc_ble_mesh_df_model.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/btc/btc_ble_mesh_dfu_model.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/btc/btc_ble_mesh_dfu_slot.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/btc/btc_ble_mesh_lcd_model.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/btc/btc_ble_mesh_mbt_model.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/btc/btc_ble_mesh_odp_model.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/btc/btc_ble_mesh_prb_model.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/btc/btc_ble_mesh_rpr_model.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/btc/btc_ble_mesh_sar_model.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/btc/btc_ble_mesh_srpl_model.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/mbt/blob_srv.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/mbt/blob_cli.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/dfu/dfu_cli.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/dfu/dfu_srv.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/dfu/dfu_slot.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/dfu/dfu_metadata.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/dfu/dfd_srv.c"
        "${CMAKE_CURRENT_LIST_DIR}/v1.1/dfu/dfd_cli.c"
        "${CMAKE_CURRENT_LIST_DIR}/lib/ext.c"
    )

    if(CONFIG_BLE_MESH_SAR_ENHANCEMENT)
        list(APPEND ble_mesh_srcs "${CMAKE_CURRENT_LIST_DIR}/core/transport.enh.c")
    else()
        list(APPEND ble_mesh_srcs "${CMAKE_CURRENT_LIST_DIR}/core/transport.c")
    endif()
else()
    list(APPEND ble_mesh_srcs "${CMAKE_CURRENT_LIST_DIR}/core/transport.c")
endif()

# Export the variables to the parent scope
set(ble_mesh_srcs "${ble_mesh_srcs}" PARENT_SCOPE)
set(ble_mesh_include_dirs "${ble_mesh_include_dirs}" PARENT_SCOPE)
