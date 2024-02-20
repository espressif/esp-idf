/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "esp_cam_ctlr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_cam_ctlr_t esp_cam_ctlr_t;  /*!< Type of CAM controller */

/**
 * @brief Cam controller interface
 */
struct esp_cam_ctlr_t {
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
    esp_err_t (*enable)(esp_cam_ctlr_t *ctlr);

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
    esp_err_t (*start)(esp_cam_ctlr_t *ctlr);

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
    esp_err_t (*stop)(esp_cam_ctlr_t *ctlr);

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
    esp_err_t (*disable)(esp_cam_ctlr_t *ctlr);

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
    esp_err_t (*receive)(esp_cam_ctlr_t *ctlr, esp_cam_ctlr_trans_t *trans, uint32_t timeout_ms);

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
    esp_err_t (*del)(esp_cam_ctlr_t *ctlr);

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
    esp_err_t (*register_event_callbacks)(esp_cam_ctlr_t *ctlr, const esp_cam_ctlr_evt_cbs_t *cbs, void *user_ctx);

    void *user_data;  ///< User data
};

#ifdef __cplusplus
}
#endif
