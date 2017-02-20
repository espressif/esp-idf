// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _DRIVER_I2S_H_
#define _DRIVER_I2S_H_
#include "esp_err.h"
#include <esp_types.h>
#include "soc/gpio_reg.h"
#include "soc/soc.h"
#include "soc/i2s_struct.h"
#include "soc/i2s_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/io_mux_reg.h"
#include "rom/gpio.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "driver/periph_ctrl.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief I2S bit width per sample.
 *
 */
typedef enum {
    I2S_BITS_PER_SAMPLE_8BIT    = 8,        /*!< I2S bits per sample: 8-bits*/
    I2S_BITS_PER_SAMPLE_16BIT   = 16,       /*!< I2S bits per sample: 16-bits*/
    I2S_BITS_PER_SAMPLE_24BIT   = 24,       /*!< I2S bits per sample: 24-bits*/
    I2S_BITS_PER_SAMPLE_32BIT   = 32,       /*!< I2S bits per sample: 32-bits*/
} i2s_bits_per_sample_t;

/**
 * @brief I2S communication standard format
 *
 */
typedef enum {
    I2S_COMM_FORMAT_I2S         = 0x01, /*!< I2S communication format I2S*/
    I2S_COMM_FORMAT_I2S_MSB     = 0x02, /*!< I2S format MSB*/
    I2S_COMM_FORMAT_I2S_LSB     = 0x04, /*!< I2S format LSB*/
    I2S_COMM_FORMAT_PCM         = 0x08, /*!< I2S communication format PCM*/
    I2S_COMM_FORMAT_PCM_SHORT   = 0x10, /*!< PCM Short*/
    I2S_COMM_FORMAT_PCM_LONG    = 0x20, /*!< PCM Long*/
} i2s_comm_format_t;


/**
 * @brief I2S channel format type
 */
typedef enum {
    I2S_CHANNEL_FMT_RIGHT_LEFT = 0x00,
    I2S_CHANNEL_FMT_ALL_RIGHT,
    I2S_CHANNEL_FMT_ALL_LEFT,
    I2S_CHANNEL_FMT_ONLY_RIGHT,
    I2S_CHANNEL_FMT_ONLY_LEFT,
} i2s_channel_fmt_t;

/**
 * @brief PDM sample rate ratio, measured in Hz.
 *
 */
typedef enum {
    PDM_SAMPLE_RATE_RATIO_64,
    PDM_SAMPLE_RATE_RATIO_128,
} pdm_sample_rate_ratio_t;

/**
 * @brief PDM PCM convter enable/disable.
 *
 */
typedef enum {
    PDM_PCM_CONV_ENABLE,
    PDM_PCM_CONV_DISABLE,
} pdm_pcm_conv_t;


/**
 * @brief I2S Peripheral, 0 & 1.
 *
 */
typedef enum {
    I2S_NUM_0 = 0x0,  /*!< I2S 0*/
    I2S_NUM_1 = 0x1,  /*!< I2S 1*/
    I2S_NUM_MAX,
} i2s_port_t;

/**
 * @brief I2S Mode, defaut is I2S_MODE_MASTER | I2S_MODE_TX
 *
 */
typedef enum {
    I2S_MODE_MASTER = 1,
    I2S_MODE_SLAVE = 2,
    I2S_MODE_TX = 4,
    I2S_MODE_RX = 8,
    I2S_MODE_DAC_BUILT_IN = 16
} i2s_mode_t;

/**
 * @brief I2S configuration parameters for i2s_param_config function
 *
 */
typedef struct {
    i2s_mode_t              mode;                   /*!< I2S work mode*/
    int                     sample_rate;            /*!< I2S sample rate*/
    i2s_bits_per_sample_t   bits_per_sample;        /*!< I2S bits per sample*/
    i2s_channel_fmt_t       channel_format;         /*!< I2S channel format */
    i2s_comm_format_t       communication_format;   /*!< I2S communication format */
    int                     intr_alloc_flags;       /*!< Flags used to allocate the interrupt. One or multiple (ORred) ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info */
    int                     dma_buf_count;          /*!< I2S DMA Buffer Count */
    int                     dma_buf_len;            /*!< I2S DMA Buffer Length */
} i2s_config_t;

/**
 * @brief I2S event types
 *
 */
typedef enum {
    I2S_EVENT_DMA_ERROR,
    I2S_EVENT_TX_DONE,     /*!< I2S DMA finish sent 1 buffer*/
    I2S_EVENT_RX_DONE,     /*!< I2S DMA finish received 1 buffer*/
    I2S_EVENT_MAX,         /*!< I2S event max index*/
} i2s_event_type_t;

/**
 * @brief Event structure used in I2S event queue
 *
 */
typedef struct {
    i2s_event_type_t    type;   /*!< I2S event type */
    size_t              size;   /*!< I2S data size for I2S_DATA event*/
} i2s_event_t;

#define I2S_PIN_NO_CHANGE (-1) /*!< Use in i2s_pin_config_t for pins which should not be changed */

/**
 * @brief I2S pin number for i2s_set_pin
 *
 */
typedef struct {
    int bck_io_num;     /*!< BCK in out pin*/
    int ws_io_num;      /*!< WS in out pin*/
    int data_out_num;   /*!< DATA out pin*/
    int data_in_num;    /*!< DATA in pin*/
} i2s_pin_config_t;

