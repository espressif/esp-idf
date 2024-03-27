/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"

/**
 * @brief Indicate to ::emac_esp_dma_receive_frame that receive frame buffer was allocated by ::emac_esp_dma_alloc_recv_buf
 *
 */
#define EMAC_HAL_BUF_SIZE_AUTO 0

typedef struct emac_esp_dma_t *emac_esp_dma_handle_t;

typedef void *emac_esp_dma_config_t;

void emac_esp_dma_reset_desc_chain(emac_esp_dma_handle_t emac_esp_dma);
uint32_t emac_esp_dma_transmit_frame(emac_esp_dma_handle_t emac_esp_dma, uint8_t *buf, uint32_t length);
uint32_t emac_esp_dma_transmit_multiple_buf_frame(emac_esp_dma_handle_t emac_esp_dma, uint8_t **buffs, uint32_t *lengths, uint32_t buffs_cnt);
uint8_t *emac_esp_dma_alloc_recv_buf(emac_esp_dma_handle_t emac_esp_dma, uint32_t *size);
uint32_t emac_esp_dma_receive_frame(emac_esp_dma_handle_t emac_esp_dma, uint8_t *buf, uint32_t size, uint32_t *frames_remain, uint32_t *free_desc);
void emac_esp_dma_flush_recv_frame(emac_esp_dma_handle_t emac_esp_dma, uint32_t *frames_remain, uint32_t *free_desc);

esp_err_t emac_esp_new_dma(const emac_esp_dma_config_t* config, emac_esp_dma_handle_t *ret_handle);
esp_err_t emac_esp_del_dma(emac_esp_dma_handle_t emac_esp_dma);

#ifdef __cplusplus
}
#endif
