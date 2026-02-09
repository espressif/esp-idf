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
#include "ble_prox_cent.h"

static const char *tag = "NimBLE_PROX_CENT";
static uint8_t link_supervision_timeout;
static int8_t tx_pwr_lvl;
static struct ble_prox_cent_conn_peer conn_peer[MYNEWT_VAL(BLE_MAX_CONNECTIONS) + 1];
static struct ble_prox_cent_link_lost_peer disconn_peer[MYNEWT_VAL(BLE_MAX_CONNECTIONS) + 1];

/* Note: Path loss is calculated using formula : threshold - RSSI value
 *       by default threshold is kept -128 as per the spec
 *       high_threshold and low_threshold are hardcoded after testing and noting
 *       RSSI values when distance between devices are less and more.
 */
static int8_t high_threshold = -70;
static int8_t low_threshold = -100;

void ble_store_config_init(void);
static void ble_prox_cent_scan(void);
static int ble_prox_cent_gap_event(struct ble_gap_event *event, void *arg);

#if MYNEWT_VAL(BLE_GATTC)
static int
ble_prox_cent_on_read(uint16_t conn_handle,
                      const struct ble_gatt_error *error,
                      struct ble_gatt_attr *attr,
                      void *arg)
{
    MODLOG_DFLT(INFO, "Read on tx power level char completed; status=%d "
                "conn_handle=%d\n",
                error->status, conn_handle);
    if (error->status == 0) {
        MODLOG_DFLT(INFO, " attr_handle=%d value=", attr->handle);
        print_mbuf(attr->om);
        os_mbuf_copydata(attr->om, 0, attr->om->om_len, &tx_pwr_lvl);
        conn_peer[conn_handle].calc_path_loss = true;
    }

    return 0;
}

/**
 * Application callback.  Called when the write of alert level char
 * characteristic has completed.
 */
static int
ble_prox_cent_on_write(uint16_t conn_handle,
                       const struct ble_gatt_error *error,
                       struct ble_gatt_attr *attr,
                       void *arg)
{
    MODLOG_DFLT(INFO, "Write alert level char completed; status=%d conn_handle=%d",
                error->status, conn_handle);

    /* Read Tx Power level characteristic. */
    const struct peer_chr *chr;
    int rc;
    const struct peer *peer = peer_find(conn_handle);
    if (peer == NULL) {
        MODLOG_DFLT(ERROR, "Error: peer not found for conn_handle=%d", conn_handle);
        return 0;
    }
    chr = peer_chr_find_uuid(peer,
                             BLE_UUID16_DECLARE(BLE_SVC_TX_POWER_UUID16),
                             BLE_UUID16_DECLARE(BLE_SVC_PROX_CHR_UUID16_TX_PWR_LVL));
    if (chr == NULL) {
        MODLOG_DFLT(ERROR, "Error: Peer doesn't support the"
                    "Tx power level characteristic\n");
        goto err;
    }

    rc = ble_gattc_read(conn_handle, chr->chr.val_handle,
                        ble_prox_cent_on_read, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Error: Failed to read characteristic; rc=%d\n",
                    rc);
        goto err;
    }

    return 0;
err:
    /* Terminate the connection. */
    return ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
}

/**
 * Performs following GATT operations against the specified peer:
 * 1. Writes the alert level characteristic.
 * 2. After write is completed, it reads the Tx Power Level characteristic.
 *
 * If the peer does not support a required service, characteristic, or
 * descriptor, then the peer lied when it claimed support for the link
 * loss service!  When this happens, or if a GATT procedure fails,
 * this function immediately terminates the connection.
 */
