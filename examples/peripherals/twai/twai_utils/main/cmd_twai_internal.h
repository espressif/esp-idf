/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include <stdatomic.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_twai.h"
#include "esp_twai_onchip.h"

/** @brief Frame buffer size based on TWAI-FD configuration */
#if CONFIG_EXAMPLE_ENABLE_TWAI_FD
#define TWAI_FRAME_BUFFER_SIZE      TWAIFD_FRAME_MAX_LEN
#else
#define TWAI_FRAME_BUFFER_SIZE      TWAI_FRAME_MAX_LEN
#endif

/**
 * @brief Time stamp mode for candump-style output
 */
typedef enum {
    TIMESTAMP_MODE_ABSOLUTE = 'a',    /**< Absolute time (default) */
    TIMESTAMP_MODE_DELTA = 'd',       /**< Delta time between frames */
    TIMESTAMP_MODE_ZERO = 'z',        /**< Relative time from start */
    TIMESTAMP_MODE_NONE = 'n'         /**< No timestamp */
} timestamp_mode_t;

/**
 * @brief Core TWAI driver context
 */
typedef struct {
    twai_onchip_node_config_t driver_config;     /**< Cached driver configuration */
    twai_event_callbacks_t driver_cbs;           /**< Driver event callbacks */
    atomic_bool is_initialized;                  /**< Initialization flag */
} twai_core_ctx_t;

/**
 * @brief Context structure for the TWAI send command
 */
typedef struct {
    SemaphoreHandle_t tx_done_sem;     /**< Semaphore for TX completion signaling */
    atomic_bool is_tx_pending;         /**< Flag to indicate if TX is in progress */
    twai_frame_t tx_frame;             /**< TX frame structure */
    uint8_t tx_frame_buffer[TWAI_FRAME_BUFFER_SIZE]; /**< TX frame buffer */
} twai_send_ctx_t;

/**
 * @brief TWAI dump module context
 */
typedef struct {
    atomic_bool is_running;            /**< Dump running flag */
    twai_mask_filter_config_t mask_filter_configs[SOC_TWAI_MASK_FILTER_NUM]; /**< Mask filter configurations */
#if SOC_TWAI_RANGE_FILTER_NUM
    twai_range_filter_config_t range_filter_configs[SOC_TWAI_RANGE_FILTER_NUM]; /**< Range filter configurations */
#endif
    QueueHandle_t rx_queue;            /**< RX frame queue */
    TaskHandle_t dump_task_handle;     /**< Handle for dump task */
    timestamp_mode_t timestamp_mode;   /**< Time stamp mode */
    int64_t start_time_us;            /**< Start time in microseconds */
    int64_t last_frame_time_us;       /**< Last frame timestamp for delta */
} twai_dump_ctx_t;

/**
 * @brief Core state machine for the TWAI console
 *
 * This structure manages core driver resources, synchronization primitives,
 * and resources for different functional modules (send, dump, player).
 * It embeds twai_utils_status_t to handle bus status and statistics.
 */
typedef struct {
    /** @brief Core Driver Resources */
    twai_core_ctx_t core_ctx;         /**< Core driver context */
    twai_node_handle_t node_handle;   /**< TWAI node handle */
    /** @brief Module Contexts */
    twai_send_ctx_t send_ctx;         /**< Send context for this controller */
    twai_dump_ctx_t dump_ctx;         /**< Dump module context */
} twai_controller_ctx_t;

/** @brief Global controller context array */
extern twai_controller_ctx_t g_twai_controller_ctx[SOC_TWAI_CONTROLLER_NUM];

/**
 * @brief Get controller by ID
 *
 * @param[in] controller_id Controller ID
 *
 * @return Pointer to controller context, or NULL if invalid
 */
twai_controller_ctx_t* get_controller_by_id(int controller_id);

/**
 * @brief Register TWAI core commands with console
 */
void register_twai_core_commands(void);

/**
 * @brief Register TWAI send commands with console
 */
void register_twai_send_commands(void);

/**
 * @brief Register TWAI dump commands with console
 */
void register_twai_dump_commands(void);

/**
 * @brief Unregister TWAI core commands and cleanup resources
 */
void unregister_twai_core_commands(void);

/**
 * @brief Unregister TWAI send commands and cleanup resources
 */
void unregister_twai_send_commands(void);

/**
 * @brief Unregister TWAI dump commands and cleanup resources
 */
void unregister_twai_dump_commands(void);

/**
 * @brief Stop dump and wait for task to exit naturally
 *
 * @param[in] controller_id Controller ID to stop dump for
 *
 * @return @c ESP_OK on success, error code on failure
 */
esp_err_t twai_dump_stop_internal(int controller_id);

#ifdef __cplusplus
}
#endif
