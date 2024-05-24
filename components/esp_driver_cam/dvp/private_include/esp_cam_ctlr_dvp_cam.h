/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "freertos/FreeRTOS.h"
#include "esp_cam_ctlr_interface.h"
#include "hal/cam_hal.h"
#include "esp_cam_ctlr_dvp_dma.h"
#include "esp_cam_ctlr_dvp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DVP CAM finite state machine
 */
typedef enum esp_cam_ctlr_dvp_cam_fsm {
    ESP_CAM_CTLR_DVP_CAM_FSM_INIT = 1,                  /*!< DVP CAM initialization state, and next state is "enabled" */
    ESP_CAM_CTLR_DVP_CAM_FSM_ENABLED,                   /*!< DVP CAM enabled state, and next state is "init" or "started" */
    ESP_CAM_CTLR_DVP_CAM_FSM_STARTED,                   /*!< DVP CAM started state, and next state is "init" or "enabled" */
} esp_cam_ctlr_dvp_cam_fsm_t;

/**
 * @brief DVP CAM object data
 */
typedef struct esp_cam_ctlr_dvp_cam {
    esp_cam_ctlr_t base;                                /*!< Camera controller base object data */
    esp_cam_ctlr_evt_cbs_t cbs;                         /*!< Camera controller callback functions */
    void *cbs_user_data;                                /*!< Camera controller callback private data */

    int ctlr_id;                                        /*!< DVP CAM port ID */
    cam_ctlr_color_t input_data_color_type;             /*!< DVP CAM input pixel format */
    struct {
        uint32_t bk_buffer_dis : 1;                     /*!< Disable backup buffer */
        uint32_t pin_dont_init : 1;                     /*!< Don't initialize DVP pins if users have called "esp_cam_ctlr_dvp_init" before */
        uint32_t pic_format_jpeg : 1;                   /*!< Input picture format is JPEG, if set this flag and "input_data_color_type" will be ignored */
    };                                                  /*!< Boolean Flags */

    cam_hal_context_t hal;                              /*!< DVP CAM HAL object data */
    esp_cam_ctlr_dvp_dma_t dma;                         /*!< DVP CAM DMA object data */
    size_t fb_size_in_bytes;                            /*!< DVP CAM frame buffer maximum size in byte */
    esp_cam_ctlr_dvp_cam_fsm_t dvp_fsm;                 /*!< DVP CAM finite state machine */
    uint8_t *cur_buf;                                   /*!< DVP CAM current buffer which is receiving stream */
    uint8_t *backup_buffer;                             /*!< DVP CAM backup buffer */
    bool bk_buffer_exposed;                             /*!< status of if back_buffer is exposed to users */
    portMUX_TYPE spinlock;                              /*!< DVP CAM spinlock */
} esp_cam_ctlr_dvp_cam_t;

#ifdef __cplusplus
}
#endif
