// Copyright 2010-2018 Espressif Systems (Shanghai) PTE LTD
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


#ifndef _DRIVER_SPI_COMMON_H_
#define _DRIVER_SPI_COMMON_H_

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "rom/lldesc.h"
#include "soc/spi_periph.h"
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
 * @param data Data to be sent, can be uint8_t, uint16_t or uint32_t. @param
 *  len Length of data to be sent, since the SPI peripheral sends from the MSB,
 *  this helps to shift the data to the MSB.
 */
#define SPI_SWAP_DATA_TX(data, len) __builtin_bswap32((uint32_t)data<<(32-len))

/**
 * Transform received data of length <= 32 bits to the format of an unsigned integer.
 *
 * E.g. to transform the data of 15 bits placed in a 4-byte array to integer:
 *
 *      uint16_t data = SPI_SWAP_DATA_RX(*(uint32_t*)t->rx_data, 15);
 *
 * @param data Data to be rearranged, can be uint8_t, uint16_t or uint32_t.
 * @param len Length of data received, since the SPI peripheral writes from
 *      the MSB, this helps to shift the data to the LSB.
 */
#define SPI_SWAP_DATA_RX(data, len) (__builtin_bswap32(data)>>(32-len))

/**
 * @brief Enum with the three SPI peripherals that are software-accessible in it
 */
typedef enum {
    SPI_HOST=0,                     ///< SPI1, SPI
    HSPI_HOST=1,                    ///< SPI2, HSPI
    VSPI_HOST=2                     ///< SPI3, VSPI
} spi_host_device_t;

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
} spi_bus_config_t;


/**
 * @brief Try to claim a SPI peripheral
 *
 * Call this if your driver wants to manage a SPI peripheral.
 *
 * @param host Peripheral to claim
 * @return True if peripheral is claimed successfully; false if peripheral already is claimed.
 */
bool spicommon_periph_claim(spi_host_device_t host);

/**
 * @brief Return the SPI peripheral so another driver can claim it.
 *
 * @param host Peripheral to return
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
 * @return True if success; false otherwise.
 */
bool spicommon_dma_chan_claim(int dma_chan);

/**
 * @brief Return the SPI DMA channel so other driver can claim it, or just to power down DMA.
 *
 * @param dma_chan channel to return
 *
 * @return True if success; false otherwise.
 */
bool spicommon_dma_chan_free(int dma_chan);

#define SPICOMMON_BUSFLAG_SLAVE         0          ///< Initialize I/O in slave mode
#define SPICOMMON_BUSFLAG_MASTER        (1<<0)     ///< Initialize I/O in master mode
#define SPICOMMON_BUSFLAG_NATIVE_PINS   (1<<1)     ///< Check using iomux pins. Or indicates the pins are configured through the IO mux rather than GPIO matrix.
#define SPICOMMON_BUSFLAG_SCLK          (1<<2)     ///< Check existing of SCLK pin. Or indicates CLK line initialized.
#define SPICOMMON_BUSFLAG_MISO          (1<<3)     ///< Check existing of MISO pin. Or indicates MISO line initialized.
#define SPICOMMON_BUSFLAG_MOSI          (1<<4)     ///< Check existing of MOSI pin. Or indicates CLK line initialized.
#define SPICOMMON_BUSFLAG_DUAL          (1<<5)     ///< Check MOSI and MISO pins can output. Or indicates bus able to work under DIO mode.
#define SPICOMMON_BUSFLAG_WPHD          (1<<6)     ///< Check existing of WP and HD pins. Or indicates WP & HD pins initialized.
#define SPICOMMON_BUSFLAG_QUAD          (SPICOMMON_BUSFLAG_DUAL|SPICOMMON_BUSFLAG_WPHD)     ///< Check existing of MOSI/MISO/WP/HD pins as output. Or indicates bus able to work under QIO mode.

