/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "esp_log.h"
#include "nvs_flash.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "services/ras/ble_svc_ras.h"
#include "ble_chan_reflector.h"
#include "host/ble_cs.h"

#if CONFIG_EXAMPLE_EXTENDED_ADV
/* Raw AD structures, each one a length byte followed by type and value: general
 * discoverable flags, the Ranging Service UUID 0x185B and the complete name.
 */
static uint8_t ext_adv_pattern_1[] = {
    0x02, BLE_HS_ADV_TYPE_FLAGS, 0x06,
    0x03, BLE_HS_ADV_TYPE_COMP_UUIDS16, 0x5B, 0x18,
    0x14, BLE_HS_ADV_TYPE_COMP_NAME, 'N', 'I', 'M', 'B', 'L', 'E', '_', 'C', 'S', '_', 'R', 'E', 'F', 'L', 'E', 'C', 'T', 'O', 'R',
};
#endif

static const char *tag = "NimBLE_CS_REFLECTOR";
static int ble_chan_sound_refl_gap_event(struct ble_gap_event *event, void *arg);
void ble_store_config_init(void);

/* RAP 6.1 SHALL: use a random device address */
static uint8_t s_own_addr_type = BLE_OWN_ADDR_RANDOM;

static void
ble_chan_sound_refl_print_conn_desc(struct ble_gap_conn_desc *desc)
{
    MODLOG_DFLT(INFO, "handle=%d our_ota_addr_type=%d our_ota_addr=",
                desc->conn_handle, desc->our_ota_addr.type);
    print_addr(desc->our_ota_addr.val);
    MODLOG_DFLT(INFO, " our_id_addr_type=%d our_id_addr=",
                desc->our_id_addr.type);
    print_addr(desc->our_id_addr.val);
    MODLOG_DFLT(INFO, " peer_ota_addr_type=%d peer_ota_addr=",
                desc->peer_ota_addr.type);
    print_addr(desc->peer_ota_addr.val);
    MODLOG_DFLT(INFO, " peer_id_addr_type=%d peer_id_addr=",
                desc->peer_id_addr.type);
    print_addr(desc->peer_id_addr.val);
    MODLOG_DFLT(INFO, " conn_itvl=%d conn_latency=%d supervision_timeout=%d "
                "encrypted=%d authenticated=%d bonded=%d\n",
                desc->conn_itvl, desc->conn_latency,
                desc->supervision_timeout,
                desc->sec_state.encrypted,
                desc->sec_state.authenticated,
                desc->sec_state.bonded);
}

#if CONFIG_EXAMPLE_EXTENDED_ADV
static void
ble_chan_sound_refl_ext_advertise(void)
{
    struct ble_gap_ext_adv_params params;
    struct os_mbuf *data;
    uint8_t instance = 0;
    int rc;

    if (ble_gap_ext_adv_active(instance)) {
        return;
    }

    memset(&params, 0, sizeof(params));
    params.connectable = 1;
    params.own_addr_type = s_own_addr_type;
    /* Advertise with legacy ADV_IND PDUs for the widest initiator
     * compatibility, so the connection is created on the 1M PHY.
     * legacy_pdu forces both PHYs to 1M; connectable legacy also
     * requires scannable (ADV_IND). */
    params.legacy_pdu = 1;
    params.scannable = 1;
    params.sid = 1;
    params.itvl_min = BLE_GAP_ADV_FAST_INTERVAL1_MIN;
    params.itvl_max = BLE_GAP_ADV_FAST_INTERVAL1_MIN;

    rc = ble_gap_ext_adv_configure(instance, &params, NULL,
                                   ble_chan_sound_refl_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "ble_chan_sound_refl_ext_advertise: configure failed rc=%d\n", rc);
        return;
    }

    data = os_msys_get_pkthdr(sizeof(ext_adv_pattern_1), 0);
    if (data == NULL) {
        MODLOG_DFLT(ERROR, "ble_chan_sound_refl_ext_advertise: no mbuf for adv data\n");
        return;
    }

    rc = os_mbuf_append(data, ext_adv_pattern_1, sizeof(ext_adv_pattern_1));
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "ble_chan_sound_refl_ext_advertise: mbuf append failed rc=%d\n", rc);
        os_mbuf_free_chain(data);
        return;
    }

    rc = ble_gap_ext_adv_set_data(instance, data);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "ble_chan_sound_refl_ext_advertise: set_data failed rc=%d (host may not be synced yet)\n", rc);
        return;
    }

    rc = ble_gap_ext_adv_start(instance, 0, 0);
    if (rc != 0) {
        if (rc == BLE_HS_HCI_ERR(BLE_ERR_CONN_REJ_RESOURCES)) {
            /* BLE_ERR_CONN_REJ_RESOURCES: controller has no room for more connections/adv.
             * Often happens right after disconnect while controller is still freeing the connection.
             * ADV_COMPLETE will fire when the previous instance stops; we will try again then. */
            MODLOG_DFLT(WARN, "ble_chan_sound_refl_ext_advertise: start failed rc=%d (limited resources, will retry on next ADV_COMPLETE)\n", rc);
        } else {
            MODLOG_DFLT(ERROR, "ble_chan_sound_refl_ext_advertise: start failed rc=%d\n", rc);
        }
    }
}
#else
static void
ble_chan_sound_refl_advertise(void)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    const char *name;
    int rc;

    memset(&fields, 0, sizeof fields);
    fields.flags = BLE_HS_ADV_F_DISC_GEN |
                   BLE_HS_ADV_F_BREDR_UNSUP;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
