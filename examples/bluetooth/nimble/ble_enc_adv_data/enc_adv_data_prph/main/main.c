/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
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
#include "enc_adv_data_prph.h"

#if CONFIG_EXAMPLE_ENC_ADV_DATA

static const char *tag = "ENC_ADV_DATA_PRPH";
static int enc_adv_data_prph_gap_event(struct ble_gap_event *event, void *arg);
const uint8_t device_name[3] = {'k', 'e', 'y'};

static uint8_t unencrypted_adv_pattern[] = {
    0x05, 0X09, 'p', 'r', 'p', 'h'
};

struct key_material km = {
    .session_key = {
        0x19, 0x6a, 0x0a, 0xd1, 0x2a, 0x61, 0x20, 0x1e,
        0x13, 0x6e, 0x2e, 0xd1, 0x12, 0xda, 0xa9, 0x57
    },
    .iv = {0x9E, 0x7a, 0x00, 0xef, 0xb1, 0x7a, 0xe7, 0x46},
};

#if CONFIG_EXAMPLE_RANDOM_ADDR
static uint8_t own_addr_type = BLE_OWN_ADDR_RANDOM;
#else
static uint8_t own_addr_type;
#endif

void ble_store_config_init(void);

/**
 * Logs information about a connection to the console.
 */
static void
enc_adv_data_prph_print_conn_desc(struct ble_gap_conn_desc *desc)
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

static int
enc_adv_data_prph_encrypt_set(uint8_t *out_encrypted_adv_data,
                              const unsigned encrypted_adv_data_len)
{
    int rc;

    const unsigned unencrypted_adv_data_len = sizeof(unencrypted_adv_pattern);

    uint8_t unencrypted_adv_data[unencrypted_adv_data_len];
    uint8_t encrypted_adv_data[encrypted_adv_data_len];

    memcpy(unencrypted_adv_data, unencrypted_adv_pattern, sizeof(unencrypted_adv_pattern));

    MODLOG_DFLT(INFO, "Data before encryption:");
    print_bytes(unencrypted_adv_data, unencrypted_adv_data_len);
    MODLOG_DFLT(INFO, "\n");

    rc = ble_ead_encrypt(km.session_key, km.iv, unencrypted_adv_data,
                         unencrypted_adv_data_len, encrypted_adv_data);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Encryption of adv data failed; rc=%d", rc);
        return rc;
    }
    MODLOG_DFLT(INFO, "Encryption of adv data done successfully");

    MODLOG_DFLT(INFO, "Data after encryption:");
    print_bytes(encrypted_adv_data, encrypted_adv_data_len);
    MODLOG_DFLT(INFO, "\n");

    /** Contains Randomiser ## Encrypted Advertising Data ## MIC */
    memcpy(out_encrypted_adv_data, encrypted_adv_data, encrypted_adv_data_len);
    return 0;
}

/**
 * Enables advertising with the following parameters:
 *     o General discoverable mode.
 *     o Undirected connectable mode.
 */
static void
enc_adv_data_prph_advertise(void)
{
    struct ble_gap_adv_params params;
    struct ble_hs_adv_fields fields;
    int rc;

    const unsigned encrypted_adv_data_len = BLE_EAD_ENCRYPTED_PAYLOAD_SIZE(sizeof(unencrypted_adv_pattern));
    uint8_t encrypted_adv_data[encrypted_adv_data_len];
    memset(encrypted_adv_data, 0, encrypted_adv_data_len);

    /* First check if any instance is already active */
    if (ble_gap_adv_active()) {
        return;
    }

    /* use defaults for non-set params */
    memset (&params, 0, sizeof(params));
    memset (&fields, 0, sizeof(fields));

    own_addr_type = BLE_OWN_ADDR_PUBLIC;

    /* enable connectable advertising */
    params.conn_mode = BLE_GAP_CONN_MODE_UND;
    params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    params.itvl_min = BLE_GAP_ADV_FAST_INTERVAL1_MIN;
    params.itvl_max = BLE_GAP_ADV_FAST_INTERVAL1_MIN;

    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    fields.name = device_name;
    fields.name_len = 3;
    fields.name_is_complete = 1;

    fields.uuids16 = (ble_uuid16_t[]) {
        BLE_UUID16_INIT(0x2C01) /** For the central to recognise this device */
    };
    fields.num_uuids16 = 1;
    fields.uuids16_is_complete = 1;

    /** Getting the encrypted advertising data */
    rc = enc_adv_data_prph_encrypt_set(encrypted_adv_data, encrypted_adv_data_len);
    if (rc != 0) {
        return;
    }

    fields.enc_adv_data = encrypted_adv_data;
    fields.enc_adv_data_len = encrypted_adv_data_len;

    rc = ble_gap_adv_set_fields(&fields);
    assert (rc == 0);

    /* start advertising */
    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER,
                           &params, enc_adv_data_prph_gap_event, NULL);
    assert (rc == 0);
}

/**
 * The nimble host executes this callback when a GAP event occurs.  The
 * application associates a GAP event callback with each connection that forms.
 * enc_adv_data_prph uses the same callback for all connections.
 *
 * @param event                 The type of event being signalled.
 * @param ctxt                  Various information pertaining to the event.
 * @param arg                   Application-specified argument; unused by
 *                                  enc_adv_data_prph.
 *
 * @return                      0 if the application successfully handled the
 *                                  event; nonzero on failure.  The semantics
 *                                  of the return code is specific to the
 *                                  particular GAP event being signalled.
 */