static void
ble_prox_cent_read_write_subscribe(const struct peer *peer)
{
    const struct peer_chr *chr;
    int rc;

    /* Storing the val handle of immediate alert characteristic */
    chr = peer_chr_find_uuid(peer,
                             BLE_UUID16_DECLARE(BLE_SVC_IMMEDIATE_ALERT_UUID16),
                             BLE_UUID16_DECLARE(BLE_SVC_PROX_CHR_UUID16_ALERT_LVL));
    if (chr != NULL) {
        conn_peer[peer->conn_handle].val_handle = chr->chr.val_handle;
    } else {
        MODLOG_DFLT(ERROR, "Error: Peer doesn't support the alert level"
                    " characteristic of immediate alert loss service\n");
    }

    /* Write alert level characteristic. */
    chr = peer_chr_find_uuid(peer,
                             BLE_UUID16_DECLARE(BLE_SVC_LINK_LOSS_UUID16),
                             BLE_UUID16_DECLARE(BLE_SVC_PROX_CHR_UUID16_ALERT_LVL));
    if (chr == NULL) {
        MODLOG_DFLT(ERROR, "Error: Peer doesn't support the alert level"
                    " characteristic\n");
        goto err;
    }

    rc = ble_gattc_write_flat(peer->conn_handle, chr->chr.val_handle,
                              &link_supervision_timeout, sizeof(link_supervision_timeout),
                              ble_prox_cent_on_write, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Error: Failed to write characteristic; rc=%d\n",
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
ble_prox_cent_on_disc_complete(const struct peer *peer, int status, void *arg)
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

    /* Now perform GATT procedures against the peer: read,
     * write.
     */
    ble_prox_cent_read_write_subscribe(peer);
}
#endif

/**
 * Initiates the GAP general discovery procedure.
 */
static void
ble_prox_cent_scan(void)
{
    uint8_t own_addr_type;
    struct ble_gap_disc_params disc_params = {0};
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
                      ble_prox_cent_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Error initiating GAP discovery procedure; rc=%d\n",
                    rc);
    }
}

/**
 * Indicates whether we should try to connect to the sender of the specified
 * advertisement.  The function returns a positive result if the device
 * advertises connectability and support for the Health Thermometer service.
 */

#if CONFIG_EXAMPLE_EXTENDED_ADV
static int
ext_ble_prox_cent_should_connect(const struct ble_gap_ext_disc_desc *disc)
{
    int offset = 0;
    int ad_struct_len = 0;
    uint8_t test_addr[6];
    uint32_t peer_addr[6];

    memset(peer_addr, 0x0, sizeof peer_addr);

    if (disc->legacy_event_type != BLE_HCI_ADV_RPT_EVTYPE_ADV_IND &&
            disc->legacy_event_type != BLE_HCI_ADV_RPT_EVTYPE_DIR_IND) {
        return 0;
    }
    if (strlen(CONFIG_EXAMPLE_PEER_ADDR) && (strncmp(CONFIG_EXAMPLE_PEER_ADDR, "ADDR_ANY", strlen    ("ADDR_ANY")) != 0)) {
        ESP_LOGI(tag, "Peer address from menuconfig: %s", CONFIG_EXAMPLE_PEER_ADDR);
        /* Convert string to address */
        sscanf(CONFIG_EXAMPLE_PEER_ADDR, "%lx:%lx:%lx:%lx:%lx:%lx",
               &peer_addr[5], &peer_addr[4], &peer_addr[3],
               &peer_addr[2], &peer_addr[1], &peer_addr[0]);

	/* Conversion */
        for (int i=0; i<6; i++) {
            test_addr[5 - i] = (uint8_t )peer_addr[i];
        }

        if (memcmp(test_addr, disc->addr.val, sizeof(disc->addr.val)) != 0) {
            return 0;
        }
    }

    /* The device has to advertise support for Proximity sensor (link loss)
    * service (0x1803).
    */
    do {
        ad_struct_len = disc->data[offset];

        if (!ad_struct_len) {
            break;
        }

        /* Search if Proximity Sensor (Link loss) UUID is advertised */
        if (disc->data[offset + 1] == 0x03) {
            int uuid_offset = offset + 2;
            int uuid_end = offset + 1 + disc->data[offset];  // len includes type+data
            while (uuid_offset + 1 < uuid_end) {
                // BLE uses little-endian: 0x1803 is stored as 0x03 0x18
                if (disc->data[uuid_offset] == 0x03 && disc->data[uuid_offset + 1] == 0x18) {
                    return 1;
                }
                uuid_offset += 2;
            }
        }

        offset += ad_struct_len + 1;

    } while ( offset < disc->length_data );

    return 0;
}
#else

