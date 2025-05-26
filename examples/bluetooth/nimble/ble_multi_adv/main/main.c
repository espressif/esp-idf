/*
 * SPDX-FileCopyrightText: 2017-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
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
#include "multi_adv.h"

static const char *tag = "NimBLE_MULTI_ADV";
static int ble_multi_adv_gap_event(struct ble_gap_event *event, void *arg);
static uint8_t own_addr_type = BLE_OWN_ADDR_RANDOM;
struct ble_instance_cb_register ble_instance_cb[BLE_ADV_INSTANCES];

void ble_store_config_init(void);
static int ble_connectable_ext_cb(uint16_t instance);
static int ble_scannable_legacy_ext_cb(uint16_t instance);
static int ble_legacy_duration_cb(uint16_t instance);
static int ble_non_conn_ext_cb(uint16_t instance);
static void ble_multi_advertise(ble_addr_t addr);

/* Advertising patterns */
static uint8_t legacy_dur_adv_pattern[] = {
    0x02, 0x01, 0x06,
    0x03, 0x03, 0xab, 0xcd,
    0x03, 0x03, 0x18, 0x11,
    0x12, 0X09, 'n', 'i', 'm', 'b', 'l', 'e', '-', 'l', 'e', 'g', 'a', 'c', 'y', '-', 'd', 'u',
    'r'
};

static uint8_t scannable_legacy_adv_pattern[] = {
    0x02, 0x01, 0x06,
    0x03, 0x03, 0xab, 0xcd,
    0x03, 0x03, 0x18, 0x11,
    0x13, 0X09, 'n', 'i', 'm', 'b', 'l', 'e', '-', 's', 'c', 'a', 'n', '-', 'l', 'e', 'g', 'a',
    'c', 'y'
};

static uint8_t connectable_adv_pattern[] = {
    0x02, 0x01, 0x06,
    0x03, 0x03, 0xab, 0xcd,
    0x03, 0x03, 0x18, 0x11,
    0x12, 0X09, 'n', 'i', 'm', 'b', 'l', 'e', '-', 'c', 'o', 'n', 'n', 'e', 't', 'a', 'b', 'l', 'e'
};

static uint8_t non_conn_adv_pattern[] = {
    0x02, 0x01, 0x06,
    0x03, 0x03, 0xab, 0xcd,
    0x03, 0x03, 0x18, 0x11,
    0x10, 0X09, 'n', 'i', 'm', 'b', 'l', 'e', '-', 'n', 'o', 'n', '-', 'c', 'o', 'n', 'n'
};

/**
 * Logs information about a connection to the console.
 */
static void
ble_multi_adv_print_conn_desc(struct ble_gap_conn_desc *desc)
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

/* Generates and sets random address */
static int
ble_multi_adv_set_addr(uint16_t instance)
{
    ble_addr_t addr;
    int rc;

    /* generate new non-resolvable private address */
    rc = ble_hs_id_gen_rnd(1, &addr);
    if (rc != 0) {
        return rc;
    }

    /* Set generated address */
    rc = ble_gap_ext_adv_set_addr(instance, &addr);
    if (rc != 0) {
        return rc;
    }

    print_addr(addr.val);

    memcpy(&ble_instance_cb[instance].addr, &addr, sizeof(addr));
    return 0;
}

/* Configures extended advertsing params*/
static void
ble_multi_adv_conf_set_addr(uint16_t instance, struct ble_gap_ext_adv_params *params,
                            uint8_t *pattern, int size_pattern, int duration)
{
    int rc;
    struct os_mbuf *data;

    if (ble_gap_ext_adv_active(instance)) {
        ESP_LOGI(tag, "Instance already advertising");
        return;
    }

    rc = ble_gap_ext_adv_configure(instance, params, NULL,
                                   ble_multi_adv_gap_event, NULL);
    assert (rc == 0);

    rc = ble_multi_adv_set_addr(instance);
    assert (rc == 0);

    /* get mbuf for adv data */
    data = os_msys_get_pkthdr(size_pattern, 0);
    assert(data);

    /* fill mbuf with adv data */
    rc = os_mbuf_append(data, pattern, size_pattern);
    assert(rc == 0);

    rc = ble_gap_ext_adv_set_data(instance, data);
    assert (rc == 0);

    /* start advertising */
    rc = ble_gap_ext_adv_start(instance, duration, 0);
    assert (rc == 0);

    ESP_LOGI(tag, "Instance %d started", instance);
}

