/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdatomic.h>
#include <sys/queue.h>
// SOC specific headers should be included early
#include "soc/soc_caps.h"
#include "sdkconfig.h"
#if CONFIG_I3C_MASTER_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for i3c master driver
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#endif
#include "esp_log.h"
// FreeRTOS headers must be included in order: FreeRTOS.h first, then others
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "hal/i3c_master_types.h"
#include "hal/i3c_master_hal.h"
#include "esp_dma_utils.h"
#include "esp_private/gdma.h"
#include "esp_private/gdma_link.h"
#include "esp_private/periph_ctrl.h"
#include "esp_intr_types.h"
#include "driver/i3c_master_types.h"
#include "esp_pm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I3C_MASTER_ALLOW_INTR_PRIORITY_MASK ESP_INTR_FLAG_LOWMED

// interface between i3c and dma need 4 bytes aligned.
#define I3C_MASTER_DMA_INTERFACE_ALIGNMENT (4)

#if SOC_PERIPH_CLK_CTRL_SHARED
#define I3C_MASTER_CLOCK_SRC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define I3C_MASTER_CLOCK_SRC_ATOMIC()
#endif

#if !SOC_RCC_IS_INDEPENDENT
#define I3C_MASTER_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define I3C_MASTER_RCC_ATOMIC()
#endif

#if CONFIG_I3C_MASTER_ISR_CACHE_SAFE
#define I3C_MASTER_MEM_ALLOC_CAPS     (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define I3C_MASTER_MEM_ALLOC_CAPS     (MALLOC_CAP_DEFAULT)
#endif

#if CONFIG_I3C_MASTER_ISR_CACHE_SAFE
#define I3C_MASTER_INTR_ALLOC_FLAG     (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_LOWMED)
#else
#define I3C_MASTER_INTR_ALLOC_FLAG     (ESP_INTR_FLAG_LOWMED)
#endif

#define I3C_ALIGN_UP(num, align)         (((num) + ((align) - 1)) & ~((align) - 1))

/**
 * @brief Forward declaration of the I3C master bus structure.
 */
typedef struct i3c_master_bus_t i3c_master_bus_t;

/**
 * @brief Forward declaration of the I3C master I2C device structure.
 */
typedef struct i3c_master_i2c_dev_t i3c_master_i2c_dev_t;

/**
 * @brief I3C master bus port number.
 */
typedef int i3c_master_bus_num_t;

/**
 * @brief Structure representing a transaction descriptor for I3C/I2C operations.
 */
typedef struct {
    i3c_master_ll_device_address_descriptor_t *addr_table; /**< Pointer to the address table, mapping device addresses. */
    size_t addr_table_num;                  /**< Number of entries in the address table. */
    i3c_master_ll_command_descriptor_t *command_table; /**< Pointer to the command table for the transaction. */
    size_t command_table_num;               /**< Number of commands in the command table. */
    uint8_t *write_buffer;                  /**< Pointer to the data buffer for writing. */
    uint8_t *read_buffer;                   /**< Pointer to the data buffer for reading. */
    uint32_t scl_freq_hz;                  /**< Speed of the SCL clock in Hz for the transaction. */
    i3c_master_i2c_device_handle_t dev_handle;  /**< Direct storage of device handle. */
} i3c_i2c_transaction_desc_t;

/**
 * @brief Enumeration representing the states of the I3C transaction queue.
 */
enum {
    I3C_TRANS_QUEUE_READY,      /**< The transaction queue is ready to accept new transactions. */
    I3C_TRANS_QUEUE_PROGRESS,   /**< A transaction is currently in progress. */
    I3C_TRANS_QUEUE_COMPLETE,   /**< All transactions in the queue are completed. */
    I3C_TRANS_QUEUE_MAX,        /**< Placeholder for the maximum number of states (not a valid state). */
};

/**
 * @brief Enumeration representing the states of the I3C finite state machine (FSM).
 */
typedef enum {
    I3C_FSM_WAIT,        /**< FSM is waiting for the I3C system to be ready. */
    I3C_FSM_ENABLE,      /**< FSM is enabling the I3C system. */
    I3C_FSM_RUN,         /**< FSM is in the running state, actively handling I3C operations. */
} i3c_fsm_t;

/**
 * @brief Function pointer type for transaction handler
 */
typedef esp_err_t (*i3c_transaction_handler_t)(i3c_master_bus_t *bus_handle, i3c_i2c_transaction_desc_t *trans_desc);