static int
ble_prox_cent_should_connect(const struct ble_gap_disc_desc *disc)
{
    struct ble_hs_adv_fields fields;
    int rc;
    int i;
    uint8_t test_addr[6];
    uint32_t peer_addr[6];

    memset(peer_addr, 0x0, sizeof peer_addr);

    /* The device has to be advertising connectability. */
    if (disc->event_type != BLE_HCI_ADV_RPT_EVTYPE_ADV_IND &&
            disc->event_type != BLE_HCI_ADV_RPT_EVTYPE_DIR_IND) {

        return 0;
    }

    rc = ble_hs_adv_parse_fields(&fields, disc->data, disc->length_data);
    if (rc != 0) {
        return rc;
    }

    if (strlen(CONFIG_EXAMPLE_PEER_ADDR) && (strncmp(CONFIG_EXAMPLE_PEER_ADDR, "ADDR_ANY", strlen("ADDR_ANY")) != 0)) {
        ESP_LOGI(tag, "Peer address from menuconfig: %s", CONFIG_EXAMPLE_PEER_ADDR);
        /* Convert string to address */
        sscanf(CONFIG_EXAMPLE_PEER_ADDR, "%lx:%lx:%lx:%lx:%lx:%lx",
               &peer_addr[5], &peer_addr[4], &peer_addr[3],
               &peer_addr[2], &peer_addr[1], &peer_addr[0]);

	/* Conversion */
        for (int i=0; i<6; i++) {
            test_addr[i] = (uint8_t )peer_addr[i];
        }

        if (memcmp(test_addr, disc->addr.val, sizeof(disc->addr.val)) != 0) {
            return 0;
        }
    }

    /* The device has to advertise support for the Proximity sensor (link loss)
     * service (0x1803).
     */
    for (i = 0; i < fields.num_uuids16; i++) {
        if (ble_uuid_u16(&fields.uuids16[i].u) == BLE_SVC_LINK_LOSS_UUID16) {
            return 1;
        }
    }

    return 0;
}
#endif

/**
 * Connects to the sender of the specified advertisement of it looks
 * interesting.  A device is "interesting" if it advertises connectability and
 * support for the Proximity Sensor service.
 */
static void
ble_prox_cent_connect_if_interesting(void *disc)
{
    uint8_t own_addr_type;
    int rc;
    ble_addr_t *addr;

    /* Don't do anything if we don't care about this advertiser. */
#if CONFIG_EXAMPLE_EXTENDED_ADV
    if (!ext_ble_prox_cent_should_connect((struct ble_gap_ext_disc_desc *)disc)) {
        return;
    }
#else
    if (!ble_prox_cent_should_connect((struct ble_gap_disc_desc *)disc)) {
        return;
    }
#endif

#if !(MYNEWT_VAL(BLE_HOST_ALLOW_CONNECT_WITH_SCAN))
    /* Scanning must be stopped before a connection can be initiated. */
    rc = ble_gap_disc_cancel();
    if (rc != 0) {
        MODLOG_DFLT(DEBUG, "Failed to cancel scan; rc=%d\n", rc);
        return;
    }
#endif

    /* Figure out address to use for connect (no privacy for now) */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
        return;
    }

    /* Try to connect the the advertiser.  Allow 30 seconds (30000 ms) for
     * timeout.
     */
#if CONFIG_EXAMPLE_EXTENDED_ADV
    addr = &((struct ble_gap_ext_disc_desc *)disc)->addr;
#else
    addr = &((struct ble_gap_disc_desc *)disc)->addr;
#endif
    rc = ble_gap_connect(own_addr_type, addr, 30000, NULL,
                         ble_prox_cent_gap_event, NULL);
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
 * established.  ble_prox_cent uses the same callback for all connections.
 *
 * @param event                 The event being signalled.
 * @param arg                   Application-specified argument; unused by
 *                              ble_prox_cent.
 *
 * @return                      0 if the application successfully handled the
 *                              event; nonzero on failure.  The semantics
 *                              of the return code is specific to the
 *                              particular GAP event being signalled.
 */
