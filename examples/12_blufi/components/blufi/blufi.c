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

/***************************************************************
*
* This file is for gatt server device. It instantiates BATTERY
* sevice. It can be scanned and connected by central device,
* and the client will get the BAS value. It calls the API bta
* layer provides.
*
****************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "controller.h"

#include "bt_trace.h"
#include "bt_types.h"

#include "blufi.h"

#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_blufi_api.h"

extern void wifi_set_blue_config(char *ssid, char *passwd);

#define HEADER_SSID "ssid"
#define HEADER_PASSWD   "passwd"
#define HEADER_CONFIRM  "confirm"

static char tmp_ssid[32 + 1];
static char tmp_passwd[64 + 1];

static void blufi_data_recv(uint8_t *data, int len)
{
    char *p = NULL;
    LOG_DEBUG("the data is:%s\n", data);

    p = strstr((char *)data, HEADER_SSID);
    if (p) {
        LOG_ERROR("SSID: %s\n", p + strlen(HEADER_SSID) + 1);
        strcpy(tmp_ssid, p + strlen(HEADER_SSID) + 1);
    }
    p = strstr((char *)data, HEADER_PASSWD);
    if (p) {
        LOG_ERROR("PASSWORD: %s\n", p + strlen(HEADER_PASSWD) + 1);
        strcpy(tmp_passwd, p + strlen(HEADER_PASSWD) + 1);
    }
    p = strstr((char *)data, HEADER_CONFIRM);
    if (p) {
        LOG_ERROR("CONFIRM\n");
        wifi_set_blue_config(tmp_ssid, tmp_passwd);
    }

}

static void blufi_callback(uint32_t event, void *param)
{
    /* actually, should post to blufi_task handle the procedure,
     * now, as a demo, we do simplely */
    switch (event) {
    case ESP_BLUFI_EVENT_INIT_FINISH:
        LOG_ERROR("blufi init finish\n");
        break;
    case ESP_BLUFI_EVENT_RECV_DATA: {
        LOG_DEBUG("blufi recv data\n");
        esp_blufi_cb_param_t *blufi_param = (esp_blufi_cb_param_t *)param;
        blufi_data_recv(blufi_param->recv_data.data, blufi_param->recv_data.data_len);
        break;
    }
    default:
        break;
    }
}

static esp_err_t blufi_startup_in_blufi_task(void *arg)
{
    /*set connectable,discoverable, pairable and paired only modes of local device*/
    tBTA_DM_DISC disc_mode = BTA_DM_BLE_GENERAL_DISCOVERABLE;
    tBTA_DM_CONN conn_mode = BTA_DM_BLE_CONNECTABLE;
    BTA_DmSetVisibility(disc_mode, conn_mode, (uint8_t)BTA_DM_NON_PAIRABLE, (uint8_t)BTA_DM_CONN_ALL);

    esp_blufi_register_callback(blufi_callback);
    esp_blufi_profile_init();

    return ESP_OK;
}


static void blufi_startup(void)
{
    blufi_transfer_context(blufi_startup_in_blufi_task, NULL);
}

esp_err_t blufi_enable(void *arg)
{
    esp_err_t err;

    BTM_SetTraceLevel(BT_TRACE_LEVEL_ERROR);

    err = esp_enable_bluetooth();
    if (err) {
        LOG_ERROR("%s failed\n", __func__);
        return err;
    }
    blufi_startup();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    return err;
}

esp_err_t blufi_disable(void *arg)
{
    esp_err_t err;

    err =  esp_disable_bluetooth();

    if (arg) {
        ((void (*)(void))arg)();
    }

    return err;
}