#if CONFIG_BT_NIMBLE_GAP_SERVICE
    name = ble_svc_gap_device_name();
    fields.name = (uint8_t *)name;
    fields.name_len = strlen(name);
    fields.name_is_complete = 1;
#endif
    fields.uuids16 = (ble_uuid16_t[]) {
        BLE_UUID16_INIT(BLE_SVC_RAS_RANGING_SERVICE_VAL)
    };
    fields.num_uuids16 = 1;
    fields.uuids16_is_complete = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error setting advertisement data; rc=%d\n", rc);
        return;
    }

    memset(&adv_params, 0, sizeof adv_params);
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    rc = ble_gap_adv_start(s_own_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, ble_chan_sound_refl_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error enabling advertisement; rc=%d\n", rc);
        return;
    }
}
#endif

static int
ble_chan_sound_refl_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        MODLOG_DFLT(INFO, "connection %s; status=%d ",
                    event->connect.status == 0 ? "established" : "failed",
                    event->connect.status);
        if (event->connect.status == 0) {
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            ble_chan_sound_refl_print_conn_desc(&desc);
        }
        MODLOG_DFLT(INFO, "\n");

        if (event->connect.status != 0) {
#if CONFIG_EXAMPLE_EXTENDED_ADV
            ble_chan_sound_refl_ext_advertise();
#else
            ble_chan_sound_refl_advertise();
#endif
            return 0;
        }

        /* Enable the reflector role immediately, before pairing. LE CS Set
         * Default Settings is a local command and needs no encryption; the
         * initiator may send LL_CS_CAPABILITIES_REQ or LL_CS_CONFIG_REQ as
         * soon as the link is up, and the controller rejects both until
         * Role_Enable has been applied. */
        {
            struct ble_cs_reflector_setup_params params;
#if CONFIG_BT_NIMBLE_RAS_SERVICE
            params.cb = ble_svc_ras_cs_event_handler;
#else
            params.cb = NULL;
#endif
            params.cb_arg = NULL;
            params.conn_handle = event->connect.conn_handle;
            rc = ble_cs_reflector_setup(&params);
            if (rc != 0) {
                MODLOG_DFLT(ERROR, "ble_cs_reflector_setup failed rc=%d conn=%d\n",
                            rc, event->connect.conn_handle);
            } else {
                MODLOG_DFLT(INFO, "CS reflector setup complete conn=%d\n",
                            event->connect.conn_handle);
            }
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "disconnect; reason=%d ", event->disconnect.reason);
        ble_chan_sound_refl_print_conn_desc(&event->disconnect.conn);
        MODLOG_DFLT(INFO, "\n");

#if CONFIG_BT_NIMBLE_RAS_SERVICE
        ble_svc_ras_on_disconnect(event->disconnect.conn.conn_handle);
#endif

#if MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)
        /* In a reflector-only setup the stack never frees the CS context
         * (see ble_cs.h); release it here so no CS RAM is held while
         * advertising.  The next connection re-allocates it inside
         * ble_cs_reflector_setup(). */
        ble_cs_state_deinit();
#endif