/**
 * @brief Connect a SPI peripheral to GPIO pins
 *
 * This routine is used to connect a SPI peripheral to the IO-pads and DMA channel given in
 * the arguments. Depending on the IO-pads requested, the routing is done either using the
 * IO_mux or using the GPIO matrix.
 *
 * @param host SPI peripheral to be routed
 * @param bus_config Pointer to a spi_bus_config struct detailing the GPIO pins
 * @param dma_chan DMA-channel (1 or 2) to use, or 0 for no DMA.
 * @param flags Combination of SPICOMMON_BUSFLAG_* flags, set to ensure the pins set are capable with some functions:
 *              - ``SPICOMMON_BUSFLAG_MASTER``: Initialize I/O in master mode
 *              - ``SPICOMMON_BUSFLAG_SLAVE``: Initialize I/O in slave mode
 *              - ``SPICOMMON_BUSFLAG_NATIVE_PINS``: Pins set should match the iomux pins of the controller.
 *              - ``SPICOMMON_BUSFLAG_SCLK``, ``SPICOMMON_BUSFLAG_MISO``, ``SPICOMMON_BUSFLAG_MOSI``:
 *                  Make sure SCLK/MISO/MOSI is/are set to a valid GPIO. Also check output capability according to the mode.
 *              - ``SPICOMMON_BUSFLAG_DUAL``: Make sure both MISO and MOSI are output capable so that DIO mode is capable.
 *              - ``SPICOMMON_BUSFLAG_WPHD`` Make sure WP and HD are set to valid output GPIOs.
 *              - ``SPICOMMON_BUSFLAG_QUAD``: Combination of ``SPICOMMON_BUSFLAG_DUAL`` and ``SPICOMMON_BUSFLAG_WPHD``.
 * @param[out] flags_o A SPICOMMON_BUSFLAG_* flag combination of bus abilities will be written to this address.
 *              Leave to NULL if not needed.
 *              - ``SPICOMMON_BUSFLAG_NATIVE_PINS``: The bus is connected to iomux pins.
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
 * @deprecated Use spicommon_bus_free_io_cfg instead.
 *
 * @param host SPI peripheral to be freed
 *
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_OK                on success
 */
esp_err_t spicommon_bus_free_io(spi_host_device_t host) __attribute__((deprecated));

/**
 * @brief Free the IO used by a SPI peripheral
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
 * @deprecated Use spicommon_cs_io, which inputs the gpio num rather than the cs id instead.
 *
 * @param host SPI peripheral
 * @param cs_num CS id to free
 */
void spicommon_cs_free(spi_host_device_t host, int cs_num) __attribute__((deprecated));

/**
 * @brief Free a chip select line
 *
 * @param cs_gpio_num CS gpio num to free
 */
void spicommon_cs_free_io(int cs_gpio_num);

/**
 * @brief Setup a DMA link chain
 *
 * This routine will set up a chain of linked DMA descriptors in the array pointed to by
 * ``dmadesc``. Enough DMA descriptors will be used to fit the buffer of ``len`` bytes in, and the
 * descriptors will point to the corresponding positions in ``buffer`` and linked together. The
 * end result is that feeding ``dmadesc[0]`` into DMA hardware results in the entirety ``len`` bytes
 * of ``data`` being read or written.
 *
 * @param dmadesc Pointer to array of DMA descriptors big enough to be able to convey ``len`` bytes
 * @param len Length of buffer
 * @param data Data buffer to use for DMA transfer
 * @param isrx True if data is to be written into ``data``, false if it's to be read from ``data``.
 */
void spicommon_setup_dma_desc_links(lldesc_t *dmadesc, int len, const uint8_t *data, bool isrx);

/**
 * @brief Get the position of the hardware registers for a specific SPI host
 *
 * @param host The SPI host
 *
 * @return A register descriptor stuct pointer, pointed at the hardware registers
 */
spi_dev_t *spicommon_hw_for_host(spi_host_device_t host);

/**
 * @brief Get the IRQ source for a specific SPI host
 *
 * @param host The SPI host
 *
 * @return The hosts IRQ source
 */
int spicommon_irqsource_for_host(spi_host_device_t host);

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
 * @return True when a DMA reset could be executed immediately. False when it could not; in this
 *         case the callback will be called with the specified argument when the logic can execute
 *         a reset, after that reset.
 */
bool spicommon_dmaworkaround_req_reset(int dmachan, dmaworkaround_cb_t cb, void *arg);


/**
 * @brief Check if a DMA reset is requested but has not completed yet
 *
 * @return True when a DMA reset is requested but hasn't completed yet. False otherwise.
 */
bool spicommon_dmaworkaround_reset_in_progress();


/**
 * @brief Mark a DMA channel as idle.
 *
 * A call to this function tells the workaround logic that this channel will
 * not be affected by a global SPI DMA reset.
 */
void spicommon_dmaworkaround_idle(int dmachan);

/**
 * @brief Mark a DMA channel as active.
 *
 * A call to this function tells the workaround logic that this channel will
 * be affected by a global SPI DMA reset, and a reset like that should not be attempted.
 */
void spicommon_dmaworkaround_transfer_active(int dmachan);



#ifdef __cplusplus
}
#endif

#endif
