/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_check.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "console/console.h"

#include "periodic_sync.h"
#include "cte_config.h"

static const char *TAG = "CTE_SYNC_EXAMPLE";
static int is_synced = 0;

static int periodic_sync_gap_event(struct ble_gap_event *event, void *arg);

#if MYNEWT_VAL(BLE_AOA_AOD)
static uint8_t cte_pattern[] = {0, 4, 8, 12};
static struct ble_gap_cte_sampling_params sync_cte_sampling_params = {
    .slot_durations = 0x2,
    .switching_pattern_length = sizeof(cte_pattern),
    .antenna_ids = cte_pattern,
};
#endif


static void periodic_sync_scan(void)
{
    uint8_t own_addr_type;
    struct ble_gap_disc_params disc_params = {0};
    int rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to determine address type; rc=%d", rc);
        return;
    }

    /**
     * Perform a passive scan.  I.e., don't send follow-up scan requests to
     * each advertiser.
     */
    disc_params.passive = 1;
    rc = ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params,
                      periodic_sync_gap_event, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "GAP discovery failed; rc=%d", rc);
    }
}

static void print_periodic_sync_data(const struct ble_gap_event *event)
{
    ESP_LOGI(TAG, "Periodic Sync Event:");
    ESP_LOGI(TAG, "  Status: %d", event->periodic_sync.status);
    ESP_LOGI(TAG, "  Sync Handle: %d", event->periodic_sync.sync_handle);
    ESP_LOGI(TAG, "  SID: %d", event->periodic_sync.sid);
    ESP_LOGI(TAG, "  Adv PHY: %s",
             event->periodic_sync.adv_phy == 1 ? "1M" :
             (event->periodic_sync.adv_phy == 2 ? "2M" : "Coded"));
    ESP_LOGI(TAG, "  Interval: %d", event->periodic_sync.per_adv_ival);
    ESP_LOGI(TAG, "  Clock Accuracy: %d", event->periodic_sync.adv_clk_accuracy);
    ESP_LOGI(TAG, "  Adv Addr: %02X:%02X:%02X:%02X:%02X:%02X",
             event->periodic_sync.adv_addr.val[5],
             event->periodic_sync.adv_addr.val[4],
             event->periodic_sync.adv_addr.val[3],
             event->periodic_sync.adv_addr.val[2],
             event->periodic_sync.adv_addr.val[1],
             event->periodic_sync.adv_addr.val[0]);
}

#if MYNEWT_VAL(BLE_AOA_AOD)
static void print_iq_report(const struct ble_gap_event *event)
{
    char buffer[512] = {0};
    int len = 0;

    ESP_LOGI(TAG, "IQ Report | Sync Handle: %d  IQ num: %d  RSSI: %d  cte_type: %d  channel: %d",
             event->connless_iq_report.sync_handle,
             event->connless_iq_report.sample_count,
             event->connless_iq_report.rssi / 10,
             event->connless_iq_report.cte_type,
             event->connless_iq_report.channel_index);

    len += snprintf(buffer + len, sizeof(buffer) - len, "I: ");
    for (int i = 0; i < event->connless_iq_report.sample_count; i++) {
        len += snprintf(buffer + len, sizeof(buffer) - len, "%d,", (int8_t)event->connless_iq_report.i_samples[i]);
        if (len >= sizeof(buffer)) break;
    }
    ESP_LOGI(TAG, "%s", buffer);

    len = 0;
    memset(buffer, 0, sizeof(buffer));
    len += snprintf(buffer + len, sizeof(buffer) - len, "Q: ");
    for (int i = 0; i < event->connless_iq_report.sample_count; i++) {
        len += snprintf(buffer + len, sizeof(buffer) - len, "%d,", (int8_t)event->connless_iq_report.q_samples[i]);
        if (len >= sizeof(buffer)) break;
    }
    ESP_LOGI(TAG, "%s", buffer);
}
#endif

static void print_periodic_adv_data(const struct ble_gap_event *event)
{
    ESP_LOGD(TAG, "Periodic Report:");
    ESP_LOGD(TAG, "  Sync Handle: %d", event->periodic_report.sync_handle);
    ESP_LOGD(TAG, "  RSSI: %d", event->periodic_report.rssi);
    ESP_LOGD(TAG, "  TX Power: %d", event->periodic_report.tx_power);
    ESP_LOGD(TAG, "  Data Status: %d", event->periodic_report.data_status);
    ESP_LOGD(TAG, "  Data Length: %d", event->periodic_report.data_length);

    ESP_LOGD(TAG, "  Data :");
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, event->periodic_report.data, event->periodic_report.data_length, ESP_LOG_DEBUG);

}

static void print_sync_lost(const struct ble_gap_event *event)
{
    ESP_LOGW(TAG, "Sync Lost (Handle: %d), Reason: %s",
             event->periodic_sync_lost.sync_handle,
             event->periodic_sync_lost.reason == 13 ? "Timeout" :
             (event->periodic_sync_lost.reason == 14 ? "Terminated Locally" : "Unknown"));
}

/**
 * The nimble host executes this callback when a GAP event occurs.  The
 * application associates a GAP event callback with each connection that is
 * established.  periodic_sync uses the same callback for all connections.
 *
 * @param event                 The event being signalled.
 * @param arg                   Application-specified argument; unused by
 *                                  periodic_sync.
 *
 * @return                      0 if the application successfully handled the
 *                                  event; nonzero on failure.  The semantics
 *                                  of the return code is specific to the
 *                                  particular GAP event being signalled.
 */
