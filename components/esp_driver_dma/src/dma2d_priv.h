/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <sys/queue.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "soc/soc_caps.h"
#include "hal/dma2d_hal.h"
#include "hal/dma2d_ll.h"
#include "esp_private/dma2d.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_DMA2D_OPERATION_FUNC_IN_IRAM || CONFIG_DMA2D_ISR_IRAM_SAFE
#define DMA2D_MEM_ALLOC_CAPS    (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define DMA2D_MEM_ALLOC_CAPS    MALLOC_CAP_DEFAULT
#endif

#if CONFIG_DMA2D_ISR_IRAM_SAFE
#define DMA2D_INTR_ALLOC_FLAGS  (ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_IRAM)
#else
#define DMA2D_INTR_ALLOC_FLAGS  ESP_INTR_FLAG_SHARED
#endif

#define DMA2D_RX_DEFAULT_INTR_FLAG  (DMA2D_LL_EVENT_RX_SUC_EOF | DMA2D_LL_EVENT_RX_ERR_EOF | DMA2D_LL_EVENT_RX_DESC_ERROR)

typedef struct dma2d_channel_t dma2d_channel_t;
typedef struct dma2d_tx_channel_t dma2d_tx_channel_t;
typedef struct dma2d_rx_channel_t dma2d_rx_channel_t;
typedef struct dma2d_group_t dma2d_group_t;

struct dma2d_trans_s {
    TAILQ_ENTRY(dma2d_trans_s) entry;     // Link entry
    const dma2d_trans_config_t *desc;     // Pointer to the structure containing all configuration items of a transaction
    dma2d_channel_handle_t rx_chan;       // Pointer to the RX channel handle that will be used to do the transaction
};

struct dma2d_group_t {
    int group_id;                                                   // Group ID, index from 0
    dma2d_hal_context_t hal;                                        // HAL instance is at group level
    portMUX_TYPE spinlock;                                          // Group level spinlock
    TAILQ_HEAD(pending_trans, dma2d_trans_s) pending_trans_tailq;   // Link head of pending 2D-DMA transactions
    uint8_t tx_channel_free_mask;                                   // Bit mask indicating the free TX channels at the moment
    uint8_t rx_channel_free_mask;                                   // Bit mask indicating the free RX channels at the moment
    uint8_t tx_channel_reserved_mask;                               // Bit mask indicating the being reserved TX channels
    uint8_t rx_channel_reserved_mask;                               // Bit mask indicating the being reserved RX channels
    uint32_t tx_periph_m2m_free_id_mask;                            // Bit mask indicating the available TX M2M peripheral selelction IDs at the moment
    uint32_t rx_periph_m2m_free_id_mask;                            // Bit mask indicating the available RX M2M peripheral selelction IDs at the moment
    dma2d_tx_channel_t *tx_chans[DMA2D_LL_GET(TX_CHANS_PER_INST)];  // Handles of 2D-DMA TX channels
    dma2d_rx_channel_t *rx_chans[DMA2D_LL_GET(RX_CHANS_PER_INST)];  // Handles of 2D-DMA RX channels
    int intr_priority;                                              // All channels in the same group should share the same interrupt priority
};

struct dma2d_channel_t {
    dma2d_group_t *group;                   // Which group the channel belongs to
    int channel_id;                         // Channel ID
    dma2d_channel_direction_t direction;    // Channel direction, TX or RX
    intr_handle_t intr;                     // Per-channel interrupt handle
    portMUX_TYPE spinlock;                  // Channel level spinlock
    struct {
        dma2d_trans_t *transaction;             // Pointer to the 2D-DMA transaction context that is currently being processed on the channel
        uint32_t reorder_en : 1;            // This flag indicates the channel will enable reorder functionality
        int periph_sel_id : (DMA2D_LL_CHANNEL_PERIPH_SEL_BIT_WIDTH + 1); // This is used to record the periph_sel_id of each channel
    } status;
};

struct dma2d_tx_channel_t {
    dma2d_channel_t base;                      // 2D-DMA channel base class
    void *user_data;                           // User registered DMA event data
    dma2d_event_callback_t on_desc_done;       // TX DONE event callback
};

struct dma2d_rx_channel_t {
    dma2d_channel_t base;                      // 2D-DMA channel base class
    void *user_data;                           // User registered DMA event data
    dma2d_event_callback_t on_recv_eof;        // RX EOF event callback
    dma2d_event_callback_t on_desc_done;       // RX DONE event callback
    dma2d_event_callback_t on_desc_empty;      // RX desc empty callback, trigger when buffer on dma is not sufficient.
    uint32_t bundled_tx_channel_mask;          // Bit mask indicating the TX channels together with the RX channel to do the transaction
};

#ifdef __cplusplus
}
#endif
