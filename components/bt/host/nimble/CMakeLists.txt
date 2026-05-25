function(set_nimble_host_compile_flags)
    if(NOT CONFIG_BT_NIMBLE_ENABLED)
        return()
    endif()

    # TODO: These warnings should be resolved in the NimBLE code
    if(CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE)
        # some variables in NimBLE are only used by asserts
        target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-but-set-variable -Wno-unused-variable)
    endif()

    if(CMAKE_C_COMPILER_ID MATCHES "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER 15.0)
        if(CONFIG_BT_NIMBLE_MESH)
            set_source_files_properties("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/nimble/nimble/host/mesh/src/prov.c"
                PROPERTIES COMPILE_FLAGS "-Wno-unterminated-string-initialization")
        endif()
    endif()
endfunction()

set(nimble_host_srcs "" PARENT_SCOPE)
set(nimble_host_include_dirs "" PARENT_SCOPE)

# API headers that are used in the docs are also compiled
# even if CONFIG_BT_ENABLED=n as long as CONFIG_IDF_DOC_BUILD=y
if(CONFIG_IDF_DOC_BUILD)
    set(nimble_host_include_dirs
        "${CMAKE_CURRENT_LIST_DIR}/esp-hci/include"
        PARENT_SCOPE
    )
    return()
endif()

if(NOT CONFIG_BT_NIMBLE_ENABLED)
    return()
endif()

list(APPEND nimble_host_srcs
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/transport/src/transport.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/util/src/addr.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/gatt/src/ble_svc_gatt.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/tps/src/ble_svc_tps.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/ias/src/ble_svc_ias.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/ipss/src/ble_svc_ipss.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/ans/src/ble_svc_ans.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/hr/src/ble_svc_hr.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/htp/src/ble_svc_htp.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/gap/src/ble_svc_gap.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/bas/src/ble_svc_bas.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/dis/src/ble_svc_dis.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/lls/src/ble_svc_lls.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/prox/src/ble_svc_prox.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/cts/src/ble_svc_cts.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/hid/src/ble_svc_hid.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/sps/src/ble_svc_sps.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/cte/src/ble_svc_cte.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/ras/src/ble_svc_ras.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_cs.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_conn.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_store_util.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_sm.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_shutdown.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_l2cap_sig_cmd.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_hci_cmd.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_id.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_att_svr.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_gatts_lcl.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_ibeacon.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_atomic.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_sm_alg.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_stop.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_hci_evt.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_mqueue.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_periodic_sync.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_att.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_ead.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_aes_ccm.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_gattc.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_store.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_sm_lgcy.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_cfg.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_att_clt.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_l2cap_coc.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_mbuf.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_att_cmd.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_log.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_eddystone.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_startup.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_l2cap_sig.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_gap.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_sm_cmd.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_uuid.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_pvcy.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_flow.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_l2cap.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_sm_sc.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_misc.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_gatts.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_adv.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_hci.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_hci_util.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_resolv.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/store/ram/src/ble_store_ram.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/store/config/src/ble_store_config.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/store/config/src/ble_store_nvs.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_gattc_cache.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_gattc_cache_conn.c"
    "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_eatt.c"
    # NimBLE Porting
    "${CMAKE_CURRENT_LIST_DIR}/nimble/porting/nimble/src/nimble_port.c"
    "${CMAKE_CURRENT_LIST_DIR}/port/src/nvs_port.c"
    "${CMAKE_CURRENT_LIST_DIR}/port/src/esp_nimble_mem.c"
    # TODO: Add this file in the blufi cmake file
    "${CMAKE_CURRENT_LIST_DIR}/../../common/btc/profile/esp/blufi/nimble_host/esp_blufi.c"
)

# TODO: Added this file in the ble mesh cmake file
if(CONFIG_BLE_MESH)
    list(APPEND nimble_host_srcs "${CMAKE_CURRENT_LIST_DIR}/../../esp_ble_mesh/core/nimble_host/adapter.c")
endif()

if(CONFIG_BT_NIMBLE_ISO)
    list(APPEND nimble_host_srcs
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_iso_hci.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/src/ble_hs_iso.c"
    )
endif()

# Nimble porting layer for FreeRTOS
if(CONFIG_BT_DUAL_MODE_ARCH)
    list(APPEND nimble_host_srcs
        "${CMAKE_CURRENT_LIST_DIR}/nimble/porting/npl/esp-idf/src/nimble_port_freertos.c"
    )
    list(APPEND nimble_host_include_dirs
        "${CMAKE_CURRENT_LIST_DIR}/nimble/porting/npl/esp-idf/include"
    )
else()
    list(APPEND nimble_host_srcs
        "${CMAKE_CURRENT_LIST_DIR}/nimble/porting/npl/freertos/src/nimble_port_freertos.c"
    )
endif()

if(CONFIG_BT_CONTROLLER_DISABLED)
    # UART HCI driver for host only mode
    if(CONFIG_BT_NIMBLE_TRANSPORT_UART)
        list(APPEND nimble_host_srcs
            "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/transport/uart_ll/src/hci_uart.c"
            "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/transport/common/hci_h4/src/hci_h4.c"
            "${CMAKE_CURRENT_LIST_DIR}/nimble/porting/nimble/src/hal_uart.c"
        )
    endif()
else()
    # Inter-Process Communication (VHCI)
    if(CONFIG_BT_NIMBLE_LEGACY_VHCI_ENABLE)
        list(APPEND nimble_host_srcs
            "${CMAKE_CURRENT_LIST_DIR}/esp-hci/src/esp_nimble_hci.c"
            "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/transport/esp_ipc_legacy/src/hci_esp_ipc_legacy.c"
        )
        list(APPEND nimble_host_include_dirs
            ${CMAKE_CURRENT_LIST_DIR}/esp-hci/include
        )
    elseif(CONFIG_BT_DUAL_MODE_ARCH)
        list(APPEND nimble_host_srcs
            "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/transport/esp_ipc_btdm/src/hci_esp_ipc.c"
        )
    else()
        list(APPEND nimble_host_srcs
            "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/transport/esp_ipc/src/hci_esp_ipc.c"
        )
    endif()
endif()

# Reuse the code from the controller to reduce flash usage
if(CONFIG_BT_CONTROLLER_DISABLED OR NOT CONFIG_SOC_ESP_NIMBLE_CONTROLLER)
    list(APPEND nimble_host_srcs
        "${CMAKE_CURRENT_LIST_DIR}/nimble/porting/nimble/src/endian.c"
        #TODO: Split this into a separate file for the controller and the host
        "${CMAKE_CURRENT_LIST_DIR}/../../porting/mem/os_mempool.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/porting/nimble/src/mem.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/porting/nimble/src/os_mbuf.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/porting/nimble/src/os_msys_init.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/porting/npl/freertos/src/npl_os_freertos.c"
    )
endif()

list(APPEND nimble_host_include_dirs
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/ans/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/bas/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/dis/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/gap/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/gatt/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/hr/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/htp/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/ias/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/ipss/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/lls/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/prox/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/cts/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/tps/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/hid/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/sps/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/cte/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/util/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/store/ram/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/store/config/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/services/ras/include

    ${CMAKE_CURRENT_LIST_DIR}/nimble/porting/nimble/include
    ${CMAKE_CURRENT_LIST_DIR}/port/include
    ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/transport/include
)

if(CONFIG_BT_CONTROLLER_DISABLED)
    list(APPEND nimble_host_include_dirs
        ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/transport/common/hci_h4/include
    )
endif()

if(CONFIG_BT_CONTROLLER_DISABLED OR NOT CONFIG_SOC_ESP_NIMBLE_CONTROLLER)
    list(APPEND nimble_host_include_dirs
        ${CMAKE_CURRENT_LIST_DIR}/../../porting/include
        ${CMAKE_CURRENT_LIST_DIR}/nimble/porting/npl/freertos/include
    )
endif()

# BLE NIMBLE MESH (Deprecated)
if(CONFIG_BT_NIMBLE_MESH)
    list(APPEND nimble_host_srcs
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/shell.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/friend.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/crypto.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/settings.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/adv.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/adv_ext.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/adv_legacy.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/model_srv.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/msg.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/beacon.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/glue.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/model_cli.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/transport.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/prov.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/mesh.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/access.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/cfg_srv.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/cfg_cli.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/light_model.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/health_cli.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/lpn.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/health_srv.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/testing.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/aes-ccm.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/app_keys.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/cdb.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/cfg.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/pb_adv.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/pb_gatt.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/pb_gatt_srv.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/prov_device.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/provisioner.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/heartbeat.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/rpl.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/subnet.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/proxy_msg.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/proxy_srv.c"
        "${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/net.c"
    )

    list(APPEND nimble_host_include_dirs
        ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/include
        ${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/include/host
    )

    set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/nimble/nimble/host/mesh/src/net.c"
        PROPERTIES COMPILE_FLAGS -Wno-type-limits)
endif()

# Export the variables to the parent scope
set(nimble_host_srcs "${nimble_host_srcs}" PARENT_SCOPE)
set(nimble_host_include_dirs "${nimble_host_include_dirs}" PARENT_SCOPE)