static int periodic_sync_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_EXT_DISC: {
        const struct ble_gap_ext_disc_desc *disc = ((struct ble_gap_ext_disc_desc *)(&event->disc));

        if (is_synced) {
            return 0;
        }

        bool should_sync = false;

#if CONFIG_EXAMPLE_SYNC_BY_SID
        if (disc->sid == CONFIG_EXAMPLE_SYNC_TARGET_SID) {
            should_sync = true;
        }
#elif CONFIG_EXAMPLE_SYNC_BY_NAME
        char dev_name[32] = {0};
        const uint8_t *adv_data = disc->data;
        int adv_data_len = disc->length_data;

        int index = 0;
        while (index < adv_data_len) {
            uint8_t len = adv_data[index];
            if (len == 0 || index + len >= adv_data_len) {
                break;
            }

            uint8_t type = adv_data[index + 1];
            if (type == 0x09 || type == 0x08) { // Complete Local Name or Shortened
                int name_len = len - 1;
                if (name_len >= sizeof(dev_name)) {
                    name_len = sizeof(dev_name) - 1;
                }

                memcpy(dev_name, &adv_data[index + 2], name_len);
                dev_name[name_len] = '\0';

                if (strcmp(dev_name, CONFIG_SYNC_TARGET_DEVNAME) == 0) {
                    should_sync = true;
                    break;
                }
            }

            index += len + 1;
        }
#else
    #error "Please select EXAMPLE_SYNC_BY_SID or EXAMPLE_SYNC_BY_NAME in menuconfig"
#endif
        if (should_sync) {
            ble_addr_t addr;
            memcpy(&addr, &disc->addr, sizeof(ble_addr_t));

            struct ble_gap_periodic_sync_params params = {
                .skip = 0,
                .sync_timeout = 1000,
                .sync_cte_type = 0x0,
            };

            int rc = ble_gap_periodic_adv_sync_create(&addr, disc->sid, &params,
                                                    periodic_sync_gap_event, NULL);
            if (rc != 0) {
                ESP_LOGE(TAG, "Failed to create periodic sync, rc=%d\n", rc);
                return 0;
            }

            is_synced = 1;
            ESP_LOGI(TAG, "Started periodic sync with device\n");
        }

        return 0;
    }

    case BLE_GAP_EVENT_PERIODIC_SYNC:
        ESP_LOGI(TAG, "Periodic Sync Established");
        print_periodic_sync_data(event);
        ble_gap_disc_cancel();

#if MYNEWT_VAL(BLE_AOA_AOD)
        int rc = ble_gap_set_connless_iq_sampling_enable(
            event->periodic_sync.sync_handle, 1, 0, &sync_cte_sampling_params);
        assert(rc == 0);
#endif
        return 0;

    case BLE_GAP_EVENT_PERIODIC_REPORT:
        print_periodic_adv_data(event);
        return 0;

    case BLE_GAP_EVENT_PERIODIC_SYNC_LOST:
        print_sync_lost(event);
        is_synced = 0;
        periodic_sync_scan();
        return 0;

#if MYNEWT_VAL(BLE_AOA_AOD)
    case BLE_GAP_EVENT_CONNLESS_IQ_REPORT:
        print_iq_report(event);
        return 0;
#endif

    default:
        return 0;
    }
}

static void periodic_sync_on_reset(int reason)
{
    ESP_LOGE(TAG, "Resetting state; reason=%d", reason);
}

static void periodic_sync_on_sync(void)
{
    int rc;
    /* Make sure we have proper identity address set (public preferred) */
    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    /* Begin scanning for a peripheral to connect to. */
    periodic_sync_scan();
}

void periodic_sync_host_task(void *param)
{
    ESP_LOGI(TAG, "BLE Host Task Started");

    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();

    nimble_port_freertos_deinit();
}

void app_main(void)
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
        ESP_LOGE(TAG, "Failed to init nimble %d \n", ret);
        return;
    }

    /* Configure the host. */
    ble_hs_cfg.reset_cb = periodic_sync_on_reset;
    ble_hs_cfg.sync_cb = periodic_sync_on_sync;

    /* Set the default device name. */
    rc = ble_svc_gap_device_name_set("periodic_sync_CTE");
    assert(rc == 0);


#if MYNEWT_VAL(BLE_AOA_AOD)
    ESP_LOGI(TAG, "\n%s", direction_finding_logo);
    ble_direction_finding_antenna_init(antenna_use_gpio, CONFIG_EXAMPLE_ANT_GPIO_BIT_COUNT);
#endif

    nimble_port_freertos_init(periodic_sync_host_task);

    ESP_LOGI(TAG, "Receive CTE Antenna Pattern Info:");
    ESP_LOGI(TAG, "aoa slot:%d",sync_cte_sampling_params.slot_durations);
    ESP_LOGI(TAG, "pattern_len: %d", sizeof(cte_pattern));
    ESP_LOGI(TAG, "pattern:");
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, cte_pattern, sizeof(cte_pattern), ESP_LOG_INFO);


    uint8_t switch_sampling_rates, num_antennae, max_switch_pattern_len, max_cte_len;
    rc = ble_gap_read_antenna_information(&switch_sampling_rates, &num_antennae,
                                          &max_switch_pattern_len, &max_cte_len);
    if (rc == 0) {
        ESP_LOGI(TAG, "Local CTE Antenna Info:");
        ESP_LOGI(TAG, "  switch_sampling_rates: %d", switch_sampling_rates);
        ESP_LOGI(TAG, "  num_antennae: %d", num_antennae);
        ESP_LOGI(TAG, "  max_switch_pattern_len: %d", max_switch_pattern_len);
        ESP_LOGI(TAG, "  max_cte_len: %d", max_cte_len);
    } else {
        ESP_LOGW(TAG, "Failed to read antenna information");
    }
}