#if CONFIG_BT_HCI_LOG_DEBUG_EN
        /* Drain the HCI ring to the console before advertising again: the dump
         * runs on the host task and blocks it for the whole transfer, so a peer
         * that reconnected first would be serviced late. */
        extern void bt_hci_log_hci_data_show(void);
        bt_hci_log_hci_data_show();
#endif

#if CONFIG_EXAMPLE_EXTENDED_ADV
        ble_chan_sound_refl_ext_advertise();
#else
        ble_chan_sound_refl_advertise();
#endif
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        MODLOG_DFLT(DEBUG, "connection updated; status=%d ",
                    event->conn_update.status);
        rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
        assert(rc == 0);
        ble_chan_sound_refl_print_conn_desc(&desc);
        MODLOG_DFLT(DEBUG, "\n");
        return 0;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        MODLOG_DFLT(INFO, "advertise complete; reason=%d",
                    event->adv_complete.reason);
        /* reason == 0 means the set terminated because a connection was
         * created; a CONNECT event always follows and restarts advertising
         * itself if that connection failed.  Restarting here instead races
         * the controller while it is still setting the connection up, which
         * it rejects with BLE_ERR_CONN_REJ_RESOURCES. */
        if (event->adv_complete.reason == 0) {
            return 0;
        }
#if CONFIG_EXAMPLE_EXTENDED_ADV
        ble_chan_sound_refl_ext_advertise();
#else
        ble_chan_sound_refl_advertise();
#endif
        return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
        MODLOG_DFLT(INFO, "encryption change event; status=%d ",
                    event->enc_change.status);
        rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
        assert(rc == 0);
        ble_chan_sound_refl_print_conn_desc(&desc);
        MODLOG_DFLT(INFO, "\n");

        /* Channel Sounding requires an encrypted LE Secure Connections link
         * (RAP 6.2).  sm_sc only advertises SC support in the pairing
         * AuthReq, so a peer that leaves that bit clear still completes
         * legacy pairing and still reports encrypted; check sc as well and
         * drop the link rather than serve ranging data over it. */
        if (event->enc_change.status != 0 || !desc.sec_state.encrypted ||
            !desc.sec_state.sc) {
            MODLOG_DFLT(ERROR, "encryption failed/missing or not LE Secure "
                        "Connections (status=%d enc=%d sc=%d) "
                        "- terminating conn=%d\n",
                        event->enc_change.status,
                        desc.sec_state.encrypted,
                        desc.sec_state.sc,
                        event->enc_change.conn_handle);
            return ble_gap_terminate(event->enc_change.conn_handle,
                                     BLE_ERR_AUTH_FAIL);
        }
        return 0;

    case BLE_GAP_EVENT_NOTIFY_TX:
        MODLOG_DFLT(DEBUG, "notify_tx event; conn_handle=%d attr_handle=%d "
                    "status=%d is_indication=%d",
                    event->notify_tx.conn_handle,
                    event->notify_tx.attr_handle,
                    event->notify_tx.status,
                    event->notify_tx.indication);
        return 0;

    case BLE_GAP_EVENT_SUBSCRIBE:
        MODLOG_DFLT(DEBUG, "subscribe event; conn_handle=%d attr_handle=%d "
                    "reason=%d prevn=%d curn=%d previ=%d curi=%d\n",
                    event->subscribe.conn_handle,
                    event->subscribe.attr_handle,
                    event->subscribe.reason,
                    event->subscribe.prev_notify,
                    event->subscribe.cur_notify,
                    event->subscribe.prev_indicate,
                    event->subscribe.cur_indicate);
        return 0;

    case BLE_GAP_EVENT_MTU:
        MODLOG_DFLT(DEBUG, "mtu update event; conn_handle=%d cid=%d mtu=%d\n",
                    event->mtu.conn_handle,
                    event->mtu.channel_id,
                    event->mtu.value);
        return 0;
    }

    return 0;
}

static void
ble_chan_sound_refl_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

