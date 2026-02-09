/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#if CONFIG_PARLIO_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/idf_additions.h"
#include "soc/soc_caps.h"
#include "hal/parlio_periph.h"
#include "hal/parlio_types.h"
#include "hal/parlio_hal.h"
#include "hal/parlio_ll.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "rom/cache.h"
#include "esp_heap_caps.h"
#include "driver/parlio_types.h"
#include "driver/bitscrambler.h"
#include "esp_cache.h"
#include "esp_clk_tree.h"
#include "esp_pm.h"
#include "esp_memory_utils.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_gpio_reserve.h"
#include "esp_private/gpio.h"
#include "esp_private/sleep_retention.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/gdma.h"
#include "esp_private/gdma_link.h"
#include "esp_private/esp_dma_utils.h"

#if CONFIG_PARLIO_OBJ_CACHE_SAFE
#define PARLIO_MEM_ALLOC_CAPS    (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define PARLIO_MEM_ALLOC_CAPS    MALLOC_CAP_DEFAULT
#endif

#if SOC_PARLIO_TX_RX_SHARE_INTERRUPT
#define PARLIO_INTR_ALLOC_FLAG_SHARED ESP_INTR_FLAG_SHARED
#else
#define PARLIO_INTR_ALLOC_FLAG_SHARED 0
#endif

#if CONFIG_PARLIO_TX_ISR_CACHE_SAFE
#define PARLIO_TX_INTR_ALLOC_FLAG   (ESP_INTR_FLAG_LOWMED | PARLIO_INTR_ALLOC_FLAG_SHARED | ESP_INTR_FLAG_IRAM)
#else
#define PARLIO_TX_INTR_ALLOC_FLAG   (ESP_INTR_FLAG_LOWMED | PARLIO_INTR_ALLOC_FLAG_SHARED)
#endif

// Use retention link only when the target supports sleep retention is enabled
#define PARLIO_USE_RETENTION_LINK  (SOC_PARLIO_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP)

#define PARLIO_DMA_DESCRIPTOR_BUFFER_MAX_SIZE 4095

#if defined(SOC_GDMA_TRIG_PERIPH_PARLIO0_BUS) // Parlio uses GDMA
#if defined(SOC_GDMA_BUS_AHB) && (SOC_GDMA_TRIG_PERIPH_PARLIO0_BUS == SOC_GDMA_BUS_AHB)
#define PARLIO_DMA_DESC_ALIGNMENT   4
#define PARLIO_GDMA_NEW_CHANNEL     gdma_new_ahb_channel
#elif defined(SOC_GDMA_BUS_AXI) && (SOC_GDMA_TRIG_PERIPH_PARLIO0_BUS == SOC_GDMA_BUS_AXI)
#define PARLIO_DMA_DESC_ALIGNMENT   8
#define PARLIO_GDMA_NEW_CHANNEL     gdma_new_axi_channel
#endif
#endif // defined(SOC_GDMA_TRIG_PERIPH_PARLIO0_BUS)

// loop transmission requires ping-pong link to prevent data tearing.
#define PARLIO_DMA_LINK_NUM         2

///!< Logging settings
#define TAG "parlio"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PARLIO_TX_QUEUE_READY,
    PARLIO_TX_QUEUE_PROGRESS,
    PARLIO_TX_QUEUE_COMPLETE,
    PARLIO_TX_QUEUE_MAX,
} parlio_tx_queue_status_t;

typedef enum {
    PARLIO_DIR_TX,
    PARLIO_DIR_RX,
} parlio_dir_t;

typedef enum {
    PARLIO_TX_FSM_INIT,
    PARLIO_TX_FSM_ENABLE,
    PARLIO_TX_FSM_RUN,
    PARLIO_TX_FSM_WAIT,
} parlio_tx_fsm_t;

typedef struct parlio_unit_t *parlio_unit_base_handle_t;

typedef struct parlio_group_t {
    int                       group_id;     // group ID, index from 0
    portMUX_TYPE              spinlock;     // to protect per-group register level concurrent access
    parlio_hal_context_t      hal;          // hal layer context
    uint32_t                  dma_align;    // DMA buffer alignment
    parlio_unit_base_handle_t    tx_units[PARLIO_LL_GET(TX_UNITS_PER_INST)]; // tx unit handles
    parlio_unit_base_handle_t    rx_units[PARLIO_LL_GET(RX_UNITS_PER_INST)]; // rx unit handles
} parlio_group_t;

