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
#include "coc_blecent.h"

static const char *tag = "NimBLE_BLE_CENT_L2CAP_COC";
static int blecent_gap_event(struct ble_gap_event *event, void *arg);

void ble_store_config_init(void);

#if MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM) >= 1

#define COC_BUF_COUNT          (3 * MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM))
#define L2CAP_COC_UUID         0x1812
#define MTU                    512

static uint16_t conn_handle_coc ;
static os_membuf_t sdu_coc_mem[OS_MEMPOOL_SIZE(COC_BUF_COUNT, MTU * 2)];
static struct os_mempool sdu_coc_mbuf_mempool;
static struct os_mbuf_pool sdu_os_mbuf_pool;
static int blecent_l2cap_coc_event_cb(struct ble_l2cap_event *event, void *arg);

struct ble_l2cap_chan *coc_chan = NULL;

/**
 * This API is used to send data over L2CAP connection oriented channel.
 */
static void
blecent_l2cap_coc_send_data(struct ble_l2cap_chan *chan)
{
    struct os_mbuf *sdu_rx_data;
    int rc = 0;
    int len = 512;
    uint8_t value[len];

    for (int i = 0; i < len; i++) {
        value[i] = i;
    }

    do {
        sdu_rx_data = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
        if (sdu_rx_data == NULL) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
            sdu_rx_data = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
        }
    } while (sdu_rx_data == NULL);

    os_mbuf_append(sdu_rx_data, value, len);

    print_mbuf_data(sdu_rx_data);

    rc = ble_l2cap_send(chan, sdu_rx_data);

    while (rc == BLE_HS_ESTALLED) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        rc = ble_l2cap_send(chan, sdu_rx_data);
    }
    if (rc == 0) {
        MODLOG_DFLT(INFO, "Data sent successfully");
    } else {
        MODLOG_DFLT(INFO, "Data sending failed, rc = %d", rc);
        os_mbuf_free_chain(sdu_rx_data);
    }
}

/**
 * After connection is established on GAP layer, service discovery is performed. On
 * it's completion, this API is called for making a connection is on L2CAP layer.
 */
static void
blecent_l2cap_coc_on_disc_complete(const struct peer *peer, int status, void *arg)
{
    uint16_t psm = 0x1002;
    struct os_mbuf *sdu_rx;

    sdu_rx = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
    ble_l2cap_connect(conn_handle_coc, psm, MTU, sdu_rx, blecent_l2cap_coc_event_cb,
                      NULL);
}

/**
 * The nimble host executes this callback when a L2CAP  event occurs.  The
 * application associates a L2CAP event callback with each connection that is
 * established.  blecent_l2cap_coc uses the same callback for all connections.
 *
 * @param event                 The event being signalled.
 * @param arg                   Application-specified argument; unused by
 *                                  blecent_l2cap_coc.
 *
 * @return                      0 if the application successfully handled the
 *                                  event; nonzero on failure.  The semantics
 *                                  of the return code is specific to the
 *                                  particular L2CAP event being signalled.
 */
static int
blecent_l2cap_coc_event_cb(struct ble_l2cap_event *event, void *arg)
{
    struct ble_l2cap_chan_info chan_info;

    switch (event->type) {
    case BLE_L2CAP_EVENT_COC_CONNECTED:
        if (event->connect.status) {
            console_printf("LE COC error: %d\n", event->connect.status);
            return 0;
        }

        if (ble_l2cap_get_chan_info(event->connect.chan, &chan_info)) {
            assert(0);
        }

        console_printf("LE COC connected, conn: %d, chan: %p, psm: 0x%02x,"
                       " scid: 0x%04x, ""dcid: 0x%04x, our_mps: %d, our_mtu: %d,"
                       " peer_mps: %d, peer_mtu: %d\n",
                       event->connect.conn_handle, event->connect.chan,
                       chan_info.psm, chan_info.scid, chan_info.dcid,
                       chan_info.our_l2cap_mtu, chan_info.our_coc_mtu,
                       chan_info.peer_l2cap_mtu, chan_info.peer_coc_mtu);

        coc_chan = event->connect.chan;
        return 0;

    case BLE_L2CAP_EVENT_COC_DISCONNECTED:
        console_printf("LE CoC disconnected, chan: %p\n",
                       event->disconnect.chan);
        return 0;

    default:
        return 0;
    }
}

static void
blecent_l2cap_coc_mem_init(void)
{
    int rc;
    rc = os_mempool_init(&sdu_coc_mbuf_mempool, COC_BUF_COUNT, MTU, sdu_coc_mem,
                         "coc_sdu_pool");
    assert(rc == 0);
    rc = os_mbuf_pool_init(&sdu_os_mbuf_pool, &sdu_coc_mbuf_mempool, MTU,
                           COC_BUF_COUNT);
    assert(rc == 0);
}
#endif // #if  MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM) >= 1

/**
 * Called when service discovery of the specified peer has completed.
 */
#if MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM) < 1
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

    /* Now user can perform GATT procedures against the peer: read,
     * write, and subscribe to notifications.
     */
}
#endif

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
 * advertises connectability and support for the Alert Notification service.
 */
