/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
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
#include "enc_adv_data_cent.h"

#if CONFIG_EXAMPLE_ENC_ADV_DATA
static int counter = 0;
static struct km_peer kmp[CONFIG_BT_NIMBLE_MAX_CONNECTIONS + 1] = {0};

static const char *tag = "ENC_ADV_DATA_CENT";
static int enc_adv_data_cent_gap_event(struct ble_gap_event *event, void *arg);
static uint8_t peer_addr[6];
static int mtu_def = 512;

void ble_store_config_init(void);

static int
enc_adv_data_find_peer(const uint8_t *peer_addr)
{
    for (int i = 0; i <= CONFIG_BT_NIMBLE_MAX_CONNECTIONS; i++) {
        if (memcmp(peer_addr, &kmp[i].peer_addr, PEER_ADDR_VAL_SIZE) == 0) {
            return i;
        }
    }
    return -1;
}

static int
enc_adv_data_set_km_exist(const uint8_t *peer_addr)
{
    int ind = enc_adv_data_find_peer(peer_addr);
    if (ind == -1) {
        return -1;
    }
    kmp[ind].key_material_exist = true;
    return 0;
}

static bool
enc_adv_data_check_km_exist(const uint8_t *peer_addr)
{
    int ind;
    ind = enc_adv_data_find_peer(peer_addr);
    if (ind == -1) {
        return false;
    }

    return kmp[ind].key_material_exist;
}

/**
 * Application callback.  Called when the read has completed.
 */
static int
enc_adv_data_cent_on_read(uint16_t conn_handle,
                          const struct ble_gatt_error *error,
                          struct ble_gatt_attr *attr,
                          void *arg)
{
    int rc;
    struct ble_store_value_ead value_ead = {0};
    struct peer *p;

    MODLOG_DFLT(INFO, "Read complete; status=%d conn_handle=%d", error->status,
                conn_handle);
    if (error->status == 0) {
        MODLOG_DFLT(INFO, " attr_handle=%d value=", attr->handle);
        print_mbuf(attr->om);
    } else {
        goto err;
    }

    p = peer_find(conn_handle);
    if (p == NULL) {
        goto err;
    }

    rc = enc_adv_data_set_km_exist(p->peer_addr);
    if (rc != 0) {
        MODLOG_DFLT(INFO, "Setting key material exist flag failed");
    }

    value_ead.km_present = 1;

    value_ead.km = (struct key_material *) malloc (sizeof(struct key_material));

    memset(value_ead.km, 0, sizeof(struct key_material));

    os_mbuf_copydata(attr->om, 0, BLE_EAD_KEY_SIZE, &value_ead.km->session_key);
    os_mbuf_copydata(attr->om, BLE_EAD_KEY_SIZE, BLE_EAD_IV_SIZE, &value_ead.km->iv);

    MODLOG_DFLT(DEBUG, "Session key:");
    print_bytes(value_ead.km->session_key, BLE_EAD_KEY_SIZE);

    MODLOG_DFLT(DEBUG, "IV:");
    print_bytes(value_ead.km->iv, BLE_EAD_IV_SIZE);

    memcpy(&value_ead.peer_addr.val, &p->peer_addr, PEER_ADDR_VAL_SIZE);

    rc = ble_store_write_ead(&value_ead);
    if (rc == 0) {
        MODLOG_DFLT(INFO, "Writing of session key, iv, and peer addr to NVS success");
    }

err:
    /* Terminate the connection. */
    return ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
}

static void
enc_adv_data_cent_read(const struct peer *peer)
{
    const struct peer_chr *chr = NULL;
    int rc;

    /* Read the supported-new-alert-category characteristic. */
    chr = peer_chr_find_uuid(peer,
                             BLE_UUID16_DECLARE(BLE_SVC_GAP_UUID16),
                             BLE_UUID16_DECLARE(BLE_SVC_GAP_CHR_UUID16_KEY_MATERIAL));
    if (chr == NULL) {
        MODLOG_DFLT(ERROR, "Error: Peer doesn't support the Key"
                    "Material characteristic\n");
        goto err;
    }

    rc = ble_gattc_read(peer->conn_handle, chr->chr.val_handle,
                        enc_adv_data_cent_on_read, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Error: Failed to read characteristic; rc=%d\n",
                    rc);
        goto err;
    }

    return;
err:
    /* Terminate the connection. */
    ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
}

