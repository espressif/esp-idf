/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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
 * @brief ESP CAM controller CSI host error flags.
 *
 * @note These flags summarize the CSI host error type(s) and avoid exposing hardware bit layout.
 */
typedef enum {
    ESP_CAM_CTLR_CSI_HOST_ERR_PHY     = (1U << 0),    /*!< PHY error. */
    ESP_CAM_CTLR_CSI_HOST_ERR_PACKET  = (1U << 1),    /*!< Packet error. */
    ESP_CAM_CTLR_CSI_HOST_ERR_FRAME   = (1U << 2),    /*!< Frame boundary or sequence error. */
    ESP_CAM_CTLR_CSI_HOST_ERR_CRC     = (1U << 3),    /*!< Frame or payload CRC error. */
    ESP_CAM_CTLR_CSI_HOST_ERR_DATA_ID = (1U << 4),    /*!< Unrecognized or unsupported data type detected. */
} esp_cam_ctlr_csi_host_error_t;

/**
 * @brief ESP CAM controller "error" event data.
 */
typedef struct {
    uint32_t csi_host_err_evts;  /*!< Bitwise OR of `esp_cam_ctlr_csi_host_error_t`. */
} esp_cam_ctlr_error_event_data_t;

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

    /**
     * @brief Callback for when an error is detected.
     *
     * @note This callback is invoked from ISR context, so it must not call any blocking APIs.
     *       When the controller's ISR cache-safe option is enabled, this callback and its
     *       user data must be placed in internal RAM.
     *
     * @param[in] handle ESP CAM controller handle.
     * @param[in] edata Error event data.
     * @param[in] user_data User-registered data.
     *
     * @return Whether a high-priority task is woken up by this function.
     */
    bool (*on_error)(esp_cam_ctlr_handle_t handle, const esp_cam_ctlr_error_event_data_t *edata, void *user_data);

} esp_cam_ctlr_evt_cbs_t;

#ifdef __cplusplus
}
#endif
