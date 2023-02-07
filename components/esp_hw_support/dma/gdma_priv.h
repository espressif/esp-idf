/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "soc/soc_caps.h"
#include "hal/gdma_hal.h"
#include "hal/gdma_ll.h"
#include "soc/gdma_periph.h"
#include "esp_private/gdma.h"

#if CONFIG_GDMA_ISR_IRAM_SAFE || CONFIG_GDMA_CTRL_FUNC_IN_IRAM
#define GDMA_MEM_ALLOC_CAPS    (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define GDMA_MEM_ALLOC_CAPS    MALLOC_CAP_DEFAULT
#endif

#if CONFIG_GDMA_ISR_IRAM_SAFE
#define GDMA_INTR_ALLOC_FLAGS  (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_INTRDISABLED)
#else
#define GDMA_INTR_ALLOC_FLAGS  ESP_INTR_FLAG_INTRDISABLED
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gdma_pair_t gdma_pair_t;
typedef struct gdma_channel_t gdma_channel_t;
typedef struct gdma_tx_channel_t gdma_tx_channel_t;
typedef struct gdma_rx_channel_t gdma_rx_channel_t;

typedef struct gdma_group_t {
    int group_id;           // Group ID, index from 0
    gdma_hal_context_t hal; // HAL instance is at group level
    portMUX_TYPE spinlock;  // group level spinlock
    uint32_t tx_periph_in_use_mask; // each bit indicates which peripheral (TX direction) has been occupied
    uint32_t rx_periph_in_use_mask; // each bit indicates which peripheral (RX direction) has been occupied
    gdma_pair_t *pairs[SOC_GDMA_PAIRS_PER_GROUP];  // handles of GDMA pairs
    int pair_ref_counts[SOC_GDMA_PAIRS_PER_GROUP]; // reference count used to protect pair install/uninstall
} gdma_group_t;

struct gdma_pair_t {
    gdma_group_t *group;        // which group the pair belongs to
    int pair_id;                // Pair ID, index from 0
    gdma_tx_channel_t *tx_chan; // pointer of tx channel in the pair
    gdma_rx_channel_t *rx_chan; // pointer of rx channel in the pair
    int occupy_code;            // each bit indicates which channel has been occupied (an occupied channel will be skipped during channel search)
    portMUX_TYPE spinlock;      // pair level spinlock
};

struct gdma_channel_t {
    gdma_pair_t *pair;  // which pair the channel belongs to
    intr_handle_t intr; // per-channel interrupt handle
    portMUX_TYPE spinlock;  // channel level spinlock
    gdma_channel_direction_t direction; // channel direction
    int periph_id; // Peripheral instance ID, indicates which peripheral is connected to this GDMA channel
    size_t sram_alignment;  // alignment for memory in SRAM
    size_t psram_alignment; // alignment for memory in PSRAM
    esp_err_t (*del)(gdma_channel_t *channel); // channel deletion function, it's polymorphic, see `gdma_del_tx_channel` or `gdma_del_rx_channel`
};

struct gdma_tx_channel_t {
    gdma_channel_t base; // GDMA channel, base class
    void *user_data;     // user registered DMA event data
    gdma_event_callback_t on_trans_eof; // TX EOF callback
};

struct gdma_rx_channel_t {
    gdma_channel_t base; // GDMA channel, base class
    void *user_data;     // user registered DMA event data
    gdma_event_callback_t on_recv_eof; // RX EOF callback
};

#ifdef __cplusplus
}
#endif