/**
 * Called when service discovery of the specified peer has completed.
 */
static void
enc_adv_data_cent_on_disc_complete(const struct peer *peer, int status, void *arg)
{
    if (status != 0) {
        /* Service discovery failed.  Terminate the connection. */
        MODLOG_DFLT(ERROR, "Error: Service discovery failed; status=%d "
                    "conn_handle=%d\n", status, peer->conn_handle);
        ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }

    /* Service discovery has completed successfully.  Now we have a complete
     * list of services, characteristics, and descriptors that the peer
     * supports.
     */
    MODLOG_DFLT(INFO, "Service discovery complete; status=%d "
                "conn_handle=%d\n", status, peer->conn_handle);

    if (!enc_adv_data_check_km_exist(peer->peer_addr)) {
        /* Now perform GATT read procedures against the peer */
        enc_adv_data_cent_read(peer);
    }
}

/**
 * Initiates the GAP general discovery procedure.
 */
static void
enc_adv_data_cent_scan(void)
{
    uint8_t own_addr_type;
    struct ble_gap_disc_params disc_params;
    int rc;

    /* Figure out address to use while advertising (no privacy for now) */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
        return;
    }

    /* Tell the controller to filter duplicates; we don't want to process
     * repeated advertisements from the same device.
     */
    disc_params.filter_duplicates = 1;

    /**
     * Perform a passive scan.  I.e., don't send follow-up scan requests to
     * each advertiser.
     */
    disc_params.passive = 1;

    /* Use defaults for the rest of the parameters. */
    disc_params.itvl = 0;
    disc_params.window = 0;
    disc_params.filter_policy = 0;
    disc_params.limited = 0;

    rc = ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params,
                      enc_adv_data_cent_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Error initiating GAP discovery procedure; rc=%d\n",
                    rc);
    }
}

static int
enc_adv_data_cent_decrypt(uint8_t length_data, const uint8_t *data, const uint8_t *peer_addr)
{
    uint8_t op;
    uint8_t len, offset = 0;
    uint8_t *enc_data;
    int rc;
    uint8_t dec_data_len;
    struct ble_store_key_ead key_ead = {0};
    struct ble_store_value_ead value_ead = {0};

    while (offset < length_data) {
        len = data[offset];
        op = data[offset + 1];
        uint8_t temp[len];

        switch (op) {
        case BLE_GAP_ENC_ADV_DATA:
            enc_data = (uint8_t *) malloc (sizeof(uint8_t) * len);
            memcpy(enc_data, data + offset + 2, len);

            memcpy(&key_ead.peer_addr.val, peer_addr, PEER_ADDR_VAL_SIZE);

            rc = ble_store_read_ead(&key_ead, &value_ead);
            if (rc != 0 || !value_ead.km_present) {
                MODLOG_DFLT(INFO, "Reading of session key and iv from NVS failed rc = %d", rc);
                return 0;
            } else {
                MODLOG_DFLT(INFO, "Read session key and iv from NVS successfully");
            }

            rc = ble_ead_decrypt(value_ead.km->session_key, value_ead.km->iv, enc_data, len,
                                 temp);
            if (rc == 0) {
                MODLOG_DFLT(INFO, "Decryption of adv data done successfully");
            } else {
                MODLOG_DFLT(INFO, "Decryption of adv data failed");
                return 0;
            }

            dec_data_len = temp[0];

            MODLOG_DFLT(DEBUG, "Data after decryption:");
            print_bytes(temp, dec_data_len);
            return 1;

        default:
            break;
        }
        offset += len + 1;
    }
    return 1;
}

/**
 * Indicates whether we should try to connect to the sender of the specified
 * advertisement.  The function returns a positive result if the device
 * advertises connectability and support for the Key Characteristic service.
 */
