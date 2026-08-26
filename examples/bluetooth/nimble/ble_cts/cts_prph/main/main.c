/*
 * SPDX-FileCopyrightText: 2017-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOSConfig.h"
/* BLE */
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "ble_cts_prph.h"
#include "services/cts/ble_svc_cts.h"

#if CONFIG_EXAMPLE_EXTENDED_ADV
static uint8_t ext_adv_pattern_1[] = {
    0x02, BLE_HS_ADV_TYPE_FLAGS, 0x06,
    0x03, BLE_HS_ADV_TYPE_COMP_UUIDS16, 0xab, 0xcd,
    0x03, BLE_HS_ADV_TYPE_COMP_UUIDS16, 0x05, 0x18,
    0x12, BLE_HS_ADV_TYPE_COMP_NAME, 'n', 'i', 'm', 'b', 'l', 'e', '-', 'c', 't', 's', '-', 'p', 'r', 'p', 'h', '-', 'e',
};
#endif

static const char *tag = "NimBLE_CTS_PRPH";
static char device_name[32] = "ble_cts_prph";
static int ble_cts_prph_gap_event(struct ble_gap_event *event, void *arg);

static uint8_t ble_cts_prph_addr_type;

#if CONFIG_EXAMPLE_CI_ID && CONFIG_EXAMPLE_CI_PIPELINE_ID
static char *esp_ble_cts_get_example_name(void)
{
    static char example_name[32];

    memset(example_name, 0, sizeof(example_name));
    snprintf(example_name, sizeof(example_name), "BE%02X_%05X_%02X",
             CONFIG_EXAMPLE_CI_ID & 0xFF,
             CONFIG_EXAMPLE_CI_PIPELINE_ID & 0xFFFFF,
             CONFIG_IDF_FIRMWARE_CHIP_ID & 0xFF);
    return example_name;
}
#endif

/**
 * Utility function to log an array of bytes.
 */
void
print_bytes(const uint8_t *bytes, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        MODLOG_DFLT(INFO, "%s0x%02x", i != 0 ? ":" : "", bytes[i]);
    }
}

void
print_addr(const void *addr)
{
    const uint8_t *u8p;

    u8p = addr;
    MODLOG_DFLT(INFO, "%02x:%02x:%02x:%02x:%02x:%02x",
                u8p[5], u8p[4], u8p[3], u8p[2], u8p[1], u8p[0]);
}

#if CONFIG_EXAMPLE_EXTENDED_ADV
/**
 * Enables advertising with the following parameters:
 *     o General discoverable mode.
 *     o Undirected connectable mode.
 */
static void
ext_ble_cts_prph_advertise(void)
{
    struct ble_gap_ext_adv_params params;
    struct os_mbuf *data;
    uint8_t instance = 0;
    int rc;
#if CONFIG_EXAMPLE_CI_ID && CONFIG_EXAMPLE_CI_PIPELINE_ID
    uint8_t ci_adv_data[32];
    uint8_t ci_adv_len;
    uint8_t name_len;
#endif

    /* First check if any instance is already active */
    if (ble_gap_ext_adv_active(instance)) {
        return;
    }

    /* use defaults for non-set params */
    memset (&params, 0, sizeof(params));

    /* enable connectable advertising */
    params.connectable = 1;

    /* advertise using configured addr */
    params.own_addr_type = ble_cts_prph_addr_type;

    params.primary_phy = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_2M;
    params.sid = 1;

    params.itvl_min = BLE_GAP_ADV_FAST_INTERVAL1_MIN;
    params.itvl_max = BLE_GAP_ADV_FAST_INTERVAL1_MIN;

    /* configure instance 0 */
    rc = ble_gap_ext_adv_configure(instance, &params, NULL,
                                   ble_cts_prph_gap_event, NULL);
    assert (rc == 0);

    /* in this case only scan response is allowed */

#if CONFIG_EXAMPLE_CI_ID && CONFIG_EXAMPLE_CI_PIPELINE_ID
    name_len = strlen(device_name);
    memset(ci_adv_data, 0, sizeof(ci_adv_data));
    ci_adv_data[0] = name_len + 1;
    ci_adv_data[1] = BLE_HS_ADV_TYPE_COMP_NAME;
    memcpy(&ci_adv_data[2], device_name, name_len);
    ci_adv_len = 2 + name_len;

    data = os_msys_get_pkthdr(ci_adv_len, 0);
    assert(data);

    rc = os_mbuf_append(data, ci_adv_data, ci_adv_len);
    assert(rc == 0);
#else
    /* get mbuf for scan rsp data */
    data = os_msys_get_pkthdr(sizeof(ext_adv_pattern_1), 0);
    assert(data);

    /* fill mbuf with scan rsp data */
    rc = os_mbuf_append(data, ext_adv_pattern_1, sizeof(ext_adv_pattern_1));
    assert(rc == 0);
#endif

    rc = ble_gap_ext_adv_set_data(instance, data);
    assert (rc == 0);

    /* start advertising */
    rc = ble_gap_ext_adv_start(instance, 0, 0);
    assert (rc == 0);
}
#else

static void
ble_cts_prph_advertise(void)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    int rc;

    /*
     *  Set the advertisement data included in our advertisements:
     *     o Flags (indicates advertisement type and other general info)
     *     o Advertising tx power
     *     o Device name
     */
    memset(&fields, 0, sizeof(fields));

    /*
     * Advertise two flags:
     *      o Discoverability in forthcoming advertisement (general)
     *      o BLE-only (BR/EDR unsupported)
     */
    fields.flags = BLE_HS_ADV_F_DISC_GEN |
                   BLE_HS_ADV_F_BREDR_UNSUP;

    /*
     * Indicate that the TX power level field should be included; have the
     * stack fill this value automatically.  This is done by assigning the
     * special value BLE_HS_ADV_TX_PWR_LVL_AUTO.
     */
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;

