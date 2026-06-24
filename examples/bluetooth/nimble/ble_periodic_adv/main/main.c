/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <assert.h>
#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"
/* BLE */
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_hs_adv.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "modlog/modlog.h"
#include "esp_peripheral.h"

static uint8_t periodic_adv_raw_data[] = {'E', 'S', 'P', '_', 'P', 'E', 'R', 'I', 'O', 'D', 'I', 'C', '_', 'A', 'D', 'V'};

static const char *tag = "NimBLE_BLE_PERIODIC_ADV";

void ble_store_config_init(void);

/**
 * Configures and starts periodic advertising with extended
 * non-connectable advertising on instance 0.
 */
static void
start_periodic_adv(uint8_t own_addr_type)
{
    int rc;
    struct ble_gap_periodic_adv_params pparams;
    struct ble_gap_ext_adv_params params;
    struct ble_hs_adv_fields adv_fields;
    struct os_mbuf *data;
    uint8_t instance = 0;
    ble_addr_t addr;
    uint8_t addr_type;
#if MYNEWT_VAL(BLE_PERIODIC_ADV_ENH)
    struct ble_gap_periodic_adv_start_params eparams;
    memset(&eparams, 0, sizeof(eparams));
#endif

    /* For periodic we use instance with non-connectable advertising */
    memset (&params, 0, sizeof(params));

    params.own_addr_type = own_addr_type;
    params.primary_phy = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_2M;
    /* Use controller-selected default TX power. */
    params.tx_power = 0x7f;
    params.sid = 2;

    /* configure instance 0 */
    rc = ble_gap_ext_adv_configure(instance, &params, NULL, NULL, NULL);
    assert (rc == 0);

    addr_type = params.own_addr_type == BLE_OWN_ADDR_RANDOM ? BLE_ADDR_RANDOM : BLE_ADDR_PUBLIC;
    rc = ble_hs_id_copy_addr(addr_type, addr.val, NULL);
    assert(rc == 0);

    MODLOG_DFLT(INFO, "Device Address: ");
    print_addr(addr.val);
    MODLOG_DFLT(INFO, "\n");

    memset(&adv_fields, 0, sizeof(adv_fields));
    adv_fields.name = (const uint8_t *)"Periodic ADV";
    adv_fields.name_len = strlen((char *)adv_fields.name);

    /* Default to legacy PDUs size, mbuf chain will be increased if needed
     */
    data = os_msys_get_pkthdr(BLE_HS_ADV_MAX_FIELD_SZ, 0);
    assert(data);

    rc = ble_hs_adv_set_fields_mbuf(&adv_fields, data);
    assert(rc == 0);

    rc = ble_gap_ext_adv_set_data(instance, data);
    assert(rc == 0);

    /* configure periodic advertising */
    memset(&pparams, 0, sizeof(pparams));
    pparams.include_tx_power = 0;
    pparams.itvl_min = BLE_GAP_PERIODIC_ITVL_MS(120);
    pparams.itvl_max = BLE_GAP_PERIODIC_ITVL_MS(240);

    rc = ble_gap_periodic_adv_configure(instance, &pparams);
    assert(rc == 0);

    data = os_msys_get_pkthdr(sizeof(periodic_adv_raw_data), 0);
    assert(data);

    rc = os_mbuf_append(data, periodic_adv_raw_data, sizeof(periodic_adv_raw_data));
    assert(rc == 0);
#if MYNEWT_VAL(BLE_PERIODIC_ADV_ENH)
    rc = ble_gap_periodic_adv_set_data(instance, data, NULL);
#else
    rc = ble_gap_periodic_adv_set_data(instance, data);
#endif
    assert (rc == 0);

    /* start periodic advertising */
#if MYNEWT_VAL(BLE_PERIODIC_ADV_ENH)
#if CONFIG_EXAMPLE_PERIODIC_ADV_ENH
    eparams.include_adi = 1;
#endif
    rc = ble_gap_periodic_adv_start(instance, &eparams);
#else
    rc = ble_gap_periodic_adv_start(instance);
#endif
    assert (rc == 0);

    /* start advertising */
    rc = ble_gap_ext_adv_start(instance, 0, 0);
    assert (rc == 0);

    MODLOG_DFLT(INFO, "instance %u started (periodic)\n", instance);
}

static void
periodic_adv_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

static void
periodic_adv_on_sync(void)
{
    int rc;
    uint8_t own_addr_type;

#if CONFIG_EXAMPLE_RANDOM_ADDR
    rc = ble_hs_util_ensure_addr(1);
#else
    rc = ble_hs_util_ensure_addr(0);
#endif
    assert(rc == 0);

    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    assert(rc == 0);

    /* Begin advertising. */
    start_periodic_adv(own_addr_type);
}

void periodic_adv_host_task(void *param)
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
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Failed to init nimble %d ", ret);
        return;
    }

    /* Initialize the NimBLE host configuration. */
    ble_hs_cfg.reset_cb = periodic_adv_on_reset;
    ble_hs_cfg.sync_cb = periodic_adv_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

#if CONFIG_BT_NIMBLE_GAP_SERVICE
    /* Set the default device name. */
    int rc = ble_svc_gap_device_name_set("nimble_periodic_adv");
    assert(rc == 0);
#endif

    /* XXX Need to have template for store */
    ble_store_config_init();

    nimble_port_freertos_init(periodic_adv_host_task);
}