static int
ext_enc_adv_data_cent_should_connect(const struct ble_gap_ext_disc_desc *disc)
{
    int offset = 0;
    int ad_struct_len = 0;

    if (disc->legacy_event_type != BLE_HCI_ADV_RPT_EVTYPE_ADV_IND &&
            disc->legacy_event_type != BLE_HCI_ADV_RPT_EVTYPE_DIR_IND) {
        return 0;
    }

    if (strlen(CONFIG_EXAMPLE_PEER_ADDR) && (strncmp(CONFIG_EXAMPLE_PEER_ADDR, "ADDR_ANY", strlen    ("ADDR_ANY")) != 0)) {
        MODLOG_DFLT(INFO, "Peer address from menuconfig: %s", CONFIG_EXAMPLE_PEER_ADDR);
        /* Convert string to address */
        sscanf(CONFIG_EXAMPLE_PEER_ADDR, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
               &peer_addr[5], &peer_addr[4], &peer_addr[3],
               &peer_addr[2], &peer_addr[1], &peer_addr[0]);
        if (memcmp(peer_addr, disc->addr.val, sizeof(disc->addr.val)) != 0) {
            return 0;
        }
    }

    /* The device has to advertise support for the Key Characteristic
    * service (0x2B88)
    */
    do {
        ad_struct_len = disc->data[offset];

        if (!ad_struct_len) {
            break;
        }

        /* Search if custom service UUID (0x2C01) is advertised */
        if (disc->data[offset] == 0x03 && disc->data[offset + 1] == 0x03) {
            if ( disc->data[offset + 2] == 0x2C && disc->data[offset + 3] == 0x01 ) {
                if (enc_adv_data_find_peer(disc->addr.val) != -1) {
                    MODLOG_DFLT(INFO, "Peer was already added with addr : %s",
                                addr_str(&disc->addr.val));
                } else {
                    MODLOG_DFLT(INFO, "Adding peer addr : %s", addr_str(&disc->addr.val));

                    memcpy(&kmp[counter].peer_addr, &disc->addr.val, PEER_ADDR_VAL_SIZE);
                    counter++;

                    if (counter > CONFIG_BT_NIMBLE_MAX_CONNECTIONS) {
                        counter = 0;
                    }
                }
                if (enc_adv_data_check_km_exist(disc->addr.val)) {
                    return enc_adv_data_cent_decrypt(disc->length_data, disc->data, disc->addr.val);
                } else {
                    return 1;
                }
            }
        }

        offset += ad_struct_len + 1;

    } while ( offset < disc->length_data );

    return 0;
}

/**
 * Connects to the sender of the specified advertisement of it looks
 * interesting.  A device is "interesting" if it advertises connectability and
 * support for the Key Characteristic service.
 */
static void
enc_adv_data_cent_connect_if_interesting(void *disc)
{
    uint8_t own_addr_type;
    int rc;
    ble_addr_t *addr;

    /* Don't do anything if we don't care about this advertiser. */
    if (!ext_enc_adv_data_cent_should_connect((struct ble_gap_ext_disc_desc *)disc)) {
        return;
    }

    /* Scanning must be stopped before a connection can be initiated. */
    rc = ble_gap_disc_cancel();
    if (rc != 0) {
        MODLOG_DFLT(DEBUG, "Failed to cancel scan; rc=%d\n", rc);
        return;
    }

    /* Figure out address to use for connect (no privacy for now) */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
        return;
    }

    /* Try to connect the the advertiser.  Allow 30 seconds (30000 ms) for
     * timeout.
     */
    addr = &((struct ble_gap_ext_disc_desc *)disc)->addr;

    rc = ble_gap_connect(own_addr_type, addr, 30000, NULL,
                         enc_adv_data_cent_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Error: Failed to connect to device; addr_type=%d "
                    "addr=%s; rc=%d\n",
                    addr->type, addr_str(addr->val), rc);
        return;
    }
}

/**
 * The nimble host executes this callback when a GAP event occurs.  The
 * application associates a GAP event callback with each connection that is
 * established.  enc_adv_data_cent uses the same callback for all connections.
 *
 * @param event                 The event being signalled.
 * @param arg                   Application-specified argument; unused by
 *                                  enc_adv_data_cent.
 *
 * @return                      0 if the application successfully handled the
 *                                  event; nonzero on failure.  The semantics
 *                                  of the return code is specific to the
 *                                  particular GAP event being signalled.
 */
