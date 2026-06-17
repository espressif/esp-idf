/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdatomic.h>
#include <sys/queue.h>
#include "esp_dma_utils.h"
#include "esp_private/gdma.h"
#include "esp_private/gdma_link.h"
#include "esp_private/periph_ctrl.h"
#include "esp_pm.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct uhci_controller_t uhci_controller_t;

#define UHCI_ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))
#define UHCI_MAX(a, b) (((a)>(b))?(a):(b))

#define UHCI_PM_LOCK_NAME_LEN_MAX              16

#if CONFIG_UHCI_ISR_HANDLER_IN_IRAM
#define UHCI_MEM_ALLOC_CAPS     (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define UHCI_MEM_ALLOC_CAPS     (MALLOC_CAP_DEFAULT)
#endif

typedef struct {
    void *buffer;               // buffer for saving the received symbols
    size_t buffer_size;         // size of the buffer, in bytes
} uhci_transaction_desc_t;

typedef enum {
    UHCI_TX_FSM_ENABLE_WAIT, /**< FSM is waiting to enable the UHCI system. */
    UHCI_TX_FSM_ENABLE,      /**< FSM is enabling the UHCI system. */
    UHCI_TX_FSM_RUN_WAIT,    /**< FSM is waiting to transition to the running state. */
    UHCI_TX_FSM_RUN,         /**< FSM is in the running state, actively handling UHCI operations. */
} uhci_tx_fsm_t;

typedef enum {
    UHCI_TRANS_QUEUE_READY,      /**< The transaction queue is ready to accept new transactions. */
    UHCI_TRANS_QUEUE_PROGRESS,   /**< A transaction is currently in progress. */
    UHCI_TRANS_QUEUE_COMPLETE,   /**< All transactions in the queue are completed. */
    UHCI_TRANS_QUEUE_MAX,        /**< Placeholder for the maximum number of states (not a valid state). */
} uhci_trans_queue_enum_t;

typedef enum {
    UHCI_RX_FSM_ENABLE_WAIT, /**< FSM is waiting to enable the UHCI system. */
    UHCI_RX_FSM_ENABLE,      /**< FSM is enabling the UHCI system. */
    UHCI_RX_FSM_RUN_WAIT,    /**< FSM is waiting to transition to the running state. */
    UHCI_RX_FSM_RUN,         /**< FSM is in the running state, actively handling UHCI operations. */
} uhci_rx_fsm_t;

typedef struct {
    gdma_channel_handle_t dma_chan;                     // GDMA channel
    uhci_tx_done_callback_t on_tx_trans_done;           // tx transaction done callback function
    gdma_link_list_handle_t dma_link;                   // GDMA link handle
    uhci_transaction_desc_t *trans_desc_pool;           // transaction descriptor pool
    uhci_transaction_desc_t *cur_trans;                 // pointer to current transaction
    QueueHandle_t trans_queues[UHCI_TRANS_QUEUE_MAX];   // transaction queue
    _Atomic uhci_tx_fsm_t tx_fsm;                       // channel life cycle specific FSM
    size_t int_mem_align;                               // Alignment for internal memory
    size_t ext_mem_align;                               // Alignment for external memory
    atomic_int num_trans_inflight;                      // Indicates the number of transactions that are undergoing but not recycled to ready_queue
} uhci_tx_dir;

typedef struct {
    gdma_channel_handle_t dma_chan;                     // GDMA channel
    uhci_rx_event_callback_t on_rx_trans_event;         // rx transaction done callback function
    gdma_link_list_handle_t dma_link;                   // GDMA link handle
    size_t *buffer_size_per_desc_node;                  // Pointer to buffer size per dma descriptor node
    size_t node_index;                                  // node index in receive
    uint8_t **buffer_pointers;                          // Pointer for saving buffer pointer
    _Atomic uhci_rx_fsm_t rx_fsm;                       // channel life cycle specific FSM
    size_t cache_line;                                  // cache line size need to be aligned up.
    size_t int_mem_align;                               // Alignment for internal memory
    size_t ext_mem_align;                               // Alignment for external memory
    size_t rx_num_dma_nodes;                            // rx dma number nodes
} uhci_rx_dir;

struct uhci_controller_t {
    int uhci_num;                                       // UHCI port number
    uhci_hal_context_t hal;                             // uhci hal layer context
    uhci_tx_dir tx_dir;                                 // tx direction structure
    uhci_rx_dir rx_dir;                                 // rx direction structure
    void *user_data;                                    // user data
    size_t int_mem_cache_line_size;                     // internal memory cache line size
    size_t ext_mem_cache_line_size;                     // external memory cache line size
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;                       // power management lock
    char pm_lock_name[UHCI_PM_LOCK_NAME_LEN_MAX];       // pm lock name
#endif
};

#ifdef __cplusplus
}
#endif
