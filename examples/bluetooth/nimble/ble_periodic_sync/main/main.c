/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "esp_log.h"
#include "nvs_flash.h"
/* BLE */
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "modlog/modlog.h"

static const char *tag = "NimBLE_BLE_PERIODIC_SYNC";
static int synced = 0;


static int periodic_sync_gap_event(struct ble_gap_event *event, void *arg);

void ble_store_config_init(void);

static void
periodic_sync_scan(void)
{
    uint8_t own_addr_type;
    struct ble_gap_ext_disc_params disc_params = {0};
    int rc;

    /* Figure out address to use while advertising (no privacy for now) */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
        return;
    }

    disc_params.passive = 1;

    rc = ble_gap_ext_disc(own_addr_type, 0, 0,
                          0, 0, 0, &disc_params, &disc_params,
                          periodic_sync_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Error initiating GAP discovery procedure; rc=%d\n",
                    rc);
    }
}

void print_periodic_sync_data(struct ble_gap_event *event)
{
    MODLOG_DFLT(INFO, "status : %d\nperiodic_sync_handle : %d\nsid : %d\n",
                event->periodic_sync.status, event->periodic_sync.sync_handle, event->periodic_sync.sid);
    MODLOG_DFLT(INFO, "adv addr : ");
    ESP_LOG_BUFFER_HEX("NimBLE", event->periodic_sync.adv_addr.val, 6);
    MODLOG_DFLT(INFO, "adv_phy : %s\n", event->periodic_sync.adv_phy == 1 ? "1m" : (event->periodic_sync.adv_phy == 2 ? "2m" : "coded"));
    MODLOG_DFLT(INFO, "per_adv_ival : %d\n", event->periodic_sync.per_adv_ival);
    MODLOG_DFLT(INFO, "adv_clk_accuracy : %d\n", event->periodic_sync.adv_clk_accuracy);
}

void print_periodic_adv_data(struct ble_gap_event *event)
{
    MODLOG_DFLT(INFO, "sync_handle : %d\n", event->periodic_report.sync_handle);
    MODLOG_DFLT(INFO, "tx_power : %d\n", event->periodic_report.tx_power);
    MODLOG_DFLT(INFO, "rssi : %d\n", event->periodic_report.rssi);
    MODLOG_DFLT(INFO, "data_status : %d\n", event->periodic_report.data_status);
    MODLOG_DFLT(INFO, "data_length : %d\n", event->periodic_report.data_length);
    MODLOG_DFLT(INFO, "data : ");
    ESP_LOG_BUFFER_HEX("NimBLE", event->periodic_report.data, event->periodic_report.data_length);
    MODLOG_DFLT(INFO, "\n");
}

void print_periodic_sync_lost_data(struct ble_gap_event *event)
{
    MODLOG_DFLT(INFO, "sync_handle : %d\n", event->periodic_sync_lost.sync_handle);
    MODLOG_DFLT(INFO, "reason : %s\n", event->periodic_sync_lost.reason == 13 ? "timeout" : (event->periodic_sync_lost.reason == 14 ? "terminated locally" : "Unknown reason"));
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
static int
periodic_sync_gap_event(struct ble_gap_event *event, void *arg)
{
    (void)arg;

    switch (event->type) {
    case BLE_GAP_EVENT_EXT_DISC: {
        /* An advertisement report was received during GAP discovery. */
        const struct ble_gap_ext_disc_desc *disc = &event->ext_disc;
        if (disc->sid == 2 && synced == 0) {
            struct ble_gap_periodic_sync_params params = {0};
            int rc;
            synced++;
            params.skip = 10;
            params.sync_timeout = 1000;

#if CONFIG_EXAMPLE_PERIODIC_ADV_ENH
            /* This way the periodic advertising reports will not be
            delivered to host unless the advertising data is changed
            or the Data-Id is updated by the advertiser */
            params.filter_duplicates = 1;
#endif
            rc = ble_gap_periodic_adv_sync_create(&disc->addr, disc->sid, &params, periodic_sync_gap_event, NULL);
            assert(rc == 0);
        }
        return 0;
    }
    case BLE_GAP_EVENT_PERIODIC_REPORT:
        MODLOG_DFLT(INFO, "Periodic adv report event: \n");
        print_periodic_adv_data(event);
        return 0;
    case BLE_GAP_EVENT_PERIODIC_SYNC_LOST:
        MODLOG_DFLT(INFO, "Periodic sync lost\n");
        print_periodic_sync_lost_data(event);
        synced = 0;
        /* Restart scanning to re-sync */
        periodic_sync_scan();
        return 0;
    case BLE_GAP_EVENT_PERIODIC_SYNC:
        MODLOG_DFLT(INFO, "Periodic sync event : \n");
        print_periodic_sync_data(event);
        if (event->periodic_sync.status != 0) {
            synced = 0;
        } else {
            /* Cancel scanning since sync is established */
            ble_gap_disc_cancel();
        }
        return 0;
    default:
        return 0;
    }
}

static void
periodic_sync_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

static void
periodic_sync_on_sync(void)
{
    int rc;
    /* Make sure we have proper identity address set (public preferred) */
    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    /* Begin scanning for a periodic advertiser to sync with. */
    periodic_sync_scan();
}

void periodic_sync_host_task(void *param)
{
    ESP_LOGI(tag, "BLE Host Task Started");
    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();

    nimble_port_freertos_deinit();
}

void
app_main(void)
{
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
    ble_hs_cfg.reset_cb = periodic_sync_on_reset;
    ble_hs_cfg.sync_cb = periodic_sync_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

#if CONFIG_BT_NIMBLE_GAP_SERVICE
    /* Set the default device name. */
    int rc = ble_svc_gap_device_name_set("nimble_periodic_sync");
    assert(rc == 0);
#endif

    /* XXX Need to have template for store */
    ble_store_config_init();
    nimble_port_freertos_init(periodic_sync_host_task);
}
