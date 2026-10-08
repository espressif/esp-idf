/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BLE_LOG_PRPH_SPI_MASTER_HD_H__
#define __BLE_LOG_PRPH_SPI_MASTER_HD_H__

/* -------------------------------------------------- */
/* BLE Log - Peripheral Interface - SPI Master HD DMA */
/* -------------------------------------------------- */

/* INCLUDE */
#include "ble_log_prph.h"

#include <string.h>
#include "driver/spi_master.h"

/* TYPEDEF */
typedef struct {
    spi_transaction_t data;
    spi_transaction_t end;
    ble_log_prph_trans_t *trans;
    volatile uint8_t status;
} ble_log_prph_trans_ctx_t;

#endif /* __BLE_LOG_PRPH_SPI_MASTER_HD_H__ */
