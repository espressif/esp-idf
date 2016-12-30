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

#define I2S_PIN_NO_CHANGE      (-1)


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
 * @brief   Set I2S pin number
 *
 *          @note
 *          Internal signal can be output to multiple GPIO pads
 *          Only one GPIO pad can connect with input signal
 *
 * @param   i2s_num     I2S_NUM_0 or I2S_NUM_1
 *
 * @param   pin  I2S Pin struct, or NULL for  2-channels, 8-bits DAC pin configuration (GPIO25 & GPIO26)
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t i2s_set_pin(i2s_port_t i2s_num, const i2s_pin_config_t *pin);

/**
 * @brief   i2s install and start driver
 *
 * @param   i2s_num         I2S_NUM_0, I2S_NUM_1
 *
 * @param   i2s_config      I2S configurations - see i2s_config_t struct
 *
 * @param   queue_size      I2S event queue size/depth.
 *
 * @param   i2s_queue       I2S event queue handle, if set NULL, driver will not use an event queue.
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
 * @brief   i2s read data buffer to i2s dma buffer
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @param src      source address to write
 *
 * @param size     size of data (size in bytes)
 *
 * @param ticks_to_wait     Write timeout
 *
 * @return  number of written bytes
 */
int i2s_write_bytes(i2s_port_t i2s_num, const char *src, size_t size, TickType_t ticks_to_wait);

/**
 * @brief   i2s write data buffer to i2s dma buffer
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @param dest     destination address to read
 *
 * @param size     size of data (size in bytes)
 *
 * @param ticks_to_wait     Read timeout
 *
 * @return  number of read bytes
 */
int i2s_read_bytes(i2s_port_t i2s_num, char* dest, size_t size, TickType_t ticks_to_wait);

/**
 * @brief   i2s push 1 sample to i2s dma buffer, with the size parameter equal to one sample's size in bytes = bits_per_sample/8.
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @param sample     destination address to write (depend on bits_per_sample, size of sample (in bytes) = 2*bits_per_sample/8)
 *
 * @param ticks_to_wait     Push timeout
 *
 * @return  number of push bytes
 */
int i2s_push_sample(i2s_port_t i2s_num, const char *sample, TickType_t ticks_to_wait);

/**
 * @brief  Pop 1 sample to i2s dma buffer, with the size parameter equal to one sample's size in bytes = bits_per_sample/8.
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @param sample    destination address to write (depend on bits_per_sample, size of sample (in bytes) = 2*bits_per_sample/8)
 *
 * @param ticks_to_wait     Pop timeout
 *
 * @return  number of pop bytes
 */
int i2s_pop_sample(i2s_port_t i2s_num, char *sample, TickType_t ticks_to_wait);


/**
 * @brief   Set clock rate used for I2S RX and TX
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @param rate  I2S clock (ex: 8000, 44100...)
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t i2s_set_sample_rates(i2s_port_t i2s_num, uint32_t rate);

/**
 * @brief   Start driver
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
* @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t i2s_start(i2s_port_t i2s_num);

/**
 * @brief   Stop driver
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t i2s_stop(i2s_port_t i2s_num);

/**
 * @brief   Set the TX DMA buffer contents to all zeroes
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t i2s_zero_dma_buffer(i2s_port_t i2s_num);

/***************************EXAMPLE**********************************
 *
 *
 * ----------------EXAMPLE OF I2S SETTING ---------------------
 * @code{c}
 *
 * #include "freertos/queue.h"
 * #define I2S_INTR_NUM 17                                      //choose one interrupt number from soc.h
 * int i2s_num = 0;                                             //i2s port number
 * i2s_config_t i2s_config = {
 *    .mode = I2S_MODE_MASTER | I2S_MODE_TX,
 *    .sample_rate = 44100,
 *    .bits_per_sample = 16,                                    //16, 32
 *    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,             //format LEFT_RIGHT
 *    .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
 *    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
 *    .dma_buf_count = 8,
 *    .dma_buf_len = 64
 * };
 *
 * i2s_pin_config_t pin_config = {
 *      .bck_io_num = 26,
 *      .ws_io_num = 25,
 *      .data_out_num = 22,
 *      .data_in_num = I2S_PIN_NO_CHANGE
 * };
 *
 * i2s_driver_install(i2s_num, &i2s_config, 0, NULL);   //install and start i2s driver
 *
 * i2s_set_pin(i2s_num, &pin_config);
 *
 * i2s_set_sample_rates(i2s_num, 22050); //set sample rates
 *
 *
 * i2s_driver_uninstall(i2s_num); //stop & destroy i2s driver
 *@endcode
 *
 * ----------------EXAMPLE USING I2S WITH DAC ---------------------
 * @code{c}
 *
 * #include "freertos/queue.h"
 * #define I2S_INTR_NUM 17                                      //choose one interrupt number from soc.h
 * int i2s_num = 0;                                             //i2s port number
 * i2s_config_t i2s_config = {
 *    .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN,
 *    .sample_rate = 44100,
 *    .bits_per_sample = 8,                                    // Only 8-bit DAC support
 *    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,            //
 *    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
 *    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
 *    .dma_buf_count = 8,
 *    .dma_buf_len = 64
 * };
 *
 *
 * i2s_driver_install(i2s_num, &i2s_config, 0, NULL);   //install and start i2s driver
 *
 * i2s_set_pin(i2s_num, NULL); //for internal DAC
 *
 * i2s_set_sample_rates(i2s_num, 22050); //set sample rates
 *
 * i2s_driver_uninstall(i2s_num); //stop & destroy i2s driver
 *@endcode
 *-----------------------------------------------------------------------------*
 ***************************END OF EXAMPLE**********************************/

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_I2S_H_ */
