/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdatomic.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/idf_additions.h"
#include "esp_err.h"
#include "soc/soc_caps.h"
#include "soc/gdma_channel.h"
#include "hal/rmt_types.h"
#include "hal/rmt_hal.h"
#include "hal/dma_types.h"
#include "hal/cache_ll.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "esp_pm.h"
#include "esp_attr.h"
#include "esp_private/gdma.h"
#include "driver/rmt_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_RMT_ISR_IRAM_SAFE || CONFIG_RMT_RECV_FUNC_IN_IRAM
#define RMT_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define RMT_MEM_ALLOC_CAPS      MALLOC_CAP_DEFAULT
#endif

// RMT driver object is per-channel, the interrupt source is shared between channels
#if CONFIG_RMT_ISR_IRAM_SAFE
#define RMT_INTR_ALLOC_FLAG     (ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_IRAM)
#else
#define RMT_INTR_ALLOC_FLAG     (ESP_INTR_FLAG_SHARED)
#endif

// Hopefully the channel offset won't change in other targets
#define RMT_TX_CHANNEL_OFFSET_IN_GROUP 0
#define RMT_RX_CHANNEL_OFFSET_IN_GROUP (SOC_RMT_CHANNELS_PER_GROUP - SOC_RMT_TX_CANDIDATES_PER_GROUP)

#define RMT_ALLOW_INTR_PRIORITY_MASK ESP_INTR_FLAG_LOWMED

// DMA buffer size must align to `rmt_symbol_word_t`
#define RMT_DMA_DESC_BUF_MAX_SIZE      (DMA_DESCRIPTOR_BUFFER_MAX_SIZE & ~(sizeof(rmt_symbol_word_t) - 1))

#define RMT_DMA_NODES_PING_PONG               2  // two nodes ping-pong
#define RMT_PM_LOCK_NAME_LEN_MAX              16
#define RMT_GROUP_INTR_PRIORITY_UNINITIALIZED (-1)

// RMT is a slow peripheral, it only supports AHB-GDMA
#define RMT_DMA_DESC_ALIGN      4
typedef dma_descriptor_align4_t rmt_dma_descriptor_t;

#ifdef CACHE_LL_L2MEM_NON_CACHE_ADDR
#define RMT_GET_NON_CACHE_ADDR(addr) ((addr) ? CACHE_LL_L2MEM_NON_CACHE_ADDR(addr) : 0)
#else
#define RMT_GET_NON_CACHE_ADDR(addr) (addr)
#endif

typedef struct {
    struct {
        rmt_symbol_word_t symbols[SOC_RMT_MEM_WORDS_PER_CHANNEL];
    } channels[SOC_RMT_CHANNELS_PER_GROUP];
} rmt_block_mem_t;

// RMTMEM address is declared in <target>.peripherals.ld
extern rmt_block_mem_t RMTMEM;

typedef enum {
    RMT_CHANNEL_DIRECTION_TX,
    RMT_CHANNEL_DIRECTION_RX,
} rmt_channel_direction_t;

typedef enum {
    RMT_FSM_INIT_WAIT,
    RMT_FSM_INIT,
    RMT_FSM_ENABLE_WAIT,
    RMT_FSM_ENABLE,
    RMT_FSM_RUN_WAIT,
    RMT_FSM_RUN,
} rmt_fsm_t;

enum {
    RMT_TX_QUEUE_READY,
    RMT_TX_QUEUE_PROGRESS,
    RMT_TX_QUEUE_COMPLETE,
    RMT_TX_QUEUE_MAX,
};

typedef struct rmt_group_t rmt_group_t;
typedef struct rmt_channel_t rmt_channel_t;
typedef struct rmt_tx_channel_t rmt_tx_channel_t;
typedef struct rmt_rx_channel_t rmt_rx_channel_t;
typedef struct rmt_sync_manager_t rmt_sync_manager_t;

struct rmt_group_t {
    int group_id;               // group ID, index from 0
    portMUX_TYPE spinlock;      // to protect per-group register level concurrent access
    rmt_hal_context_t hal;      // hal layer for each group
    rmt_clock_source_t clk_src; // record the group clock source, group clock is shared by all channels
    uint32_t resolution_hz;     // resolution of group clock
    uint32_t occupy_mask;       // a set bit in the mask indicates the channel is not available
    rmt_tx_channel_t *tx_channels[SOC_RMT_TX_CANDIDATES_PER_GROUP]; // array of RMT TX channels
    rmt_rx_channel_t *rx_channels[SOC_RMT_RX_CANDIDATES_PER_GROUP]; // array of RMT RX channels
    rmt_sync_manager_t *sync_manager; // sync manager, this can be extended into an array if there're more sync controllers in one RMT group
    int intr_priority;     // RMT interrupt priority
};

struct rmt_channel_t {
    int channel_id;         // channel ID, index from 0
    int gpio_num;           // GPIO number used by RMT RX channel
    uint32_t channel_mask;  // mask of the memory blocks that occupied by the channel
    size_t mem_block_num;   // number of occupied RMT memory blocks
    rmt_group_t *group;     // which group the channel belongs to
    portMUX_TYPE spinlock;  // prevent channel resource accessing by user and interrupt concurrently
    uint32_t resolution_hz; // channel clock resolution
    intr_handle_t intr;     // allocated interrupt handle for each channel
    _Atomic rmt_fsm_t fsm;  // channel life cycle specific FSM
    rmt_channel_direction_t direction; // channel direction
    rmt_symbol_word_t *hw_mem_base;    // base address of RMT channel hardware memory
    gdma_channel_handle_t dma_chan;    // DMA channel
    esp_pm_lock_handle_t pm_lock;      // power management lock
#if CONFIG_PM_ENABLE
    char pm_lock_name[RMT_PM_LOCK_NAME_LEN_MAX]; // pm lock name
#endif
    // RMT channel common interface
    // The following IO functions will have per-implementation for TX and RX channel
    esp_err_t (*del)(rmt_channel_t *channel);
    esp_err_t (*set_carrier_action)(rmt_channel_t *channel, const rmt_carrier_config_t *config);
    esp_err_t (*enable)(rmt_channel_t *channel);
    esp_err_t (*disable)(rmt_channel_t *channel);
};

