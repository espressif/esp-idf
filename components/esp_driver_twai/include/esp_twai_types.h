/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h>
#include "hal/twai_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ESP TWAI controller handle
 */
typedef struct twai_node_base *twai_node_handle_t;

/**
 * @brief TWAI bitrate timing config basic (simple) mode
 */
typedef struct {
    uint32_t bitrate;               /**< Expected TWAI bus baud_rate/bitrate in bits/second */
    uint16_t sp_permill;            /**< Optional, sampling point in permill (1/1000) of the entire bit time */
    uint16_t ssp_permill;           /**< Optional, secondary sample point(ssp) in permill (1/1000) of the entire bit time */
} twai_timing_basic_config_t;

/**
 * @brief TWAI transaction frame param type
 */
typedef struct {
    twai_frame_header_t header;     /**< message attribute/metadata, exclude data buffer*/
    uint8_t *buffer;                /**< buffer address for tx and rx message data*/
    size_t buffer_len;              /**< buffer length of provided data buffer pointer, in bytes.*/
} twai_frame_t;

/**
 * @brief TWAI node's status
 */
typedef struct {
    twai_error_state_t state;       /**< Node's error state */
    uint16_t tx_error_count;        /**< Node's TX error count */
    uint16_t rx_error_count;        /**< Node's RX error count */
} twai_node_status_t;

/**
 * @brief TWAI node's statistics/record type
 *
 * This structure contains some statistics regarding a node's communication on the TWAI bus
 */
typedef struct {
    uint32_t bus_err_num;           /**< Cumulative number (since `twai_node_enable()`) of bus errors */
} twai_node_record_t;

/**
 * @brief TWAI "TX done" event data
 */
typedef struct {
    bool is_tx_success;                 /**< Indicate if frame send successful, refer `on_error` callback for fail reason if send failed */
    const twai_frame_t *done_tx_frame;  /**< Pointer to the frame that has been transmitted */
} twai_tx_done_event_data_t;

/**
 * @brief TWAI "RX done" event data
 */
typedef struct {
} twai_rx_done_event_data_t;

/**
 * @brief TWAI "state change" event data
 */
typedef struct {
    twai_error_state_t old_sta;     /**< Previous error state */
    twai_error_state_t new_sta;     /**< New error state after the change */
} twai_state_change_event_data_t;

/**
 * @brief TWAI "error" event data
 */
typedef struct {
    twai_error_flags_t err_flags;   /**< Error flags indicating the type of the error */
} twai_error_event_data_t;

/**
 * @brief Group of supported TWAI callbacks
 *
 * @note All of these callbacks is invoked from ISR context. Thus, the implementation of the callback function must
 * adhere to the ISR restrictions such as not calling any blocking APIs.
 *
 * @note Set the particular event callback's entry to NULL to unregister it if not required.
 * @note When TWAI_ISR_CACHE_SAFE is enabled, the callbacks must be placed in IRAM.
 */
typedef struct {
    /**
     * @brief TWAI "TX done" event callback prototype
     *
     * @param handle TWAI node handle
     * @param edata "TX done" event data (passed by the driver)
     * @param user_ctx User data, passed from `twai_node_register_event_callbacks()`
     * @return Whether a higher priority task has been unblocked by this function
     */
    bool (*on_tx_done)(twai_node_handle_t handle, const twai_tx_done_event_data_t *edata, void *user_ctx);

    /**
     * @brief TWAI "RX done" event callback prototype
     *
     * @param handle TWAI node handle
     * @param edata "RX done" event data (passed by the driver)
     * @param user_ctx User data, passed from `twai_node_register_event_callbacks()`
     * @return Whether a higher priority task has been unblocked by this function
     */
    bool (*on_rx_done)(twai_node_handle_t handle, const twai_rx_done_event_data_t *edata, void *user_ctx);

    /**
     * @brief TWAI "state change" event callback prototype
     *
     * @param handle TWAI node handle
     * @param edata "state change" event data (passed by the driver)
     * @param user_ctx User data, passed from `twai_node_register_event_callbacks()`
     * @return Whether a higher priority task has been unblocked by this function
     */
    bool (*on_state_change)(twai_node_handle_t handle, const twai_state_change_event_data_t *edata, void *user_ctx);

    /**
     * @brief TWAI "error" event callback prototype
     *
     * @param[in] handle TWAI node handle
     * @param[in] edata "error" event data (passed by the driver)
     * @param[in] user_ctx User data, passed from `twai_node_register_event_callbacks()`
     * @return Whether a higher priority task has been unblocked by this function
     */
    bool (*on_error)(twai_node_handle_t handle, const twai_error_event_data_t *edata, void *user_ctx);
} twai_event_callbacks_t;

#ifdef __cplusplus
}
#endif
