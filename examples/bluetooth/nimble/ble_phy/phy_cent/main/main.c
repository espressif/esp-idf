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
#include "phy_cent.h"

static const char *tag = "NimBLE_BLE_PHY_CENT";
static int blecent_gap_event(struct ble_gap_event *event, void *arg);
static ble_addr_t conn_addr;

static void blecent_scan(void);

static uint8_t s_current_phy;
void ble_store_config_init(void);

#if MYNEWT_VAL(BLE_GATTC)
/**
 * Performs GATT operation against the specified peer:
 * 1. Reads the Supported LE PHY characteristic.
 * 2. After read is completed, performs write blob
 *
 * If the peer does not support a required service, characteristic, or
 * descriptor, then the peer lied when it claimed support for the LE
 * PHY service!  When this happens, or if a GATT procedure fails,
 * this function immediately terminates the connection.
 */
static int
blecent_on_write(uint16_t conn_handle,
                 const struct ble_gatt_error *error,
                 struct ble_gatt_attr *attr,
                 void *arg)
{
    MODLOG_DFLT(INFO, "Write complete; status=%d conn_handle=%d", error->status,
                conn_handle);
    if (error->status == 0) {
        MODLOG_DFLT(INFO, " attr_handle=%d", attr->handle);
    }

    /* Terminate the connection once GATT procedure is completed */
    ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);

    MODLOG_DFLT(INFO, "\n");
    return 0;
}

static int
blecent_on_read(uint16_t conn_handle,
                const struct ble_gatt_error *error,
                struct ble_gatt_attr *attr,
                void *arg)
{
    MODLOG_DFLT(INFO, "Read complete; status=%d conn_handle=%d", error->status,
                conn_handle);
    if (error->status == 0) {
        MODLOG_DFLT(INFO, " attr_handle=%d value=", attr->handle);
        print_mbuf(attr->om);
    }

    MODLOG_DFLT(INFO, "\n");

    /* Write 1000 bytes to the LE PHY characteristic.*/
    const struct peer_chr *chr;
    int len = 1000;
    uint8_t value[len];
    int rc;
    struct os_mbuf *txom;

    const struct peer *peer = peer_find(conn_handle);
    chr = peer_chr_find_uuid(peer,
                             BLE_UUID16_DECLARE(LE_PHY_UUID16),
                             BLE_UUID16_DECLARE(LE_PHY_CHR_UUID16));
    if (chr == NULL) {
        MODLOG_DFLT(ERROR, "Error: Peer doesn't support the Alert "
                    "Notification Control Point characteristic\n");
        goto err;
    }

    /* Fill the value array with data */
    for (int i = 0; i < len; i++) {
        value[i] = i;
    }

    txom = ble_hs_mbuf_from_flat(&value, len);
    if (!txom) {
        MODLOG_DFLT(ERROR, "Insufficient memory");
        goto err;
    }

    rc = ble_gattc_write_long(conn_handle, chr->chr.val_handle, 0,
                              txom, blecent_on_write, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Error: Failed to write characteristic; rc=%d\n",
                    rc);
        goto err;
    }
    return 0;

err:
    /* Terminate the connection. */
    return ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
}

static void
blecent_read(const struct peer *peer)
{
    const struct peer_chr *chr;
    int rc;

    /* Read the supported-new-alert-category characteristic. */
    chr = peer_chr_find_uuid(peer,
                             BLE_UUID16_DECLARE(LE_PHY_UUID16),
                             BLE_UUID16_DECLARE(LE_PHY_CHR_UUID16));
    if (chr == NULL) {
        MODLOG_DFLT(ERROR, "Error: Peer doesn't support the Supported "
                    "LE PHY characteristic\n");
        goto err;
    }

    rc = ble_gattc_read(peer->conn_handle, chr->chr.val_handle,
                        blecent_on_read, NULL);
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
blecent_on_disc_complete(const struct peer *peer, int status, void *arg)
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

    /* Now perform GATT procedure against the peer: read
     */
    blecent_read(peer);

}
#endif

/* Set default LE PHY before establishing connection */
void set_default_le_phy(uint8_t tx_phys_mask, uint8_t rx_phys_mask)
{
    int rc = ble_gap_set_prefered_default_le_phy(tx_phys_mask, rx_phys_mask);
    if (rc == 0) {
        MODLOG_DFLT(INFO, "Default LE PHY set successfully; tx_phy = %d, rx_phy = %d",
                    tx_phys_mask, rx_phys_mask);
    } else {
        MODLOG_DFLT(ERROR, "Failed to set default LE PHY");
    }
}

/**
 * Initiates the GAP general discovery procedure.
 */
static void
blecent_scan(void)
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
                      blecent_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Error initiating GAP discovery procedure; rc=%d\n",
                    rc);
    }
}

/**
 * Indicates whether we should try to connect to the sender of the specified
 * advertisement.  The function returns a positive result if the device
 * advertises connectability and support for the LE PHY service.
 */
