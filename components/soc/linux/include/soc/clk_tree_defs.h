/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

/**
 * Mock definitions for running on the host.
 */

/**
 * @brief Supported clock sources for modules (CPU, peripherals, RTC, etc.)
 *
 * @note enum starts from 1, to save 0 for special purpose
 */
typedef enum {
    SOC_MOD_CLK_APB = 1,
} soc_module_clk_t;

/**
 * @brief Type of SPI clock source.
 */
typedef enum {
    SPI_CLK_SRC_DEFAULT = SOC_MOD_CLK_APB,
    SPI_CLK_SRC_APB = SOC_MOD_CLK_APB,
} soc_periph_spi_clk_src_t;