#if CONFIG_EXAMPLE_EXTENDED_ADV
static int
ext_blecent_should_connect(const struct ble_gap_ext_disc_desc *disc)
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
    if (strlen(CONFIG_EXAMPLE_PEER_ADDR) &&
            (strncmp(CONFIG_EXAMPLE_PEER_ADDR, "ADDR_ANY", strlen("ADDR_ANY")) != 0)) {
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
    /* The device has to advertise support for L2CAP COC UUID (0x1812).
    */
    do {
        ad_struct_len = disc->data[offset];

        if (!ad_struct_len) {
            break;
        }

        /* Search if ANS UUID is advertised */
        if (disc->data[offset] == 0x03 && disc->data[offset + 1] == 0x03) {
            if ( disc->data[offset + 2] == 0x18 && disc->data[offset + 3] == 0x12 ) {
                return 1;
            }
        }

        offset += ad_struct_len + 1;

    } while ( offset < disc->length_data );
    return 0;
}
#else
static int
blecent_should_connect(const struct ble_gap_disc_desc *disc)
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
        return 0;
    }

    if (strlen(CONFIG_EXAMPLE_PEER_ADDR) &&
            (strncmp(CONFIG_EXAMPLE_PEER_ADDR, "ADDR_ANY", strlen("ADDR_ANY")) != 0)) {
        MODLOG_DFLT(INFO, "Peer address from menuconfig:%s", CONFIG_EXAMPLE_PEER_ADDR);
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

    /* The device has to advertise support for L2CAP COC UUID (0x1812).
     */
    for (i = 0; i < fields.num_uuids16; i++) {
        if (ble_uuid_u16(&fields.uuids16[i].u) == 0x1812) {
            return 1;
        }
    }
    return 0;
}
#endif

/**
 * Connects to the sender of the specified advertisement if it looks
 * interesting.  A device is "interesting" if it advertises connectability and
 * support for the Alert Notification service.
 */
static void
blecent_connect_if_interesting(void *disc)
{
    uint8_t own_addr_type;
    int rc;
    ble_addr_t *addr;

    /* Don't do anything if we don't care about this advertiser. */
#if CONFIG_EXAMPLE_EXTENDED_ADV
    if (!ext_blecent_should_connect((struct ble_gap_ext_disc_desc *)disc)) {
        return;
    }
#else
    if (!blecent_should_connect((struct ble_gap_disc_desc *)disc)) {
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
                         blecent_gap_event, NULL);
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
 * established.  blecent uses the same callback for all connections.
 *
 * @param event                 The event being signalled.
 * @param arg                   Application-specified argument; unused by
 *                              blecent.
 *
 * @return                      0 if the application successfully handled the
 *                              event; nonzero on failure.  The semantics
 *                              of the return code is specific to the
 *                              particular GAP event being signalled.
 */
static int
blecent_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    struct ble_hs_adv_fields fields;
    int rc;
    peer_disc_fn *disc_cb = NULL;

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
        blecent_connect_if_interesting(&event->disc);
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

#if MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM) >= 1
            conn_handle_coc = event->connect.conn_handle;
            disc_cb = blecent_l2cap_coc_on_disc_complete;
#else
            disc_cb = blecent_on_disc_complete;
#endif
            /* Remember peer. */
            rc = peer_add(event->connect.conn_handle);
            if (rc != 0) {
                MODLOG_DFLT(ERROR, "Failed to add peer; rc=%d\n", rc);
                return 0;
            }

            /* Perform service discovery. */
            rc = peer_disc_all(event->connect.conn_handle,
                               disc_cb, NULL);
            if (rc != 0) {
                MODLOG_DFLT(ERROR, "Failed to discover services; rc=%d\n", rc);
                return 0;
            }
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

        /* Resume scanning. */
        blecent_scan();
        return 0;

    case BLE_GAP_EVENT_DISC_COMPLETE:
        MODLOG_DFLT(INFO, "discovery complete; reason=%d\n",
                    event->disc_complete.reason);
        return 0;

#if CONFIG_EXAMPLE_EXTENDED_ADV
    case BLE_GAP_EVENT_EXT_DISC:
        /* An advertisement report was received during GAP discovery. */
        ext_print_adv_report(&event->ext_disc);

        blecent_connect_if_interesting(&event->ext_disc);
        return 0;
#endif

    default:
        return 0;
    }
}

void
ble_coc_cent_task(void *pvParameters)
{
    while (1) {
        if (coc_chan) {
            for (int i = 0; i < 5; i++) {
                blecent_l2cap_coc_send_data(coc_chan);
                vTaskDelay(500 / portTICK_PERIOD_MS);
            }
            coc_chan = NULL;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
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
    int rc;

    /* Make sure we have proper identity address set (public preferred) */
    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    /* Begin scanning for a peripheral to connect to. */
    blecent_scan();
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
    assert(ret == 0);

    /* Configure the host. */
    ble_hs_cfg.reset_cb = blecent_on_reset;
    ble_hs_cfg.sync_cb = blecent_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

#if MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM) >= 1
    blecent_l2cap_coc_mem_init();
    xTaskCreate(ble_coc_cent_task, "ble_coc_cent_task", 4096, NULL, 10, NULL);
#endif

    /* Initialize data structures to track connected peers. */
    rc = peer_init(MYNEWT_VAL(BLE_MAX_CONNECTIONS), 64, 64, 64);
    assert(rc == 0);

#if CONFIG_BT_NIMBLE_GAP_SERVICE
    /* Set the default device name. */
    rc = ble_svc_gap_device_name_set("blecent-l2coc");
    assert(rc == 0);
#endif

    /* XXX Need to have template for store */
    ble_store_config_init();

    nimble_port_freertos_init(blecent_host_task);
}
