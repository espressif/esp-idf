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
#include "esp32/rom/lldesc.h"
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
 * @param host SPI peripheral that controls this bus
 * @param bus_config Pointer to a spi_bus_config_t struct specifying how the host should be initialized
 * @param dma_chan Either channel 1 or 2, or 0 in the case when no DMA is required. Selecting a DMA channel
 *                 for a SPI bus allows transfers on the bus to have sizes only limited by the amount of
 *                 internal memory. Selecting no DMA channel (by passing the value 0) limits the amount of
 *                 bytes transfered to a maximum of 64. Set to 0 if only the SPI flash uses
 *                 this bus.
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
 *         - ESP_ERR_NO_MEM        if out of memory
 *         - ESP_OK                on success
 */
esp_err_t spi_bus_initialize(spi_host_device_t host, const spi_bus_config_t *bus_config, int dma_chan);

/**
 * @brief Free a SPI bus
 *
 * @warning In order for this to succeed, all devices have to be removed first.
 *
 * @param host SPI peripheral to free
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_ERR_INVALID_STATE if not all devices on the bus are freed
 *         - ESP_OK                on success
 */
esp_err_t spi_bus_free(spi_host_device_t host);


/** @cond */    //Doxygen command to hide deprecated function (or non-public) from API Reference

/**
 * @brief Try to claim a SPI peripheral
 *
 * Call this if your driver wants to manage a SPI peripheral.
 *
 * @param host Peripheral to claim
 * @param source The caller indentification string.
 *
 * @note This public API is deprecated.
 *
 * @return True if peripheral is claimed successfully; false if peripheral already is claimed.
 */
bool spicommon_periph_claim(spi_host_device_t host, const char* source);

/**
 * @brief Check whether the spi periph is in use.
 *
 * @param host Peripheral to check.
 *
 * @note This public API is deprecated.
 *
 * @return True if in use, otherwise false.
 */
bool spicommon_periph_in_use(spi_host_device_t host);

/**
 * @brief Return the SPI peripheral so another driver can claim it.
 *
 * @param host Peripheral to return
 *
 * @note This public API is deprecated.
 *
 * @return True if peripheral is returned successfully; false if peripheral was free to claim already.
 */
bool spicommon_periph_free(spi_host_device_t host);

/**
 * @brief Try to claim a SPI DMA channel
 *
 *  Call this if your driver wants to use SPI with a DMA channnel.
 *
 * @param dma_chan channel to claim
 *
 * @note This public API is deprecated.
 *
 * @return True if success; false otherwise.
 */
bool spicommon_dma_chan_claim(int dma_chan);

/**
 * @brief Check whether the spi DMA channel is in use.
 *
 * @param dma_chan DMA channel to check.
 *
 * @note This public API is deprecated.
 *
 * @return True if in use, otherwise false.
 */
bool spicommon_dma_chan_in_use(int dma_chan);

/**
 * @brief Return the SPI DMA channel so other driver can claim it, or just to power down DMA.
 *
 * @param dma_chan channel to return
 *
 * @note This public API is deprecated.
 *
 * @return True if success; false otherwise.
 */
bool spicommon_dma_chan_free(int dma_chan);

/// @note macros deprecated from public API
#define SPICOMMON_BUSFLAG_SLAVE         0          ///< Initialize I/O in slave mode
#define SPICOMMON_BUSFLAG_MASTER        (1<<0)     ///< Initialize I/O in master mode
#define SPICOMMON_BUSFLAG_IOMUX_PINS    (1<<1)     ///< Check using iomux pins. Or indicates the pins are configured through the IO mux rather than GPIO matrix.
#define SPICOMMON_BUSFLAG_SCLK          (1<<2)     ///< Check existing of SCLK pin. Or indicates CLK line initialized.
#define SPICOMMON_BUSFLAG_MISO          (1<<3)     ///< Check existing of MISO pin. Or indicates MISO line initialized.
#define SPICOMMON_BUSFLAG_MOSI          (1<<4)     ///< Check existing of MOSI pin. Or indicates CLK line initialized.
#define SPICOMMON_BUSFLAG_DUAL          (1<<5)     ///< Check MOSI and MISO pins can output. Or indicates bus able to work under DIO mode.
#define SPICOMMON_BUSFLAG_WPHD          (1<<6)     ///< Check existing of WP and HD pins. Or indicates WP & HD pins initialized.
#define SPICOMMON_BUSFLAG_QUAD          (SPICOMMON_BUSFLAG_DUAL|SPICOMMON_BUSFLAG_WPHD)     ///< Check existing of MOSI/MISO/WP/HD pins as output. Or indicates bus able to work under QIO mode.

