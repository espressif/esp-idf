/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/soc_caps.h"

#if SOC_KEY_MANAGER_SUPPORTED

#include "esp_assert.h"
#include "rom/km.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HUK_INFO_SIZE           384

/**
 * @brief Mode for Hardware Unique Key Process: recovery, generation
 */
typedef enum {
    ESP_HUK_MODE_RECOVERY = 0,     /* HUK recovery mode */
    ESP_HUK_MODE_GENERATION,       /* HUK generation mode */
} esp_huk_mode_t;

ESP_STATIC_ASSERT(sizeof(esp_huk_mode_t) == sizeof(huk_mode_t), "Size of esp_huk_mode_t should match huk_mode_t (from ROM)");

/**
 * @brief State of Hardware Unique Key Generator: idle, load, gain or busy.
 *
 */
typedef enum {
    ESP_HUK_STATE_IDLE = 0, /* Key Manager is idle */
    ESP_HUK_STATE_LOAD,     /* Key Manager is read to recieve input */
    ESP_HUK_STATE_GAIN,     /* Key Manager is ready to provide output */
    ESP_HUK_STATE_BUSY      /* Key Manager is busy */
} esp_huk_state_t;

/**
 * @brief Status of the Hardware Unique Key Generation:
 * not generated, generated and valid, generated and invalid
 */
typedef enum {
    ESP_HUK_STATUS_NOT_GENERATED = 0,      /* HUK is not generated */
    ESP_HUK_STATUS_GENERATED_AND_VALID,    /* HUK is generated and valid */
    ESP_HUK_STATUS_GENERATED_AND_INVALID   /* HUK is generated and is invalid */
} esp_huk_gen_status_t;

/**
 * @brief
 * HUK interrupt types
 */
typedef enum {
    ESP_HUK_INT_PREP_DONE = 0x01,
    ESP_HUK_INT_PROC_DONE,
    ESP_HUK_INT_POST_DONE,
} esp_huk_interrupt_type_t;

#ifdef __cplusplus
}
#endif
#endif
