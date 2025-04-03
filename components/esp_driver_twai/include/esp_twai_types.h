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
    uint32_t bitrate;     /**< Expected TWAI bus baud_rate/bitrate in bits/second */
    uint16_t sp_permill;  /**< Optional, sampling point in permill (1/1000) of the entire bit time */
    uint16_t ssp_permill; /**< Optional, secondary sample point(ssp) in permill (1/1000) of the entire bit time */
} twai_timing_basic_config_t;

/**
 * @brief TWAI transaction frame param type
 */
typedef struct twai_frame_t {
    twai_frame_header_t header;     /**< message attribute/metadata, exclude data buffer*/
    uint8_t *buffer;                /**< buffer address for tx and rx message data*/
    size_t buffer_len;              /**< buffer length of provided data buffer pointer, in bytes.*/
} twai_frame_t;

/**
 * @brief Configuration for TWAI mask filter
 *
 * @note Set both id and mask to `0` to receive ALL frames, both `0xFFFFFFFF` to receive NONE frames
 */
typedef struct {
    uint32_t id;                    /**< Base ID for filtering */
    uint32_t mask;                  /**< Mask to determine the matching bits (1 = match bit, 0 = any bit) */
    struct {
        uint32_t is_ext: 1;         /**< True for extended ID filtering, false for standard ID */
        uint32_t no_classic: 1;     /**< If true, Classic CAN frames are excluded (only CAN FD allowed) */
        uint32_t no_fd: 1;          /**< If true, CAN FD frames are excluded (only Classic CAN allowed) */
    };
} twai_mask_filter_config_t;

/**
 * @brief Range-based filter configuration structure
 */
typedef struct {
    uint32_t range_low;             /**< Lower bound of the filtering range */
    uint32_t range_high;            /**< Upper bound of the filtering range */
    struct {
        uint32_t is_ext: 1;         /**< True for extended ID filtering, false for standard ID */
        uint32_t no_classic: 1;     /**< If true, Classic CAN frames are excluded (only CAN FD allowed) */
        uint32_t no_fd: 1;          /**< If true, CAN FD frames are excluded (only Classic CAN allowed) */
    };
} twai_range_filter_config_t;

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
    twai_error_state_t old_sta;     // Previous error state
    twai_error_state_t new_sta;     // New error state after the change
} twai_state_change_event_data_t;

/**
 * @brief TWAI Error Type Structure
 */
typedef union {
    struct {
        uint32_t arb_lost: 1;       /**< Arbitration lost error (lost arbitration during transmission) */
        uint32_t bit_err: 1;        /**< Bit error detected (dominant/recessive mismatch during arbitration or transmission) */
        uint32_t form_err: 1;       /**< Form error detected (frame fixed-form bit violation) */
        uint32_t stuff_err: 1;      /**< Stuff error detected (e.g. dominant error frame received) */
        uint32_t reserved: 28;      /**< Reserved bits for future use, must be set to zero */
    };
    uint32_t val;                   /**< Integrated error code */
} twai_error_code_t;

/**
 * @brief TWAI "error" event data
 */
typedef struct {
    twai_error_code_t err_code; /**< Error code indicating the type of the error */
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
