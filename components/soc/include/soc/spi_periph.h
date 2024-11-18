/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/spi_pins.h"
#if SOC_PAU_SUPPORTED
#include "soc/regdma.h"
#include "soc/retention_periph_defs.h"
#endif

//include soc related (generated) definitions
#include "soc/interrupts.h"
#include "soc/spi_reg.h"
#include "soc/spi_struct.h"
#include "soc/gpio_sig_map.h"
#if SOC_MEMSPI_IS_INDEPENDENT
#include "soc/spi_mem_struct.h"
#include "soc/spi_mem_reg.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32S2
#define SPI_FREAD_DIO 0
#define SPI_FREAD_QIO 0
#define SPI_FWRITE_DIO 0
#define SPI_FWRITE_QIO 0
#endif

/*
 Stores a bunch of per-spi-peripheral data.
*/
typedef struct {
    const uint8_t spiclk_out;       //GPIO mux output signals
    const uint8_t spiclk_in;
    const uint8_t spid_out;
    const uint8_t spiq_out;
    const uint8_t spiwp_out;
    const uint8_t spihd_out;
#if SOC_SPI_SUPPORT_OCT
    const uint8_t spid4_out;
    const uint8_t spid5_out;
    const uint8_t spid6_out;
    const uint8_t spid7_out;
#endif // SOC_SPI_SUPPORT_OCT
    const uint8_t spid_in;          //GPIO mux input signals
    const uint8_t spiq_in;
    const uint8_t spiwp_in;
    const uint8_t spihd_in;
#if SOC_SPI_SUPPORT_OCT
    const uint8_t spid4_in;
    const uint8_t spid5_in;
    const uint8_t spid6_in;
    const uint8_t spid7_in;
#endif // SOC_SPI_SUPPORT_OCT
    const uint8_t spics_out[SOC_SPI_MAX_CS_NUM];     // /CS GPIO output mux signals
    const uint8_t spics_in;
    const uint8_t spidqs_out;
    const uint8_t spicd_out;
    const uint8_t spiclk_iomux_pin;    //IO pins of IO_MUX muxed signals
    const uint8_t spid_iomux_pin;
    const uint8_t spiq_iomux_pin;
    const uint8_t spiwp_iomux_pin;
    const uint8_t spihd_iomux_pin;
    const uint8_t spics0_iomux_pin;
    const uint8_t irq;              //irq source for interrupt mux
    const uint8_t irq_dma;          //dma irq source for interrupt mux
    const int func;                 //function number for IOMUX
    spi_dev_t *hw;                  //Pointer to the hardware registers
} spi_signal_conn_t;

extern const spi_signal_conn_t spi_periph_signal[SOC_SPI_PERIPH_NUM];

#if SOC_PAU_SUPPORTED
typedef struct {
    const periph_retention_module_t module_id;
    const regdma_entries_config_t *entry_array;
    uint32_t array_size;
} spi_reg_retention_info_t;

extern const spi_reg_retention_info_t spi_reg_retention_info[SOC_SPI_PERIPH_NUM - 1];   // -1 to except mspi
#endif  // SOC_PAU_SUPPORTED

#ifdef __cplusplus
}
#endif
