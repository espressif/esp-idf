/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum timeout value in ticks for the ESP CAM controller.
 */
#define ESP_CAM_CTLR_MAX_DELAY UINT32_MAX

/**
 * @brief Handle for the ESP CAM controller.
 */
typedef struct esp_cam_ctlr_t *esp_cam_ctlr_handle_t;

/**
 * @brief Transaction type for the ESP CAM controller.
 */
typedef struct {
    void *buffer;            /*!< Transaction buffer. */
    size_t buflen;           /*!< Length of the transaction buffer. */
    size_t received_size;    /*!< Received size. This value is written by the driver and indicates the actual received size. */
} esp_cam_ctlr_trans_t;

/**
 * @brief Event callbacks for the ESP CAM controller.
 */
typedef struct {
    /**
     * @brief Callback for receiving a new transaction.
     *
     * @param[in] handle ESP CAM controller handle.
     * @param[in] trans New transaction.
     * @param[in] user_data User-registered data.
     *
     * @return Whether a high-priority task is woken up by this function.
     */
    bool (*on_get_new_trans)(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data);

    /**
     * @brief Callback for a finished transaction.
     *
     * @param[in] handle ESP CAM controller handle.
     * @param[out] trans Finished transaction.
     * @param[in] user_data User-registered data.
     *
     * @return Whether a high-priority task is woken up by this function.
     */
    bool (*on_trans_finished)(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data);

} esp_cam_ctlr_evt_cbs_t;

#ifdef __cplusplus
}
#endif