typedef intr_handle_t i2s_isr_handle_t;
/**
 * @brief Set I2S pin number
 *
 * @note
 * The I2S peripheral output signals can be connected to multiple GPIO pads.
 * However, the I2S peripheral input signal can only be connected to one GPIO pad.
 *
 * @param   i2s_num     I2S_NUM_0 or I2S_NUM_1
 *
 * @param   pin         I2S Pin structure, or NULL to set 2-channel 8-bit internal DAC pin configuration (GPIO25 & GPIO26)
 *
 * Inside the pin configuration structure, set I2S_PIN_NO_CHANGE for any pin where
 * the current configuration should not be changed.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t i2s_set_pin(i2s_port_t i2s_num, const i2s_pin_config_t *pin);

/**
 * @brief Install and start I2S driver.
 *
 * @param i2s_num         I2S_NUM_0, I2S_NUM_1
 *
 * @param i2s_config      I2S configurations - see i2s_config_t struct
 *
 * @param queue_size      I2S event queue size/depth.
 *
 * @param i2s_queue       I2S event queue handle, if set NULL, driver will not use an event queue.
 *
 * This function must be called before any I2S driver read/write operations.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t i2s_driver_install(i2s_port_t i2s_num, const i2s_config_t *i2s_config, int queue_size, void* i2s_queue);

/**
 * @brief Uninstall I2S driver.
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t i2s_driver_uninstall(i2s_port_t i2s_num);

/**
 * @brief Write data to I2S DMA transmit buffer.
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @param src      Source address to write from
 *
 * @param size     Size of data in bytes
 *
 * @param ticks_to_wait TX buffer wait timeout in RTOS ticks. If this
 * many ticks pass without space becoming available in the DMA
 * transmit buffer, then the function will return (note that if the
 * data is written to the DMA buffer in pieces, the overall operation
 * may still take longer than this timeout.) Pass portMAX_DELAY for no
 * timeout.
 *
 * Format of the data in source buffer is determined by the I2S
 * configuration (see i2s_config_t).
 *
 * @return  Number of bytes written, or ESP_FAIL (-1) for parameter error. If a timeout occurred, bytes written will be less than total size.
 */
int i2s_write_bytes(i2s_port_t i2s_num, const char *src, size_t size, TickType_t ticks_to_wait);

/**
 * @brief Read data from I2S DMA receive buffer
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @param dest     Destination address to read into
 *
 * @param size     Size of data in bytes
 *
 * @param ticks_to_wait RX buffer wait timeout in RTOS ticks. If this many ticks pass without bytes becoming available in the DMA receive buffer, then the function will return (note that if data is read from the DMA buffer in pieces, the overall operation may still take longer than this timeout.) Pass portMAX_DELAY for no timeout.
 *
 * Format of the data in source buffer is determined by the I2S
 * configuration (see i2s_config_t).
 *
 * @return  Number of bytes read, or ESP_FAIL (-1) for parameter error. If a timeout occurred, bytes read will be less than total size.
 */
int i2s_read_bytes(i2s_port_t i2s_num, char* dest, size_t size, TickType_t ticks_to_wait);

/**
 * @brief Push (write) a single sample to the I2S DMA TX buffer.
 *
 * Size of the sample is determined by the channel_format (mono or stereo)) & bits_per_sample configuration (see i2s_config_t).
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @param sample Pointer to buffer containing sample to write. Size of buffer (in bytes) = (number of channels) * bits_per_sample / 8.
 *
 * @param ticks_to_wait Push timeout in RTOS ticks. If space is not available in the DMA TX buffer within this period, no data is written and function returns 0.
 *
 * @return Number of bytes successfully pushed to DMA buffer, or ESP_FAIL (-1) for parameter error. Will be either zero or the size of configured sample buffer.
 */
int i2s_push_sample(i2s_port_t i2s_num, const char *sample, TickType_t ticks_to_wait);

/**
 * @brief Pop (read) a single sample from the I2S DMA RX buffer.
 *
 * Size of the sample is determined by the channel_format (mono or stereo)) & bits_per_sample configuration (see i2s_config_t).
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @param sample Buffer sample data will be read into. Size of buffer (in bytes) = (number of channels) * bits_per_sample / 8.
 *
 * @param ticks_to_wait Pop timeout in RTOS ticks. If a sample is not available in the DMA buffer within this period, no data is read and function returns zero.
 *
 * @return Number of bytes successfully read from DMA buffer, or ESP_FAIL (-1) for parameter error. Byte count will be either zero or the size of the configured sample buffer.

 */
int i2s_pop_sample(i2s_port_t i2s_num, char *sample, TickType_t ticks_to_wait);


/**
 * @brief Set sample rate used for I2S RX and TX.
 *
 * The bit clock rate is determined by the sample rate and i2s_config_t configuration parameters (number of channels, bits_per_sample).
 *
 * `bit_clock = rate * (number of channels) * bits_per_sample`
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @param rate I2S sample rate (ex: 8000, 44100...)
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t i2s_set_sample_rates(i2s_port_t i2s_num, uint32_t rate);

/**
 * @brief Stop I2S driver
 *
 * Disables I2S TX/RX, until i2s_start() is called.
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t i2s_stop(i2s_port_t i2s_num);

/**
 * @brief Start I2S driver
 *
 * It is not necessary to call this function after i2s_driver_install() (it is started automatically), however it is necessary to call it after i2s_stop().
 *
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
* @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t i2s_start(i2s_port_t i2s_num);

/**
 * @brief Zero the contents of the TX DMA buffer.
 *
 * Pushes zero-byte samples into the TX DMA buffer, until it is full.
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t i2s_zero_dma_buffer(i2s_port_t i2s_num);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_I2S_H_ */
