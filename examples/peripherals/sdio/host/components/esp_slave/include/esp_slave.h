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

#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "soc/host_reg.h"

/* 
 * NOTE: This component is for example purpose only. Assertion fails if any of
 * the preconditions (connections, grounding, slave data preparation, etc.) is
 * not met.
 * Please do check and handle the return value in your real product.
*/

#define ESP_ERR_NOT_FINISHED    0x201

/** Context used by the ``esp_slave`` component.
 */
typedef struct {
    sdmmc_card_t*   card;               ///< Initialized sdmmc_cmd card
    uint16_t        buffer_size;
                        ///< All data that do not fully fill a buffer is still counted as one buffer. E.g. 10 bytes data costs 2 buffers if the size is 8 bytes per buffer.
                        ///< Buffer size of the slave pre-defined between host and slave before communication.
    uint16_t        block_size;
                        ///< If this is too large, it takes time to send stuff bits; while if too small, intervals between blocks cost much.
                        ///< Should be set according to length of data, and larger than ``TRANS_LEN_MAX/511``.
                        ///< Block size of the SDIO function 1. After the initialization this will hold the value the slave really do. Valid value is 1-2048.
    size_t          tx_sent_buffers;    ///< Counter hold the amount of buffers already sent to ESP32 slave. Should be set to 0 when initialization.
    size_t          rx_got_bytes;       ///< Counter hold the amount of bytes already received from ESP32 slave. Should be set to 0 when initialization.
} esp_slave_context_t;

/** Initialize ``esp_slave_context_t`` by this macro.
*/
#define ESP_SLAVE_DEFAULT_CONTEXT(card) (esp_slave_context_t){\
    .card = card, \
    .block_size = 0x200,    \
    .buffer_size = 128,   \
    .tx_sent_buffers = 0, \
    .rx_got_bytes = 0, \
    }

/** SDIO Initialize process of a ESP32 slave device.
 *
 * @param context Context of the ``esp_slave`` component. Send to other functions later.
 *
 * @return
 *      - ESP_OK if success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t esp_slave_init_io(esp_slave_context_t *context);

/** Wait for interrupt of a ESP32 slave device.
 *
 * @param context Context of the ``esp_slave`` component.
 *
 * @return
 *      - ESP_OK if success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t esp_slave_wait_for_ioready(esp_slave_context_t *context);

/** Get buffer num for the host to send data to the slave. The buffers are size of ``buffer_size``.
 *
 * @param context Context of the component.
 * @param tx_num Output of buffer num that host can send data to ESP32 slave.
 *
 * @return
 *      - ESP_OK Success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t esp_slave_get_tx_buffer_num(esp_slave_context_t *context, uint32_t* tx_num);

/** Get amount of data the ESP32 slave preparing to send to host.
 *
 * @param context Context of the component.
 * @param rx_size Output of data size to read from slave.
 *
 * @return
 *      - ESP_OK Success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t esp_slave_get_rx_data_size(esp_slave_context_t *context, uint32_t* rx_size);


/** Reset the counters of this component. Usually you don't need to do this unless you know the slave is reset.
 *
 * @param context Context of the component.
 */
inline static  void esp_slave_reset_cnt(esp_slave_context_t *context)
{
    context->rx_got_bytes = 0;
    context->tx_sent_buffers = 0;
}

/** Send a packet to the ESP32 slave. The slave receive the packet into buffers whose size is ``buffer_size`` in the context.
 *
 * @param context Context of the component.
 * @param start Start address of the packet to send
 * @param length Length of data to send, if the packet is over-size, the it will be divided into blocks and hold into different buffers automatically.
 * @param wait_ms Time to wait before timeout, in ms.
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_TIMEOUT No buffer to use, or error ftrom SDMMC host controller
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t esp_slave_send_packet(esp_slave_context_t *context, const void* start, size_t length, uint32_t wait_ms);

/** Get a packet from ESP32 slave.
 *
 * @param context Context of the component.
 * @param[out] out_data Data output address
 * @param size The size of the output buffer, if the buffer is smaller than the size of data to receive from slave, the driver returns ``ESP_ERR_NOT_FINISHED``
 * @param[out] out_length Output of length the data actually received from slave.
 * @param wait_ms Time to wait before timeout, in ms.
 *
 * @return
 *     - ESP_OK Success, all the data are read from the slave.
 *     - ESP_ERR_NOT_FINISHED Read success, while there're data remaining.
 *     - One of the error codes from SDMMC host controller
 */
