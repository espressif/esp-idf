/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
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

#define TAG                     "NimBLE_BLE_PAwR_CONN"
#define TARGET_NAME             "Nimble_PAwR_CONN"
#define BLE_PAWR_RSP_DATA_LEN   (10)
static uint8_t sub_data_pattern[BLE_PAWR_RSP_DATA_LEN] = {0};

static int create_periodic_sync(struct ble_gap_ext_disc_desc *disc);
static void start_scan(void);

static struct ble_hs_adv_fields fields;
static bool synced = false;
uint8_t device_addr[6];

static struct ble_gap_conn_desc desc;
char *
addr_str(const void *addr)
{
    static char buf[6 * 2 + 5 + 1];
    const uint8_t *u8p;

    u8p = addr;
    sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x",
            u8p[5], u8p[4], u8p[3], u8p[2], u8p[1], u8p[0]);

    return buf;
}

void
print_conn_desc(const struct ble_gap_conn_desc *desc)
{
   ESP_LOGI(TAG,"handle=%d our_ota_addr_type=%d our_ota_addr=%s ",
                desc->conn_handle, desc->our_ota_addr.type,
                addr_str(desc->our_ota_addr.val));
   ESP_LOGI(TAG, "our_id_addr_type=%d our_id_addr=%s ",
                desc->our_id_addr.type, addr_str(desc->our_id_addr.val));
   ESP_LOGI(TAG, "peer_ota_addr_type=%d peer_ota_addr=%s ",
                desc->peer_ota_addr.type, addr_str(desc->peer_ota_addr.val));
   ESP_LOGI(TAG, "peer_id_addr_type=%d peer_id_addr=%s ",
                desc->peer_id_addr.type, addr_str(desc->peer_id_addr.val));
   ESP_LOGI(TAG, "conn_itvl=%d conn_latency=%d supervision_timeout=%d "
                "encrypted=%d authenticated=%d bonded=%d\n",
                desc->conn_itvl, desc->conn_latency,
                desc->supervision_timeout,
                desc->sec_state.encrypted,
                desc->sec_state.authenticated,
                desc->sec_state.bonded);
}
static int
gap_event_cb(struct ble_gap_event *event, void *arg)
{
    int rc;
    uint8_t *addr;
    struct ble_gap_ext_disc_desc *disc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0) {
        ESP_LOGI(TAG, "Connection established, conn_handle = 0x%0x, sync handle= 0x%02x, status = 0x%0x\n",event->connect.conn_handle, event->connect.sync_handle, event->connect.status);
        rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
        if (rc == 0) {
            print_conn_desc(&desc);
        }
        else{
            ESP_LOGE(TAG,"Failed to find Conn Information");
        }
        } else{
            ESP_LOGW(TAG, "[Connection Failed], conn_handle = 0x%02x, sync handle = 0x%0x, status = 0x%0x\n",event->connect.conn_handle, event->connect.sync_handle, event->connect.status);
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGW(TAG, "[Disconnected], conn_handle = 0x%02x, status = 0x%0x\n",event->disconnect.conn.conn_handle,event->disconnect.reason);
        return 0;

    case BLE_GAP_EVENT_EXT_DISC:
        disc = &event->ext_disc;
        addr = disc->addr.val;
        ESP_LOGI(TAG, "[Disc advertiser] addr %02x:%02x:%02x:%02x:%02x:%02x, props: 0x%x, rssi:%d",
                 addr[5], addr[4], addr[3], addr[2], addr[1], addr[0], disc->props, disc->rssi);
        if (synced) {
            return 0;
        }

        rc = ble_hs_adv_parse_fields(&fields, disc->data, disc->length_data);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to parse adv data, rc = %d", rc);
            return 0;
        }

        if (fields.name_len && !memcmp(fields.name, TARGET_NAME, strlen(TARGET_NAME))) {
            create_periodic_sync(disc);
        }
        return 0;

    case BLE_GAP_EVENT_PERIODIC_REPORT:
        ESP_LOGI(TAG, "[Periodic Adv Report] handle:%d, event_counter(%d), subevent(%d)",
            event->periodic_report.sync_handle,
            event->periodic_report.event_counter,
            event->periodic_report.subevent);

        struct ble_gap_periodic_adv_response_params param = {
            .request_event = event->periodic_report.event_counter,
            .request_subevent = event->periodic_report.subevent,
            .response_subevent = event->periodic_report.subevent,
            .response_slot = 2,
        };

        struct os_mbuf *data = os_msys_get_pkthdr(BLE_PAWR_RSP_DATA_LEN, 0);
        if (!data) {
            ESP_LOGE(TAG, "No memory");
            return 0;
        }
        // create a special data for checking manually in ADV side

        sub_data_pattern[0] = event->periodic_report.subevent;
        rc = ble_hs_id_copy_addr(BLE_ADDR_PUBLIC, device_addr, NULL);
        sub_data_pattern[1] = param.response_slot;
        memcpy(&sub_data_pattern[2],device_addr,BLE_DEV_ADDR_LEN);

        os_mbuf_append(data, sub_data_pattern, BLE_PAWR_RSP_DATA_LEN);

        rc = ble_gap_periodic_adv_set_response_data(event->periodic_report.sync_handle, &param, data);
        if (rc) {
            ESP_LOGE(TAG, "Set response data failed, sync handle: %d, subev(%x), rsp_slot(%d), rc(0x%x)",
                     event->periodic_report.sync_handle, param.response_subevent, param.response_slot, rc);
        } else {
            ESP_LOGW(TAG, "[RSP Data Set] sync handle: %d, subev(%x), rsp_slot(%d), rc(0x%x)",
                     event->periodic_report.sync_handle, param.response_subevent, param.response_slot, rc);

        }
        os_mbuf_free_chain(data);

        return 0;

    case BLE_GAP_EVENT_PERIODIC_SYNC_LOST:
        ESP_LOGE(TAG, "[Periodic Sync Lost] handle:%d, Reason = 0x%x",
                 event->periodic_sync_lost.sync_handle, event->periodic_sync_lost.reason);
        synced = false;
        start_scan();
        return 0;

    case  BLE_GAP_EVENT_PERIODIC_SYNC:
        if (!event->periodic_sync.status) {
            ESP_LOGI(TAG, "[Periodic Sync Established] sync handle:%d, num_subevents:0x%x",
                     event->periodic_sync.sync_handle, event->periodic_sync.num_subevents);
            ESP_LOGI(TAG, "subevent_interval:0x%x, slot_delay:0x%x,slot_spacing:0x%x",
                     event->periodic_sync.subevent_interval,
                     event->periodic_sync.response_slot_delay,
                     event->periodic_sync.response_slot_spacing);
            ble_gap_disc_cancel();

            // choose subevents in range 0 to (num_subevents - 1)
            uint8_t subevents[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            int result = ble_gap_periodic_adv_sync_subev(event->periodic_sync.sync_handle, 0, sizeof(subevents), subevents);
            if (result == ESP_OK) {
                ESP_LOGI(TAG, "[Subevent Sync OK] sync handle:%d, sync_subevents:%d\n", event->periodic_sync.sync_handle, sizeof(subevents));
            } else {
                ESP_LOGE(TAG, "Failed to sync subevents, rc = 0x%x", result);
            }
        } else {
            ESP_LOGE(TAG, "Periodic Sync Error, status = %d", event->periodic_sync.status);
            synced = false;
            start_scan();
        }
        return 0;

    default:
        return 0;
    }
}

