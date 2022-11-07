/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <esp_types.h>
#include <esp_err.h>

/** Context used by the ``esp_serial_slave_link`` component.
 */
struct essl_dev_t {
    void *args;

    esp_err_t (*init)(void *ctx, uint32_t wait_ms);

    esp_err_t (*wait_for_ready)(void *ctx, uint32_t wait_ms);
    esp_err_t (*update_tx_buffer_num)(void *ctx, uint32_t wait_ms);
    esp_err_t (*update_rx_data_size)(void *ctx, uint32_t wait_ms);
    esp_err_t (*send_packet)(void *ctx, const void *start, size_t length, uint32_t wait_ms);
    esp_err_t (*get_packet)(void *ctx, void *out_data, size_t size, uint32_t wait_ms);
    esp_err_t (*write_reg)(void *ctx, uint8_t addr, uint8_t value, uint8_t *value_o, uint32_t wait_ms);
    esp_err_t (*read_reg)(void *ctx, uint8_t add, uint8_t *value_o, uint32_t wait_ms);
    esp_err_t (*wait_int)(void *ctx, uint32_t wait_ms);
    esp_err_t (*clear_intr)(void *ctx, uint32_t intr_mask, uint32_t wait_ms);
    esp_err_t (*get_intr)(void *ctx, uint32_t *intr_raw, uint32_t *intr_st, uint32_t wait_ms);
    esp_err_t (*set_intr_ena)(void *ctx, uint32_t ena_mask, uint32_t wait_ms);
    esp_err_t (*get_intr_ena)(void *ctx, uint32_t *ena_mask_o, uint32_t wait_ms);
    esp_err_t (*send_slave_intr)(void *ctx, uint32_t intr_mask, uint32_t wait_ms);

    uint32_t (*get_tx_buffer_num)(void *ctx);
    uint32_t (*get_rx_data_size)(void *ctx);
    void (*reset_cnt)(void *ctx);
};

typedef struct essl_dev_t essl_dev_t;