/**
 * @brief The common field of rx and tx unit structure
 */
struct parlio_unit_t {
    int             unit_id;  // unit id
    parlio_dir_t    dir;      // direction
    parlio_group_t  *group;   // group handle
};

typedef struct {
    uint32_t idle_value; // Parallel IO bus idle value
    const void *payload; // payload to be transmitted
    size_t payload_bits; // payload size in bits
    int dma_link_idx;  // index of DMA link list
    const void *bitscrambler_program; // bitscrambler program binary
    struct {
        uint32_t loop_transmission : 1; // whether the transmission is in loop mode
    } flags;                           // Extra configuration flags
} parlio_tx_trans_desc_t;

// original function pointer type definition

typedef esp_err_t (*parlio_tx_bs_enable_fn_t)(parlio_tx_unit_handle_t tx_unit, parlio_tx_trans_desc_t *t);
typedef esp_err_t (*parlio_tx_bs_disable_fn_t)(parlio_tx_unit_handle_t tx_unit);

typedef struct parlio_tx_unit_t {
    struct parlio_unit_t base; // base unit
    size_t data_width;     // data width
    gpio_num_t data_gpio_nums[SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH]; // data GPIO numbers
    gpio_num_t valid_gpio_num;   // valid signal GPIO number
    gpio_num_t clk_out_gpio_num; // output clock GPIO number
    gpio_num_t clk_in_gpio_num;  // input clock GPIO number
    intr_handle_t intr;    // allocated interrupt handle
    gdma_channel_handle_t dma_chan; // DMA channel
    gdma_link_list_handle_t dma_link[PARLIO_DMA_LINK_NUM]; // DMA link list handle
    size_t int_mem_align; // Alignment for internal memory
    size_t ext_mem_align; // Alignment for external memory
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;   // power management lock
#endif
    portMUX_TYPE spinlock;     // prevent resource accessing by user and interrupt concurrently
    uint32_t out_clk_freq_hz;  // output clock frequency
    parlio_clock_source_t clk_src;  // Parallel IO internal clock source
    size_t max_transfer_bits;  // maximum transfer size in bits
    size_t queue_depth;        // size of transaction queue
    size_t num_trans_inflight; // indicates the number of transactions that are undergoing but not recycled to ready_queue
    QueueHandle_t trans_queues[PARLIO_TX_QUEUE_MAX]; // transaction queues
    parlio_tx_trans_desc_t *cur_trans; // points to current transaction
    uint32_t idle_value_mask;          // mask of idle value
    _Atomic parlio_tx_fsm_t fsm;       // Driver FSM state
    _Atomic bool buffer_need_switch;   // whether the buffer need to be switched
    parlio_tx_done_callback_t on_trans_done; // callback function when the transmission is done
    parlio_tx_buffer_switched_callback_t on_buffer_switched; // callback function when the buffer is switched in loop transmission
    void *user_data;                   // user data passed to the callback function
    bitscrambler_handle_t bs_handle;   // bitscrambler handle
    parlio_tx_bs_enable_fn_t bs_enable_fn;   // bitscrambler enable function
    parlio_tx_bs_disable_fn_t bs_disable_fn; // bitscrambler disable function
    parlio_tx_trans_desc_t trans_desc_pool[];   // transaction descriptor pool
} parlio_tx_unit_t;

/**
 * @brief Register the rx or tx unit to the parlio group
 *
 * @param[in]  unit         The TX/RX unit base handle
 * @return
 *      - ESP_ERR_NO_MEM    No memory for the unit
 *      - ESP_ERR_NOT_FOUND No available unit found on this group
 *      - ESP_OK            Success to register the unit on the group
 */
esp_err_t parlio_register_unit_to_group(parlio_unit_base_handle_t unit);

/**
 * @brief Unregister the rx or tx unit from the parlio group
 *
 * @param[in]  unit        The TX/RX unit base handle
 */
void parlio_unregister_unit_from_group(parlio_unit_base_handle_t unit);

#if PARLIO_USE_RETENTION_LINK
esp_err_t parlio_create_sleep_retention_link_cb(void *arg);
void parlio_create_retention_module(parlio_group_t *group);
#endif

#ifdef __cplusplus
}
#endif