static int
enc_adv_data_prph_gap_event(struct ble_gap_event *event, void *arg)
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
            enc_adv_data_prph_print_conn_desc(&desc);
        }
        MODLOG_DFLT(INFO, "\n");

        if (event->connect.status != 0) {
            /* Connection failed; resume advertising. */
            enc_adv_data_prph_advertise();
        }

        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "disconnect; reason=%d ", event->disconnect.reason);
        enc_adv_data_prph_print_conn_desc(&event->disconnect.conn);
        MODLOG_DFLT(INFO, "\n");

        /* Connection terminated; resume advertising. */
        enc_adv_data_prph_advertise();
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        /* The central has updated the connection parameters. */
        MODLOG_DFLT(INFO, "connection updated; status=%d ",
                    event->conn_update.status);
        rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
        assert(rc == 0);
        enc_adv_data_prph_print_conn_desc(&desc);
        MODLOG_DFLT(INFO, "\n");
        return 0;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        MODLOG_DFLT(INFO, "advertise complete; reason=%d",
                    event->adv_complete.reason);
        return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
        MODLOG_DFLT(INFO, "encryption change event; status=%d ",
                    event->enc_change.status);
        rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
        assert(rc == 0);
        enc_adv_data_prph_print_conn_desc(&desc);
        MODLOG_DFLT(INFO, "\n");
        return 0;

    case BLE_GAP_EVENT_PASSKEY_ACTION:
        ESP_LOGI(tag, "PASSKEY_ACTION_EVENT started");
        struct ble_sm_io pkey = {0};

        /** For now only BLE_SM_IOACT_DISP is handled */
        if (event->passkey.params.action == BLE_SM_IOACT_DISP) {
            pkey.action = event->passkey.params.action;
            pkey.passkey = 123456;
            ESP_LOGI(tag, "Enter passkey %" PRIu32 " on the peer side", pkey.passkey);
            rc = ble_sm_inject_io(event->passkey.conn_handle, &pkey);
            ESP_LOGI(tag, "ble_sm_inject_io result: %d", rc);
        }

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

    case BLE_GAP_EVENT_AUTHORIZE:
        MODLOG_DFLT(INFO, "authorization event; conn_handle=%d attr_handle=%d is_read=%d",
                    event->authorize.conn_handle,
                    event->authorize.attr_handle,
                    event->authorize.is_read);
        /** Accept all authorization requests for now */
        event->authorize.out_response = BLE_GAP_AUTHORIZE_ACCEPT;
        return 0;
    }

    return 0;
}

static void
enc_adv_data_prph_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

#if CONFIG_EXAMPLE_RANDOM_ADDR
static void
ble_app_set_addr(void)
{
    ble_addr_t addr;
    int rc;

    /* generate new non-resolvable private address */
    rc = ble_hs_id_gen_rnd(0, &addr);
    assert(rc == 0);

    /* set generated address */
    rc = ble_hs_id_set_rnd(addr.val);

    assert(rc == 0);
}
#endif

static void
enc_adv_data_prph_on_sync(void)
{
    int rc;

#if CONFIG_EXAMPLE_RANDOM_ADDR
    /* Generate a non-resolvable private address. */
    ble_app_set_addr();
#endif

    /* Make sure we have proper identity address set (public preferred) */
#if CONFIG_EXAMPLE_RANDOM_ADDR
    rc = ble_hs_util_ensure_addr(1);
#else
    rc = ble_hs_util_ensure_addr(0);
#endif
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
    enc_adv_data_prph_advertise();
}

void enc_adv_data_prph_host_task(void *param)
{
    MODLOG_DFLT(INFO, "BLE Host Task Started");
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
    ble_hs_cfg.reset_cb = enc_adv_data_prph_on_reset;
    ble_hs_cfg.sync_cb = enc_adv_data_prph_on_sync;
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

#if CONFIG_EXAMPLE_BONDING
    ble_hs_cfg.sm_bonding = 1;
    /* Enable the appropriate bit masks to make sure the keys
     * that are needed are exchanged
     */
    ble_hs_cfg.sm_our_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC;
    ble_hs_cfg.sm_their_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC;
#else
    ble_hs_cfg.sm_bonding = 0;
#endif

    /** This feature requires authentication */
    ble_hs_cfg.sm_mitm = 1;
    ble_hs_cfg.sm_io_cap = CONFIG_EXAMPLE_IO_TYPE;

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

#if MYNEWT_VAL(BLE_GATTS)
    rc = gatt_svr_init();
    assert(rc == 0);

    /* Set the default device name. */
    rc = ble_svc_gap_device_name_set("enc_adv_data_prph");
    assert(rc == 0);
#endif

    /* Set the session key and initialization vector */
    rc = ble_svc_gap_device_key_material_set(km.session_key, km.iv);
    assert(rc == 0);

    /* XXX Need to have template for store */
    ble_store_config_init();

    nimble_port_freertos_init(enc_adv_data_prph_host_task);
}
#else
void
app_main(void)
{
    return;
}
#endif
