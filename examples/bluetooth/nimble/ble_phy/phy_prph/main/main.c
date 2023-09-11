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
#include "phy_prph.h"

static uint8_t ext_adv_pattern_1M[] = {
    0x02, 0x01, 0x06,
    0x03, 0x03, 0xab, 0xcd,
    0x03, 0x03, 0xAB, 0xF2,
    0x0e, 0X09, 'b', 'l', 'e', 'p', 'r', 'p', 'h', '-', 'p', 'h', 'y', '-', '1', 'M',
};

static uint8_t ext_adv_pattern_2M[] = {
    0x02, 0x01, 0x06,
    0x03, 0x03, 0xab, 0xcd,
    0x03, 0x03, 0xAB, 0xF2,
    0x0e, 0X09, 'b', 'l', 'e', 'p', 'r', 'p', 'h', '-', 'p', 'h', 'y', '-', '2', 'M',
};

static uint8_t ext_adv_pattern_coded[] = {
    0x02, 0x01, 0x06,
    0x03, 0x03, 0xab, 0xcd,
    0x03, 0x03, 0xAB, 0xF2,
    0x11, 0X09, 'b', 'l', 'e', 'p', 'r', 'p', 'h', '-', 'p', 'h', 'y', '-', 'c', 'o', 'd', 'e',
    'd',
};

static const char *tag = "NimBLE_BLE_PHY_PRPH";
static int bleprph_gap_event(struct ble_gap_event *event, void *arg);
static uint8_t own_addr_type;

static uint8_t s_current_phy;
void ble_store_config_init(void);

/* Set default LE PHY before establishing connection */
void set_default_le_phy(uint8_t tx_phys_mask, uint8_t rx_phys_mask)
{
    int rc = ble_gap_set_prefered_default_le_phy(tx_phys_mask, rx_phys_mask);
    if (rc == 0) {
        MODLOG_DFLT(INFO, "Default LE PHY set successfully");
    } else {
        MODLOG_DFLT(ERROR, "Failed to set default LE PHY");
    }
}

/**
 * Logs information about a connection to the console.
 */
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

static struct os_mbuf *
ext_get_data(uint8_t ext_adv_pattern[], int size)
{
    struct os_mbuf *data;
    int rc;
    data = os_msys_get_pkthdr(size, 0);
    assert(data);
    rc = os_mbuf_append(data, ext_adv_pattern, size);
    assert(rc == 0);
    return data;
}

/**
 * Enables advertising with the following parameters:
 *     o General discoverable mode.
 *     o Undirected connectable mode.
 */
static void
ext_bleprph_advertise(void)
{
    struct ble_gap_ext_adv_params params;
    struct os_mbuf *data = NULL;
    uint8_t instance = 1;
    int rc;

    /* use defaults for non-set params */
    memset (&params, 0, sizeof(params));

    if (s_current_phy == BLE_HCI_LE_PHY_1M_PREF_MASK) {
         params.scannable = 1;
         params.legacy_pdu = 1;
    }

    /*enable connectable advertising for all Phy*/
    params.connectable = 1;

    /* advertise using random addr */
    params.own_addr_type = BLE_OWN_ADDR_PUBLIC;

    /* Set current phy; get mbuf for scan rsp data; fill mbuf with scan rsp data */
    switch (s_current_phy) {
    case BLE_HCI_LE_PHY_1M_PREF_MASK:
        params.primary_phy = BLE_HCI_LE_PHY_1M;
        params.secondary_phy = BLE_HCI_LE_PHY_1M;
        data = ext_get_data(ext_adv_pattern_1M, sizeof(ext_adv_pattern_1M));
        params.sid = 0;
        break;

    case BLE_HCI_LE_PHY_2M_PREF_MASK:
        params.primary_phy = BLE_HCI_LE_PHY_1M;
        params.secondary_phy = BLE_HCI_LE_PHY_2M;
        data = ext_get_data(ext_adv_pattern_2M, sizeof(ext_adv_pattern_2M));
        params.sid = 1;
        break;

    case BLE_HCI_LE_PHY_CODED_PREF_MASK:
        params.primary_phy = BLE_HCI_LE_PHY_CODED;
        params.secondary_phy = BLE_HCI_LE_PHY_CODED;
        data = ext_get_data(ext_adv_pattern_coded, sizeof(ext_adv_pattern_coded));
        params.sid = 2;
        break;
    }

    params.itvl_min = BLE_GAP_ADV_FAST_INTERVAL1_MIN;
    params.itvl_max = BLE_GAP_ADV_FAST_INTERVAL1_MIN;

    /* configure instance 0 */
    rc = ble_gap_ext_adv_configure(instance, &params, NULL,
                                   bleprph_gap_event, NULL);
    assert (rc == 0);

    rc = ble_gap_ext_adv_set_data(instance, data);
    assert (rc == 0);

    /* start advertising */
    rc = ble_gap_ext_adv_start(instance, 0, 0);
    assert (rc == 0);
}

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
            ext_bleprph_advertise();
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "disconnect; reason=%d ", event->disconnect.reason);
        bleprph_print_conn_desc(&event->disconnect.conn);
        MODLOG_DFLT(INFO, "\n");

        /* Connection terminated; resume advertising. */

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

        default:
            return 0;
        }

        ext_bleprph_advertise();
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
    uint8_t all_phy;

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

    s_current_phy = BLE_HCI_LE_PHY_1M_PREF_MASK;

    all_phy = BLE_HCI_LE_PHY_1M_PREF_MASK | BLE_HCI_LE_PHY_2M_PREF_MASK | BLE_HCI_LE_PHY_CODED_PREF_MASK;

    set_default_le_phy(all_phy, all_phy);

    /* Begin advertising. */
    ext_bleprph_advertise();
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

    //ESP_ERROR_CHECK(esp_nimble_hci_and_controller_init());

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        MODLOG_DFLT(ERROR, "Failed to init nimble %d \n", ret);
        return;
    }

    /* Initialize the NimBLE host configuration. */
    ble_hs_cfg.reset_cb = bleprph_on_reset;
    ble_hs_cfg.sync_cb = bleprph_on_sync;
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    ble_hs_cfg.sm_io_cap = CONFIG_EXAMPLE_IO_TYPE;
#ifdef CONFIG_EXAMPLE_BONDING
    ble_hs_cfg.sm_bonding = 1;
#endif
#ifdef CONFIG_EXAMPLE_MITM
    ble_hs_cfg.sm_mitm = 1;
#endif
#ifdef CONFIG_EXAMPLE_USE_SC
    ble_hs_cfg.sm_sc = 1;
#else
    ble_hs_cfg.sm_sc = 0;
#endif
#ifdef CONFIG_EXAMPLE_BONDING
    ble_hs_cfg.sm_our_key_dist = 1;
    ble_hs_cfg.sm_their_key_dist = 1;
#endif

    rc = gatt_svr_init_le_phy();
    assert(rc == 0);

    /* Set the default device name. */
    rc = ble_svc_gap_device_name_set("bleprph-phy");
    assert(rc == 0);

    /* XXX Need to have template for store */
    ble_store_config_init();

    nimble_port_freertos_init(bleprph_host_task);

    /* Initialize command line interface to accept input from user */
    rc = scli_init();
    if (rc != ESP_OK) {
        ESP_LOGE(tag, "scli_init() failed");
    }
}