static int
ext_blecent_should_connect(const struct ble_gap_ext_disc_desc *disc)
{
    int offset = 0;
    int ad_struct_len = 0;
    uint8_t test_addr[6];
    uint32_t peer_addr[6];
    uint8_t type = 0;

    memset(peer_addr, 0x0, sizeof peer_addr);

    if (disc->legacy_event_type != BLE_HCI_ADV_RPT_EVTYPE_ADV_IND &&
            disc->legacy_event_type != BLE_HCI_ADV_RPT_EVTYPE_DIR_IND) {
        return 0;
    }
    if (strlen(CONFIG_EXAMPLE_PEER_ADDR) && (strncmp(CONFIG_EXAMPLE_PEER_ADDR, "ADDR_ANY", strlen("ADDR_ANY")) != 0)) {
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

    /* The device has to advertise support LE PHY UUID (0xABF2).
    */
    do {
        ad_struct_len = disc->data[offset];
        if (!ad_struct_len || (offset + ad_struct_len + 1 > disc->length_data)) {
            break;
        }
        type = disc->data[offset + 1];
        if ((type == 0x02 || type == 0x03) && ad_struct_len >= 3) {
            /* Scan UUID bytes for LE_PHY_UUID16 (little-endian: 0xF2 0xAB) */
            for (int i = 2; i + 1 < ad_struct_len; i += 2) {
                if (disc->data[offset + i] == 0xF2 && disc->data[offset + i + 1] == 0xAB) {
                    return 1;
                }
            }
        }
        offset += ad_struct_len + 1;
    } while (offset < disc->length_data);
    return 0;
}

/**
 * Connects to the sender of the specified advertisement of it looks
 * interesting.  A device is "interesting" if it advertises connectability and
 * support for the LE PHY service.
 */
static void
blecent_connect_if_interesting(void *disc)
{
    uint8_t own_addr_type;
    int rc;
    ble_addr_t *addr;

    /* Don't do anything if we don't care about this advertiser. */
    if (!ext_blecent_should_connect((struct ble_gap_ext_disc_desc *)disc)) {
        return;
    }

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

    addr = &((struct ble_gap_ext_disc_desc *)disc)->addr;

    /* Copy addr information for next connection */
    memcpy(&conn_addr, addr, sizeof(conn_addr));

    MODLOG_DFLT(INFO, "Attempting to connect to : %x %x %x %x %x %x with type %d \n",
		    conn_addr.val[5],conn_addr.val[4], conn_addr.val[3], conn_addr.val[2], conn_addr.val[1], conn_addr.val[0],
		    conn_addr.type);
    /* Try to connect the the advertiser.  Allow 30 seconds (30000 ms) for
     * timeout.
     */

    rc = ble_gap_connect(own_addr_type, &conn_addr, 30000, NULL,
                         blecent_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Error: Failed to connect to device; addr_type=%d "
                    "addr=%s; rc=%d\n",
                    conn_addr.type, addr_str(conn_addr.val), rc);
        return;
    }
}

/**
 * The nimble host executes this callback when a GAP event occurs.  The
 * application associates a GAP event callback with each connection that is
 * established.  blecent uses the same callback for all connections.
 *
 * @param event                 The event being signalled.
 * @param arg                   Application-specified argument; unused by
 *                                  blecent.
 *
 * @return                      0 if the application successfully handled the
 *                                  event; nonzero on failure.  The semantics
 *                                  of the return code is specific to the
 *                                  particular GAP event being signalled.
 */
static int
blecent_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            /* Connection successfully established. */

	    switch (s_current_phy) {
            case BLE_HCI_LE_PHY_1M_PREF_MASK:
                MODLOG_DFLT(INFO,"Connection established on 1M Phy");
                break;

            case BLE_HCI_LE_PHY_2M_PREF_MASK:
            case BLE_HCI_LE_PHY_1M_PREF_MASK | BLE_HCI_LE_PHY_2M_PREF_MASK:
	        MODLOG_DFLT(INFO,"Connection established on 2M Phy");
                break;

            case BLE_HCI_LE_PHY_CODED_PREF_MASK:
	        MODLOG_DFLT(INFO,"Connection established on Coded Phy");
                break;
            }

            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            print_conn_desc(&desc);
            MODLOG_DFLT(INFO, "\n");

            /* Remember peer. */
            rc = peer_add(event->connect.conn_handle);
            if (rc != 0) {
                MODLOG_DFLT(ERROR, "Failed to add peer; rc=%d\n", rc);
                return 0;
            }

#if MYNEWT_VAL(BLE_GATTC)
            /* Perform service discovery. */
            rc = peer_disc_all(event->connect.conn_handle,
                               blecent_on_disc_complete, NULL);
            if (rc != 0) {
                MODLOG_DFLT(ERROR, "Failed to discover services; rc=%d\n", rc);
                return 0;
            }