static int
create_periodic_sync(struct ble_gap_ext_disc_desc *disc)
{
    int rc;
    struct ble_gap_periodic_sync_params params;

    params.skip = 0;
    params.sync_timeout = 4000;
    params.reports_disabled = 0;

#if CONFIG_EXAMPLE_PERIODIC_ADV_ENH
    /* This way the periodic advertising reports will not be
    delivered to host unless the advertising data is changed
    or the Data-Id is updated by the advertiser */
    params.filter_duplicates = 1;
#endif

    rc = ble_gap_periodic_adv_sync_create(&disc->addr, disc->sid, &params, gap_event_cb, NULL);
    if (!rc) {
        synced = true;
        ESP_LOGI(TAG, "Create sync");
    } else {
        ESP_LOGE(TAG, "Failed to create sync, rc = %d",  rc);
    }

    return rc;
}

static void
start_scan(void)
{
    int rc;
    struct ble_gap_ext_disc_params disc_params;

    /* Perform a passive scan.  I.e., don't send follow-up scan requests to
     * each advertiser.
     */
    disc_params.itvl = BLE_GAP_SCAN_ITVL_MS(600);
    disc_params.window = BLE_GAP_SCAN_ITVL_MS(300);
    disc_params.passive = 1;

    /* Tell the controller to filter duplicates; we don't want to process
     * repeated advertisements from the same device.
     */
    rc = ble_gap_ext_disc(BLE_OWN_ADDR_PUBLIC, 0, 0, 1, 0, 0,  NULL, &disc_params,
                          gap_event_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Error initiating GAP discovery procedure; rc=%d\n", rc);
    }
}

static void
on_reset(int reason)
{
    ESP_LOGE(TAG, "Resetting state; reason=%d\n", reason);
}

/* Cnnot find `ble_single_xxxx()`, workaround */
// static void
// on_sync(void)
// {
//     int ble_single_env_init(void);
//     int ble_single_init(void);

//     int rc;

//     rc = ble_single_env_init();
//     assert(!rc);
//     rc = ble_single_init();
//     assert(!rc);

//     start_scan();
// }

static void
on_sync(void)
{
    int rc;

    /* Make sure we have proper identity address set (public preferred) */
    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    start_scan();
}

void pawr_host_task(void *param)
{
    ESP_LOGI(TAG, "BLE Host Task Started");
    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();

    nimble_port_freertos_deinit();
}

void
app_main(void)
{
    esp_err_t ret;

    /* Initialize NVS — it is used to store PHY calibration data */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init nimble %d ", ret);
        return;
    }

    /* Initialize the NimBLE host configuration. */
    ble_hs_cfg.reset_cb = on_reset;
    ble_hs_cfg.sync_cb = on_sync;

    nimble_port_freertos_init(pawr_host_task);
}