/* Starts advertising instance with 5sec timeout */
static void
start_legacy_duration(void)
{
    uint8_t instance = 3;
    int duration = 500; /* 5seconds, 10ms units */
    struct ble_gap_ext_adv_params params;
    int size_pattern = sizeof(legacy_dur_adv_pattern) / sizeof(legacy_dur_adv_pattern[0]);

    memset (&params, 0, sizeof(params));

    params.legacy_pdu = 1;
    params.own_addr_type = BLE_OWN_ADDR_RANDOM;
    params.sid = 3;
    params.primary_phy = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_1M;
    params.tx_power = 127;

    ble_multi_adv_conf_set_addr(instance, &params, legacy_dur_adv_pattern,
                                size_pattern, duration);

    ble_instance_cb[instance].cb = &ble_legacy_duration_cb;
}

/* Starts simple non-connectable scannable instance using legacy PUDs that
 * runs forever
 */
static void
start_scannable_legacy_ext(void)
{
    uint8_t instance = 2;
    struct ble_gap_ext_adv_params params;
    int size_pattern = sizeof(scannable_legacy_adv_pattern)/sizeof(scannable_legacy_adv_pattern[0]);

    memset (&params, 0, sizeof(params));

    params.legacy_pdu = 1;
    params.scannable = 1;
    params.own_addr_type = BLE_OWN_ADDR_RANDOM;
    params.sid = 2;
    params.primary_phy = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_1M;
    params.tx_power = 127;

    ble_multi_adv_conf_set_addr(instance, &params, scannable_legacy_adv_pattern
                                , size_pattern, 0);

    ble_instance_cb[instance].cb = &ble_scannable_legacy_ext_cb;
}

/* this is simple scannable instance that runs forever
 * TODO Get scan request notifications.
 */
static void
start_connectable_ext(void)
{
    uint8_t instance = 1;
    struct ble_gap_ext_adv_params params;
    int size_pattern = sizeof(connectable_adv_pattern) / sizeof(connectable_adv_pattern[0]);

    memset (&params, 0, sizeof(params));

    params.connectable = 1;
    params.own_addr_type = BLE_OWN_ADDR_RANDOM;
    params.sid = 1;
    params.primary_phy = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_1M;
    params.tx_power = 127;

    ble_multi_adv_conf_set_addr(instance, &params, connectable_adv_pattern,
                                size_pattern, 0);

    ble_instance_cb[instance].cb = &ble_connectable_ext_cb;
}

/* Starts a connectable instance */
static void
start_non_connectable_ext(void)
{
    uint8_t instance = 0;
    struct ble_gap_ext_adv_params params;
    int size_pattern = sizeof(non_conn_adv_pattern) / sizeof(non_conn_adv_pattern[0]);

    memset (&params, 0, sizeof(params));

    params.own_addr_type = BLE_OWN_ADDR_RANDOM;
    params.sid = 0;
    params.primary_phy = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_2M;
    params.tx_power = 127;

    ble_multi_adv_conf_set_addr(instance, &params, non_conn_adv_pattern,
                                size_pattern, 0);

    ble_instance_cb[instance].cb = &ble_non_conn_ext_cb;
}

/* Callbacks */
static int
ble_connectable_ext_cb(uint16_t instance)
{
    ESP_LOGI(tag, "In %s, instance = %d", __func__, instance);
    return 0;
}

static int
ble_scannable_legacy_ext_cb(uint16_t instance)
{
    ESP_LOGI(tag, "In %s, instance = %d", __func__, instance);
    return 0;
}

static int
ble_legacy_duration_cb(uint16_t instance)
{
    ESP_LOGI(tag, "In %s, instance = %d", __func__, instance);
    return 0;
}

static int
ble_non_conn_ext_cb(uint16_t instance)
{
    ESP_LOGI(tag, "In %s, instance = %d", __func__, instance);
    return 0;
}

/* Finds instance associated with the random address passed and calls its callback function */
static void
ble_multi_advertise(ble_addr_t addr)
{
    for (int i = 0; i < BLE_ADV_INSTANCES; i++) {
        if (memcmp(&addr, &ble_instance_cb[i].addr, sizeof(addr)) == 0) {
            switch(i) {
                case 0:
                    start_non_connectable_ext();
                    break;
                case 1:
                    start_connectable_ext();
                    break;
                case 2:
                    start_scannable_legacy_ext();
                    break;
                case 3:
                    start_legacy_duration();
                    break;
                default:
                    ESP_LOGI(tag, "Instance not found");
            }
        }
    }
}

