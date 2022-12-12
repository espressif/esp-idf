/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/periph_defs.h"

//include soc related (generated) definitions
#include "soc/soc_caps.h"
#include "soc/soc_pins.h"
#include "soc/spi_reg.h"
#include "soc/spi_struct.h"
#include "soc/spi_pins.h"
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
    const periph_module_t module;   //peripheral module, for enabling clock etc
    const int func;             //function number for IOMUX
    spi_dev_t *hw;              //Pointer to the hardware registers
} spi_signal_conn_t;

extern const spi_signal_conn_t spi_periph_signal[SOC_SPI_PERIPH_NUM];

#ifdef __cplusplus
}
#endif
