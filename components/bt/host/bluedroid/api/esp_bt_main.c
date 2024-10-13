/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "common/bt_target.h"
#include "esp_bt_main.h"
#include "btc/btc_task.h"
#include "btc/btc_main.h"
#if (BT_CONTROLLER_INCLUDED == TRUE)
#include "esp_bt.h"
#endif
#include "osi/future.h"
#include "osi/allocator.h"
#include "config/stack_config.h"
#include "hci_log/bt_hci_log.h"
#include "bt_common.h"

static bool bd_already_enable = false;
static bool bd_already_init = false;

esp_bluedroid_status_t esp_bluedroid_get_status(void)
{
    if (bd_already_init) {
        if (bd_already_enable) {
            return ESP_BLUEDROID_STATUS_ENABLED;
        } else {
            return ESP_BLUEDROID_STATUS_INITIALIZED;
        }
    } else {
        return ESP_BLUEDROID_STATUS_UNINITIALIZED;
    }
}

esp_err_t esp_bluedroid_enable(void)
{
    btc_msg_t msg;
    future_t **future_p;

    if (!bd_already_init) {
        LOG_ERROR("Bludroid not initialised\n");
        return ESP_ERR_INVALID_STATE;
    }

    if (bd_already_enable) {
        LOG_ERROR("Bluedroid already enabled\n");
        return ESP_ERR_INVALID_STATE;
    }

    future_p = btc_main_get_future_p(BTC_MAIN_ENABLE_FUTURE);
    *future_p = future_new();
    if (*future_p == NULL) {
        LOG_ERROR("Bluedroid enable failed\n");
        return ESP_ERR_NO_MEM;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MAIN_INIT;
    msg.act = BTC_MAIN_ACT_ENABLE;

    if (btc_transfer_context(&msg, NULL, 0, NULL, NULL) != BT_STATUS_SUCCESS) {
        LOG_ERROR("Bluedroid enable failed\n");
        return ESP_FAIL;
    }

    if (future_await(*future_p) == FUTURE_FAIL) {
        LOG_ERROR("Bluedroid enable failed\n");
        return ESP_FAIL;
    }

    bd_already_enable = true;

    return ESP_OK;
}

esp_err_t esp_bluedroid_disable(void)
{
    btc_msg_t msg;
    future_t **future_p;

    if (!bd_already_enable) {
        LOG_ERROR("Bluedroid already disabled\n");
        return ESP_ERR_INVALID_STATE;
    }

    future_p = btc_main_get_future_p(BTC_MAIN_DISABLE_FUTURE);
    *future_p = future_new();
    if (*future_p == NULL) {
        LOG_ERROR("Bluedroid disable failed\n");
        return ESP_ERR_NO_MEM;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MAIN_INIT;
    msg.act = BTC_MAIN_ACT_DISABLE;

    if (btc_transfer_context(&msg, NULL, 0, NULL, NULL) != BT_STATUS_SUCCESS) {
        LOG_ERROR("Bluedroid disable failed\n");
        return ESP_FAIL;
    }

    if (future_await(*future_p) == FUTURE_FAIL) {
        LOG_ERROR("Bluedroid disable failed\n");
        return ESP_FAIL;
    }

    bd_already_enable = false;

    return ESP_OK;
}

esp_err_t esp_bluedroid_init(void)
{
    esp_bluedroid_config_t cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    return esp_bluedroid_init_with_cfg(&cfg);
}

esp_err_t esp_bluedroid_init_with_cfg(esp_bluedroid_config_t *cfg)
{
    btc_msg_t msg;
    future_t **future_p;
    bt_status_t ret;

    if (!cfg) {
        LOG_ERROR("%s cfg is NULL", __func__);
        return ESP_ERR_INVALID_ARG;
    }

#if (BT_CONTROLLER_INCLUDED == TRUE)
    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        LOG_ERROR("Controller not initialised\n");
        return ESP_ERR_INVALID_STATE;
    }
#endif

    if (bd_already_init) {
        LOG_ERROR("Bluedroid already initialised\n");
        return ESP_ERR_INVALID_STATE;
    }

#if HEAP_MEMORY_DEBUG
    osi_mem_dbg_init();
#endif

    ret = bluedriod_config_init(cfg);
    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("Bluedroid stack initialize fail, ret:%d", ret);
        return ESP_FAIL;
    }

    /*
     * BTC Init
     */
    ret = btc_init();
    if (ret != BT_STATUS_SUCCESS) {
        LOG_ERROR("Bluedroid Initialize Fail");
        return ESP_FAIL;
    }

    future_p = btc_main_get_future_p(BTC_MAIN_INIT_FUTURE);
    *future_p = future_new();
    if (*future_p == NULL) {
        LOG_ERROR("Bluedroid Initialize Fail!");
        return ESP_ERR_NO_MEM;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MAIN_INIT;
    msg.act = BTC_MAIN_ACT_INIT;

    if (btc_transfer_context(&msg, NULL, 0, NULL, NULL) != BT_STATUS_SUCCESS) {
        LOG_ERROR("Bluedroid Initialize Fail");
        return ESP_FAIL;
    }

    if (future_await(*future_p) == FUTURE_FAIL) {
        LOG_ERROR("Bluedroid Initialize Fail");
        return ESP_FAIL;
    }

    bd_already_init = true;

#if (BT_HCI_LOG_INCLUDED == TRUE)
    bt_hci_log_init();
#endif // (BT_HCI_LOG_INCLUDED == TRUE)

    return ESP_OK;
}


esp_err_t esp_bluedroid_deinit(void)
{
    btc_msg_t msg;
    future_t **future_p;

    if (!bd_already_init) {
        LOG_ERROR("Bluedroid already de-initialised\n");
        return ESP_ERR_INVALID_STATE;
    }

    if (bd_already_enable) {
        LOG_ERROR("Bludroid already enabled, do disable first\n");
        return ESP_ERR_INVALID_STATE;
    }

    future_p = btc_main_get_future_p(BTC_MAIN_DEINIT_FUTURE);
    *future_p = future_new();
    if (*future_p == NULL) {
        LOG_ERROR("Bluedroid de-initialise failed\n");
        return ESP_ERR_NO_MEM;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MAIN_INIT;
    msg.act = BTC_MAIN_ACT_DEINIT;

    if (btc_transfer_context(&msg, NULL, 0, NULL, NULL) != BT_STATUS_SUCCESS) {
        LOG_ERROR("Bluedroid de-initialise failed\n");
        return ESP_FAIL;
    }

    if (future_await(*future_p) == FUTURE_FAIL) {
        LOG_ERROR("Bluedroid de-initialise failed\n");
        return ESP_FAIL;
    }

    btc_deinit();

    bluedriod_config_deinit();

#if (BT_HCI_LOG_INCLUDED == TRUE)
    bt_hci_log_deinit();
#endif // (BT_HCI_LOG_INCLUDED == TRUE)

    bd_already_init = false;

    return ESP_OK;
}

#if defined(CONFIG_EXAMPLE_CI_ID) && defined(CONFIG_EXAMPLE_CI_PIPELINE_ID)
char *esp_bluedroid_get_example_name(void)
{
    static char example_name[ESP_BLE_ADV_NAME_LEN_MAX];
    memset(example_name, 0, sizeof(example_name));
    sprintf(example_name, "BE%02X_%05X_%02X", CONFIG_EXAMPLE_CI_ID & 0xFF,
            CONFIG_EXAMPLE_CI_PIPELINE_ID & 0xFFFFF, CONFIG_IDF_FIRMWARE_CHIP_ID & 0xFF);
    return example_name;
}
#endif
