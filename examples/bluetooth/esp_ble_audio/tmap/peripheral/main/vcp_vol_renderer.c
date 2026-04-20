/*
 * SPDX-FileCopyrightText: 2020 Bose Corporation
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2022 Codecoup
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "tmap_peripheral.h"

#define VOCS_INST_COUNT         CONFIG_BT_VCP_VOL_REND_VOCS_INSTANCE_COUNT
#define AICS_INST_COUNT         CONFIG_BT_VCP_VOL_REND_AICS_INSTANCE_COUNT

#define VOCS_OUTPUT_DESC_SIZE   CONFIG_BT_VOCS_MAX_OUTPUT_DESCRIPTION_SIZE
#define AICS_INPUT_DESC_SIZE    CONFIG_BT_AICS_MAX_INPUT_DESCRIPTION_SIZE

static esp_ble_audio_vcp_included_t vcp_included;

static void vcs_state_cb(esp_ble_conn_t *conn, int err, uint8_t volume, uint8_t mute)
{
    if (err) {
        ESP_LOGE(TAG, "Failed to get VCS state, err %d", err);
    } else {
        ESP_LOGI(TAG, "VCS volume %u, mute %u", volume, mute);
    }
}

static void vcs_flags_cb(esp_ble_conn_t *conn, int err, uint8_t flags)
{
    if (err) {
        ESP_LOGE(TAG, "Failed to get VCS flags, err %d", err);
    } else {
        ESP_LOGI(TAG, "VCS flags 0x%02x", flags);
    }
}

static void aics_state_cb(esp_ble_audio_aics_t *inst,
                          int err, int8_t gain,
                          uint8_t mute, uint8_t mode)
{
    if (err) {
        ESP_LOGE(TAG, "AICS state get failed (%d) for inst %p", err, inst);
    } else {
        ESP_LOGI(TAG, "AICS inst %p state gain %d, mute %u, mode %u",
                 inst, gain, mute, mode);
    }
}

static void aics_gain_setting_cb(esp_ble_audio_aics_t *inst,
                                 int err, uint8_t units,
                                 int8_t minimum, int8_t maximum)
{
    if (err) {
        ESP_LOGE(TAG, "AICS gain settings get failed (%d) for inst %p", err, inst);
    } else {
        ESP_LOGI(TAG, "AICS inst %p gain settings units %u, min %d, max %d",
                 inst, units, minimum, maximum);
    }
}

static void aics_input_type_cb(esp_ble_audio_aics_t *inst, int err, uint8_t input_type)
{
    if (err) {
        ESP_LOGE(TAG, "AICS input type get failed (%d) for inst %p", err, inst);
    } else {
        ESP_LOGI(TAG, "AICS inst %p input type %u", inst, input_type);
    }
}

static void aics_status_cb(esp_ble_audio_aics_t *inst, int err, bool active)
{
    if (err) {
        ESP_LOGE(TAG, "AICS status get failed (%d) for inst %p", err, inst);
    } else {
        ESP_LOGI(TAG, "AICS inst %p status %s", inst, active ? "active" : "inactive");
    }
}
static void aics_description_cb(esp_ble_audio_aics_t *inst, int err, char *description)
{
    if (err) {
        ESP_LOGE(TAG, "AICS description get failed (%d) for inst %p", err, inst);
    } else {
        ESP_LOGI(TAG, "AICS inst %p description %s", inst, description);
    }
}
static void vocs_state_cb(esp_ble_audio_vocs_t *inst, int err, int16_t offset)
{
    if (err) {
        ESP_LOGE(TAG, "VOCS state get failed (%d) for inst %p", err, inst);
    } else {
        ESP_LOGI(TAG, "VOCS inst %p offset %d", inst, offset);
    }
}

static void vocs_location_cb(esp_ble_audio_vocs_t *inst, int err, uint32_t location)
{
    if (err) {
        ESP_LOGE(TAG, "VOCS location get failed (%d) for inst %p", err, inst);
    } else {
        ESP_LOGI(TAG, "VOCS inst %p location %u", inst, location);
    }
}

static void vocs_description_cb(esp_ble_audio_vocs_t *inst, int err, char *description)
{
    if (err) {
        ESP_LOGE(TAG, "VOCS description get failed (%d) for inst %p", err, inst);
    } else {
        ESP_LOGI(TAG, "VOCS inst %p description %s", inst, description);
    }
}

static esp_ble_audio_vcp_vol_rend_cb_t vcp_cbs = {
    .state = vcs_state_cb,
    .flags = vcs_flags_cb,
};

static esp_ble_audio_aics_cb_t aics_cbs = {
    .state        = aics_state_cb,
    .gain_setting = aics_gain_setting_cb,
    .type         = aics_input_type_cb,
    .status       = aics_status_cb,
    .description  = aics_description_cb
};

static esp_ble_audio_vocs_cb_t vocs_cbs = {
    .state       = vocs_state_cb,
    .location    = vocs_location_cb,
    .description = vocs_description_cb
};

int vcp_vol_renderer_init(void)
{
    esp_ble_audio_vocs_register_param_t vocs_param[VOCS_INST_COUNT] = {0};
    esp_ble_audio_aics_register_param_t aics_param[AICS_INST_COUNT] = {0};
    esp_ble_audio_vcp_vol_rend_register_param_t vcp_register_param = {0};
    char output_desc[VOCS_INST_COUNT][VOCS_OUTPUT_DESC_SIZE];
    char input_desc[AICS_INST_COUNT][AICS_INPUT_DESC_SIZE];
    int err;

    vcp_register_param.vocs_param = vocs_param;
    vcp_register_param.aics_param = aics_param;

    for (size_t i = 0; i < VOCS_INST_COUNT; i++) {
        vcp_register_param.vocs_param[i].location_writable = true;
        snprintf(output_desc[i], sizeof(output_desc[i]), "Output %d", i + 1);
        vcp_register_param.vocs_param[i].output_desc = output_desc[i];
        vcp_register_param.vocs_param[i].desc_writable = true;
        vcp_register_param.vocs_param[i].cb = &vocs_cbs;
    }

    for (size_t i = 0; i < AICS_INST_COUNT; i++) {
        vcp_register_param.aics_param[i].gain_mode = ESP_BLE_AUDIO_AICS_MODE_MANUAL;
        vcp_register_param.aics_param[i].units = 1;
        vcp_register_param.aics_param[i].min_gain = -100;
        vcp_register_param.aics_param[i].max_gain = 100;
        vcp_register_param.aics_param[i].type = ESP_BLE_AUDIO_AICS_INPUT_TYPE_UNSPECIFIED;
        vcp_register_param.aics_param[i].status = true;
        vcp_register_param.aics_param[i].desc_writable = true;
        snprintf(input_desc[i], sizeof(input_desc[i]), "Input %d", i + 1);
        vcp_register_param.aics_param[i].description = input_desc[i];
        vcp_register_param.aics_param[i].cb = &aics_cbs;
    }

    vcp_register_param.step = 1;
    vcp_register_param.mute = ESP_BLE_AUDIO_VCP_STATE_UNMUTED;
    vcp_register_param.volume = 10;
    vcp_register_param.cb = &vcp_cbs;

    err = esp_ble_audio_vcp_vol_rend_register(&vcp_register_param);
    if (err) {
        ESP_LOGE(TAG, "Failed to register VCP renderer, err %d", err);
        return err;
    }

    err = esp_ble_audio_vcp_vol_rend_included_get(&vcp_included);
    if (err) {
        ESP_LOGE(TAG, "Failed to get VCP renderer included service, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "vcp vol renderer, vocs_cnt %u aics_cnt %u",
             vcp_included.vocs_cnt, vcp_included.aics_cnt);
    for (size_t i = 0; i < vcp_included.vocs_cnt; i++) {
        ESP_LOGI(TAG, "vcp vol renderer, vocs_%zu %p", i, vcp_included.vocs[i]);
    }
    for (size_t i = 0; i < vcp_included.aics_cnt; i++) {
        ESP_LOGI(TAG, "vcp vol renderer, aics_%zu %p", i, vcp_included.aics[i]);
    }

    ESP_LOGI(TAG, "VCP volume renderer initialized");

    return 0;
}