static void
ble_multi_perform_gatt_proc(ble_addr_t addr)
{
    /* GATT procedures like notify, indicate can be performed now */
    for (int i = 0; i < BLE_ADV_INSTANCES; i++) {
        if (memcmp(&addr, &ble_instance_cb[i].addr, sizeof(addr)) == 0) {
            if (ble_instance_cb[i].cb) {
                ble_instance_cb[i].cb(i);
            }
        }
    }
    return;
}

/**
 * The nimble host executes this callback when a GAP event occurs.  The
 * application associates a GAP event callback with each connection that forms.
 * bleprph uses the same callback for all connections.
 *
 * @param event                 The type of event being signalled.
 * @param ctxt                  Various information pertaining to the event.
 * @param arg                   Application-specified argument; unused by
 *                              bleprph.
 *
 * @return                      0 if the application successfully handled the
 *                              event; nonzero on failure.  The semantics
 *                              of the return code is specific to the
 *                              particular GAP event being signalled.
 */
static int
ble_multi_adv_gap_event(struct ble_gap_event *event, void *arg)
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
            ble_multi_adv_print_conn_desc(&desc);

            ble_multi_perform_gatt_proc(desc.our_id_addr);
        }
        MODLOG_DFLT(INFO, "\n");
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "disconnect; reason=%d ", event->disconnect.reason);
        ble_multi_adv_print_conn_desc(&event->disconnect.conn);
        MODLOG_DFLT(INFO, "\n");

        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        /* The central has updated the connection parameters. */
        MODLOG_DFLT(INFO, "connection updated; status=%d ",
                    event->conn_update.status);
        rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
        assert(rc == 0);
        ble_multi_adv_print_conn_desc(&desc);
        MODLOG_DFLT(INFO, "\n");
        return 0;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        MODLOG_DFLT(INFO, "advertise complete; reason=%d",
                    event->adv_complete.reason);
        ble_multi_advertise(ble_instance_cb[event->adv_complete.instance].addr);
        return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
        /* Encryption has been enabled or disabled for this connection. */
        MODLOG_DFLT(INFO, "encryption change event; status=%d ",
                    event->enc_change.status);
        rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
        assert(rc == 0);
        ble_multi_adv_print_conn_desc(&desc);
        MODLOG_DFLT(INFO, "\n");
        return 0;

    case BLE_GAP_EVENT_NOTIFY_TX:
        MODLOG_DFLT(INFO, "notify_tx event; conn_handle=%d attr_handle=%d "
                    "status=%d is_indication=%d",
                    event->notify_tx.conn_handle,
                    event->notify_tx.attr_handle,
                    event->notify_tx.status,
                    event->notify_tx.indication);
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
ble_multi_adv_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

static void
ble_multi_adv_on_sync(void)
{
    int rc;

    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    /* Figure out address to use while advertising (no privacy for now) */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
        return;
    }

    start_non_connectable_ext();

    start_connectable_ext();

    start_scannable_legacy_ext();

    start_legacy_duration();
}

void ble_multi_adv_host_task(void *param)
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
    ble_hs_cfg.reset_cb = ble_multi_adv_on_reset;
    ble_hs_cfg.sync_cb = ble_multi_adv_on_sync;
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    ble_hs_cfg.sm_bonding = 1;
    ble_hs_cfg.sm_mitm = 1;
    ble_hs_cfg.sm_sc = 1;
    ble_hs_cfg.sm_sc = 0;
    /* Enable the appropriate bit masks to make sure the keys
     * that are needed are exchanged
     */
    ble_hs_cfg.sm_our_key_dist = BLE_SM_PAIR_KEY_DIST_ENC;
    ble_hs_cfg.sm_their_key_dist = BLE_SM_PAIR_KEY_DIST_ENC;

    /* Initialize callbacks to NULL */
    for (int i = 0; i < BLE_ADV_INSTANCES; i++) {
        ble_instance_cb[i].cb = NULL;
    }

    rc = gatt_svr_init();
    assert(rc == 0);

#if CONFIG_BT_NIMBLE_GAP_SERVICE
    /* Set the default device name. */
    rc = ble_svc_gap_device_name_set("nimble-multi-adv");
    assert(rc == 0);
#endif

    /* XXX Need to have template for store */
    ble_store_config_init();

    nimble_port_freertos_init(ble_multi_adv_host_task);
}
