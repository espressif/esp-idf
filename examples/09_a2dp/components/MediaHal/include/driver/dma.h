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

#ifndef _DRIVER_DMA_H_
#define _DRIVER_DMA_H__
#include <esp_types.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup Driver_APIs Driver APIs
  * @brief Driver APIs
  */

/** @addtogroup Driver_APIs
  * @{
  */

/**
 * @brief DMA queue description.
 */
typedef struct {
    uint32_t  block_size: 12;
    uint32_t  data_length: 12;
    uint32_t  unused: 5;
    uint32_t  sub_sof: 1;
    uint32_t  eof: 1;
    uint32_t  owner: 1;
    uint32_t  buf_ptr;
    uint32_t  next_link_ptr;
} dma_queue_t;

/**
 * @brief DMA element description
 */
typedef struct {
    uint32_t        *buffer_addr;
    dma_queue_t     *first_queue;
    dma_queue_t     *last_queue;
    dma_queue_t     backup_queue;
} dma_element_t;

/**
 * @brief DMA ping-pong buffer object description
 */
typedef struct {
    dma_element_t   *ping;
    dma_element_t   *pong;
    uint32_t        len;
    uint32_t        queue_cnt;
} ping_pong_buf_t;


/**
 * @brief Create a ping-pong buffer object used by DMA.
 *
 * @param [in] bufLen
 *             Set the buffer length.
 *             --------------------
 *             | ping   | Pong    |
 *             --------------------
 *             | bufLen | bufLen  |
 *             --------------------
 *
 * @return uint32*, NULL:indicates parameter error, others indicates ping-pong buffer address.
 */
ping_pong_buf_t* dma_buf_create(uint32_t bufLen);

/**
 * @brief Reset the dma buffer length.
 *
 * @param [in] obj
 *             Pointer to a struct ping_pong_buf_t that indicates the object length to be reset to bufLen.
 *
 * @return uint32*, ESP_FAIL:indicates parameter error, ESP_OK :indicates success.
 */
esp_err_t dma_buf_len_reset(ping_pong_buf_t *obj);

/**
 * @brief Set the buffer length before the start.
 *
 * @param [in] obj
 *             Pointer to a struct spi_dma_attr_t.
 * @param [in] len
 *             Set the transmit length .
 *
 * @return uint32*, ESP_FAIL:indicates parameter error, ESP_OK: indicates success.
 */
esp_err_t dma_buf_len_set(ping_pong_buf_t *obj, dma_element_t *element, uint32_t len);

/**
 * @brief Destroy the ping-pong buffer instance.
 *
 * @param [in] obj
 *             Pointer to a struct ping_pong_buf_t that indicates the object to be destroy.
 *
 * @return NULL
 */
void dma_buf_destroy(ping_pong_buf_t *obj);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif // __DMA_H__
