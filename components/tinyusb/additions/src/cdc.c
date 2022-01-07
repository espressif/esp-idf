/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "tusb.h"
#include "cdc.h"

#define CDC_INTF_NUM            CFG_TUD_CDC // number of cdc blocks
static esp_tusb_cdc_t *cdc_obj[CDC_INTF_NUM] = {};
static const char *TAG = "tusb_cdc";

esp_tusb_cdc_t *tinyusb_cdc_get_intf(int itf_num)
{
    if (itf_num >= CDC_INTF_NUM || itf_num < 0) {
        return NULL;
    }
    return cdc_obj[itf_num];
}

static esp_err_t cdc_obj_check(int itf, bool expected_inited, tusb_class_code_t expected_type)
{
    esp_tusb_cdc_t *this_itf = tinyusb_cdc_get_intf(itf);

    bool inited = (this_itf != NULL);
    ESP_RETURN_ON_FALSE(expected_inited == inited, ESP_ERR_INVALID_STATE, TAG, "Wrong state of the interface. Expected state: %s", expected_inited ? "initialized" : "not initialized");
    ESP_RETURN_ON_FALSE(!(inited && (expected_type != -1) && !(this_itf->type == expected_type)), ESP_ERR_INVALID_STATE, TAG, "Wrong type of the interface. Should be : 0x%x (tusb_class_code_t)", expected_type);
    return ESP_OK;
}

static esp_err_t tusb_cdc_comm_init(int itf)
{
    ESP_RETURN_ON_ERROR(cdc_obj_check(itf, false, -1), TAG, "cdc_obj_check failed");
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
    ESP_RETURN_ON_ERROR(cdc_obj_check(itf, true, TUSB_CLASS_CDC), TAG, "cdc_obj_check failed");
    free(cdc_obj[itf]);
    cdc_obj[itf] = NULL;
    return ESP_OK;
}

static esp_err_t tusb_cdc_data_init(int itf)
{
    ESP_RETURN_ON_ERROR(cdc_obj_check(itf, false, TUSB_CLASS_CDC_DATA), TAG, "cdc_obj_check failed");
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
    ESP_RETURN_ON_ERROR(cdc_obj_check(itf, true, TUSB_CLASS_CDC_DATA), TAG, "cdc_obj_check failed");
    free(cdc_obj[itf]);
    cdc_obj[itf] = NULL;
    return ESP_OK;
}

esp_err_t tinyusb_cdc_init(int itf, const tinyusb_config_cdc_t *cfg)
{
    ESP_RETURN_ON_ERROR(cdc_obj_check(itf, false, -1), TAG, "cdc_obj_check failed");

    ESP_LOGD(TAG, "Init CDC %d", itf);
    if (cfg->cdc_class == TUSB_CLASS_CDC) {
        ESP_RETURN_ON_ERROR(tusb_cdc_comm_init(itf), TAG, "tusb_cdc_comm_init failed");
        cdc_obj[itf]->cdc_subclass.comm_subclass = cfg->cdc_subclass.comm_subclass;
    } else {
        ESP_RETURN_ON_ERROR(tusb_cdc_data_init(itf), TAG, "tusb_cdc_data_init failed");
        cdc_obj[itf]->cdc_subclass.data_subclass = cfg->cdc_subclass.data_subclass;
    }
    cdc_obj[itf]->usb_dev = cfg->usb_dev;
    return ESP_OK;
}

esp_err_t tinyusb_cdc_deinit(int itf)
{
    ESP_RETURN_ON_ERROR(cdc_obj_check(itf, true, -1), TAG, "cdc_obj_check failed");

    ESP_LOGD(TAG, "Deinit CDC %d", itf);
    if (cdc_obj[itf]->type == TUSB_CLASS_CDC) {
        ESP_RETURN_ON_ERROR(tusb_cdc_deinit_comm(itf), TAG, "tusb_cdc_deinit_comm failed");
    } else if (cdc_obj[itf]->type == TUSB_CLASS_CDC_DATA) {
        ESP_RETURN_ON_ERROR(tusb_cdc_deinit_data(itf), TAG, "tusb_cdc_deinit_data failed");
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}
