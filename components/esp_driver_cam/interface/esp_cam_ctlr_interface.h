/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "esp_cam_ctlr_types.h"
#include "hal/cam_ctlr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of CAM controller.
 */
typedef struct esp_cam_ctlr_t esp_cam_ctlr_t;

/**
 * @brief CAM controller interface.
 */
struct esp_cam_ctlr_t {
    /**
     * @brief Enable ESP CAM controller.
     *
     * @param[in] handle ESP CAM controller handle.
     *
     * @return
     *      - ESP_OK: Success.
     *      - ESP_ERR_INVALID_ARG: Invalid argument.
     *      - ESP_ERR_INVALID_STATE: Invalid state.
     */
    esp_err_t (*enable)(esp_cam_ctlr_t *ctlr);

    /**
     * @brief Start ESP CAM controller.
     *
     * @param[in] handle ESP CAM controller handle.
     *
     * @return
     *      - ESP_OK: Success.
     *      - ESP_ERR_INVALID_ARG: Invalid argument.
     *      - ESP_ERR_INVALID_STATE: Invalid state.
     */
    esp_err_t (*start)(esp_cam_ctlr_t *ctlr);

    /**
     * @brief Stop ESP CAM controller.
     *
     * @param[in] handle ESP CAM controller handle.
     *
     * @return
     *      - ESP_OK: Success.
     *      - ESP_ERR_INVALID_ARG: Invalid argument.
     *      - ESP_ERR_INVALID_STATE: Invalid state.
     */
    esp_err_t (*stop)(esp_cam_ctlr_t *ctlr);

    /**
     * @brief Disable ESP CAM controller.
     *
     * @param[in] handle ESP CAM controller handle.
     *
     * @return
     *      - ESP_OK: Success.
     *      - ESP_ERR_INVALID_ARG: Invalid argument.
     *      - ESP_ERR_INVALID_STATE: Invalid state.
     */
    esp_err_t (*disable)(esp_cam_ctlr_t *ctlr);

    /**
     * @brief Receive data to the given transaction.
     *
     * @param[in] handle      ESP CAM controller handle.
     * @param[in] trans       ESP CAM controller transaction type.
     * @param[in] timeout_ms  Timeout. Measurement unit: milliseconds.
     *
     * @return
     *      - ESP_OK: Success.
     *      - ESP_ERR_INVALID_ARG: Invalid argument.
     *      - ESP_ERR_INVALID_STATE: Invalid state.
     */
    esp_err_t (*receive)(esp_cam_ctlr_t *ctlr, esp_cam_ctlr_trans_t *trans, uint32_t timeout_ms);

    /**
     * @brief Delete ESP CAM controller handle.
     *
     * @param[in] handle ESP CAM controller handle.
     *
     * @return
     *      - ESP_OK: Success.
     *      - ESP_ERR_INVALID_ARG: Invalid argument.
     *      - ESP_ERR_INVALID_STATE: Invalid state.
     */
    esp_err_t (*del)(esp_cam_ctlr_t *ctlr);

    /**
     * @brief Register ESP CAM controller event callbacks.
     *
     * @param[in] handle  ESP CAM controller handle.
     *
     * @return
     *      - ESP_OK: Success.
     *      - ESP_ERR_INVALID_ARG: Invalid argument.
     *      - ESP_ERR_INVALID_STATE: Invalid state.
     */
    esp_err_t (*register_event_callbacks)(esp_cam_ctlr_t *ctlr, const esp_cam_ctlr_evt_cbs_t *cbs, void *user_ctx);

    /**
     * @brief Get ESP CAM controller internal malloced backup buffer(s) address.
     *
     * @param[in]   esp_cam_ctlr_t *    ESP CAM controller handle.
     * @param[in]   uint32_t            Number of frame buffer(s) to get. This value must match the number of the following fbN parameters.
     * @param[out]  const void **       Address of the frame buffer 0 (first frame buffer).
     * @param[out]  ...                 List of other frame buffers if any.
     *
     * @return
     *      - ESP_OK: Success.
     *      - ESP_ERR_INVALID_ARG: Invalid argument.
     *      - ESP_ERR_INVALID_STATE: Invalid driver state.
     */
    esp_err_t (*get_internal_buffer)(esp_cam_ctlr_t *, uint32_t, const void **, ...);

    /**
     * @brief Get ESP CAM controller internal backup buffer length.
     *
     * @param[in]   esp_cam_ctlr_t *    ESP CAM controller handle.
     * @param[out]  size_t *          The size of each frame buffer. Measurement unit: bytes.
     *
     * @return
     *      - ESP_OK: Success.
     *      - ESP_ERR_INVALID_ARG: NULL pointer.
     *      - ESP_ERR_INVALID_STATE: Invalid driver state.
     */
    esp_err_t (*get_buffer_len)(esp_cam_ctlr_t *, size_t *);

    /**
     * @brief Allocate aligned camera buffer for ESP CAM controller.
     *
     * @param[in]   esp_cam_ctlr_t *    ESP CAM controller handle.
     * @param[in]   size_t              Buffer size. Measurement unit: bytes.
     * @param[in]   uint32_t            Buffer allocation capabilities.
     *
     * @return
     *      - Buffer pointer on success.
     *      - NULL on failure.
     */
    void *(*alloc_buffer)(esp_cam_ctlr_t *, size_t, uint32_t);

    /**
     * @brief Configure format conversion.
     *
     * @param[in] esp_cam_ctlr_t *      ESP CAM controller handle.
     * @param[in] const cam_ctlr_format_conv_config_t * Color conversion configuration.
     *
     * @return
     *      - ESP_OK: Success.
     *      - ESP_ERR_INVALID_ARG: Invalid argument.
     *      - ESP_ERR_NOT_SUPPORTED: Format conversion not supported by this controller.
     */
    esp_err_t (*format_conversion)(esp_cam_ctlr_t *, const cam_ctlr_format_conv_config_t *);

    void *user_data; /*!< User data. */
};

#ifdef __cplusplus
}
#endif
