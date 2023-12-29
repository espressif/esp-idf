/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parallel IO sample edge
 */
typedef enum {
    PARLIO_SAMPLE_EDGE_NEG, /*!< Sample data on falling edge of clock */
    PARLIO_SAMPLE_EDGE_POS, /*!< Sample data on rising edge of clock */
} parlio_sample_edge_t;

/**
 * @brief Parallel IO bit packing order
 *
 * Data in memory:
 * Byte 0: MSB < B0.7 B0.6 B0.5 B0.4 B0.3 B0.2 B0.1 B0.0 > LSB
 * Byte 1: MSB < B1.7 B1.6 B1.5 B1.4 B1.3 B1.2 B1.1 B1.0 > LSB
 *
 * Output on line (PARLIO_BIT_PACK_ORDER_LSB):
 *          Cycle 0   Cycle 1   Cycle 2  ---> time
 * GPIO 0:   B0.0      B0.4      B1.0
 * GPIO 1:   B0.1      B0.5      B1.1
 * GPIO 2:   B0.2      B0.6      B1.2
 * GPIO 3:   B0.3      B0.7      B1.3
 *
 * Output on line (PARLIO_BIT_PACK_ORDER_MSB):
 *          Cycle 0   Cycle 1   Cycle 2  ---> time
 * GPIO 0:   B0.4      B0.0      B1.4
 * GPIO 1:   B0.5      B0.1      B1.5
 * GPIO 2:   B0.6      B0.2      B1.6
 * GPIO 3:   B0.7      B0.3      B1.7
 */
typedef enum {
    PARLIO_BIT_PACK_ORDER_LSB, /*!< Bit pack order: LSB */
    PARLIO_BIT_PACK_ORDER_MSB, /*!< Bit pack order: MSB */
} parlio_bit_pack_order_t;

#if SOC_PARLIO_SUPPORTED
/**
 * @brief Parallel IO clock source
 * @note User should select the clock source based on the power and resolution requirement
 */
typedef soc_periph_parlio_clk_src_t parlio_clock_source_t;

/// Maximum data width of TX unit
#define PARLIO_TX_UNIT_MAX_DATA_WIDTH SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH
/// Maximum data width of RX unit
#define PARLIO_RX_UNIT_MAX_DATA_WIDTH SOC_PARLIO_RX_UNIT_MAX_DATA_WIDTH
#else
typedef int parlio_clock_source_t;
#define PARLIO_TX_UNIT_MAX_DATA_WIDTH 0
#define PARLIO_RX_UNIT_MAX_DATA_WIDTH 0
#endif // SOC_PARLIO_SUPPORTED

#ifdef __cplusplus
}
#endif