#endif
        } else {
            /* Connection attempt failed; resume scanning. */
            MODLOG_DFLT(ERROR, "Error: Connection failed; status=%d\n",
                        event->connect.status);
            blecent_scan();
        }

        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        /* Connection terminated. */
        MODLOG_DFLT(INFO, "disconnect; reason=%d ", event->disconnect.reason);
        print_conn_desc(&event->disconnect.conn);
        MODLOG_DFLT(INFO, "\n");

        /* Forget about peer. */
        peer_delete(event->disconnect.conn.conn_handle);

        switch (s_current_phy) {
        case BLE_HCI_LE_PHY_1M_PREF_MASK:
            /* Setting current phy to create connection on 2M PHY */
            s_current_phy = BLE_HCI_LE_PHY_2M_PREF_MASK;
            break;

        case BLE_HCI_LE_PHY_2M_PREF_MASK:
            /* Setting current phy to create connection on CODED PHY */
            s_current_phy = BLE_HCI_LE_PHY_CODED_PREF_MASK;
            break;

        case BLE_HCI_LE_PHY_CODED_PREF_MASK:
            return 0;
        }

	vTaskDelay(200);

        /* Attempt direct connection on 2M or Coded phy now */
        if (s_current_phy == BLE_HCI_LE_PHY_CODED_PREF_MASK) {
            MODLOG_DFLT(INFO, " Attempting to initiate connection on Coded PHY \n");
            ble_gap_ext_connect(0, &conn_addr, 30000, BLE_HCI_LE_PHY_CODED_PREF_MASK,
			        NULL, NULL, NULL, blecent_gap_event, NULL);
        }
        else if (s_current_phy == BLE_HCI_LE_PHY_2M_PREF_MASK) {
            MODLOG_DFLT(INFO, " Attempting to initiate connection on 2M PHY \n");
            ble_gap_ext_connect(0, &conn_addr, 30000, (BLE_HCI_LE_PHY_1M_PREF_MASK | BLE_HCI_LE_PHY_2M_PREF_MASK),
	                        NULL, NULL, NULL, blecent_gap_event, NULL);
        }
        return 0;

    case BLE_GAP_EVENT_DISC_COMPLETE:
        MODLOG_DFLT(INFO, "discovery complete; reason=%d\n",
                    event->disc_complete.reason);
        return 0;

    case BLE_GAP_EVENT_EXT_DISC:
        /* An advertisement report was received during GAP discovery. */
        ext_print_adv_report(&event->ext_disc);

        blecent_connect_if_interesting(&event->ext_disc);
        return 0;

    default:
        return 0;
    }
}

static void
blecent_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

static void
blecent_on_sync(void)
{
    int ii, rc;
    uint8_t all_phy;
    uint8_t test_addr[6];
    uint32_t peer_addr[6];

    memset(peer_addr, 0x0, sizeof peer_addr);

    /* Make sure we have proper identity address set (public preferred) */
    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    all_phy = BLE_HCI_LE_PHY_1M_PREF_MASK | BLE_HCI_LE_PHY_2M_PREF_MASK | BLE_HCI_LE_PHY_CODED_PREF_MASK;

    set_default_le_phy(all_phy, all_phy);

    if (strlen(CONFIG_EXAMPLE_PEER_ADDR) &&
	(strncmp(CONFIG_EXAMPLE_PEER_ADDR, "ADDR_ANY", strlen("ADDR_ANY")) != 0)) {
        /* User wants to connect on 2M or coded phy directly */
        sscanf(CONFIG_EXAMPLE_PEER_ADDR, "%lx:%lx:%lx:%lx:%lx:%lx",
               &peer_addr[5], &peer_addr[4], &peer_addr[3],
               &peer_addr[2], &peer_addr[1], &peer_addr[0]);

        /* Conversion */
        for (int i=0; i<6; i++) {
            test_addr[i] = (uint8_t )peer_addr[i];
        }

        for(ii = 0 ;ii < 6; ii++)
            conn_addr.val[ii] = test_addr[ii];

        conn_addr.type = 0;

        vTaskDelay(300);

	    s_current_phy = BLE_HCI_LE_PHY_1M_PREF_MASK | BLE_HCI_LE_PHY_2M_PREF_MASK ;

        ble_gap_ext_connect(0, &conn_addr, 30000, s_current_phy,
		        NULL, NULL, NULL, blecent_gap_event, NULL);
    }
    else {
        s_current_phy = BLE_HCI_LE_PHY_1M_PREF_MASK;
        /* Begin scanning for a peripheral to connect to. */
        blecent_scan();
    }
}

void blecent_host_task(void *param)
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
    if  (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        MODLOG_DFLT(ERROR, "Failed to init nimble %d \n", ret);
        return;
    }

    /* Configure the host. */
    ble_hs_cfg.reset_cb = blecent_on_reset;
    ble_hs_cfg.sync_cb = blecent_on_sync;
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
    rc = ble_svc_gap_device_name_set("blecent-phy");
    assert(rc == 0);
#endif

    /* XXX Need to have template for store */
    ble_store_config_init();

    nimble_port_freertos_init(blecent_host_task);

}