#if !(CONFIG_EXAMPLE_CI_ID && CONFIG_EXAMPLE_CI_PIPELINE_ID)
    static const ble_uuid16_t adv_uuids16[] = {
        BLE_UUID16_INIT(BLE_SVC_CTS_UUID16)
    };
    fields.uuids16 = adv_uuids16;
    fields.num_uuids16 = 1;
    fields.uuids16_is_complete = 1;
#endif

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error setting advertisement data; rc=%d\n", rc);
        return;
    }

    /* Begin advertising */
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(ble_cts_prph_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, ble_cts_prph_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error enabling advertisement; rc=%d\n", rc);
        return;
    }
}
#endif

static int
ble_cts_prph_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed */
        MODLOG_DFLT(INFO, "connection %s; status=%d\n",
                    event->connect.status == 0 ? "established" : "failed",
                    event->connect.status);

        if (event->connect.status == 0) {
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            if (rc == 0) {
                ESP_LOGI(tag, "Connected, conn_handle %d, remote %02x:%02x:%02x:%02x:%02x:%02x",
                         event->connect.conn_handle,
                         desc.peer_ota_addr.val[5], desc.peer_ota_addr.val[4],
                         desc.peer_ota_addr.val[3], desc.peer_ota_addr.val[2],
                         desc.peer_ota_addr.val[1], desc.peer_ota_addr.val[0]);
            }
        } else {
            /* Connection failed; resume advertising */
#if CONFIG_EXAMPLE_EXTENDED_ADV
            ext_ble_cts_prph_advertise();
#else
            ble_cts_prph_advertise();
#endif

        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "disconnect; reason=%d\n", event->disconnect.reason);

        /* Connection terminated; resume advertising */
#if CONFIG_EXAMPLE_EXTENDED_ADV
        ext_ble_cts_prph_advertise();
#else
        ble_cts_prph_advertise();
#endif

        break;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        MODLOG_DFLT(INFO, "adv complete\n");
#if CONFIG_EXAMPLE_EXTENDED_ADV
        ext_ble_cts_prph_advertise();
#else
        ble_cts_prph_advertise();
#endif
        break;

    case BLE_GAP_EVENT_SUBSCRIBE:
        MODLOG_DFLT(INFO, "subscribe event; cur_notify=%d\n value handle; "
                    "val_handle=%d\n",
                    event->subscribe.cur_notify, event->subscribe.attr_handle);

        break;

    case BLE_GAP_EVENT_MTU:
        MODLOG_DFLT(INFO, "mtu update event; conn_handle=%d mtu=%d\n",
                    event->mtu.conn_handle,
                    event->mtu.value);
        break;

    }

    return 0;
}

static void
ble_cts_prph_on_sync(void)
{
    int rc;

    rc = ble_hs_id_infer_auto(0, &ble_cts_prph_addr_type);
    assert(rc == 0);

    uint8_t addr_val[6] = {0};
    rc = ble_hs_id_copy_addr(ble_cts_prph_addr_type, addr_val, NULL);

    MODLOG_DFLT(INFO, "Device Address: ");
    print_addr(addr_val);
    MODLOG_DFLT(INFO, "\n");

    /* Begin advertising */
#if CONFIG_EXAMPLE_EXTENDED_ADV
    ext_ble_cts_prph_advertise();
#else
    ble_cts_prph_advertise();
#endif
}

static void
ble_cts_prph_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

void ble_cts_prph_host_task(void *param)
{
    ESP_LOGI(tag, "BLE Host Task Started");
    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();

    nimble_port_freertos_deinit();
}

void app_main(void)
{
    /* Initialize NVS — it is used to store PHY calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        MODLOG_DFLT(ERROR, "Failed to init nimble %d \n", ret);
        return;
    }

    /* Initialize the NimBLE host configuration */
    ble_hs_cfg.sync_cb = ble_cts_prph_on_sync;
    ble_hs_cfg.reset_cb = ble_cts_prph_on_reset;

    /* Enable bonding */
    ble_hs_cfg.sm_bonding = 1;
    ble_hs_cfg.sm_our_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;
    ble_hs_cfg.sm_their_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;

    ble_hs_cfg.sm_sc = 1;
    ble_hs_cfg.sm_mitm = 1;

#if MYNEWT_VAL(BLE_GATTS)
    int rc;
    rc = gatt_svr_init();
    assert(rc == 0);

#if CONFIG_EXAMPLE_CI_ID && CONFIG_EXAMPLE_CI_PIPELINE_ID
    strncpy(device_name, esp_ble_cts_get_example_name(), sizeof(device_name) - 1);
    device_name[sizeof(device_name) - 1] = '\0';
    ESP_LOGI(tag, "DeviceName:%s, CIID:%02X, PipelineID:%05X, ChipID:%02X",
             device_name, CONFIG_EXAMPLE_CI_ID, CONFIG_EXAMPLE_CI_PIPELINE_ID, CONFIG_IDF_FIRMWARE_CHIP_ID);
#endif

#if CONFIG_BT_NIMBLE_GAP_SERVICE
    /* Set the default device name */
    rc = ble_svc_gap_device_name_set(device_name);
    assert(rc == 0);
#endif
#endif

    /* Start the task */
    nimble_port_freertos_init(ble_cts_prph_host_task);

}
