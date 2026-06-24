/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"

/* From ESP Bluetooth */
#include "esp_bt.h"
#include "esp_hci_transport.h"
#if UC_BT_CTRL_BLE_IS_ENABLE
#include "ble_priv.h"
#endif
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
#include "bredr_priv.h"
#endif
#include "btdm_osal.h"
#include "btdm_coex.h"
#include "btdm_lp.h"
#include "btdm_log.h"
#include "btdm_external.h"

/*
 ***************************************************************************************************
 * Local Defined Macros
 ***************************************************************************************************
 */
#define BTDM_LOG_TAG "BTDM_INIT"

#if UC_BT_CTRL_HCI_INTERFACE_USE_RAM
#define BT_HCI_TRANSPORT_MODE HCI_TRANSPORT_VHCI
#elif UC_BT_CTRL_HCI_INTERFACE_USE_UART
#if UC_BT_CTRL_UART_HCI_DMA_MODE
#define BT_HCI_TRANSPORT_MODE HCI_TRANSPORT_UART_UHCI
#else
#define BT_HCI_TRANSPORT_MODE HCI_TRANSPORT_UART_NO_DMA
#endif // UC_BT_CTRL_UART_HCI_DMA_MODE
#else
#error "Unknown HCI transport mode!!"
#endif // UC_BT_CTRL_HCI_INTERFACE_USE_RAM

#if defined(UNUSED)
#undef UNUSED
#endif
#define UNUSED(x)                           (void)(x)

/*
 ***************************************************************************************************
 * External Functions
 ***************************************************************************************************
 */
extern const char *r_btdm_get_compile_version(void);
extern int r_btdm_hci_fc_env_init(void);
extern void r_btdm_hci_fc_env_deinit(void);
extern int r_btdm_hci_fc_enable(void);
extern void r_btdm_hci_fc_disable(void);

extern int r_btdm_task_init(esp_btdm_controller_config_t *cfg);
extern void r_btdm_task_deinit(void);
extern int r_btdm_task_enable(void);
extern void r_btdm_task_disable(void);
extern void r_btdm_task_shutdown(void);

/*
 ***************************************************************************************************
 * Local Variables
 ***************************************************************************************************
 */
esp_bt_controller_status_t s_btdm_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;

/*
 ***************************************************************************************************
 * Static Function Definitions
 ***************************************************************************************************
 */
static void
bt_shutdown(void)
{
    if (s_btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return;
    }

    s_btdm_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;
    r_btdm_task_shutdown();
    btdm_lp_shutdown();
}

static esp_err_t
bt_controller_deinit(void)
{
    s_btdm_controller_status = ESP_BT_CONTROLLER_STATUS_IDLE;

    hci_transport_deinit();

#if UC_BT_CTRL_BR_EDR_IS_ENABLE
    bredr_stack_deinit();
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE

#if UC_BT_CTRL_BLE_IS_ENABLE
    ble_stack_deinit();
#endif // UC_BT_CTRL_BLE_IS_ENABLE

#if UC_BT_CTRL_CONN_FC_ENABLE
    r_btdm_hci_fc_env_deinit();
#endif // UC_BT_CTRL_CONN_FC_ENABLE

    btdm_lp_deinit();
    r_btdm_task_deinit();
    btdm_lp_disable_clock();
    btdm_coex_deinit();
    btdm_external_deinit();
    btdm_log_deinit();
    btdm_osal_elem_mempool_deinit();

    return ESP_OK;
}

static esp_err_t
bt_controller_disable(void)
{
    s_btdm_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;

    esp_unregister_shutdown_handler(bt_shutdown);

    r_btdm_task_disable();

#if UC_BT_CTRL_CONN_FC_ENABLE
    r_btdm_hci_fc_disable();
#endif // UC_BT_CTRL_CONN_FC_ENABLE

#if UC_BT_CTRL_BR_EDR_IS_ENABLE
    bredr_stack_disable();
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE

#if UC_BT_CTRL_BLE_IS_ENABLE
    ble_stack_disable();
#endif // UC_BT_CTRL_BLE_IS_ENABLE

    btdm_coex_disable();
    btdm_lp_reset(false);

    return ESP_OK;
}

/*
 ***************************************************************************************************
 * Public Function Definitions
 ***************************************************************************************************
 */
esp_err_t
esp_bt_mem_release(esp_bt_mode_t mode)
{
    // TODO: Support bluetooth stack memory release
    ESP_LOGW(BTDM_LOG_TAG, "esp_bt_mem_release not implement yet!");
    return ESP_OK;
}

esp_err_t
esp_bt_controller_mem_release(esp_bt_mode_t mode)
{
    // TODO: Support bt controller memory release
    ESP_LOGW(BTDM_LOG_TAG, "esp_bt_controller_mem_release not implement yet!");
    return ESP_OK;
}

