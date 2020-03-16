if(CONFIG_NIMBLE_ENABLED)

    set(COMPONENT_ADD_INCLUDEDIRS
            nimble/porting/nimble/include
            port/include
            nimble/nimble/include
            nimble/nimble/host/include
            nimble/nimble/host/services/ans/include
            nimble/nimble/host/services/bas/include
            nimble/nimble/host/services/gap/include
            nimble/nimble/host/services/gatt/include
            nimble/nimble/host/services/ias/include
            nimble/nimble/host/services/lls/include
            nimble/nimble/host/services/tps/include
            nimble/nimble/host/util/include
            nimble/nimble/host/store/ram/include
            nimble/nimble/host/store/config/include
            nimble/porting/npl/freertos/include
            esp-hci/include)


    set(COMPONENT_SRCS "./nimble/nimble/host/util/src/addr.c"
            "./nimble/nimble/host/services/gatt/src/ble_svc_gatt.c"
            "./nimble/nimble/host/services/tps/src/ble_svc_tps.c"
            "./nimble/nimble/host/services/ias/src/ble_svc_ias.c"
            "./nimble/nimble/host/services/ans/src/ble_svc_ans.c"
            "./nimble/nimble/host/services/gap/src/ble_svc_gap.c"
            "./nimble/nimble/host/services/bas/src/ble_svc_bas.c"
            "./nimble/nimble/host/services/lls/src/ble_svc_lls.c"
            "./nimble/nimble/host/src/ble_hs_conn.c"
            "./nimble/nimble/host/src/ble_store_util.c"
            "./nimble/nimble/host/src/ble_sm.c"
            "./nimble/nimble/host/src/ble_hs_shutdown.c"
            "./nimble/nimble/host/src/ble_l2cap_sig_cmd.c"
            "./nimble/nimble/host/src/ble_hs_hci_cmd.c"
            "./nimble/nimble/host/src/ble_hs_id.c"
            "./nimble/nimble/host/src/ble_att_svr.c"
            "./nimble/nimble/host/src/ble_gatts_lcl.c"
            "./nimble/nimble/host/src/ble_ibeacon.c"
            "./nimble/nimble/host/src/ble_hs_atomic.c"
            "./nimble/nimble/host/src/ble_sm_alg.c"
            "./nimble/nimble/host/src/ble_hs_stop.c"
            "./nimble/nimble/host/src/ble_hs.c"
            "./nimble/nimble/host/src/ble_hs_hci_evt.c"
            "./nimble/nimble/host/src/ble_hs_dbg.c"
            "./nimble/nimble/host/src/ble_hs_mqueue.c"
            "./nimble/nimble/host/src/ble_att.c"
            "./nimble/nimble/host/src/ble_gattc.c"
            "./nimble/nimble/host/src/ble_store.c"
            "./nimble/nimble/host/src/ble_sm_lgcy.c"
            "./nimble/nimble/host/src/ble_hs_cfg.c"
            "./nimble/nimble/host/src/ble_monitor.c"
            "./nimble/nimble/host/src/ble_att_clt.c"
            "./nimble/nimble/host/src/ble_l2cap_coc.c"
            "./nimble/nimble/host/src/ble_hs_mbuf.c"
            "./nimble/nimble/host/src/ble_att_cmd.c"
            "./nimble/nimble/host/src/ble_hs_log.c"
            "./nimble/nimble/host/src/ble_eddystone.c"
            "./nimble/nimble/host/src/ble_hs_startup.c"
            "./nimble/nimble/host/src/ble_l2cap_sig.c"
            "./nimble/nimble/host/src/ble_gap.c"
            "./nimble/nimble/host/src/ble_sm_cmd.c"
            "./nimble/nimble/host/src/ble_uuid.c"
            "./nimble/nimble/host/src/ble_hs_pvcy.c"
            "./nimble/nimble/host/src/ble_hs_flow.c"
            "./nimble/nimble/host/src/ble_l2cap.c"
            "./nimble/nimble/host/src/ble_sm_sc.c"
            "./nimble/nimble/host/src/ble_hs_misc.c"
            "./nimble/nimble/host/src/ble_gatts.c"
            "./nimble/nimble/host/src/ble_hs_adv.c"
            "./nimble/nimble/host/src/ble_hs_hci.c"
            "./nimble/nimble/host/src/ble_hs_hci_util.c"
            "./nimble/nimble/host/src/ble_hs_resolv.c"
            "./nimble/nimble/host/store/ram/src/ble_store_ram.c"
            "./nimble/nimble/host/store/config/src/ble_store_config.c"
            "./nimble/nimble/host/store/config/src/ble_store_nvs.c"
            "./nimble/nimble/src/ble_util.c"
            "./nimble/porting/npl/freertos/src/nimble_port_freertos.c"
            "./nimble/porting/npl/freertos/src/npl_os_freertos.c"
            "./nimble/porting/nimble/src/endian.c"
            "./nimble/porting/nimble/src/os_cputime_pwr2.c"
            "./nimble/porting/nimble/src/hal_timer.c"
            "./nimble/porting/nimble/src/os_mempool.c"
            "./nimble/porting/nimble/src/os_msys_init.c"
            "./nimble/porting/nimble/src/nimble_port.c"
            "./nimble/porting/nimble/src/mem.c"
            "./nimble/porting/nimble/src/os_mbuf.c"
            "./nimble/porting/nimble/src/os_cputime.c"
            "./esp-hci/src/esp_nimble_hci.c"
            "./port/src/esp_nimble_mem.c")

    if(NOT CONFIG_NIMBLE_CRYPTO_STACK_MBEDTLS)

        list(APPEND COMPONENT_ADD_INCLUDEDIRS
            nimble/ext/tinycrypt/include)

        list(APPEND COMPONENT_SRCS "./nimble/ext/tinycrypt/src/utils.c"
            "./nimble/ext/tinycrypt/src/sha256.c"
            "./nimble/ext/tinycrypt/src/ecc.c"
            "./nimble/ext/tinycrypt/src/ctr_prng.c"
            "./nimble/ext/tinycrypt/src/ctr_mode.c"
            "./nimble/ext/tinycrypt/src/aes_decrypt.c"
            "./nimble/ext/tinycrypt/src/aes_encrypt.c"
            "./nimble/ext/tinycrypt/src/ccm_mode.c"
            "./nimble/ext/tinycrypt/src/ecc_dsa.c"
            "./nimble/ext/tinycrypt/src/cmac_mode.c"
            "./nimble/ext/tinycrypt/src/ecc_dh.c"
            "./nimble/ext/tinycrypt/src/hmac_prng.c"
            "./nimble/ext/tinycrypt/src/ecc_platform_specific.c"
            "./nimble/ext/tinycrypt/src/hmac.c"
            "./nimble/ext/tinycrypt/src/cbc_mode.c")
    endif()


    if(CONFIG_NIMBLE_MESH)

        list(APPEND COMPONENT_ADD_INCLUDEDIRS
            nimble/nimble/host/mesh/include)

        list(APPEND COMPONENT_SRCS
            "./nimble/nimble/host/mesh/src/shell.c"
            "./nimble/nimble/host/mesh/src/friend.c"
            "./nimble/nimble/host/mesh/src/crypto.c"
            "./nimble/nimble/host/mesh/src/settings.c"
            "./nimble/nimble/host/mesh/src/adv.c"
            "./nimble/nimble/host/mesh/src/model_srv.c"
            "./nimble/nimble/host/mesh/src/beacon.c"
            "./nimble/nimble/host/mesh/src/glue.c"
            "./nimble/nimble/host/mesh/src/model_cli.c"
            "./nimble/nimble/host/mesh/src/transport.c"
            "./nimble/nimble/host/mesh/src/prov.c"
            "./nimble/nimble/host/mesh/src/mesh.c"
            "./nimble/nimble/host/mesh/src/access.c"
            "./nimble/nimble/host/mesh/src/cfg_srv.c"
            "./nimble/nimble/host/mesh/src/cfg_cli.c"
            "./nimble/nimble/host/mesh/src/light_model.c"
            "./nimble/nimble/host/mesh/src/health_cli.c"
            "./nimble/nimble/host/mesh/src/lpn.c"
            "./nimble/nimble/host/mesh/src/proxy.c"
            "./nimble/nimble/host/mesh/src/health_srv.c"
            "./nimble/nimble/host/mesh/src/testing.c"
            "./nimble/nimble/host/mesh/src/net.c")

    endif()
endif()

# requirements can't depend on config
set(COMPONENT_PRIV_REQUIRES bt nvs_flash)

register_component()
