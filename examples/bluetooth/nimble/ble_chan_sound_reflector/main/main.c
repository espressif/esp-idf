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
/* BLE */
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
static uint8_t ext_adv_pattern_1[] = {
    0x02, BLE_HS_ADV_TYPE_FLAGS, 0x06,
    0x03, BLE_HS_ADV_TYPE_COMP_UUIDS16, 0xab, 0xcd,
    0x03, BLE_HS_ADV_TYPE_COMP_UUIDS16, 0x5B, 0x18,
    0x11, BLE_HS_ADV_TYPE_COMP_NAME, 'n', 'i', 'm', 'b', 'l', 'e', '-', 'b', 'l', 'e', 'p', 'r', 'p', 'h', '-', 'e',
};
#endif
static const char *tag = "NimBLE_BLE_CHAN_REFLECTOR";
static int bleprph_gap_event(struct ble_gap_event *event, void *arg);
void ble_store_config_init(void);

struct ble_cs_event ranging_subevent;
static int32_t most_recent_local_ranging_counter= -1 ;

static int ind;
static int idx;

void print_cs_event(const struct ble_cs_event *event)
{
    MODLOG_DFLT(INFO, "ble_cs_event.type = %u\n", event->type);

    switch (event->type) {
        case BLE_CS_EVENT_CS_PROCEDURE_COMPLETE:
            MODLOG_DFLT(INFO, "procedure_complete.conn_handle = %u\n", event->procedure_complete.conn_handle);
            MODLOG_DFLT(INFO, "procedure_complete.status = %u\n", event->procedure_complete.status);
            break;
        case BLE_CS_EVENT_SUBEVET_RESULT:
            MODLOG_DFLT(INFO, "subev_result.conn_handle = %u\n", event->subev_result.conn_handle);
            MODLOG_DFLT(INFO, "subev_result.config_id = %u\n", event->subev_result.config_id);
            MODLOG_DFLT(INFO, "subev_result.start_acl_conn_event_counter = %u\n", event->subev_result.start_acl_conn_event_counter);
            MODLOG_DFLT(INFO, "subev_result.procedure_counter = %u\n", event->subev_result.procedure_counter);
            MODLOG_DFLT(INFO, "subev_result.frequency_compensation = %u\n", event->subev_result.frequency_compensation);
            MODLOG_DFLT(INFO, "subev_result.reference_power_level = %u\n", event->subev_result.reference_power_level);
            MODLOG_DFLT(INFO, "subev_result.procedure_done_status = %u\n", event->subev_result.procedure_done_status);
            MODLOG_DFLT(INFO, "subev_result.subevent_done_status = %u\n", event->subev_result.subevent_done_status);
            MODLOG_DFLT(INFO, "subev_result.abort_reason = %u\n", event->subev_result.abort_reason);
            MODLOG_DFLT(INFO, "subev_result.num_antenna_paths = %u\n", event->subev_result.num_antenna_paths);
            MODLOG_DFLT(INFO, "subev_result.num_steps_reported = %u\n", event->subev_result.num_steps_reported);

            for (int i = 0; i < event->subev_result.num_steps_reported; i++) {
                const struct cs_steps_data *step = &event->subev_result.steps[i];
                MODLOG_DFLT(INFO, "steps[%d]: mode=%u, channel=%u, data_len=%u, data=", i, step->mode, step->channel, step->data_len);
                for (int j = 0; j < step->data_len; j++) {
                    esp_rom_printf("%02x ", step->data[j]);
                }
                esp_rom_printf("\n");
            }
            break;
        case BLE_CS_EVENT_SUBEVET_RESULT_CONTINUE:
            MODLOG_DFLT(INFO, "subev_result_continue.conn_handle = %u\n", event->subev_result_continue.conn_handle);
            MODLOG_DFLT(INFO, "subev_result_continue.config_id = %u\n", event->subev_result_continue.config_id);
            MODLOG_DFLT(INFO, "subev_result_continue.procedure_done_status = %u\n", event->subev_result_continue.procedure_done_status);
            MODLOG_DFLT(INFO, "subev_result_continue.subevent_done_status = %u\n", event->subev_result_continue.subevent_done_status);
            MODLOG_DFLT(INFO, "subev_result_continue.abort_reason = %u\n", event->subev_result_continue.abort_reason);
            MODLOG_DFLT(INFO, "subev_result_continue.num_antenna_paths = %u\n", event->subev_result_continue.num_antenna_paths);
            MODLOG_DFLT(INFO, "subev_result_continue.num_steps_reported = %u\n", event->subev_result_continue.num_steps_reported);

            for (int i = 0; i < event->subev_result_continue.num_steps_reported; i++) {
                const struct cs_steps_data *step = &event->subev_result_continue.steps[i];
                MODLOG_DFLT(INFO, "steps[%d]: mode=%u, channel=%u, data_len=%u, data=", i, step->mode, step->channel, step->data_len);
                for (int j = 0; j < step->data_len; j++) {
                    esp_rom_printf("%02x ", step->data[j]);
                }
                esp_rom_printf("\n");
            }

            break;
        default:
            MODLOG_DFLT(INFO, "Unknown event type\n");
            break;
    }
}

