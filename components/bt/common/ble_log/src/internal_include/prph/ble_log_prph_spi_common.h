/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BLE_LOG_PRPH_SPI_COMMON_H__
#define __BLE_LOG_PRPH_SPI_COMMON_H__

/* ------------------------------------------------------ */
/* BLE Log - Peripheral Interface - SPI Master Common IO  */
/* ------------------------------------------------------ */

/* INCLUDE */
#include "hal/spi_types.h"
#include "soc/spi_pins.h"

/* MACRO */
#define BLE_LOG_SPI_BUS                     SPI2_HOST
#define BLE_LOG_SPI_MAX_TRANSFER_SIZE       (10240)
#define BLE_LOG_SPI_DMA_ALIGN_BYTES         (4U)

/* Fall back to the target's native SPI2 IOMUX pin when the configured
 * GPIO number is negative. */
#define BLE_LOG_SPI_MOSI_IO_NUM              \
    (CONFIG_BLE_LOG_PRPH_SPI_MASTER_DMA_MOSI_IO_NUM < 0 ? \
     SPI2_IOMUX_PIN_NUM_MOSI : CONFIG_BLE_LOG_PRPH_SPI_MASTER_DMA_MOSI_IO_NUM)
#define BLE_LOG_SPI_SCLK_IO_NUM              \
    (CONFIG_BLE_LOG_PRPH_SPI_MASTER_DMA_SCLK_IO_NUM < 0 ? \
     SPI2_IOMUX_PIN_NUM_CLK : CONFIG_BLE_LOG_PRPH_SPI_MASTER_DMA_SCLK_IO_NUM)
#define BLE_LOG_SPI_CS_IO_NUM                \
    (CONFIG_BLE_LOG_PRPH_SPI_MASTER_DMA_CS_IO_NUM < 0 ? \
     SPI2_IOMUX_PIN_NUM_CS : CONFIG_BLE_LOG_PRPH_SPI_MASTER_DMA_CS_IO_NUM)

#endif /* __BLE_LOG_PRPH_SPI_COMMON_H__ */