/**
 * @brief Structure representing the I3C master bus.
 *
 * This structure contains all the resources and states needed for
 * managing an I3C master bus, including hardware abstraction, transaction
 * handling, and synchronization.
 */
struct i3c_master_bus_t {
    i3c_master_bus_num_t i3c_num; /**< I3C bus number. */
    i3c_master_hal_context_t hal; /**< HAL layer context for each port (bus). */
    i3c_master_clock_source_t clock_source; /**< Source of the I3C clock. */
    uint32_t clock_source_freq; /**< Frequency of the clock source in Hz. */
    bool dma_initialized; /**< Flag indicating whether DMA has been initialized via decorator. */
    bool use_dma_transaction; /**< Flag indicating whether DMA is used for transactions. */
    bool async_transaction; /**< Flag indicating whether asynchronous transactions are enabled. */
    QueueHandle_t event_queue; /**< Queue for handling I3C events. */
    SLIST_HEAD(i3c_i2c_device_list_head, i3c_master_i2c_dev_t) device_list; /**< List of I2C devices on the bus. */
    i3c_i2c_transaction_desc_t *cur_trans; /**< Pointer to the current transaction descriptor. */
    i3c_i2c_transaction_desc_t *trans_desc_pool; /**< Pool of pre-allocated transaction descriptors. */
    uint32_t ops_prepare_idx; /**< Index for preparing operations. */
    bool async_memory_allocated; /**< The async transaction is allocated or not */
    i3c_master_ll_device_address_descriptor_t (*i3c_async_addr_table)[SOC_I3C_MASTER_ADDRESS_TABLE_NUM]; /**< Address table for asynchronous transactions. */
    i3c_master_ll_command_descriptor_t (*i3c_async_command_table)[SOC_I3C_MASTER_COMMAND_TABLE_NUM]; /**< Command table for asynchronous transactions. */
    QueueHandle_t trans_queues[I3C_TRANS_QUEUE_MAX]; /**< Array of transaction queues for different states. */
    intr_handle_t intr_handle; /**< Interrupt handle for I3C interrupts. */
    _Atomic i3c_fsm_t fsm; /**< Current state of the I3C finite state machine. */
    uint8_t num_trans_inflight; /**< Number of in-flight transactions. */
    size_t queue_depth; /**< Depth of the transaction queue. */
    SemaphoreHandle_t bus_lock_mux; /**< Semaphore for bus locking. */
    portMUX_TYPE spinlock; /**< Spinlock for protecting critical sections. */
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock; /**< Power management lock handle. */
#endif
    size_t cache_line_size; /**< Cache line size for doing C2M operation */
    uint8_t *write_fifo_buffer_pointer; /**< Pointer to the write FIFO buffer. */
    size_t write_buffer_left_size; /**< Remaining size of the write buffer. */
    uint8_t *read_fifo_buffer_pointer; /**< Pointer to the read FIFO buffer. */
    size_t read_buffer_left_size; /**< Remaining size of the read buffer. */
    gdma_channel_handle_t dma_tx_chan; /**< DMA channel handle for TX. */
    gdma_channel_handle_t dma_rx_chan; /**< DMA channel handle for RX. */
    gdma_link_list_handle_t tx_dma_link; /**< Linked list for TX DMA. */
    gdma_link_list_handle_t rx_dma_link; /**< Linked list for RX DMA. */
    size_t dma_buffer_alignment; /**< Alignment of the DMA buffer. */
    i3c_transaction_handler_t transaction_handler; /**< Function pointer for transaction handling (FIFO or DMA) */
};

/**
 * @brief Structure representing an I2C device managed by the I3C master.
 *
 * This structure contains all the necessary information and configuration
 * required to communicate with an I2C device on an I3C master bus.
 */
struct i3c_master_i2c_dev_t {
    i3c_master_bus_t *bus_handle; /**< Handle to the I3C master bus managing this device. */
    uint8_t address; /**< 7-bit I2C address of the device. */
    uint32_t scl_freq_hz; /**< I2C clock speed for this device, in Hz. */
    i3c_master_i2c_callback_t on_trans_done; /**< Callback function invoked upon transaction completion. */
    void *user_ctx; /**< User-defined context passed to the callback function. */
    SLIST_ENTRY(i3c_master_i2c_dev_t) next; /**< Pointer to the next device in the single-linked list. */
};

#ifdef __cplusplus
}
#endif