static int blecs_gap_event(struct ble_cs_event *event, void *arg)
{
    switch(event->type){

        case BLE_CS_EVENT_SUBEVET_RESULT:
            MODLOG_DFLT(INFO, "LE CS Subevent Result ,procedure counter: %d\n", event->subev_result.procedure_counter);

            if (event->subev_result.procedure_done_status == BLE_HCI_LE_CS_SUBEVENT_DONE_STATUS_COMPLETE) {
                MODLOG_DFLT(INFO, "LE CS Subevent Result , status: Complete, procedure counter %d\n", event->subev_result.procedure_counter);
                most_recent_local_ranging_counter=event->subev_result.procedure_counter;

            } else if(event->subev_result.procedure_done_status == BLE_HCI_LE_CS_SUBEVENT_DONE_STATUS_PARTIAL) {
               ranging_subevent.type=BLE_CS_EVENT_SUBEVET_RESULT;
               ranging_subevent.subev_result= event->subev_result;
               idx++;
                if (idx==1) {
                    most_recent_local_ranging_counter=event->subev_result.procedure_counter;
                }
                MODLOG_DFLT(INFO, "LE CS Subevent Result , status: Partial, procedure counter %d\n", event->subev_result.procedure_counter);
            } else {
                MODLOG_DFLT(INFO, "LE CS Subevent Result , status: Unknown\n");
            }
            break;

        case BLE_CS_EVENT_SUBEVET_RESULT_CONTINUE:
            if( event->subev_result_continue.procedure_done_status == BLE_HCI_LE_CS_SUBEVENT_DONE_STATUS_ABORTED) {
                MODLOG_DFLT(INFO, "LE CS Subevent Result Continue , status: Aborted\n");
            } else if ( event->subev_result_continue.procedure_done_status == BLE_HCI_LE_CS_SUBEVENT_DONE_STATUS_COMPLETE) {
                MODLOG_DFLT(INFO, "LE CS Subevent Result Continue , status: Complete\n");
                ranging_subevent.subev_result_continue = event->subev_result_continue;
                /* To
                * Get total number of CS procedure from CS enable event and then accordigly indicate to most recent ranging counter
                Currently we are considering only one CS procedure
                */
               ind ++;
               if (ind==1) {
                ble_gatts_store_ranging_data(ranging_subevent);
                ble_gatts_indicate_ranging_data_ready(most_recent_local_ranging_counter);
               }
            }

            break;
        default:
            return 0;

    }
    return 0;
}

static void
bleprph_print_conn_desc(struct ble_gap_conn_desc *desc)
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
/**
 * Enables advertising with the following parameters:
 *     o General discoverable mode.
 *     o Undirected connectable mode.
 */
static void
ext_bleprph_advertise(void)
{
    struct ble_gap_ext_adv_params params;
    struct os_mbuf *data;
    uint8_t instance = 0;
    int rc;
    /* First check if any instance is already active */
    if(ble_gap_ext_adv_active(instance)) {
        return;
    }
    /* use defaults for non-set params */
    memset (&params, 0, sizeof(params));
    /* enable connectable advertising */
    params.connectable = 1;
    /* advertise using random addr */
    params.own_addr_type = BLE_OWN_ADDR_PUBLIC;
    params.primary_phy = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_2M;
    params.sid = 1;
    params.itvl_min = BLE_GAP_ADV_FAST_INTERVAL1_MIN;
    params.itvl_max = BLE_GAP_ADV_FAST_INTERVAL1_MIN;
    /* configure instance 0 */
    rc = ble_gap_ext_adv_configure(instance, &params, NULL,
                                   bleprph_gap_event, NULL);
    assert (rc == 0);
    /* in this case only scan response is allowed */

    /* get mbuf for scan rsp data */
    data = os_msys_get_pkthdr(sizeof(ext_adv_pattern_1), 0);
    assert(data);

    /* fill mbuf with scan rsp data */
    rc = os_mbuf_append(data, ext_adv_pattern_1, sizeof(ext_adv_pattern_1));
    assert(rc == 0);

    rc = ble_gap_ext_adv_set_data(instance, data);
    assert (rc == 0);

    /* start advertising */
    rc = ble_gap_ext_adv_start(instance, 0, 0);
}
#else
/**
 * Enables advertising with the following parameters:
 *     o General discoverable mode.
 *     o Undirected connectable mode.
 */
