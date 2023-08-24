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
#include "periodic_adv.h"
#include "host/ble_gap.h"
#include "host/ble_hs_adv.h"
#include "patterns.h"

#if CONFIG_EXAMPLE_EXTENDED_ADV
static uint8_t periodic_adv_raw_data[] = {'E', 'S', 'P', '_', 'P', 'E', 'R', 'I', 'O', 'D', 'I', 'C', '_', 'A', 'D', 'V'};
#endif

static const char *tag = "NimBLE_BLE_PERIODIC_ADV";
#if CONFIG_EXAMPLE_RANDOM_ADDR
static uint8_t own_addr_type = BLE_OWN_ADDR_RANDOM;
#else
static uint8_t own_addr_type;
#endif

void ble_store_config_init(void);

#if CONFIG_EXAMPLE_EXTENDED_ADV
/**
 * Enables advertising with the following parameters:
 *     o General discoverable mode.
 *     o Undirected connectable mode.
 */
static void
start_periodic_adv(void)
{
    int rc;
    struct ble_gap_periodic_adv_params pparams;
    struct ble_gap_ext_adv_params params;
    struct ble_hs_adv_fields adv_fields;
    struct os_mbuf *data;
    uint8_t instance = 1;
    ble_addr_t addr;
#if MYNEWT_VAL(BLE_PERIODIC_ADV_ENH)
    struct ble_gap_periodic_adv_enable_params eparams;
    memset(&eparams, 0, sizeof(eparams));
#endif

    /* set random (NRPA) address for instance */
    rc = ble_hs_id_gen_rnd(1, &addr);
    assert (rc == 0);

    MODLOG_DFLT(INFO, "Device Address: ");
    print_addr(addr.val);
    MODLOG_DFLT(INFO, "\n");

    /* For periodic we use instance with non-connectable advertising */
    memset (&params, 0, sizeof(params));

    /* advertise using random addr */
    params.own_addr_type = BLE_OWN_ADDR_RANDOM;
    params.primary_phy = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_2M;
    params.sid = 2;

    /* configure instance 1 */
    rc = ble_gap_ext_adv_configure(instance, &params, NULL, NULL, NULL);
    assert (rc == 0);

    rc = ble_gap_ext_adv_set_addr(instance, &addr );
    assert (rc == 0);

    memset(&adv_fields, 0, sizeof(adv_fields));
    adv_fields.name = (const uint8_t *)"Periodic ADV";
    adv_fields.name_len = strlen((char *)adv_fields.name);

    /* Default to legacy PDUs size, mbuf chain will be increased if needed
     */
    data = os_msys_get_pkthdr(BLE_HCI_MAX_ADV_DATA_LEN, 0);
    assert(data);

    rc = ble_hs_adv_set_fields_mbuf(&adv_fields, data);
    assert(rc == 0);

    rc = ble_gap_ext_adv_set_data(instance, data);
    assert(rc == 0);

    /* configure periodic advertising */
    memset(&pparams, 0, sizeof(pparams));
    pparams.include_tx_power = 0;
    pparams.itvl_min = BLE_GAP_ADV_ITVL_MS(120);
    pparams.itvl_max = BLE_GAP_ADV_ITVL_MS(240);

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
#endif

static void
periodic_adv_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

#if CONFIG_EXAMPLE_RANDOM_ADDR
static void
periodic_adv_set_addr(void)
{
    ble_addr_t addr;
    int rc;

    /* generate new non-resolvable private address */
    rc = ble_hs_id_gen_rnd(0, &addr);
    assert(rc == 0);

    /* set generated address */
    rc = ble_hs_id_set_rnd(addr.val);

    assert(rc == 0);
}
#endif

static void
periodic_adv_on_sync(void)
{
    int rc;

#if CONFIG_EXAMPLE_RANDOM_ADDR
    /* Generate a non-resolvable private address. */
    periodic_adv_set_addr();
    /* Make sure we have proper identity address set (public preferred) */
    rc = ble_hs_util_ensure_addr(1);
#else
    rc = ble_hs_util_ensure_addr(0);
#endif
    assert(rc == 0);

    /* Figure out address to use while advertising (no privacy for now) */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
        return;
    }

    /* Begin advertising. */
#if CONFIG_EXAMPLE_EXTENDED_ADV
    start_periodic_adv();
#endif
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
        ESP_LOGE(tag, "Failed to init nimble %d ", ret);
        return;
    }

    /* Initialize the NimBLE host configuration. */
    ble_hs_cfg.reset_cb = periodic_adv_on_reset;
    ble_hs_cfg.sync_cb = periodic_adv_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
    /* Set the default device name. */
    rc = ble_svc_gap_device_name_set("nimble_periodic_adv");
    assert(rc == 0);

    /* XXX Need to have template for store */
    ble_store_config_init();

    nimble_port_freertos_init(periodic_adv_host_task);
}