esp_err_t
esp_bt_controller_init(esp_bt_controller_config_t *cfg)
{
    int ret;
    // TODO: Delete workaround
    btdm_osal_elem_num_t elem = {
        .evt_count = 3 + 100,
        .evtq_count = 1 + 2,
        .co_count = 0 + 10,
        .sem_count = 0,
        .mutex_count = 1,
    };

    if (cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(BTDM_LOG_TAG, "BTDM version [%s]", r_btdm_get_compile_version());

    if (cfg->btdm.bluetooth_mode == ESP_BT_MODE_IDLE) {
        ESP_LOGE(BTDM_LOG_TAG, "bluetooth_mode in cfg is IDLE");
        return ESP_ERR_INVALID_ARG;
    }

    if (s_btdm_controller_status != ESP_BT_CONTROLLER_STATUS_IDLE) {
        return ESP_ERR_INVALID_STATE;
    }

#if UC_BT_CTRL_BLE_IS_ENABLE
    ble_osal_elem_calc(cfg, &elem);
#endif // UC_BT_CTRL_BLE_IS_ENABLE

#if UC_BT_CTRL_BR_EDR_IS_ENABLE
    bredr_osal_elem_calc(cfg, &elem);
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE

    ret = btdm_osal_elem_mempool_init(&elem);
    if (ret) {
        ESP_LOGE(BTDM_LOG_TAG, "btdm_osal_elem_mempool_init failed: %d", ret);
        goto init_failed;
    }

    ret = btdm_log_init();
    if (ret) {
        ESP_LOGE(BTDM_LOG_TAG, "btdm_log_init failed: %d", ret);
        goto init_failed;
    }

    ret = btdm_external_init();
    if (ret) {
        ESP_LOGE(BTDM_LOG_TAG, "btdm_external_init failed: %d", ret);
        goto init_failed;
    }

    ret = btdm_coex_init();
    if (ret) {
        ESP_LOGE(BTDM_LOG_TAG, "btdm_coex_init failed: %d", ret);
        goto init_failed;
    }

    btdm_lp_enable_clock(&cfg->btdm);

    ret = r_btdm_task_init(&cfg->btdm);
    if (ret) {
        ESP_LOGE(BTDM_LOG_TAG, "r_btdm_task_init failed: %d", ret);
        goto init_failed;
    }

    ret = btdm_lp_init();
    if (ret != ESP_OK) {
        ESP_LOGE(BTDM_LOG_TAG, "btdm_lp_init failed %d", ret);
        goto init_failed;
    }

#if UC_BT_CTRL_CONN_FC_ENABLE
    ret = r_btdm_hci_fc_env_init();
    if (ret) {
        ESP_LOGE(BTDM_LOG_TAG, "r_btdm_hci_fc_env_init failed: %d", ret);
        goto init_failed;
    }
#endif // UC_BT_CTRL_CONN_FC_ENABLE

#if UC_BT_CTRL_BLE_IS_ENABLE
    ret = ble_stack_init(cfg);
    if (ret) {
        ESP_LOGE(BTDM_LOG_TAG, "ble_stack_init failed: %d", ret);
        goto init_failed;
    }
#endif // UC_BT_CTRL_BLE_IS_ENABLE

#if UC_BT_CTRL_BR_EDR_IS_ENABLE
    ret = bredr_stack_init(cfg);
    if (ret) {
        ESP_LOGE(BTDM_LOG_TAG, "bredr_stack_init failed: %d", ret);
        goto init_failed;
    }
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE

    ret = hci_transport_init(BT_HCI_TRANSPORT_MODE);
    if (ret) {
        ESP_LOGE(BTDM_LOG_TAG, "hci_transport_init failed %d", ret);
        goto init_failed;
    }

    ESP_LOGI(BTDM_LOG_TAG, "BTDM controller init OK");
    s_btdm_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;

    return ESP_OK;

init_failed:
    bt_controller_deinit();

    return ESP_FAIL;
}

esp_err_t
esp_bt_controller_deinit(void)
{
    if (s_btdm_controller_status != ESP_BT_CONTROLLER_STATUS_INITED) {
        return ESP_ERR_INVALID_STATE;
    }
    return bt_controller_deinit();
}

esp_err_t
esp_bt_controller_enable(esp_bt_mode_t mode)
{
    int ret;

    UNUSED(mode);
    if (s_btdm_controller_status != ESP_BT_CONTROLLER_STATUS_INITED) {
        return ESP_ERR_INVALID_STATE;
    }

    btdm_lp_reset(true);

    ret = btdm_coex_enable();
    if (ret) {
        ESP_LOGE(BTDM_LOG_TAG, "btdm_coex_enable failed %d", ret);
        goto enable_failed;
    }

#if UC_BT_CTRL_BLE_IS_ENABLE
    ret = ble_stack_enable();
    if (ret) {
        ESP_LOGE(BTDM_LOG_TAG, "ble_stack_enable failed %d", ret);
        goto enable_failed;
    }
#endif // UC_BT_CTRL_BLE_IS_ENABLE

#if UC_BT_CTRL_BR_EDR_IS_ENABLE
    ret = bredr_stack_enable();
    if (ret) {
        ESP_LOGE(BTDM_LOG_TAG, "bredr_stack_enable failed %d", ret);
        goto enable_failed;
    }
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE

#if UC_BT_CTRL_CONN_FC_ENABLE
    ret = r_btdm_hci_fc_enable();
    if (ret) {
        ESP_LOGE(BTDM_LOG_TAG, "r_btdm_hci_fc_enable failed %d", ret);
        goto enable_failed;
    }
#endif // UC_BT_CTRL_CONN_FC_ENABLE

    ret = r_btdm_task_enable();
    if (ret) {
        ESP_LOGE(BTDM_LOG_TAG, "r_btdm_task_enable failed %d", ret);
        goto enable_failed;
    }

    ret = esp_register_shutdown_handler(bt_shutdown);
    if (ret) {
        ESP_LOGW(BTDM_LOG_TAG, "Register shutdown handler failed, ret = 0x%x", ret);
    }

    s_btdm_controller_status = ESP_BT_CONTROLLER_STATUS_ENABLED;
    return ESP_OK;

enable_failed:
    bt_controller_disable();
    return ESP_FAIL;
}

esp_err_t
esp_bt_controller_disable(void)
{
    if (s_btdm_controller_status != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    bt_controller_disable();

    return ESP_OK;
}

esp_bt_controller_status_t
esp_bt_controller_get_status(void)
{
    return s_btdm_controller_status;
}
