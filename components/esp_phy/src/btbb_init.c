/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sys/lock.h"
#include "esp_check.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "esp_private/btbb.h"

#define BTBB_ENABLE_VERSION_PRINT 1

static _lock_t s_btbb_access_lock;
/* Reference count of enabling BT BB */
static uint8_t s_btbb_access_ref = 0;


#if SOC_PM_MODEM_RETENTION_BY_REGDMA && CONFIG_FREERTOS_USE_TICKLESS_IDLE
#include "esp_private/sleep_retention.h"
#include "btbb_retention_reg.h"
static const char* TAG = "btbb_init";

#if SOC_PM_RETENTION_HAS_CLOCK_BUG
#define BTBB_LINK_OWNER  ENTRY(3)
#else
#define BTBB_LINK_OWNER  ENTRY(0) | ENTRY(2)
#endif // SOC_PM_RETENTION_HAS_CLOCK_BUG

static esp_err_t btbb_sleep_retention_init(void *arg)
{
    const static sleep_retention_entries_config_t btbb_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEM_BT_BB_LINK(0x00), BB_PART_0_ADDR, BB_PART_0_ADDR, BB_PART_0_SIZE, 0, 0), .owner = BTBB_LINK_OWNER },
        [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEM_BT_BB_LINK(0x01), BB_PART_1_ADDR, BB_PART_1_ADDR, BB_PART_1_SIZE, 0, 0), .owner = BTBB_LINK_OWNER },
#if BB_PART_CNT > 2
        [2] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEM_BT_BB_LINK(0x02), BB_PART_2_ADDR, BB_PART_2_ADDR, BB_PART_2_SIZE, 0, 0), .owner = BTBB_LINK_OWNER },
#endif // BB_PART_CNT > 2
#if BB_PART_CNT > 3
        [3] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEM_BT_BB_LINK(0x03), BB_PART_3_ADDR, BB_PART_3_ADDR, BB_PART_3_SIZE, 0, 0), .owner = BTBB_LINK_OWNER },
#endif // BB_PART_CNT > 3
    };
    esp_err_t err = sleep_retention_entries_create(btbb_regs_retention, ARRAY_SIZE(btbb_regs_retention), REGDMA_LINK_PRI_BT_MAC_BB, SLEEP_RETENTION_MODULE_BT_BB);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for btbb retention");
    ESP_LOGD(TAG, "btbb sleep retention initialization");
    return ESP_OK;
}

static void btbb_sleep_retention_disable(void)
{
    esp_err_t err = sleep_retention_module_detach(SLEEP_RETENTION_MODULE_BT_BB);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "failed to detach sleep retention linked list for btbb retention");
    }
    err = sleep_retention_module_free(SLEEP_RETENTION_MODULE_BT_BB);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "failed to free sleep retention linked list for btbb retention");
    }
    err = sleep_retention_module_deinit(SLEEP_RETENTION_MODULE_BT_BB);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Modem BT BB retention callback unregister failed");
    }
}

static esp_err_t btbb_sleep_retention_enable(void)
{
    sleep_retention_module_init_param_t init_param = {
        .cbs     = { .create = { .handle = btbb_sleep_retention_init, .arg = NULL } },
        .attribute = SLEEP_RETENTION_MODULE_ATTR_ATTACH,
        .depends = RETENTION_MODULE_BITMAP_INIT(CLOCK_MODEM)
    };
    esp_err_t err = sleep_retention_module_init(SLEEP_RETENTION_MODULE_BT_BB, &init_param);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Modem BT BB retention callback register failed");
        return err;
    }
    err = sleep_retention_module_allocate(SLEEP_RETENTION_MODULE_BT_BB);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "failed to allocate sleep retention linked list for btbb retention");
        return err;
    }
    err = sleep_retention_module_attach(SLEEP_RETENTION_MODULE_BT_BB);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "failed to attach sleep retention linked list for btbb retention");
    }
    return err;
}
#endif // SOC_PM_MODEM_RETENTION_BY_REGDMA && CONFIG_FREERTOS_USE_TICKLESS_IDLE

void esp_btbb_enable(void)
{
    _lock_acquire(&s_btbb_access_lock);
    if (s_btbb_access_ref == 0) {
        bt_bb_v2_init_cmplx(BTBB_ENABLE_VERSION_PRINT);
#if SOC_PM_MODEM_RETENTION_BY_REGDMA && CONFIG_FREERTOS_USE_TICKLESS_IDLE
        esp_err_t err = btbb_sleep_retention_enable();
        if (err != ESP_OK) {
            btbb_sleep_retention_disable();
            _lock_release(&s_btbb_access_lock);
            return;
        }
#endif // SOC_PM_MODEM_RETENTION_BY_REGDMA && CONFIG_FREERTOS_USE_TICKLESS_IDLE
    }
    s_btbb_access_ref++;
    _lock_release(&s_btbb_access_lock);
}

void esp_btbb_disable(void)
{
    _lock_acquire(&s_btbb_access_lock);
    if (s_btbb_access_ref && (--s_btbb_access_ref == 0)) {
#if SOC_PM_MODEM_RETENTION_BY_REGDMA && CONFIG_FREERTOS_USE_TICKLESS_IDLE
        btbb_sleep_retention_disable();
#endif // SOC_PM_MODEM_RETENTION_BY_REGDMA && CONFIG_FREERTOS_USE_TICKLESS_IDLE
    }
    _lock_release(&s_btbb_access_lock);
}
