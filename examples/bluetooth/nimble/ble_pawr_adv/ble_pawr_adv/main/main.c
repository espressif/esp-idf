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

#define BLE_PAWR_EVENT_INTERVAL               (600)
#define BLE_PAWR_NUM_SUBEVTS                  (10)
#define BLE_PAWR_SUB_INTERVAL                 (44)  /*!< Interval between subevents (N * 1.25 ms)   */
#define BLE_PAWR_RSP_SLOT_DELAY               (20)   /*!< The first response slot delay (N * 1.25 ms)*/
#define BLE_PAWR_RSP_SLOT_SPACING             (32)  /*!< Time between response slots (N * 0.125 ms) */
#define BLE_PAWR_NUM_RSP_SLOTS                (5)   /*!< Number of subevent response slots          */
#define BLE_PAWR_SUB_DATA_LEN                 (20)

#define TAG  "NimBLE_BLE_PAwR"

static struct ble_gap_set_periodic_adv_subev_data_params sub_data_params[BLE_PAWR_NUM_SUBEVTS];
static uint8_t sub_data_pattern[BLE_PAWR_SUB_DATA_LEN] = {0};

static int
gap_event_cb(struct ble_gap_event *event, void *arg)
{
    int rc;
    uint8_t sub;
    uint8_t sent_num;
    struct os_mbuf *data;

    switch (event->type) {

    case BLE_GAP_EVENT_PER_SUBEV_DATA_REQ:
        ESP_LOGI(TAG, "[Request] data: %x, subevt start:%d, subevt count:%d",
                  sub_data_pattern[0],
                  event->periodic_adv_subev_data_req.subevent_start,
                  event->periodic_adv_subev_data_req.subevent_data_count);

        sent_num = event->periodic_adv_subev_data_req.subevent_data_count;
        for (uint8_t i = 0; i < sent_num; i++) {
            data = os_msys_get_pkthdr(BLE_PAWR_SUB_DATA_LEN, 0);
            if (!data) {
                ESP_LOGE(TAG, "No memory, %d", i);
                break;
            }
            sub = (i + event->periodic_adv_subev_data_req.subevent_start) % BLE_PAWR_NUM_SUBEVTS;
            memset(&sub_data_pattern[1], sub, BLE_PAWR_SUB_DATA_LEN - 1);
            os_mbuf_append(data, sub_data_pattern, BLE_PAWR_SUB_DATA_LEN);
            sub_data_params[i].subevent = sub;
            sub_data_params[i].response_slot_start = 0;
            sub_data_params[i].response_slot_count = BLE_PAWR_NUM_RSP_SLOTS;
            sub_data_params[i].data = data;
            sub_data_pattern[0]++;
        }

        rc = ble_gap_set_periodic_adv_subev_data(event->periodic_adv_subev_data_req.adv_handle,
                                                 sent_num, sub_data_params);
        if (rc) {
            ESP_LOGE(TAG, "Failed to set Subevent Data, rc = 0x%x", rc);
        }
        return 0;

    case BLE_GAP_EVENT_PER_SUBEV_RESP:
        if (event->periodic_adv_response.data_status == BLE_GAP_PER_ADV_DATA_STATUS_COMPLETE) {
            ESP_LOGI(TAG, "[Response] subevent:%d, response_slot:%d, data_length:%d",
                        event->periodic_adv_response.subevent,
                        event->periodic_adv_response.response_slot,
                        event->periodic_adv_response.data_length);
            const uint8_t *data = event->periodic_adv_response.data;
            ESP_LOGI(TAG, "data: 0x%0x, 0x%0x", data[0], data[1]);
        } else {
            ESP_LOGE(TAG, "[Response] subevent:%d, response_slot:%d, rsp_data status:%d",
                        event->periodic_adv_response.subevent,
                        event->periodic_adv_response.response_slot,
                        event->periodic_adv_response.data_status);
        }
        return 0;
    default:
        return 0;
    }
}

static void
start_periodic_adv(void)
{
    int rc;
    uint8_t addr[6];
    struct ble_gap_periodic_adv_params pparams;
    struct ble_gap_ext_adv_params params;
    struct ble_hs_adv_fields adv_fields;
    struct os_mbuf *data;
    uint8_t instance = 0;

#if MYNEWT_VAL(BLE_PERIODIC_ADV_ENH)
    struct ble_gap_periodic_adv_enable_params eparams;
    memset(&eparams, 0, sizeof(eparams));
#endif

    /* Get the local public address. */
    rc = ble_hs_id_copy_addr(BLE_ADDR_PUBLIC, addr, NULL);
    assert (rc == 0);

    ESP_LOGI(TAG, "Device Address %02x:%02x:%02x:%02x:%02x:%02x", addr[5], addr[4], addr[3],
             addr[2], addr[1], addr[0]);

    /* For periodic we use instance with non-connectable advertising */
    memset (&params, 0, sizeof(params));
    params.own_addr_type = BLE_OWN_ADDR_PUBLIC;
    params.primary_phy = BLE_HCI_LE_PHY_CODED;
    params.secondary_phy = BLE_HCI_LE_PHY_1M;
    params.sid = 0;
    params.itvl_min = BLE_PAWR_EVENT_INTERVAL;
    params.itvl_max = BLE_PAWR_EVENT_INTERVAL;

    rc = ble_gap_ext_adv_configure(instance, &params, NULL, gap_event_cb, NULL);
    assert (rc == 0);

    memset(&adv_fields, 0, sizeof(adv_fields));
    adv_fields.name = (const uint8_t *)"Nimble_PAwR";
    adv_fields.name_len = strlen((char *)adv_fields.name);

    /* mbuf chain will be increased if needed */
    data = os_msys_get_pkthdr(BLE_HCI_MAX_ADV_DATA_LEN, 0);
    assert(data);

    rc = ble_hs_adv_set_fields_mbuf(&adv_fields, data);
    assert(rc == 0);

    rc = ble_gap_ext_adv_set_data(instance, data);
    assert(rc == 0);

    /* configure periodic advertising */
    memset(&pparams, 0, sizeof(pparams));
    pparams.include_tx_power = 0;
    pparams.itvl_min = BLE_GAP_PERIODIC_ITVL_MS(3000);
    pparams.itvl_max = BLE_GAP_PERIODIC_ITVL_MS(3000);
    /* Configure the parameters of PAwR. */
    pparams.num_subevents           = BLE_PAWR_NUM_SUBEVTS;
    pparams.subevent_interval       = BLE_PAWR_SUB_INTERVAL;
    pparams.response_slot_delay     = BLE_PAWR_RSP_SLOT_DELAY;
    pparams.response_slot_spacing   = BLE_PAWR_RSP_SLOT_SPACING;
    pparams.num_response_slots      = BLE_PAWR_NUM_RSP_SLOTS;

    rc = ble_gap_periodic_adv_configure(instance, &pparams);
    assert(rc == 0);

    /* start periodic advertising */
#if MYNEWT_VAL(BLE_PERIODIC_ADV_ENH)
    eparams.include_adi = 1;
    rc = ble_gap_periodic_adv_start(instance, &eparams);
#else
    rc = ble_gap_periodic_adv_start(instance);
#endif
    assert (rc == 0);

    /* start advertising */
    rc = ble_gap_ext_adv_start(instance, 0, 0);
    assert (rc == 0);

    ESP_LOGI(TAG, "instance %u started (periodic)\n", instance);
}

static void
on_reset(int reason)
{
    ESP_LOGE(TAG, "Resetting state; reason=%d\n", reason);
}

static void
on_sync(void)
{
    /* Begin advertising. */
    start_periodic_adv();
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
