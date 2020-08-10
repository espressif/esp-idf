// Copyright 2020 Espressif Systems (Shanghai) Co. Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include "esp_err.h"
#include "esp_log.h"
#include "tusb.h"
#include "cdc.h"
#include "sdkconfig.h"

static const char *TAG = "tusb_cdc";

#define ESP_RETURN_ON_ERROR(x) do {esp_err_t r = (x); if (r != ESP_OK) return r;} while(0)

#define CDC_INTF_NUM            CFG_TUD_CDC // number of cdc blocks

static esp_tusb_cdc_t *cdc_obj[CDC_INTF_NUM] = {};

/* Common CDC functions
   ********************************************************************* */
bool tinyusb_cdc_initialized(int itf)
{
    return (cdc_obj[itf] != NULL);
}

static esp_err_t cdc_interface_check(int itf)
{
    if (tinyusb_cdc_initialized(itf)) {
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Interface is not initialized. Use `tinyusb_cdc_init` for initialization");
        return ESP_ERR_INVALID_STATE;
    }
}

/**
 * @brief
 *
 * @param itf
 * @param expected_inited
 * @param expected_type use -1 if you don't care
 * @return esp_err_t
 */
static esp_err_t cdc_obj_check(int itf, bool expected_inited, tusb_class_code_t expected_type)
{
    bool inited = (cdc_obj[itf] != NULL);
    if (expected_inited != inited) {
        ESP_LOGE(TAG, "Wrong state of the interface. Expected state: %s",
                 expected_inited ? "initialized" : "not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    if (inited && (expected_type != -1) && !(cdc_obj[itf]->type == expected_type)) {
        ESP_LOGE(TAG, "Wrong type of the interface. Should be : 0x%x (tusb_class_code_t)", expected_type);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_tusb_cdc_t *tinyusb_cdc_get_intf(int itf_num)
{
    if (cdc_interface_check(itf_num) != ESP_OK) {
        return NULL;
    }
    return cdc_obj[itf_num];
}

/*********************************************************************** Common CDC functions*/
/* CDC class funcs
   ********************************************************************* */
static esp_err_t tusb_cdc_comm_init(int itf)
{
    ESP_RETURN_ON_ERROR(cdc_obj_check(itf, false, -1));
    cdc_obj[itf] = calloc(1, sizeof(esp_tusb_cdc_t));
    if (cdc_obj[itf] != NULL) {
        cdc_obj[itf]->type = TUSB_CLASS_CDC;
        ESP_LOGD(TAG, "CDC Comm class initialized");
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "CDC Comm initialization error");
        return ESP_FAIL;
    }
}

static esp_err_t tusb_cdc_deinit_comm(int itf)
{
    ESP_RETURN_ON_ERROR(cdc_obj_check(itf, true, TUSB_CLASS_CDC));
    free(cdc_obj[itf]);
    cdc_obj[itf] = NULL;
    return ESP_OK;
}

static esp_err_t tusb_cdc_data_init(int itf)
{
    ESP_RETURN_ON_ERROR(cdc_obj_check(itf, false, TUSB_CLASS_CDC_DATA));
    cdc_obj[itf] = calloc(1, sizeof(esp_tusb_cdc_t));
    if (cdc_obj[itf] != NULL) {
        cdc_obj[itf]->type = TUSB_CLASS_CDC_DATA;
        ESP_LOGD(TAG, "CDC Data class initialized");
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "CDC Data initialization error");
        return ESP_FAIL;
    }
}

static esp_err_t tusb_cdc_deinit_data(int itf)
{
    ESP_RETURN_ON_ERROR(cdc_obj_check(itf, true, TUSB_CLASS_CDC_DATA));
    free(cdc_obj[itf]);
    cdc_obj[itf] = NULL;
    return ESP_OK;
}
/*********************************************************************** CDC class funcs*/
/* CDC initialization
   ********************************************************************* */
esp_err_t tinyusb_cdc_init(int itf, const tinyusb_config_cdc_t *cfg)
{
    ESP_LOGD(TAG, "CDC initialization...");
    if (itf != 0) {
        ESP_LOGE(TAG, "There is not CDC no.%d", itf);
        return ESP_ERR_INVALID_ARG;
    }
    if (cfg->cdc_class == TUSB_CLASS_CDC) {
        ESP_RETURN_ON_ERROR(tusb_cdc_comm_init(itf));
        cdc_obj[itf]->cdc_subclass.comm_subclass = cfg->cdc_subclass.comm_subclass;
    } else {
        ESP_RETURN_ON_ERROR(tusb_cdc_data_init(itf));
        cdc_obj[itf]->cdc_subclass.data_subclass = cfg->cdc_subclass.data_subclass;
    }
    cdc_obj[itf]->usb_dev = cfg->usb_dev;
    return ESP_OK;
}


esp_err_t tinyusb_cdc_deinit(int itf)
{
    if (itf != 0) {
        ESP_LOGE(TAG, "There is not CDC no.%d", itf);
        return ESP_ERR_INVALID_ARG;
    }
    if (cdc_obj[itf]->type == TUSB_CLASS_CDC) {
        ESP_RETURN_ON_ERROR(tusb_cdc_deinit_comm(itf));
    } else if (cdc_obj[itf]->type == TUSB_CLASS_CDC_DATA) {
        ESP_RETURN_ON_ERROR(tusb_cdc_deinit_data(itf));
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_LOGD(TAG, "De-initialized");
    return ESP_OK;
}
/*********************************************************************** CDC initialization*/
