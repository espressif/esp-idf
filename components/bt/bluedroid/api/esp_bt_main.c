// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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


#include "esp_bt_main.h"
#include "btc_task.h"
#include "btc_main.h"
#include "future.h"

static bool esp_already_enable = false;
static bool esp_already_init = false;

esp_err_t esp_bluedroid_enable(void)
{
    btc_msg_t msg;
    future_t **future_p;

    if (esp_already_enable) {
        LOG_ERROR("%s already enable\n", __func__);
        return ESP_ERR_INVALID_STATE;
    }

    future_p = btc_main_get_future_p(BTC_MAIN_ENABLE_FUTURE);
    *future_p = future_new();
    if (*future_p == NULL) {
        LOG_ERROR("%s failed\n", __func__);
        return ESP_ERR_NO_MEM;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MAIN_INIT;
    msg.act = BTC_MAIN_ACT_ENABLE;
    btc_transfer_context(&msg, NULL, 0, NULL);

    if (future_await(*future_p) == FUTURE_FAIL) {
        LOG_ERROR("%s failed\n", __func__);
        return ESP_FAIL;
    }

    esp_already_enable = true;

    return ESP_OK;
}

esp_err_t esp_bluedroid_disable(void)
{
    btc_msg_t msg;
    future_t **future_p;

    if (!esp_already_enable) {
        LOG_ERROR("%s already disable\n", __func__);
        return ESP_ERR_INVALID_STATE;
    }

    future_p = btc_main_get_future_p(BTC_MAIN_DISABLE_FUTURE);
    *future_p = future_new();
    if (*future_p == NULL) {
        LOG_ERROR("%s failed\n", __func__);
        return ESP_ERR_NO_MEM;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MAIN_INIT;
    msg.act = BTC_MAIN_ACT_DISABLE;
    btc_transfer_context(&msg, NULL, 0, NULL);

    if (future_await(*future_p) == FUTURE_FAIL) {
        LOG_ERROR("%s failed\n", __func__);
        return ESP_FAIL;
    }

    esp_already_enable = false;

    return ESP_OK;
}

esp_err_t esp_bluedroid_init(void)
{
    btc_msg_t msg;
    future_t **future_p;

    if (esp_already_init) {
        LOG_ERROR("%s already init\n", __func__);
        return ESP_ERR_INVALID_STATE;
    }

    future_p = btc_main_get_future_p(BTC_MAIN_INIT_FUTURE);
    *future_p = future_new();
    if (*future_p == NULL) {
        LOG_ERROR("%s failed\n", __func__);
        return ESP_ERR_NO_MEM;
    }

    btc_init();

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MAIN_INIT;
    msg.act = BTC_MAIN_ACT_INIT;
    btc_transfer_context(&msg, NULL, 0, NULL);

    if (future_await(*future_p) == FUTURE_FAIL) {
        LOG_ERROR("%s failed\n", __func__);
        return ESP_FAIL;
    }

    esp_already_init = true;;

    return ESP_OK;
}


esp_err_t esp_bluedroid_deinit(void)
{
    btc_msg_t msg;
    future_t **future_p;

    if (!esp_already_init) {
        LOG_ERROR("%s already deinit\n", __func__);
        return ESP_ERR_INVALID_STATE;
    }

    future_p = btc_main_get_future_p(BTC_MAIN_DEINIT_FUTURE);
    *future_p = future_new();
    if (*future_p == NULL) {
        LOG_ERROR("%s failed\n", __func__);
        return ESP_ERR_NO_MEM;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MAIN_INIT;
    msg.act = BTC_MAIN_ACT_DEINIT;
    btc_transfer_context(&msg, NULL, 0, NULL);

    if (future_await(*future_p) == FUTURE_FAIL) {
        LOG_ERROR("%s failed\n", __func__);
        return ESP_FAIL;
    }

    btc_deinit();

    esp_already_init = false;

    return ESP_OK;
}


