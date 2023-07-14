/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_random.h"
/* BLE */
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "ble_multi_conn_prph.h"

#if CONFIG_EXAMPLE_EXTENDED_ADV
static uint8_t ext_adv_pattern_1[] = {
    0x02, 0x01, 0x06,
    0x03, 0x03, 0xab, 0xcd,
    0x03, 0x03, 0x18, 0x11,
    0x11, 0X09, 'e', 's', 'p', '-', 'm', 'u', 'l', 't', 'i', '-', 'c', 'o', 'n', 'n', '-', 'e',
};
#endif

static const char *TAG = "ESP_MULTI_CONN_PRPH";
static uint8_t s_ble_prph_conn_num = 0;
static SemaphoreHandle_t s_sem_restart_adv = NULL;

static int ble_prph_gap_event(struct ble_gap_event *event, void *arg);
void ble_store_config_init(void);

/**
 * Enables advertising with the following parameters:
 *     o General discoverable mode.
 *     o Undirected connectable mode.
 */
static void
ble_prph_advertise(void)
{
    int rc;
    ble_addr_t addr;

    if (s_ble_prph_conn_num >= CONFIG_BT_NIMBLE_MAX_CONNECTIONS) {
        return;
    }

#if CONFIG_EXAMPLE_EXTENDED_ADV
    struct ble_gap_ext_adv_params params;
    struct os_mbuf *data;
    uint8_t instance = 0;

    /* First check if any instance is already active */
    if(ble_gap_ext_adv_active(instance)) {
        return;
    }

    memset (&params, 0, sizeof(params));

    /* enable connectable advertising */
    params.connectable = 1;

    /* advertise using random addr */
    params.own_addr_type = BLE_OWN_ADDR_RANDOM;
    params.primary_phy = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_2M;
    params.tx_power = 127;
    params.sid = 1;
    params.itvl_min = BLE_GAP_ADV_ITVL_MS(100);
    params.itvl_max = BLE_GAP_ADV_ITVL_MS(100);

    rc = ble_gap_ext_adv_configure(instance, &params, NULL,
                                   ble_prph_gap_event, NULL);
    assert(rc == 0);

    data = os_msys_get_pkthdr(sizeof(ext_adv_pattern_1), 0);
    assert(data);
    rc = os_mbuf_append(data, ext_adv_pattern_1, sizeof(ext_adv_pattern_1));
    assert(rc == 0);
    rc = ble_gap_ext_adv_set_data(instance, data);
    assert(rc == 0);

    /* We won't connect to a connected device. Change our static random address to simulate
     * multi-connection with only one central and one peripheral.
     */
    rc = ble_hs_id_gen_rnd(0, &addr);
    assert(rc == 0);

    rc = ble_gap_ext_adv_set_addr(instance, &addr);
    assert(rc == 0);

    /* start advertising */
    rc = ble_gap_ext_adv_start(instance, 0, 0);
    assert(rc == 0);
#else
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    const char *name;

    if (ble_gap_adv_active()) {
        return;
    }

    /* We won't connect to a connected device. Change our static random address to simulate
     * multi-connection with only one central and one peripheral.
     */
    rc = ble_hs_id_gen_rnd(0, &addr);
    assert(rc == 0);
    /* set generated address */
    rc = ble_hs_id_set_rnd(addr.val);
    assert(rc == 0);

    /**
     *  Set the advertisement data included in our advertisements:
     *     o Flags (indicates advertisement type and other general info).
     *     o Advertising tx power.
     *     o Device name.
     */
    memset(&fields, 0, sizeof fields);

    /* Advertise two flags:
     *     o Discoverability in forthcoming advertisement (general)
     *     o BLE-only (BR/EDR unsupported).
     */
    fields.flags = BLE_HS_ADV_F_DISC_GEN |
                   BLE_HS_ADV_F_BREDR_UNSUP;

    /* Indicate that the TX power level field should be included; have the
     * stack fill this value automatically.  This is done by assigning the
     * special value BLE_HS_ADV_TX_PWR_LVL_AUTO.
     */
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    name = ble_svc_gap_device_name();
    fields.name = (uint8_t *)name;
    fields.name_len = strlen(name);
    fields.name_is_complete = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to set advertisement data; rc=%d\n", rc);
        return;
    }

    /* Begin advertising. */
    memset(&adv_params, 0, sizeof adv_params);
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(BLE_OWN_ADDR_RANDOM, NULL, BLE_HS_FOREVER,
                           &adv_params, ble_prph_gap_event, NULL);
#endif // CONFIG_EXAMPLE_EXTENDED_ADV

    if (rc) {
        ESP_LOGE(TAG, "Failed to enable advertisement; rc=%d\n", rc);
        return;
    } else {
        ESP_LOGI(TAG, "Started adv, Device Address %s", addr_str(addr.val));
    }
}