static int
ble_prox_cent_gap_event(struct ble_gap_event *event, void *arg)
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

        /* An advertisement report was received during GAP discovery. */
        print_adv_fields(&fields);

        /* Try to connect to the advertiser if it looks interesting. */
        ble_prox_cent_connect_if_interesting(&event->disc);
        return 0;

    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            /* Connection successfully established. */
            MODLOG_DFLT(INFO, "Connection established ");

            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            print_conn_desc(&desc);
            MODLOG_DFLT(INFO, "\n");

            link_supervision_timeout = 8 * desc.conn_itvl;

            /* Remember peer. */
            rc = peer_add(event->connect.conn_handle);
            if (rc != 0) {
                MODLOG_DFLT(ERROR, "Failed to add peer; rc=%d\n", rc);
                return 0;
            }

            /* Check if this device is reconnected */
            for (int i = 0; i <= MYNEWT_VAL(BLE_MAX_CONNECTIONS); i++) {
                if (disconn_peer[i].addr != NULL) {
                    if (memcmp(disconn_peer[i].addr, &desc.peer_id_addr.val, BLE_ADDR_LEN)) {
                        /* Peer reconnected. Stop alert for this peer */
                        free(disconn_peer[i].addr);
                        disconn_peer[i].addr = NULL;
                        disconn_peer[i].link_lost = false;
                        break;
                    }
                }
            }

#if CONFIG_EXAMPLE_ENCRYPTION
            /** Initiate security - It will perform
             * Pairing (Exchange keys)
             * Bonding (Store keys)
             * Encryption (Enable encryption)
             * Will invoke event BLE_GAP_EVENT_ENC_CHANGE
             **/
            rc = ble_gap_security_initiate(event->connect.conn_handle);
            if (rc != 0) {
                MODLOG_DFLT(INFO, "Security could not be initiated, rc = %d\n", rc);
                return ble_gap_terminate(event->connect.conn_handle,
                                         BLE_ERR_REM_USER_CONN_TERM);
            } else {
                MODLOG_DFLT(INFO, "Connection secured\n");
            }
#else
#if MYNEWT_VAL(BLE_GATT_CACHING_ASSOC_ENABLE)
            rc =  ble_gattc_cache_assoc(desc.peer_id_addr);
            if (rc != 0) {
                MODLOG_DFLT(ERROR, "Cache Association Failed; rc=%d\n", rc);
                return 0;
            }
#else
#if MYNEWT_VAL(BLE_GATTC)
            /* Perform service discovery */
            rc = peer_disc_all(event->connect.conn_handle,
                               ble_prox_cent_on_disc_complete, NULL);
            if (rc != 0) {
                MODLOG_DFLT(ERROR, "Failed to discover services; rc=%d\n", rc);
                return 0;
            }
#endif
#endif // BLE_GATT_CACHING_ASSOC_ENABLE
#endif
        } else {
            /* Connection attempt failed; resume scanning. */
            MODLOG_DFLT(ERROR, "Error: Connection failed; status=%d\n",
                        event->connect.status);
        }
        ble_prox_cent_scan();
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        /* Connection terminated. */
        MODLOG_DFLT(INFO, "disconnect; reason=%d ", event->disconnect.reason);
        print_conn_desc(&event->disconnect.conn);
        MODLOG_DFLT(INFO, "\n");

        /* Start the link loss alert for this connection handle */
        for (int i = 0; i <= MYNEWT_VAL(BLE_MAX_CONNECTIONS); i++) {
            if (disconn_peer[i].addr == NULL) {
                disconn_peer[i].addr = (uint8_t *)malloc(BLE_ADDR_LEN * sizeof(uint8_t));
                if (disconn_peer[i].addr == NULL) {
                    return BLE_HS_ENOMEM;
                }
                memcpy(disconn_peer[i].addr, &event->disconnect.conn.peer_id_addr.val,
                       BLE_ADDR_LEN);
                disconn_peer[i].link_lost = true;
                break;
            }
        }
        /* Stop calculating path loss, restart once connection is established again */
        conn_peer[event->disconnect.conn.conn_handle].calc_path_loss = false;
        conn_peer[event->disconnect.conn.conn_handle].val_handle = 0;

        /* Forget about peer. */
        peer_delete(event->disconnect.conn.conn_handle);

        /* Resume scanning. */
        ble_prox_cent_scan();
        return 0;

    case BLE_GAP_EVENT_DISC_COMPLETE:
        MODLOG_DFLT(INFO, "discovery complete; reason=%d\n",
                    event->disc_complete.reason);
        return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
        /* Encryption has been enabled or disabled for this connection. */
        MODLOG_DFLT(INFO, "encryption change event; status=%d ",
                    event->enc_change.status);
        rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
        assert(rc == 0);
        print_conn_desc(&desc);