esp_err_t esp_slave_get_packet(esp_slave_context_t *context, void* out_data, size_t size, size_t *out_length, uint32_t wait_ms);

/** wait for an interrupt of the slave
 *
 * @param context Context of the component.
 * @param wait Ticks to wait.
 *
 * @return
 *      - ESP_ERR_NOT_SUPPORTED Currently our driver doesnot support SDIO with SPI interface.
 *      - ESP_OK If interrupt triggered.
 *      - ESP_ERR_TIMEOUT No interrupts before timeout.
 */
inline static esp_err_t esp_slave_wait_int(esp_slave_context_t *context, TickType_t wait)
{
    return sdmmc_io_wait_int(context->card, wait);
}

/** Clear interrupt bits of ESP32 slave. All the bits set in the mask will be cleared, while other bits will stay the same.
 *
 * @param context Context of the component.
 * @param intr_mask Mask of interrupt bits to clear.
 *
 * @return
 *      - ESP_OK Success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t esp_slave_clear_intr(esp_slave_context_t *context, uint32_t intr_mask);

/** Get interrupt bits of ESP32 slave.
 *
 * @param context Context of the component.
 * @param intr_raw Output of the raw interrupt bits. Set to NULL if only masked bits are read.
 * @param intr_st Output of the masked interrupt bits. set to NULL if only raw bits are read.
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_INVALID_ARG   if both ``intr_raw`` and ``intr_st`` are NULL.
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t esp_slave_get_intr(esp_slave_context_t *context, uint32_t *intr_raw, uint32_t *intr_st);

/** Set interrupt enable bits of ESP32 slave. The slave only sends interrupt on the line when there is a bit both the raw status and the enable are set.
 *
 * @param context Context of the component.
 * @param ena_mask Mask of the interrupt bits to enable.
 *
 * @return
 *      - ESP_OK Success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t esp_slave_set_intr_ena(esp_slave_context_t *context, uint32_t ena_mask);

/** Get interrupt enable bits of ESP32 slave.
 *
 * @param context Context of the component.
 * @param ena_mask_o Output of interrupt bit enable mask.
 *
 * @return
 *      - ESP_OK Success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t esp_slave_get_intr_ena(esp_slave_context_t *context, uint32_t *ena_mask_o);

/** Write general purpose R/W registers (8-bit) of ESP32 slave.
 *
 * @param context Context of the component.
 * @param addr Address of register to write. Valid address: 0-27, 32-63 (28-31 reserved).
 * @param value Value to write to the register.
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG Address not valid.
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t esp_slave_write_reg(esp_slave_context_t *context, uint8_t addr, uint8_t value, uint8_t* value_o);

/** Read general purpose R/W registers (8-bit) of ESP32 slave.
 *
 * @param context Context of the component.
 * @param add Address of register to read. Valid address: 0-27, 32-63 (28-31 reserved, return interrupt bits on read).
 * @param value Output value read from the register.
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG Address not valid.
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t esp_slave_read_reg(esp_slave_context_t *context, uint8_t add, uint8_t *value_o);

/** Send interrupts to slave. Each bit of the interrupt will be triggered.
 *
 * @param context Context of the component.
 * @param intr_mask Mask of interrupt bits to send to slave.
 *
 * @return
 *      - ESP_OK Success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t esp_slave_send_slave_intr(esp_slave_context_t *context, uint8_t intr_mask);


