// Copyright 2010-2017 Espressif Systems (Shanghai) PTE LTD
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
#include "soc/spi_struct.h"
#include "rom/lldesc.h"


#ifdef __cplusplus
extern "C"
{
#endif


//Maximum amount of bytes that can be put in one DMA descriptor
#define SPI_MAX_DMA_LEN (4096-4)


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
 */
typedef struct {
    int mosi_io_num;                ///< GPIO pin for Master Out Slave In (=spi_d) signal, or -1 if not used.
    int miso_io_num;                ///< GPIO pin for Master In Slave Out (=spi_q) signal, or -1 if not used.
    int sclk_io_num;                ///< GPIO pin for Spi CLocK signal, or -1 if not used.
    int quadwp_io_num;              ///< GPIO pin for WP (Write Protect) signal which is used as D2 in 4-bit communication modes, or -1 if not used.
    int quadhd_io_num;              ///< GPIO pin for HD (HolD) signal which is used as D3 in 4-bit communication modes, or -1 if not used.
    int max_transfer_sz;            ///< Maximum transfer size, in bytes. Defaults to 4094 if 0.
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
 * @param host Peripheral to claim
 * @return True if peripheral is returned successfully; false if peripheral was free to claim already.
 */
bool spicommon_periph_free(spi_host_device_t host);


#define SPICOMMON_BUSFLAG_SLAVE  0          ///< Initialize I/O in slave mode
#define SPICOMMON_BUSFLAG_MASTER (1<<0)     ///< Initialize I/O in master mode
#define SPICOMMON_BUSFLAG_QUAD   (1<<1)     ///< Also initialize WP/HD pins, if specified

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
 * @param flags Combination of SPICOMMON_BUSFLAG_* flags
 * @param[out] is_native A value of 'true' will be written to this address if the GPIOs can be
 *                  routed using the IO_mux, 'false' if the GPIO matrix is used.
 * @return 
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_OK                on success
 */
esp_err_t spicommon_bus_initialize_io(spi_host_device_t host, const spi_bus_config_t *bus_config, int dma_chan, int flags, bool *is_native);

/**
 * @brief Free the IO used by a SPI peripheral
 *
 * @param host SPI peripheral to be freed
 * @return 
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_OK                on success
 */

esp_err_t spicommon_bus_free_io(spi_host_device_t host);

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
 *
 * @param host SPI peripheral
 * @param cs_num CS id to free
 */
void spicommon_cs_free(spi_host_device_t host, int cs_num);


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