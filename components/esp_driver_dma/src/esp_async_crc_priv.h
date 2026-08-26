/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "esp_async_crc.h"
#include "esp_private/esp_dma_utils.h"
#include "esp_private/gdma.h"
#include "esp_private/gdma_link.h"

#define DEFAULT_TRANSACTION_QUEUE_LENGTH 8

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CRC_FSM_IDLE, // CRC engine is in idle, ready for new request
    CRC_FSM_RUN,  // CRC engine is processing a request
    CRC_FSM_WAIT, // intermediate state, for state transitions
} async_crc_fsm_t;

typedef struct async_crc_context_t async_crc_context_t;

struct async_crc_context_t {
    /// @brief Start a new async CRC calculation
    esp_err_t (*calc)(async_crc_context_t *ctx, const void *data, size_t size,
                      const async_crc_params_t *params, async_crc_isr_cb_t cb_isr, void *cb_args);
    /// @brief Delete async CRC driver context
    esp_err_t (*del)(async_crc_context_t *ctx);
};

// Template function for different GDMA types
typedef esp_err_t (*gdma_new_channel_t)(const gdma_channel_alloc_config_t *config,
                                        gdma_channel_handle_t *ret_tx_chan, gdma_channel_handle_t *ret_rx_chan);

esp_err_t esp_async_crc_install_gdma_template(const async_crc_config_t *config, async_crc_handle_t *crc_hdl,
                                              gdma_new_channel_t new_channel_func, uint32_t gdma_bus_id);

#ifdef __cplusplus
}
#endif