#if CONFIG_EXAMPLE_ENCRYPTION
#if MYNEWT_VAL(BLE_GATT_CACHING_ASSOC_ENABLE)
        rc =  ble_gattc_cache_assoc(desc.peer_id_addr);
        if (rc != 0) {
            MODLOG_DFLT(ERROR, "Cache Association Failed; rc=%d\n", rc);
            return 0;
        }
#else
#if MYNEWT_VAL(BLE_GATTC)
        /*** Go for service discovery after encryption has been successfully enabled ***/
        rc = peer_disc_all(event->enc_change.conn_handle,
                           ble_prox_cent_on_disc_complete, NULL);
        if (rc != 0) {
            MODLOG_DFLT(ERROR, "Failed to discover services; rc=%d\n", rc);
            return 0;
        }
#endif
#endif // BLE_GATT_CACHING_ASSOC_ENABLE
#endif
        return 0;

    case BLE_GAP_EVENT_CACHE_ASSOC:
#if MYNEWT_VAL(BLE_GATT_CACHING_ASSOC_ENABLE)
          /* Cache association result for this connection */
          MODLOG_DFLT(INFO, "cache association; conn_handle=%d status=%d cache_state=%s\n",
                      event->cache_assoc.conn_handle,
                      event->cache_assoc.status,
                      (event->cache_assoc.cache_state == 0) ? "INVALID" : "LOADED");
          /* Perform service discovery */
          rc = peer_disc_all(event->cache_assoc.conn_handle,
                             blecent_on_disc_complete, NULL);
          if(rc != 0) {
                MODLOG_DFLT(ERROR, "Failed to discover services; rc=%d\n", rc);
                return 0;
          }
#endif
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

    case BLE_GAP_EVENT_REPEAT_PAIRING:
        /* We already have a bond with the peer, but it is attempting to
         * establish a new secure link.  This app sacrifices security for
         * convenience: just throw away the old bond and accept the new link.
         */

        /* Delete the old bond. */
        rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
        assert(rc == 0);
        ble_store_util_delete_peer(&desc.peer_id_addr);

        /* Return BLE_GAP_REPEAT_PAIRING_RETRY to indicate that the host should
         * continue with the pairing operation.
         */
        return BLE_GAP_REPEAT_PAIRING_RETRY;

#if CONFIG_EXAMPLE_EXTENDED_ADV
    case BLE_GAP_EVENT_EXT_DISC:
        /* An advertisement report was received during GAP discovery. */
        ext_print_adv_report(&event->ext_disc);

        ble_prox_cent_connect_if_interesting(&event->ext_disc);
        return 0;
#endif

    default:
        return 0;
    }
}