static void
bleprph_advertise(void)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    const char *name;
    int rc;
    /**
     *  Set the advertisement data included in our advertisements:
     *     o Flags (indicates advertisement type and other general info).
     *     o Advertising tx power.
     *     o Device name.
     *     o 16-bit service UUIDs (alert notifications).
     */
    memset(&fields, 0, sizeof fields);
    /* Advertise two flags:
     *     o Discoverability in forthcoming advertisement (general)
     *     o BLE-only (BR/EDR unsupported).
     */
    fields.flags = BLE_HS_ADV_F_DISC_GEN |
                   BLE_HS_ADV_F_BREDR_UNSUP;
    /* Indicate that the TX power level field should be included; have the
     * stack fill this value automatically.  This is done by assigning the
     * special value BLE_HS_ADV_TX_PWR_LVL_AUTO.
     */
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    name = ble_svc_gap_device_name();
    fields.name = (uint8_t *)name;
    fields.name_len = strlen(name);
    fields.name_is_complete = 1;
    fields.uuids16 = (ble_uuid16_t[]) {
        BLE_UUID16_INIT(BLE_UUID_RANGING_SERVICE_VAL)
    };
    fields.num_uuids16 = 1;
    fields.uuids16_is_complete = 1;
    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error setting advertisement data; rc=%d\n", rc);
        return;
    }
    /* Begin advertising. */
    memset(&adv_params, 0, sizeof adv_params);
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(0, NULL, BLE_HS_FOREVER,
                           &adv_params, bleprph_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error enabling advertisement; rc=%d\n", rc);
        return;
    }
}
#endif

/**
 * The nimble host executes this callback when a GAP event occurs.  The
 * application associates a GAP event callback with each connection that forms.
 * bleprph uses the same callback for all connections.
 *
 * @param event                 The type of event being signalled.
 * @param ctxt                  Various information pertaining to the event.
 * @param arg                   Application-specified argument; unused by
 *                                  bleprph.
 *
 * @return                      0 if the application successfully handled the
 *                                  event; nonzero on failure.  The semantics
 *                                  of the return code is specific to the
 *                                  particular GAP event being signalled.
 */
static int
bleprph_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        MODLOG_DFLT(INFO, "connection %s; status=%d ",
                    event->connect.status == 0 ? "established" : "failed",
                    event->connect.status);
        if (event->connect.status == 0) {
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            bleprph_print_conn_desc(&desc);
        }
        MODLOG_DFLT(INFO, "\n");
        if (event->connect.status != 0) {
            /* Connection failed; resume advertising. */
#if CONFIG_EXAMPLE_EXTENDED_ADV
            ext_bleprph_advertise();
#else
            bleprph_advertise();
#endif
        }

        return 0;
    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "disconnect; reason=%d ", event->disconnect.reason);
        bleprph_print_conn_desc(&event->disconnect.conn);
        MODLOG_DFLT(INFO, "\n");
        /* Connection terminated; resume advertising. */
#if CONFIG_EXAMPLE_EXTENDED_ADV
        ext_bleprph_advertise();
#else
        bleprph_advertise();
#endif
        return 0;
    case BLE_GAP_EVENT_CONN_UPDATE:
        /* The central has updated the connection parameters. */
        MODLOG_DFLT(INFO, "connection updated; status=%d ",
                    event->conn_update.status);
        rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
        assert(rc == 0);
        bleprph_print_conn_desc(&desc);
        MODLOG_DFLT(INFO, "\n");
        return 0;
    case BLE_GAP_EVENT_ADV_COMPLETE:
        MODLOG_DFLT(INFO, "advertise complete; reason=%d",
                    event->adv_complete.reason);