typedef struct {
    rmt_encoder_handle_t encoder;  // encode user payload into RMT symbols
    const void *payload;           // encoder payload
    size_t payload_bytes;          // payload size
    int loop_count;                // transaction can be continued in a loop for specific times
    int remain_loop_count;         // user required loop count may exceed hardware limitation, the driver will transfer them in batches
    size_t transmitted_symbol_num; // track the number of transmitted symbols
    struct {
        uint32_t eot_level : 1;    // Set the output level for the "End Of Transmission"
        uint32_t encoding_done: 1; // Indicate whether the encoding has finished (not the encoding of transmission)
    } flags;

} rmt_tx_trans_desc_t;

struct rmt_tx_channel_t {
    rmt_channel_t base; // channel base class
    rmt_symbol_word_t *dma_mem_base;    // base address of RMT channel DMA buffer
    rmt_symbol_word_t *dma_mem_base_nc; // base address of RMT channel DMA buffer, accessed in non-cached way
    size_t mem_off;     // runtime argument, indicating the next writing position in the RMT hardware memory
    size_t mem_end;     // runtime argument, indicating the end of current writing region
    size_t ping_pong_symbols;  // ping-pong size (half of the RMT channel memory)
    size_t queue_size;         // size of transaction queue
    size_t num_trans_inflight; // indicates the number of transactions that are undergoing but not recycled to ready_queue
    QueueHandle_t trans_queues[RMT_TX_QUEUE_MAX]; // transaction queues
    rmt_tx_trans_desc_t *cur_trans; // points to current transaction
    void *user_data;                // user context
    rmt_tx_done_callback_t on_trans_done; // callback, invoked on trans done
    rmt_dma_descriptor_t *dma_nodes;    // DMA descriptor nodes
    rmt_dma_descriptor_t *dma_nodes_nc; // DMA descriptor nodes accessed in non-cached way
    rmt_tx_trans_desc_t trans_desc_pool[];   // transfer descriptor pool
};

typedef struct {
    void *buffer;               // buffer for saving the received symbols
    size_t buffer_size;         // size of the buffer, in bytes
    size_t received_symbol_num; // track the number of received symbols
    size_t copy_dest_off;       // tracking offset in the copy destination
    int dma_desc_index;         // tracking the DMA descriptor used by ping-pong
    struct {
        uint32_t en_partial_rx: 1; // packet is too long, we need to notify the user to process the data piece by piece, in a ping-pong approach
    } flags;
} rmt_rx_trans_desc_t;

struct rmt_rx_channel_t {
    rmt_channel_t base;                  // channel base class
    size_t mem_off;                      // starting offset to fetch the symbols in RMT-MEM
    size_t ping_pong_symbols;            // ping-pong size (half of the RMT channel memory)
    rmt_rx_done_callback_t on_recv_done; // callback, invoked on receive done
    void *user_data;                     // user context
    rmt_rx_trans_desc_t trans_desc;      // transaction description
    size_t num_dma_nodes;                // number of DMA nodes, determined by how big the memory block that user configures
    rmt_dma_descriptor_t *dma_nodes;     // DMA link nodes
    rmt_dma_descriptor_t *dma_nodes_nc;  // DMA descriptor nodes accessed in non-cached way
};

/**
 * @brief Acquire RMT group handle
 *
 * @param group_id Group ID
 * @return RMT group handle
 */
rmt_group_t *rmt_acquire_group_handle(int group_id);

/**
 * @brief Release RMT group handle
 *
 * @param group RMT group handle, returned from `rmt_acquire_group_handle`
 */
void rmt_release_group_handle(rmt_group_t *group);

/**
 * @brief Set clock source for RMT peripheral
 *
 * @param chan RMT channel handle
 * @param clk_src Clock source
 * @return
 *      - ESP_OK: Set clock source successfully
 *      - ESP_ERR_NOT_SUPPORTED: Set clock source failed because the clk_src is not supported
 *      - ESP_ERR_INVALID_STATE: Set clock source failed because the clk_src is different from other RMT channel
 *      - ESP_FAIL: Set clock source failed because of other error
 */
esp_err_t rmt_select_periph_clock(rmt_channel_handle_t chan, rmt_clock_source_t clk_src);

/**
 * @brief Set interrupt priority to RMT group
 * @param group RMT group to set interrupt priority to
 * @param intr_priority User-specified interrupt priority (in num, not bitmask)
 * @return If the priority conflicts
 *      - true:  Interrupt priority conflict with previous specified
 *      - false: Interrupt priority set successfully
 */
bool rmt_set_intr_priority_to_group(rmt_group_t *group, int intr_priority);

/**
 * @brief Get isr_flags to be passed to `esp_intr_alloc_intrstatus()` according to `intr_priority` set in RMT group
 * @param group RMT group
 * @return isr_flags
 */
int rmt_get_isr_flags(rmt_group_t *group);

#ifdef __cplusplus
}
#endif