void
ble_prox_cent_path_loss_task(void *pvParameters)
{
    int8_t rssi = 0;
    int rc;
    int path_loss;

    while (1) {
        for (int i = 0; i <= MYNEWT_VAL(BLE_MAX_CONNECTIONS); i++) {
            if (conn_peer[i].calc_path_loss) {
                MODLOG_DFLT(INFO, "Connection handle : %d", i);
                rc = ble_gap_conn_rssi(i, &rssi);
                if (rc == 0) {
                    MODLOG_DFLT(INFO, "Current RSSI = %d", rssi);
                } else {
                    MODLOG_DFLT(ERROR, "Failed to get current RSSI");
                }

                path_loss = tx_pwr_lvl - rssi;
                MODLOG_DFLT(INFO, "path loss = %d pwr lvl = %d rssi = %d",
                            path_loss, tx_pwr_lvl, rssi);

                if ((conn_peer[i].val_handle != 0) &&
                        (path_loss > high_threshold || path_loss < low_threshold)) {

                    if (path_loss < low_threshold) {
                        path_loss = 0;
                    }

#if MYNEWT_VAL(BLE_GATTC)
                    rc = ble_gattc_write_no_rsp_flat(i, conn_peer[i].val_handle,
                                                     &path_loss, sizeof(path_loss));
                    if (rc != 0) {
                        MODLOG_DFLT(ERROR, "Error: Failed to write characteristic; rc=%d\n",
                                    rc);
                    } else {
                        MODLOG_DFLT(INFO, "Write to alert level characteristis done");
                    }
#endif
                }
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void
ble_prox_cent_link_loss_task(void *pvParameters)
{
    while (1) {
        for (int i = 0; i <= MYNEWT_VAL(BLE_MAX_CONNECTIONS); i++) {
            if (disconn_peer[i].link_lost && disconn_peer[i].addr != NULL) {
                MODLOG_DFLT(INFO, "Link lost for peer %02x:%02x:%02x:%02x:%02x:%02x, slot %d",
                           disconn_peer[i].addr[5], disconn_peer[i].addr[4],
                           disconn_peer[i].addr[3], disconn_peer[i].addr[2],
                           disconn_peer[i].addr[1], disconn_peer[i].addr[0], i);
            }
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

static void
ble_prox_cent_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

static void
ble_prox_cent_on_sync(void)
{
    int rc;

    /* Make sure we have proper identity address set (public preferred) */
    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    /* Begin scanning for a peripheral to connect to. */
    ble_prox_cent_scan();
}

void ble_prox_cent_host_task(void *param)
{
    ESP_LOGI(tag, "BLE Host Task Started");
    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();

    nimble_port_freertos_deinit();
}

static void
ble_prox_cent_init(void)
{
    /* Task for calculating path loss */
    xTaskCreate(ble_prox_cent_path_loss_task, "ble_prox_cent_path_loss_task", 4096, NULL, 10, NULL);

    /* Task for alerting when link is lost */
    xTaskCreate(ble_prox_cent_link_loss_task, "ble_prox_cent_link_loss_task", 4096, NULL, 10, NULL);
    return;
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

    /* Initialize a task to keep checking path loss of the link */
    ble_prox_cent_init();
    for (int i = 0; i <= MYNEWT_VAL(BLE_MAX_CONNECTIONS); i++) {
        disconn_peer[i].addr = NULL;
        disconn_peer[i].link_lost = true;
    }

    /* Configure the host. */
    ble_hs_cfg.reset_cb = ble_prox_cent_on_reset;
    ble_hs_cfg.sync_cb = ble_prox_cent_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    /* Initialize data structures to track connected peers. */
#if MYNEWT_VAL(BLE_INCL_SVC_DISCOVERY) || MYNEWT_VAL(BLE_GATT_CACHING_INCLUDE_SERVICES)
    rc = peer_init(MYNEWT_VAL(BLE_MAX_CONNECTIONS), 64, 64, 64, 64);
    assert(rc == 0);
#else
    rc = peer_init(MYNEWT_VAL(BLE_MAX_CONNECTIONS), 64, 64, 64);
    assert(rc == 0);
#endif
#if CONFIG_BT_NIMBLE_GAP_SERVICE
    /* Set the default device name. */
    rc = ble_svc_gap_device_name_set("nimble-prox-cent");
    assert(rc == 0);
#endif

    /* XXX Need to have template for store */
    ble_store_config_init();

    nimble_port_freertos_init(ble_prox_cent_host_task);
}