static void
ble_prph_restart_adv(void)
{
#if CONFIG_EXAMPLE_RESTART_ADV_AFTER_CONNECTED
    if (!xSemaphoreGive(s_sem_restart_adv)) {
        ESP_LOGE(TAG, "Failed to give Semaphor");
    }
#else
    ble_prph_advertise();
#endif // CONFIG_EXAMPLE_RESTART_ADV_AFTER_CONNECTED
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
ble_prph_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0) {
            /* A new connection was established. */
            ESP_LOGI(TAG, "Connection established. Handle:%d. Total:%d", event->connect.conn_handle,
                     ++s_ble_prph_conn_num);
#if !CONFIG_EXAMPLE_EXTENDED_ADV && CONFIG_EXAMPLE_RESTART_ADV_AFTER_CONNECTED
             ble_prph_restart_adv();
#endif // !CONFIG_EXAMPLE_EXTENDED_ADV && CONFIG_EXAMPLE_RESTART_ADV_AFTER_CONNECTED
        } else {
            /* Restart the advertising */
            ble_prph_restart_adv();
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "Disconnect. Handle:%d. Reason=%d. Total:%d",
                 event->disconnect.conn.conn_handle, event->disconnect.reason, --s_ble_prph_conn_num);

        /* Connection terminated; resume advertising. */
        ble_prph_restart_adv();
        return 0;

#if CONFIG_EXAMPLE_EXTENDED_ADV
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(TAG, "advertisement completed. Reason=%d.",event->adv_complete.reason);
#if CONFIG_EXAMPLE_RESTART_ADV_AFTER_CONNECTED
        ble_prph_restart_adv();
#endif // CONFIG_EXAMPLE_RESTART_ADV_AFTER_CONNECTED
        return 0;
#endif // CONFIG_EXAMPLE_EXTENDED_ADV

#if MYNEWT_VAL(BLE_POWER_CONTROL)
    case BLE_GAP_EVENT_TRANSMIT_POWER:
        ESP_LOGD(TAG, "Transmit power event : status=%d conn_handle=%d reason=%d "
                 "phy=%d power_level=%x power_level_flag=%d delta=%d",
                 event->transmit_power.status, event->transmit_power.conn_handle,
                 event->transmit_power.reason, event->transmit_power.phy,
                 event->transmit_power.transmit_power_level,
                 event->transmit_power.transmit_power_level_flag, event->transmit_power.delta);
        return 0;

     case BLE_GAP_EVENT_PATHLOSS_THRESHOLD:
        ESP_LOGD(TAG, "Pathloss threshold event : conn_handle=%d current path loss=%d "
                 "zone_entered =%d", event->pathloss_threshold.conn_handle,
                 event->pathloss_threshold.current_path_loss, event->pathloss_threshold.zone_entered);
        return 0;
#endif
    }

    return 0;
}

static void
bleprph_on_reset(int reason)
{
    ESP_LOGE(TAG, "Resetting state; reason=%d\n", reason);
}

static void
bleprph_on_sync(void)
{
    /* Begin advertising. */
    ble_prph_advertise();
}

void bleprph_host_task(void *param)
{
    ESP_LOGI(TAG, "BLE Host Task Started");
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

    s_sem_restart_adv = xSemaphoreCreateBinary();
    assert(s_sem_restart_adv);

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init nimble %d ", ret);
        return;
    }
    /* Initialize the NimBLE host configuration. */
    ble_hs_cfg.reset_cb = bleprph_on_reset;
    ble_hs_cfg.sync_cb = bleprph_on_sync;
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    rc = gatt_svr_init();
    assert(rc == 0);

    /* Set the default device name. */
    rc = ble_svc_gap_device_name_set("esp-multi-conn");
    assert(rc == 0);

    /* XXX Need to have template for store */
    ble_store_config_init();

    nimble_port_freertos_init(bleprph_host_task);

#if CONFIG_EXAMPLE_RESTART_ADV_AFTER_CONNECTED
    int delay_ms;

    /* Restart the advertising if the connection has been established successfully. This can
     * help to simulate multiple devices with only one peripheral development board.
     */
    while (true)
    {
        if (xSemaphoreTake(s_sem_restart_adv, portMAX_DELAY)) {
            /* Delay a random time to increase the randomness of the test. */
            delay_ms = (esp_random() % 300) + 100;
            vTaskDelay(pdMS_TO_TICKS(delay_ms));
            ble_prph_advertise();
        } else {
            ESP_LOGE(TAG, "Failed to take Semaphor");
        }
    }
#endif // CONFIG_EXAMPLE_RESTART_ADV_AFTER_CONNECTED
}