static void
ble_chan_sound_refl_on_sync(void)
{
    int rc;
    uint8_t own_addr_type = 0;

    /* Feature bit 47 is LE Channel Sounding Host Support.  The controller
     * refuses every CS command until the host has set it, so this has to happen
     * before anything else CS related.
     */
    MODLOG_DFLT(INFO, "TX HCI LE Set Host Feature (opcode 0x2074) bit=47 "
                "(LE Channel Sounding Host Support) val=1\n");
    rc = ble_gap_set_host_feat(47, 0x01);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "LE Set Host Feature (CS host support) FAILED; rc=%d\n", rc);
    } else {
        MODLOG_DFLT(INFO, "LE Set Host Feature (CS host support) accepted by controller\n");
    }

    {
        struct ble_cs_local_supp_cap cap;

        rc = ble_cs_read_local_supp_cap(&cap);
        if (rc != 0) {
            MODLOG_DFLT(ERROR, "LE CS Read Local Supported Capabilities FAILED; rc=%d\n", rc);
        } else {
            MODLOG_DFLT(INFO, "CS local caps: num_antennas=%u max_antenna_paths=%u "
                        "roles=0x%02x modes=0x%02x num_configs=%u max_consec_proc=%u\n",
                        cap.num_antennas_supported, cap.max_antenna_paths_supported,
                        cap.roles_supported, cap.optional_modes_supported,
                        cap.num_config_supported, cap.max_consecutive_procedures_supported);
            MODLOG_DFLT(INFO, "CS local caps: rtt_cap=0x%02x aa_only_n=%u sounding_n=%u "
                        "random_n=%u cs_sync_phys=0x%02x subfeatures=0x%04x t_sw=%u "
                        "tx_snr=0x%02x\n",
                        cap.rtt_capability, cap.rtt_aa_only_n, cap.rtt_sounding_n,
                        cap.rtt_random_payload_n, cap.optional_cs_sync_phys_supported,
                        cap.optional_subfeatures_supported, cap.t_sw_time_supported,
                        cap.tx_snr_capability);
        }
    }

    /* prefer_random=0 is correct here: ble_hs_id_infer_auto(1) falls back
     * to BLE_OWN_ADDR_RPA_PUBLIC_DEFAULT when no random identity exists,
     * which still uses an RPA on the air (satisfies RAP 6.1). */
    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    /* RAP 6.1 SHALL: use a random device address */
    rc = ble_hs_id_infer_auto(1, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
        return;
    }
    s_own_addr_type = own_addr_type;

    uint8_t addr_val[6] = {0};
    rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);
    MODLOG_DFLT(INFO, "Device Address: ");
    print_addr(addr_val);
    MODLOG_DFLT(INFO, "\n");

#if CONFIG_EXAMPLE_EXTENDED_ADV
    ble_chan_sound_refl_ext_advertise();
#else
    ble_chan_sound_refl_advertise();
#endif
}

void ble_chan_sound_refl_host_task(void *param)
{
    ESP_LOGI(tag, "BLE Host Task Started");
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void
app_main(void)
{
    int rc;

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Failed to init nimble %d ", ret);
        return;
    }

    ble_hs_cfg.reset_cb = ble_chan_sound_refl_on_reset;
    ble_hs_cfg.sync_cb = ble_chan_sound_refl_on_sync;
#if CONFIG_BT_NIMBLE_RAS_SERVICE
    ble_hs_cfg.gatts_register_cb = ble_svc_ras_register_cb;
#endif
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
    ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_NO_IO;

#ifdef CONFIG_EXAMPLE_BONDING
    ble_hs_cfg.sm_bonding = 1;
    ble_hs_cfg.sm_our_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC;
    ble_hs_cfg.sm_their_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC;
#endif
#ifdef CONFIG_EXAMPLE_MITM
    ble_hs_cfg.sm_mitm = 1;
#endif
    /* RAP 6.2 SHALL: LE Secure Connections pairing (unconditional) */
    ble_hs_cfg.sm_sc = 1;
#ifdef CONFIG_EXAMPLE_RESOLVE_PEER_ADDR
    ble_hs_cfg.sm_our_key_dist |= BLE_SM_PAIR_KEY_DIST_ID;
    ble_hs_cfg.sm_their_key_dist |= BLE_SM_PAIR_KEY_DIST_ID;
#endif

    rc = gatt_svr_init();
    assert(rc == 0);

#if CONFIG_BT_NIMBLE_GAP_SERVICE
    rc = ble_svc_gap_device_name_set("NIMBLE_CS_REFLECTOR");
    assert(rc == 0);
#endif

    ble_store_config_init();

    nimble_port_freertos_init(ble_chan_sound_refl_host_task);
}