static int
enc_adv_data_cent_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    struct ble_hs_adv_fields fields;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_DISC:
        rc = ble_hs_adv_parse_fields(&fields, event->disc.data,
                                     event->disc.length_data);
        if (rc != 0) {
            return 0;
        }

        /* An advertisment report was received during GAP discovery. */
        print_adv_fields(&fields);

        /* Try to connect to the advertiser if it looks interesting. */
        enc_adv_data_cent_connect_if_interesting(&event->disc);
        return 0;

    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            /* Connection successfully established. */
            MODLOG_DFLT(INFO, "Connection established ");

            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            print_conn_desc(&desc);
            MODLOG_DFLT(INFO, "");

            rc = ble_att_set_preferred_mtu(mtu_def);
            if (rc != 0) {
                ESP_LOGE(tag, "Failed to set preferred MTU; rc = %d", rc);
            }

            rc = ble_gattc_exchange_mtu(event->connect.conn_handle, NULL, NULL);
            if (rc != 0) {
                ESP_LOGE(tag, "Failed to negotiate MTU; rc = %d", rc);
            }

            /* Remember peer. */
            rc = peer_add(event->connect.conn_handle);
            if (rc != 0) {
                MODLOG_DFLT(ERROR, "Failed to add peer; rc=%d\n", rc);
                return 0;
            }

            rc = peer_set_addr(event->connect.conn_handle, desc.peer_id_addr.val);
            if (rc != 0) {
                MODLOG_DFLT(ERROR, "Failed to set peer addr; rc=%d\n", rc);
                return 0;
            }

            /* Perform service discovery */
            rc = peer_disc_all(event->connect.conn_handle,
                               enc_adv_data_cent_on_disc_complete, NULL);
            if (rc != 0) {
                MODLOG_DFLT(ERROR, "Failed to discover services; rc=%d\n", rc);
                return 0;
            }
        } else {
            /* Connection attempt failed; resume scanning. */
            MODLOG_DFLT(ERROR, "Error: Connection failed; status=%d\n",
                        event->connect.status);
            enc_adv_data_cent_scan();
        }

        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        /* Connection terminated. */
        MODLOG_DFLT(INFO, "disconnect; reason=%d ", event->disconnect.reason);
        print_conn_desc(&event->disconnect.conn);
        MODLOG_DFLT(INFO, "");

        /* Forget about peer. */
        peer_delete(event->disconnect.conn.conn_handle);

        /* Resume scanning. */
        enc_adv_data_cent_scan();
        return 0;

    case BLE_GAP_EVENT_DISC_COMPLETE:
        MODLOG_DFLT(INFO, "discovery complete; reason=%d\n",
                    event->disc_complete.reason);
        return 0;

    case BLE_GAP_EVENT_NOTIFY_RX:
        /* Peer sent us a notification or indication. */
        MODLOG_DFLT(INFO, "received %s; conn_handle=%d attr_handle=%d "
                    "attr_len=%d\n",
                    event->notify_rx.indication ?
                    "indication" :
                    "notification",
                    event->notify_rx.conn_handle,
                    event->notify_rx.attr_handle,
                    OS_MBUF_PKTLEN(event->notify_rx.om));

        /* Attribute data is contained in event->notify_rx.om. Use
         * `os_mbuf_copydata` to copy the data received in notification mbuf */
        return 0;

    case BLE_GAP_EVENT_MTU:
        MODLOG_DFLT(INFO, "mtu update event; conn_handle=%d cid=%d mtu=%d\n",
                    event->mtu.conn_handle,
                    event->mtu.channel_id,
                    event->mtu.value);
        return 0;

    case BLE_GAP_EVENT_EXT_DISC:
        /* An advertisment report was received during GAP discovery. */
        ext_print_adv_report(&event->disc);

        enc_adv_data_cent_connect_if_interesting(&event->disc);
        return 0;

    default:
        return 0;
    }
}

static void
enc_adv_data_cent_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

static void
enc_adv_data_cent_on_sync(void)
{
    int rc;

    /* Make sure we have proper identity address set (public preferred) */
    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    enc_adv_data_cent_scan();
}

void enc_adv_data_cent_host_task(void *param)
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
    if  (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Failed to init nimble %d ", ret);
        return;
    }

    /* Configure the host. */
    ble_hs_cfg.reset_cb = enc_adv_data_cent_on_reset;
    ble_hs_cfg.sync_cb = enc_adv_data_cent_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    /* Initialize data structures to track connected peers. */
    rc = peer_init(MYNEWT_VAL(BLE_MAX_CONNECTIONS), 64, 64, 64);
    assert(rc == 0);

    /* Set the default device name. */
    rc = ble_svc_gap_device_name_set("nimble-enc_adv_data_cent");
    assert(rc == 0);

    /* XXX Need to have template for store */
    ble_store_config_init();

    nimble_port_freertos_init(enc_adv_data_cent_host_task);
}
#else
void
app_main(void)
{
    return;
}
#endif
