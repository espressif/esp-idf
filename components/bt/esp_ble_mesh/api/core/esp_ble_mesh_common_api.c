/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_err.h"

#include "btc_ble_mesh_prov.h"
#include "esp_ble_mesh_defs.h"

esp_err_t esp_ble_mesh_init(esp_ble_mesh_prov_t *prov, esp_ble_mesh_comp_t *comp)
{
    btc_ble_mesh_prov_args_t arg = {0};
    SemaphoreHandle_t semaphore = NULL;
    btc_msg_t msg = {0};
    esp_err_t ret = ESP_OK;

    if (prov == NULL || comp == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    ret = bt_mesh_host_init();
    if (ret != ESP_OK) {
        return ret;
    }

    // Create a semaphore
    if ((semaphore = xSemaphoreCreateCounting(1, 0)) == NULL) {
        BT_ERR("Failed to create semaphore");
        return ESP_ERR_NO_MEM;
    }

    arg.mesh_init.prov = prov;
    arg.mesh_init.comp = comp;
    /* Transport semaphore pointer to BTC layer, and will give the semaphore in the BTC task */
    arg.mesh_init.semaphore = semaphore;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_MESH_INIT;

    if (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL) != BT_STATUS_SUCCESS) {
        vSemaphoreDelete(semaphore);
        BT_ERR("Failed to start mesh init");
        return ESP_FAIL;
    }

    /* Take the Semaphore, wait BLE Mesh initialization to finish. */
    xSemaphoreTake(semaphore, portMAX_DELAY);
    /* Don't forget to delete the semaphore at the end. */
    vSemaphoreDelete(semaphore);

    return ESP_OK;
}

#if CONFIG_BLE_MESH_DEINIT
esp_err_t esp_ble_mesh_deinit(esp_ble_mesh_deinit_param_t *param)
{
    btc_ble_mesh_prov_args_t arg = {0};
    SemaphoreHandle_t semaphore = NULL;
    btc_msg_t msg = {0};
    esp_err_t ret = ESP_OK;

    if (param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    // Create a semaphore
    if ((semaphore = xSemaphoreCreateCounting(1, 0)) == NULL) {
        BT_ERR("Failed to create semaphore");
        return ESP_ERR_NO_MEM;
    }

    arg.mesh_deinit.param.erase_flash = param->erase_flash;
    /* Transport semaphore pointer to BTC layer, and will give the semaphore in the BTC task */
    arg.mesh_deinit.semaphore = semaphore;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_DEINIT_MESH;

    if (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL) != BT_STATUS_SUCCESS) {
        vSemaphoreDelete(semaphore);
        BT_ERR("Failed to start mesh deinit");
        return ESP_FAIL;
    }

    /* Take the Semaphore, wait BLE Mesh de-initialization to finish. */
    xSemaphoreTake(semaphore, portMAX_DELAY);
    /* Don't forget to delete the semaphore at the end. */
    vSemaphoreDelete(semaphore);

    ret = bt_mesh_host_deinit();
    if (ret != ESP_OK) {
        return ret;
    }

    return ESP_OK;
}
#endif /* CONFIG_BLE_MESH_DEINIT */
