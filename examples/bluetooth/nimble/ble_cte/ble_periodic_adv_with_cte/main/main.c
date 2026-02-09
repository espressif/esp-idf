/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "esp_log.h"
#include "nvs_flash.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "periodic_adv.h"
#include "host/ble_gap.h"
#include "host/ble_hs_adv.h"
#include "cte_config.h"

/* Global constants */
static const char *TAG = "CTE_ADV_EXAMPLE";
static uint8_t s_periodic_adv_raw_data[] = {0x0D, BLE_HS_ADV_TYPE_COMP_NAME, 'C','T','E',' ','P','e','r','i','o','d','i','c'};

/* Configuration based on Kconfig settings */
#if CONFIG_EXAMPLE_RANDOM_ADDR
static uint8_t s_own_addr_type = BLE_OWN_ADDR_RANDOM;
#else
static uint8_t s_own_addr_type;
#endif


/**
 * @brief Configure and start periodic advertising with CTE
 */
static void start_periodic_adv_cte(void)
{
    int rc;
    uint8_t instance = 1;
    ble_addr_t addr;

    /* Generate random address for instance */
    rc = ble_hs_id_gen_rnd(1, &addr);
    assert(rc == 0);

    ESP_LOGI(TAG, "Device Address: ");
    ESP_LOGI(TAG, "%02x:%02x:%02x:%02x:%02x:%02x",
                   addr.val[5], addr.val[4], addr.val[3], addr.val[2], addr.val[1], addr.val[0]);

    /* Configure extended advertising parameters */
    struct ble_gap_ext_adv_params ext_adv_params = {
        .own_addr_type = BLE_OWN_ADDR_RANDOM,
        .primary_phy = BLE_HCI_LE_PHY_1M,
        .secondary_phy = BLE_HCI_LE_PHY_1M,
        .sid = 2,
        .tx_power = 0
    };

    rc = ble_gap_ext_adv_configure(instance, &ext_adv_params, NULL, NULL, NULL);
    assert(rc == 0);

    rc = ble_gap_ext_adv_set_addr(instance, &addr);
    assert(rc == 0);

    /* Configure advertising data */
    struct ble_hs_adv_fields adv_fields = {
        .name = (const uint8_t *)"CTE_Periodic_Adv",
        .name_len = strlen((char *)adv_fields.name)
    };

    struct os_mbuf *data = os_msys_get_pkthdr(BLE_HCI_MAX_ADV_DATA_LEN, 0);
    assert(data);

    rc = ble_hs_adv_set_fields_mbuf(&adv_fields, data);
    assert(rc == 0);

    rc = ble_gap_ext_adv_set_data(instance, data);
    assert(rc == 0);

    /* Configure periodic advertising parameters */
    struct ble_gap_periodic_adv_params pparams = {
        .include_tx_power = 0,
        .itvl_min = BLE_GAP_ADV_ITVL_MS(200),
        .itvl_max = BLE_GAP_ADV_ITVL_MS(400)
    };

    rc = ble_gap_periodic_adv_configure(instance, &pparams);
    assert(rc == 0);

    /* Set periodic advertising data */
    data = os_msys_get_pkthdr(sizeof(s_periodic_adv_raw_data), 0);
    assert(data);

    rc = os_mbuf_append(data, s_periodic_adv_raw_data, sizeof(s_periodic_adv_raw_data));
    assert(rc == 0);

    rc = ble_gap_periodic_adv_set_data(instance, data);
    assert(rc == 0);

    /* Configure CTE parameters */
#if defined(CONFIG_EXAMPLE_ADV_DIRECTION_FINDING_AOA)
    /* Configure CTE parameters */
    struct ble_gap_periodic_adv_cte_params cte_params = {
        .cte_length = 0x14,
        .cte_type = BLE_CTE_TYPE_AOA,
        .cte_count = 1,
    };
#elif defined(CONFIG_EXAMPLE_ADV_DIRECTION_FINDING_AOD)
    struct ble_gap_periodic_adv_cte_params cte_params = {
        .cte_length = 0x14,
        .cte_type = BLE_CTE_TYPE_AOD_2US,
        .cte_count = 1,
        .switching_pattern_length = 4,
        .antenna_ids = (uint8_t[]){0x00, 0x01, 0x02, 0x03}
    };
#endif

    rc = ble_gap_set_connless_cte_transmit_params(instance, &cte_params);
    if (rc != 0) {
        ESP_LOGE(TAG, "CTE params config failed (rc=0x%x)", rc);
        assert(rc == 0);
    }

    /* Start advertising instances */
    rc = ble_gap_periodic_adv_start(instance);
    assert(rc == 0);

    rc = ble_gap_ext_adv_start(instance, 0, 0);
    assert(rc == 0);

    /* Enable CTE transmission */
    rc = ble_gap_set_connless_cte_transmit_enable(instance, 1);
    if (rc != 0) {
        ESP_LOGE(TAG, "CTE enable failed (rc=0x%x)", rc);
        assert(rc == 0);
    }

    ESP_LOGI(TAG, "Instance %u started (periodic)", instance);
}

/**
 * @brief Reset handler for BLE stack
 */
static void periodic_adv_on_reset(int reason)
{
    ESP_LOGE(TAG, "Resetting state; reason=%d", reason);
}

/**
 * @brief Synchronization callback for BLE stack
 */
static void periodic_sync_cb(void)
{
    int rc;
#if CONFIG_EXAMPLE_RANDOM_ADDR
    ble_addr_t addr;
    if (ble_hs_id_gen_rnd(0, &addr) == 0) {
        ble_hs_id_set_rnd(addr.val);
    }
    /* Ensure proper identity address */
    rc = ble_hs_util_ensure_addr(1);
#else
    rc = ble_hs_util_ensure_addr(0);
#endif
    assert(rc == 0);

    /* Infer address type */
    rc = ble_hs_id_infer_auto(0, &s_own_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to infer address type (rc=%d)", rc);
        return;
    }

    /* Start advertising */
    start_periodic_adv_cte();
}

/**
 * @brief BLE host task
 */
void periodic_adv_host_task(void *param)
{
    ESP_LOGI(TAG, "BLE Host Task Started");
    /* Run the BLE stack event loop */
    nimble_port_run();
    nimble_port_freertos_deinit();
}

/**
 * @brief Main application entry point
 */
void app_main(void)
{
    int rc;
    /* Initialize NVS for PHY calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Initialize NimBLE stack */
    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Nimble init failed (%d)", ret);
        return;
    }

#if MYNEWT_VAL(BLE_AOA_AOD)
    ESP_LOGI(TAG, "%s", direction_finding_logo);
#if defined(CONFIG_EXAMPLE_ADV_DIRECTION_FINDING_AOD)
    ESP_LOGI(TAG, "DIRECTION_FINDING Example Periodic Adv AOD Mode");
    ble_direction_finding_antenna_init(antenna_use_gpio,CONFIG_EXAMPLE_AOD_GPIO_BIT_COUNT);
#elif defined(CONFIG_EXAMPLE_ADV_DIRECTION_FINDING_AOA)
    ESP_LOGI(TAG, "DIRECTION_FINDING Example Periodic Adv AOA Mode");
#endif
#endif

    /* Configure BLE stack */
    ble_hs_cfg.reset_cb = periodic_adv_on_reset;
    ble_hs_cfg.sync_cb = periodic_sync_cb;

    /* Set device name */
    rc = ble_svc_gap_device_name_set("Periodic ADV with CTE");
    assert(rc == 0);

    /* Start BLE host task */
    nimble_port_freertos_init(periodic_adv_host_task);
}