#define SPICOMMON_BUSFLAG_NATIVE_PINS   SPICOMMON_BUSFLAG_IOMUX_PINS


/**
 * @brief Connect a SPI peripheral to GPIO pins
 *
 * This routine is used to connect a SPI peripheral to the IO-pads and DMA channel given in
 * the arguments. Depending on the IO-pads requested, the routing is done either using the
 * IO_mux or using the GPIO matrix.
 *
 * @note This public API is deprecated. Please call ``spi_bus_initialize`` for master
 *       bus initialization and ``spi_slave_initialize`` for slave initialization.
 *
 * @param host SPI peripheral to be routed
 * @param bus_config Pointer to a spi_bus_config struct detailing the GPIO pins
 * @param dma_chan DMA-channel (1 or 2) to use, or 0 for no DMA.
 * @param flags Combination of SPICOMMON_BUSFLAG_* flags, set to ensure the pins set are capable with some functions:
 *              - ``SPICOMMON_BUSFLAG_MASTER``: Initialize I/O in master mode
 *              - ``SPICOMMON_BUSFLAG_SLAVE``: Initialize I/O in slave mode
 *              - ``SPICOMMON_BUSFLAG_IOMUX_PINS``: Pins set should match the iomux pins of the controller.
 *              - ``SPICOMMON_BUSFLAG_SCLK``, ``SPICOMMON_BUSFLAG_MISO``, ``SPICOMMON_BUSFLAG_MOSI``:
 *                  Make sure SCLK/MISO/MOSI is/are set to a valid GPIO. Also check output capability according to the mode.
 *              - ``SPICOMMON_BUSFLAG_DUAL``: Make sure both MISO and MOSI are output capable so that DIO mode is capable.
 *              - ``SPICOMMON_BUSFLAG_WPHD`` Make sure WP and HD are set to valid output GPIOs.
 *              - ``SPICOMMON_BUSFLAG_QUAD``: Combination of ``SPICOMMON_BUSFLAG_DUAL`` and ``SPICOMMON_BUSFLAG_WPHD``.
 * @param[out] flags_o A SPICOMMON_BUSFLAG_* flag combination of bus abilities will be written to this address.
 *              Leave to NULL if not needed.
 *              - ``SPICOMMON_BUSFLAG_IOMUX_PINS``: The bus is connected to iomux pins.
 *              - ``SPICOMMON_BUSFLAG_SCLK``, ``SPICOMMON_BUSFLAG_MISO``, ``SPICOMMON_BUSFLAG_MOSI``: The bus has
 *                  CLK/MISO/MOSI connected.
 *              - ``SPICOMMON_BUSFLAG_DUAL``: The bus is capable with DIO mode.
 *              - ``SPICOMMON_BUSFLAG_WPHD`` The bus has WP and HD connected.
 *              - ``SPICOMMON_BUSFLAG_QUAD``: Combination of ``SPICOMMON_BUSFLAG_DUAL`` and ``SPICOMMON_BUSFLAG_WPHD``.
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_OK                on success
 */
esp_err_t spicommon_bus_initialize_io(spi_host_device_t host, const spi_bus_config_t *bus_config, int dma_chan, uint32_t flags, uint32_t *flags_o);

/**
 * @brief Free the IO used by a SPI peripheral
 *
 * @note This public API is deprecated. Please call ``spi_bus_free`` for master
 *       bus deinitialization and ``spi_slave_free`` for slave deinitialization.
 *
 * @param bus_cfg Bus config struct which defines which pins to be used.
 *
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_OK                on success
 */
esp_err_t spicommon_bus_free_io_cfg(const spi_bus_config_t *bus_cfg);

