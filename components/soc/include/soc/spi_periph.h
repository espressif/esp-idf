// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _SOC_SPI_PERIPH_H_
#define _SOC_SPI_PERIPH_H_

#include <stdint.h>
#include "soc/soc.h"
#include "soc/periph_defs.h"
//include soc related (generated) definitions
#include "soc/spi_pins.h"
#include "soc/spi_reg.h"
#include "soc/spi_struct.h"
#include "soc/gpio_sig_map.h"

#ifdef __cplusplus
extern "C"
{
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
    const uint8_t spid_in;          //GPIO mux input signals
    const uint8_t spiq_in;
    const uint8_t spiwp_in;
    const uint8_t spihd_in;
    const uint8_t spics_out[3];     // /CS GPIO output mux signals
    const uint8_t spics_in;
    const uint8_t spiclk_iomux_pin;    //IO pins of IO_MUX muxed signals
    const uint8_t spid_iomux_pin;
    const uint8_t spiq_iomux_pin;
    const uint8_t spiwp_iomux_pin;
    const uint8_t spihd_iomux_pin;
    const uint8_t spics0_iomux_pin;
    const uint8_t irq;              //irq source for interrupt mux
    const uint8_t irq_dma;          //dma irq source for interrupt mux
    const periph_module_t module;   //peripheral module, for enabling clock etc
    spi_dev_t *hw;              //Pointer to the hardware registers
} spi_signal_conn_t;

extern const spi_signal_conn_t spi_periph_signal[3];

#ifdef __cplusplus
}
#endif

#endif /* _SOC_SPI_PERIPH_H_ */