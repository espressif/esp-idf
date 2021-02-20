// Copyright 2010-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "soc/lldesc.h"
#include "soc/spi_periph.h"
#include "hal/spi_types.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C"
{
#endif

//Maximum amount of bytes that can be put in one DMA descriptor
#define SPI_MAX_DMA_LEN (4096-4)

/**
 * Transform unsigned integer of length <= 32 bits to the format which can be
 * sent by the SPI driver directly.
 *
 * E.g. to send 9 bits of data, you can:
 *
 *      uint16_t data = SPI_SWAP_DATA_TX(0x145, 9);
 *
 * Then points tx_buffer to ``&data``.
 *
 * @param DATA Data to be sent, can be uint8_t, uint16_t or uint32_t.
 * @param LEN Length of data to be sent, since the SPI peripheral sends from
 *      the MSB, this helps to shift the data to the MSB.
 */
#define SPI_SWAP_DATA_TX(DATA, LEN) __builtin_bswap32((uint32_t)(DATA)<<(32-(LEN)))

/**
 * Transform received data of length <= 32 bits to the format of an unsigned integer.
 *
 * E.g. to transform the data of 15 bits placed in a 4-byte array to integer:
 *
 *      uint16_t data = SPI_SWAP_DATA_RX(*(uint32_t*)t->rx_data, 15);
 *
 * @param DATA Data to be rearranged, can be uint8_t, uint16_t or uint32_t.
 * @param LEN Length of data received, since the SPI peripheral writes from
 *      the MSB, this helps to shift the data to the LSB.
 */
#define SPI_SWAP_DATA_RX(DATA, LEN) (__builtin_bswap32(DATA)>>(32-(LEN)))

#define SPICOMMON_BUSFLAG_SLAVE         0          ///< Initialize I/O in slave mode
#define SPICOMMON_BUSFLAG_MASTER        (1<<0)     ///< Initialize I/O in master mode
#define SPICOMMON_BUSFLAG_IOMUX_PINS    (1<<1)     ///< Check using iomux pins. Or indicates the pins are configured through the IO mux rather than GPIO matrix.
#define SPICOMMON_BUSFLAG_GPIO_PINS     (1<<2)     ///< Force the signals to be routed through GPIO matrix. Or indicates the pins are routed through the GPIO matrix.
#define SPICOMMON_BUSFLAG_SCLK          (1<<3)     ///< Check existing of SCLK pin. Or indicates CLK line initialized.
#define SPICOMMON_BUSFLAG_MISO          (1<<4)     ///< Check existing of MISO pin. Or indicates MISO line initialized.
#define SPICOMMON_BUSFLAG_MOSI          (1<<5)     ///< Check existing of MOSI pin. Or indicates MOSI line initialized.
#define SPICOMMON_BUSFLAG_DUAL          (1<<6)     ///< Check MOSI and MISO pins can output. Or indicates bus able to work under DIO mode.
#define SPICOMMON_BUSFLAG_WPHD          (1<<7)     ///< Check existing of WP and HD pins. Or indicates WP & HD pins initialized.
#define SPICOMMON_BUSFLAG_QUAD          (SPICOMMON_BUSFLAG_DUAL|SPICOMMON_BUSFLAG_WPHD)     ///< Check existing of MOSI/MISO/WP/HD pins as output. Or indicates bus able to work under QIO mode.

#define SPICOMMON_BUSFLAG_NATIVE_PINS   SPICOMMON_BUSFLAG_IOMUX_PINS

/**
 * @brief SPI DMA channels
 */
typedef enum {
  SPI_DMA_DISABLED = 0,     ///< Do not enable DMA for SPI
#if CONFIG_IDF_TARGET_ESP32
  SPI_DMA_CH1      = 1,     ///< Enable DMA, select DMA Channel 1
  SPI_DMA_CH2      = 2,     ///< Enable DMA, select DMA Channel 2
#endif
  SPI_DMA_CH_AUTO  = 3,     ///< Enable DMA, channel is automatically selected by driver
} spi_common_dma_t;

#if __cplusplus
/* Needed for C++ backwards compatibility with earlier ESP-IDF where this argument is a bare 'int'. Can be removed in ESP-IDF 5 */
typedef int spi_dma_chan_t;
#else
typedef spi_common_dma_t spi_dma_chan_t;
#endif

/**
 * @brief This is a configuration structure for a SPI bus.
 *
 * You can use this structure to specify the GPIO pins of the bus. Normally, the driver will use the
 * GPIO matrix to route the signals. An exception is made when all signals either can be routed through
 * the IO_MUX or are -1. In that case, the IO_MUX is used, allowing for >40MHz speeds.
 *
 * @note Be advised that the slave driver does not use the quadwp/quadhd lines and fields in spi_bus_config_t refering to these lines will be ignored and can thus safely be left uninitialized.
 */
typedef struct {
    int mosi_io_num;                ///< GPIO pin for Master Out Slave In (=spi_d) signal, or -1 if not used.
    int miso_io_num;                ///< GPIO pin for Master In Slave Out (=spi_q) signal, or -1 if not used.
    int sclk_io_num;                ///< GPIO pin for Spi CLocK signal, or -1 if not used.
    int quadwp_io_num;              ///< GPIO pin for WP (Write Protect) signal which is used as D2 in 4-bit communication modes, or -1 if not used.
    int quadhd_io_num;              ///< GPIO pin for HD (HolD) signal which is used as D3 in 4-bit communication modes, or -1 if not used.
    int max_transfer_sz;            ///< Maximum transfer size, in bytes. Defaults to 4094 if 0.
    uint32_t flags;                 ///< Abilities of bus to be checked by the driver. Or-ed value of ``SPICOMMON_BUSFLAG_*`` flags.
    int intr_flags;    /**< Interrupt flag for the bus to set the priority, and IRAM attribute, see
                         *  ``esp_intr_alloc.h``. Note that the EDGE, INTRDISABLED attribute are ignored
                         *  by the driver. Note that if ESP_INTR_FLAG_IRAM is set, ALL the callbacks of
                         *  the driver, and their callee functions, should be put in the IRAM.
                         */
} spi_bus_config_t;


/**
 * @brief Initialize a SPI bus
 *
 * @warning For now, only supports HSPI and VSPI.
 *
 * @param host_id       SPI peripheral that controls this bus
 * @param bus_config    Pointer to a spi_bus_config_t struct specifying how the host should be initialized
 * @param dma_chan      - Selecting a DMA channel for an SPI bus allows transactions on the bus with size only limited by the amount of internal memory.
 *                      - Selecting SPI_DMA_DISABLED limits the size of transactions.
 *                      - Set to SPI_DMA_DISABLED if only the SPI flash uses this bus.
 *                      - Set to SPI_DMA_CH_AUTO to let the driver to allocate the DMA channel.
 *
 * @warning If a DMA channel is selected, any transmit and receive buffer used should be allocated in
 *          DMA-capable memory.
 *
 * @warning The ISR of SPI is always executed on the core which calls this
 *          function. Never starve the ISR on this core or the SPI transactions will not
 *          be handled.
 *
 * @return
 *         - ESP_ERR_INVALID_ARG   if configuration is invalid
 *         - ESP_ERR_INVALID_STATE if host already is in use
 *         - ESP_ERR_NOT_FOUND     if there is no available DMA channel
 *         - ESP_ERR_NO_MEM        if out of memory
 *         - ESP_OK                on success
 */
esp_err_t spi_bus_initialize(spi_host_device_t host_id, const spi_bus_config_t *bus_config, spi_dma_chan_t dma_chan);

/**
 * @brief Free a SPI bus
 *
 * @warning In order for this to succeed, all devices have to be removed first.
 *
 * @param host_id SPI peripheral to free
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_ERR_INVALID_STATE if not all devices on the bus are freed
 *         - ESP_OK                on success
 */
esp_err_t spi_bus_free(spi_host_device_t host_id);

#ifdef __cplusplus
}
#endif
