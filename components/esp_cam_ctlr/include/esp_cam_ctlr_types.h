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
 * @brief ESP CAM controller handle
 */
typedef struct esp_cam_ctlr_t *esp_cam_ctlr_handle_t;

/**
 * @brief ESP CAM controller transaction type
 */
typedef struct {
    void *buffer;            ///< Transaction buffer
    size_t buflen;           ///< Len of the transaction buffer
    size_t received_size;    ///< Received size, this received_size will be written by the driver, indicating the real received size
} esp_cam_ctlr_trans_t;

/**
 * @brief ESP CAM controller event callbacks
 */
typedef struct {
    /**
     * @brief On get new transaction callback
     *
     * @param[in] handle     ESP CAM controller handle
     * @param[in] trans      New transaction
     * @param[in] user_data  User registered data
     *
     * @return Whether a high priority task is woken up by this function
     */
    bool (*on_get_new_trans)(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data);

    /**
     * @brief On transaction finish callback
     *
     * @param[in] handle     ESP CAM controller handle
     * @param[out] trans     Finished transaction
     * @param[in] user_data  User registered data
     *
     * @return Whether a high priority task is woken up by this function
     */
    bool (*on_trans_finished)(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data);

} esp_cam_ctlr_evt_cbs_t;

#ifdef __cplusplus
}
#endif