#if CONFIG_EXAMPLE_EXTENDED_ADV
        ext_bleprph_advertise();
#else
        bleprph_advertise();
#endif
        return 0;
    case BLE_GAP_EVENT_ENC_CHANGE:
        /* Encryption has been enabled or disabled for this connection. */
        MODLOG_DFLT(INFO, "encryption change event; status=%d ",
                    event->enc_change.status);
        rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
        assert(rc == 0);
        bleprph_print_conn_desc(&desc);
        struct ble_cs_reflector_setup_params params;
        params.cb=blecs_gap_event;
        ble_cs_reflector_setup(&params);

        return 0;
    case BLE_GAP_EVENT_NOTIFY_TX:
        MODLOG_DFLT(INFO, "notify_tx event; conn_handle=%d attr_handle=%d "
                    "status=%d is_indication=%d",
                    event->notify_tx.conn_handle,
                    event->notify_tx.attr_handle,
                    event->notify_tx.status,
                    event->notify_tx.indication);

        if (event->notify_tx.status == BLE_HS_EDONE) {
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            ble_gatts_indicate_control_point_response(event->notify_tx.attr_handle,most_recent_local_ranging_counter);
        }
        return 0;
    case BLE_GAP_EVENT_SUBSCRIBE:
        MODLOG_DFLT(INFO, "subscribe event; conn_handle=%d attr_handle=%d "
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
        MODLOG_DFLT(INFO, "mtu update event; conn_handle=%d cid=%d mtu=%d\n",
                    event->mtu.conn_handle,
                    event->mtu.channel_id,
                    event->mtu.value);
        return 0;
    }
    return 0;
}
static void
bleprph_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

static void
bleprph_on_sync(void)
{
    int rc;
    uint8_t own_addr_type = 0;
    /* Make sure we have set Host feature bit for Channel Sounding*/
    rc = ble_gap_set_host_feat(47,0x01);
    /* Make sure we have proper identity address set (public preferred) */
    rc = ble_hs_util_ensure_addr(0);

    assert(rc == 0);
    /* Figure out address to use while advertising (no privacy for now) */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
        return;
    }
    /* Printing ADDR */
    uint8_t addr_val[6] = {0};
    rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);
    MODLOG_DFLT(INFO, "Device Address: ");
    print_addr(addr_val);
    MODLOG_DFLT(INFO, "\n");
    /* Begin advertising. */
#if CONFIG_EXAMPLE_EXTENDED_ADV
    ext_bleprph_advertise();
#else
    bleprph_advertise();
#endif
}
void bleprph_host_task(void *param)
{
    ESP_LOGI(tag, "BLE Host Task Started");
    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();
    nimble_port_freertos_deinit();
}
void
app_main(void)
{
    int rc;
    /* Initialize NVS — it is used to store PHY calibration data */
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
    /* Initialize the NimBLE host configuration. */
    ble_hs_cfg.reset_cb = bleprph_on_reset;
    ble_hs_cfg.sync_cb = bleprph_on_sync;
    ble_hs_cfg.gatts_register_cb = custom_gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
    ble_hs_cfg.sm_io_cap = 0x03;
#ifdef CONFIG_EXAMPLE_BONDING
    ble_hs_cfg.sm_bonding = 1;
    /* Enable the appropriate bit masks to make sure the keys
     * that are needed are exchanged
     */
    ble_hs_cfg.sm_our_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC;
    ble_hs_cfg.sm_their_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC;
#endif
#ifdef CONFIG_EXAMPLE_MITM
    ble_hs_cfg.sm_mitm = 1;
#endif
#ifdef CONFIG_EXAMPLE_USE_SC
    ble_hs_cfg.sm_sc = 1;
#else
    ble_hs_cfg.sm_sc = 0;
#endif
#ifdef CONFIG_EXAMPLE_RESOLVE_PEER_ADDR
    /* Stores the IRK */
    ble_hs_cfg.sm_our_key_dist |= BLE_SM_PAIR_KEY_DIST_ID;
    ble_hs_cfg.sm_their_key_dist |= BLE_SM_PAIR_KEY_DIST_ID;
#endif
    rc = custom_gatt_svr_init();
    assert(rc == 0);
    /* Set the default device name. */
    rc = ble_svc_gap_device_name_set("nimble-ble_chan_reflector");
    assert(rc == 0);
    /* XXX Need to have template for store */
    ble_store_config_init();
    nimble_port_freertos_init(bleprph_host_task);

}
