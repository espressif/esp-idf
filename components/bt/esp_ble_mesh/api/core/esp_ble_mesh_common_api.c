// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "btc/btc_task.h"
#include "btc/btc_manage.h"
#include "osi/alarm.h"

#include "esp_err.h"

#include "btc_ble_mesh_prov.h"
#include "esp_ble_mesh_defs.h"

esp_err_t esp_ble_mesh_init(esp_ble_mesh_prov_t *prov, esp_ble_mesh_comp_t *comp)
{
    btc_ble_mesh_prov_args_t arg = {0};
    SemaphoreHandle_t semaphore = NULL;
    btc_msg_t msg = {0};
    esp_err_t ret;

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
        LOG_ERROR("%s, Failed to allocate memory for the semaphore", __func__);
        return ESP_ERR_NO_MEM;
    }

    arg.mesh_init.prov = prov;
    arg.mesh_init.comp = comp;
    /* Transport semaphore pointer to BTC layer, and will give the semaphore in the BTC task */
    arg.mesh_init.semaphore = semaphore;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_MESH_INIT;

    if (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL) != BT_STATUS_SUCCESS) {
        vSemaphoreDelete(semaphore);
        LOG_ERROR("%s, BLE Mesh initialise failed", __func__);
        return ESP_FAIL;
    }

    /* Take the Semaphore, wait BLE Mesh initialization to finish. */
    xSemaphoreTake(semaphore, portMAX_DELAY);
    /* Don't forget to delete the semaphore at the end. */
    vSemaphoreDelete(semaphore);

    return ESP_OK;
}

