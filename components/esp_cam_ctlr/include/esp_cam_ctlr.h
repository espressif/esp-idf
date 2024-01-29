/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_cam_ctlr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable ESP CAM controller
 *
 * @param[in] handle  ESP CAM controller handle
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_INVALID_STATE: Invalid state
 */
esp_err_t esp_cam_ctlr_enable(esp_cam_ctlr_handle_t handle);

/**
 * @brief Start ESP CAM controller
 *
 * @param[in] handle  ESP CAM controller handle
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_INVALID_STATE: Invalid state
 */
esp_err_t esp_cam_ctlr_start(esp_cam_ctlr_handle_t handle);

/**
 * @brief Stop ESP CAM controller
 *
 * @param[in] handle  ESP CAM controller handle
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_INVALID_STATE: Invalid state
 */
esp_err_t esp_cam_ctlr_stop(esp_cam_ctlr_handle_t handle);

/**
 * @brief Disable ESP CAM controller
 *
 * @param[in] handle  ESP CAM controller handle
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_INVALID_STATE: Invalid state
 */
esp_err_t esp_cam_ctlr_disable(esp_cam_ctlr_handle_t handle);

/**
 * @brief Receive data to the given transaction
 *
 * @param[in] handle      ESP CAM controller handle
 * @param[in] trans       ESP CAM controller transaction type
 * @param[in] timeout_ms  Timeout in ms
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_INVALID_STATE: Invalid state
 */
esp_err_t esp_cam_ctlr_receive(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, uint32_t timeout_ms);

/**
 * @brief Delete ESP CAM controller handle
 *
 * @param[in] handle  ESP CAM controller handle
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_INVALID_STATE: Invalid state
 */
esp_err_t esp_cam_del_ctlr(esp_cam_ctlr_handle_t handle);

/**
 * @brief Register ESP CAM controller event callbacks
 *
 * @param[in] handle  ESP CAM controller handle
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_INVALID_STATE: Invalid state
 */
esp_err_t esp_cam_ctlr_register_event_callbacks(esp_cam_ctlr_handle_t handle, const esp_cam_ctlr_evt_cbs_t *cbs, void *user_data);

#ifdef __cplusplus
}
#endif
