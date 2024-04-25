/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "soc/soc_caps.h"
#include "hal/gdma_types.h"

// TODO: don't expose the SOC header files, we can typedef a new type for the register dev pointer
#if SOC_AHB_GDMA_VERSION == 1
#include "soc/gdma_struct.h"
#endif
#if SOC_AHB_GDMA_VERSION == 2
#include "soc/ahb_dma_struct.h"
#endif
#if SOC_AXI_GDMA_SUPPORTED
#include "soc/axi_dma_struct.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// forward declaration of the HAL context
typedef struct gdma_hal_context_t gdma_hal_context_t;

/**
 * @brief GDMA HAL configuration
 */
typedef struct {
    int group_id;  /*!< GDMA group ID */
} gdma_hal_config_t;

typedef struct {
    uint32_t init_value;     /*!< CRC initial value */
    uint32_t crc_bit_width;  /*!< CRC bit width */
    uint32_t poly_hex;       /*!< Polynomial Formula, in hex */
    bool reverse_data_mask;  /*!< Reverse data mask */
} gdma_hal_crc_config_t;

/**
 * @brief GDMA HAL private data
 */
typedef struct {
    // The bitmap of the IDs that can be used by M2M are different between AXI DMA and AHB DMA, so we need to save a copy for each of them
    uint32_t m2m_free_periph_mask;
    // TODO: we can add more private data here, e.g. the interrupt event mask of interest
    // for now, the AXI DMA and AHB DMA are sharing the same interrupt mask, so we don't need to store it here
    // If one day they become incompatible, we shall save a copy for each of them as a private data
} gdma_hal_priv_data_t;

/**
 * @brief HAL context definition
 */
struct gdma_hal_context_t {
    /// the underlying hardware can be different
    union {
#if SOC_AHB_GDMA_VERSION == 1
        gdma_dev_t *dev;
#endif
#if SOC_AHB_GDMA_VERSION == 2
        ahb_dma_dev_t *ahb_dma_dev;
#endif
#if SOC_AXI_GDMA_SUPPORTED
        axi_dma_dev_t *axi_dma_dev;
#endif
        void *generic_dev;
    };
    gdma_hal_priv_data_t *priv_data; /// private data for the HAL
    void (*start_with_desc)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, intptr_t desc_base_addr); /// start the channel with the start address of the descriptor
    void (*stop)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir); /// stop the channel
    void (*append)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir); /// Append a descriptor to the channel
    void (*reset)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir); /// Reset the channel
    void (*set_priority)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t priority); /// Set the channel priority
    void (*connect_peri)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, gdma_trigger_peripheral_t periph, int periph_sub_id); /// Connect the channel to a peripheral
    void (*disconnect_peri)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir); /// Disconnect the channel from a peripheral
    void (*enable_burst)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_data_burst, bool en_desc_burst); /// Enable burst mode
    void (*set_burst_size)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t burst_sz); /// Set burst transfer size
    void (*set_strategy)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_owner_check, bool en_desc_write_back, bool eof_till_popped); /// Set some misc strategy of the channel behaviour
    uint32_t (*get_intr_status_reg)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir); // Get the interrupt status register address
    void (*enable_intr)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t intr_event_mask, bool en_or_dis); /// Enable the channel interrupt
    void (*clear_intr)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t intr_event_mask); /// Clear the channel interrupt
    uint32_t (*read_intr_status)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool raw); /// Read the channel interrupt status
    uint32_t (*get_eof_desc_addr)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool is_success); /// Get the address of the descriptor with success/error EOF flag set
    void (*enable_access_encrypt_mem)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_or_dis); /// Enable the access to the encrypted memory
#if SOC_GDMA_SUPPORT_CRC
    void (*clear_crc)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir); /// Clear the CRC interim results
    void (*set_crc_poly)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, const gdma_hal_crc_config_t *config); /// Set the CRC polynomial
    uint32_t (*get_crc_result)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir); /// Get the CRC result
#endif // SOC_GDMA_SUPPORT_CRC
#if SOC_GDMA_SUPPORT_ETM
    void (*enable_etm_task)(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_or_dis); /// Enable the ETM task
#endif // SOC_GDMA_SUPPORT_ETM
};

void gdma_hal_deinit(gdma_hal_context_t *hal);

void gdma_hal_start_with_desc(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, intptr_t desc_base_addr);

void gdma_hal_stop(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir);

void gdma_hal_append(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir);

void gdma_hal_reset(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir);

void gdma_hal_set_priority(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t priority);

void gdma_hal_connect_peri(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, gdma_trigger_peripheral_t periph, int periph_sub_id);

void gdma_hal_disconnect_peri(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir);

void gdma_hal_enable_burst(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_data_burst, bool en_desc_burst);

void gdma_hal_set_burst_size(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t burst_sz);

void gdma_hal_set_strategy(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_owner_check, bool en_desc_write_back, bool eof_till_popped);

void gdma_hal_enable_intr(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t intr_event_mask, bool en_or_dis);

void gdma_hal_clear_intr(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t intr_event_mask);

uint32_t gdma_hal_get_intr_status_reg(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir);

uint32_t gdma_hal_read_intr_status(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool raw);

uint32_t gdma_hal_get_eof_desc_addr(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool is_success);

void gdma_hal_enable_access_encrypt_mem(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_or_dis);

#if SOC_GDMA_SUPPORT_CRC
void gdma_hal_build_parallel_crc_matrix(int crc_width, uint32_t crc_poly_hex, int data_width,
                                        uint32_t *lfsr_transform_matrix, uint32_t *data_transform_matrix);

uint32_t gdma_hal_get_data_mask_from_matrix(uint32_t *data_transform_matrix, int data_width, int crc_bit);

uint32_t gdma_hal_get_lfsr_mask_from_matrix(uint32_t *lfsr_transform_matrix, int crc_width, int crc_bit);

void gdma_hal_clear_crc(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir);

void gdma_hal_set_crc_poly(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, const gdma_hal_crc_config_t *config);

uint32_t gdma_hal_get_crc_result(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir);
#endif // SOC_GDMA_SUPPORT_CRC

#if SOC_GDMA_SUPPORT_ETM
void gdma_hal_enable_etm_task(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_or_dis);
#endif

#ifdef __cplusplus
}
#endif