/**
 * @brief Initialize a Chip Select pin for a specific SPI peripheral
 *
 * @note This public API is deprecated. Please call corresponding device initialization
 *       functions.
 *
 * @param host SPI peripheral
 * @param cs_io_num GPIO pin to route
 * @param cs_num CS id to route
 * @param force_gpio_matrix If true, CS will always be routed through the GPIO matrix. If false,
 *                          if the GPIO number allows it, the routing will happen through the IO_mux.
 */
void spicommon_cs_initialize(spi_host_device_t host, int cs_io_num, int cs_num, int force_gpio_matrix);

/**
 * @brief Free a chip select line
 *
 * @param cs_gpio_num CS gpio num to free
 *
 * @note This public API is deprecated.
 */
void spicommon_cs_free_io(int cs_gpio_num);

/**
 * @brief Check whether all pins used by a host are through IOMUX.
 *
 * @param host SPI peripheral
 *
 * @note This public API is deprecated.
 *
 * @return false if any pins are through the GPIO matrix, otherwise true.
 */
bool spicommon_bus_using_iomux(spi_host_device_t host);

/**
 * @brief Check whether all pins used by a host are through IOMUX.
 *
 * @param host SPI peripheral
 *
 * @note This public API is deprecated.
 *
 * @return false if any pins are through the GPIO matrix, otherwise true.
 */
bool spicommon_bus_using_iomux(spi_host_device_t host);

/**
 * @brief Get the IRQ source for a specific SPI host
 *
 * @param host The SPI host
 *
 * @note This public API is deprecated.
 *
 * @return The hosts IRQ source
 */
int spicommon_irqsource_for_host(spi_host_device_t host);

/**
 * @brief Get the IRQ source for a specific SPI DMA
 *
 * @param host The SPI host
 *
 * @note This public API is deprecated.
 *
 * @return The hosts IRQ source
 */
int spicommon_irqdma_source_for_host(spi_host_device_t host);

/**
 * Callback, to be called when a DMA engine reset is completed
*/
typedef void(*dmaworkaround_cb_t)(void *arg);


/**
 * @brief Request a reset for a certain DMA channel
 *
 * @note In some (well-defined) cases in the ESP32 (at least rev v.0 and v.1), a SPI DMA channel will get confused. This can be remedied
 * by resetting the SPI DMA hardware in case this happens. Unfortunately, the reset knob used for thsi will reset _both_ DMA channels, and
 * as such can only done safely when both DMA channels are idle. These functions coordinate this.
 *
 * Essentially, when a reset is needed, a driver can request this using spicommon_dmaworkaround_req_reset. This is supposed to be called
 * with an user-supplied function as an argument. If both DMA channels are idle, this call will reset the DMA subsystem and return true.
 * If the other DMA channel is still busy, it will return false; as soon as the other DMA channel is done, however, it will reset the
 * DMA subsystem and call the callback. The callback is then supposed to be used to continue the SPI drivers activity.
 *
 * @param dmachan DMA channel associated with the SPI host that needs a reset
 * @param cb Callback to call in case DMA channel cannot be reset immediately
 * @param arg Argument to the callback
 *
 * @note This public API is deprecated.
 *
 * @return True when a DMA reset could be executed immediately. False when it could not; in this
 *         case the callback will be called with the specified argument when the logic can execute
 *         a reset, after that reset.
 */
bool spicommon_dmaworkaround_req_reset(int dmachan, dmaworkaround_cb_t cb, void *arg);


/**
 * @brief Check if a DMA reset is requested but has not completed yet
 *
 * @note This public API is deprecated.
 *
 * @return True when a DMA reset is requested but hasn't completed yet. False otherwise.
 */
bool spicommon_dmaworkaround_reset_in_progress(void);


/**
 * @brief Mark a DMA channel as idle.
 *
 * A call to this function tells the workaround logic that this channel will
 * not be affected by a global SPI DMA reset.
 *
 * @note This public API is deprecated.
 */
void spicommon_dmaworkaround_idle(int dmachan);

/**
 * @brief Mark a DMA channel as active.
 *
 * A call to this function tells the workaround logic that this channel will
 * be affected by a global SPI DMA reset, and a reset like that should not be attempted.
 *
 * @note This public API is deprecated.
 */
void spicommon_dmaworkaround_transfer_active(int dmachan);

/** @endcond */

#ifdef __cplusplus
}
#endif
