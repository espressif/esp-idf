/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "soc/soc_caps.h"
#include "soc/gdma_channel.h"
#include "soc/io_mux_reg.h"
#include "hal/parlio_types.h"
#include "hal/parlio_hal.h"
#include "hal/parlio_ll.h"
#include "hal/dma_types.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "rom/cache.h"
#include "esp_heap_caps.h"
#include "driver/parlio_types.h"
#include "esp_cache.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_gpio_reserve.h"
#include "esp_private/gpio.h"
#include "esp_private/sleep_retention.h"

#if CONFIG_PARLIO_ISR_IRAM_SAFE
#define PARLIO_MEM_ALLOC_CAPS    (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define PARLIO_MEM_ALLOC_CAPS    MALLOC_CAP_DEFAULT
#endif
#define PARLIO_DMA_MEM_ALLOC_CAPS    (MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA)

#if SOC_PARLIO_TX_RX_SHARE_INTERRUPT
#define PARLIO_INTR_ALLOC_FLAG_SHARED ESP_INTR_FLAG_SHARED
#else
#define PARLIO_INTR_ALLOC_FLAG_SHARED 0
#endif

#if CONFIG_PARLIO_ISR_IRAM_SAFE
#define PARLIO_INTR_ALLOC_FLAG   (ESP_INTR_FLAG_LOWMED | PARLIO_INTR_ALLOC_FLAG_SHARED | ESP_INTR_FLAG_IRAM)
#else
#define PARLIO_INTR_ALLOC_FLAG   (ESP_INTR_FLAG_LOWMED | PARLIO_INTR_ALLOC_FLAG_SHARED)
#endif

// Use retention link only when the target supports sleep retention is enabled
#define PARLIO_USE_RETENTION_LINK  (SOC_PARLIO_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP)

#if defined(SOC_GDMA_TRIG_PERIPH_PARLIO0_BUS) // Parlio uses GDMA
#if defined(SOC_GDMA_BUS_AHB) && (SOC_GDMA_TRIG_PERIPH_PARLIO0_BUS == SOC_GDMA_BUS_AHB)
typedef dma_descriptor_align4_t     parlio_dma_desc_t;
#define PARLIO_DMA_DESC_ALIGNMENT   4
#define PARLIO_GDMA_NEW_CHANNEL     gdma_new_ahb_channel
#elif defined(SOC_GDMA_BUS_AXI) && (SOC_GDMA_TRIG_PERIPH_PARLIO0_BUS == SOC_GDMA_BUS_AXI)
typedef dma_descriptor_align8_t     parlio_dma_desc_t;
#define PARLIO_DMA_DESC_ALIGNMENT   8
#define PARLIO_GDMA_NEW_CHANNEL     gdma_new_axi_channel
#endif
#endif // defined(SOC_GDMA_TRIG_PERIPH_PARLIO0_BUS)

#define ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#define PARLIO_MAX_ALIGNED_DMA_BUF_SIZE     DMA_DESCRIPTOR_BUFFER_MAX_SIZE_64B_ALIGNED
#else
#define PARLIO_MAX_ALIGNED_DMA_BUF_SIZE     DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED
#endif

#ifdef CACHE_LL_L2MEM_NON_CACHE_ADDR
/* The descriptor address can be mapped by a fixed offset */
#define PARLIO_GET_NON_CACHED_DESC_ADDR(desc)   (desc ? (parlio_dma_desc_t *)(CACHE_LL_L2MEM_NON_CACHE_ADDR(desc)) : NULL)
#else
#define PARLIO_GET_NON_CACHED_DESC_ADDR(desc)   (desc)
#endif // CACHE_LL_L2MEM_NON_CACHE_ADDR

#if SOC_PERIPH_CLK_CTRL_SHARED
#define PARLIO_CLOCK_SRC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define PARLIO_CLOCK_SRC_ATOMIC()
#endif

#if !SOC_RCC_IS_INDEPENDENT
// Reset and Clock Control registers are mixing with other peripherals, so we need to use a critical section
#define PARLIO_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define PARLIO_RCC_ATOMIC()
#endif  // SOC_RCC_IS_INDEPENDENT

#define PARLIO_PM_LOCK_NAME_LEN_MAX 16

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
    PARLIO_TX_FSM_INIT_WAIT,
    PARLIO_TX_FSM_INIT,
    PARLIO_TX_FSM_ENABLE_WAIT,
    PARLIO_TX_FSM_ENABLE,
    PARLIO_TX_FSM_RUN_WAIT,
    PARLIO_TX_FSM_RUN,
} parlio_tx_fsm_t;

typedef struct parlio_unit_t *parlio_unit_base_handle_t;

typedef struct parlio_group_t {
    int                       group_id;     // group ID, index from 0
    portMUX_TYPE              spinlock;     // to protect per-group register level concurrent access
    parlio_hal_context_t      hal;          // hal layer context
    uint32_t                  dma_align;    // DMA buffer alignment
    parlio_unit_base_handle_t    tx_units[SOC_PARLIO_TX_UNITS_PER_GROUP]; // tx unit handles
    parlio_unit_base_handle_t    rx_units[SOC_PARLIO_RX_UNITS_PER_GROUP]; // rx unit handles
} parlio_group_t;

/**
 * @brief The common field of rx and tx unit structure
 */
struct parlio_unit_t {
    int             unit_id;  // unit id
    parlio_dir_t    dir;      // direction
    parlio_group_t  *group;   // group handle
};

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
