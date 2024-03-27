/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "stdint.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_heap_caps.h"
#include "esp_probe.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_PROBE_DEFAULT_Q_DEPTH   8
#define ESP_PROBE_DEFAULT_MAX_RECV_SIZE   (ESP_PROBE_DEFAULT_Q_DEPTH * 4092)
#if CONFIG_PARLIO_ISR_IRAM_SAFE
#define ESP_PROBE_ALLOC_CAPS        (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define ESP_PROBE_ALLOC_CAPS        MALLOC_CAP_DEFAULT
#endif

struct esp_probe_t {
    uint32_t sample_width;      /*!< sample width, i.e., enabled probe channel nums */
    uint32_t sample_rate_hz;    /*!< sample rate in Hz */
    FILE *out_stream;           /*!< Output stream */
    TaskHandle_t dump_task;     /*!< Task handle of the raw data dump task */
    TaskHandle_t flush_task;    /*!< Task handle of the raw data flush task, only created in buffer mode */
    int dump_task_priority;     /*!< Task priority */
    uint32_t max_dump_size;     /*!< Max dump size */
    uint32_t dump_data_size;    /*!< Dump data size */
    QueueHandle_t recv_que;     /*!< Receive data queue */
    QueueHandle_t flush_que;    /*!< Flush data queue */
    uint8_t *buffer;            /*!< The storage buffer for dump data */
    uint32_t buf_size;          /*!< The storage buffer size/depth */
    uint32_t w_ptr;             /*!< The pointer of the current write offset of the buffer */
    uint32_t f_ptr;             /*!< The pointer of the current flush offset of the buffer */
    struct {
        volatile uint32_t is_started: 1; /*!< Is the sampling started */
    } flags;
};

typedef struct {
    void        *data;          /*!< The pointer of the received raw data buffer */
    uint32_t    size;           /*!< The size of the received raw data buffer */
} esp_probe_recv_data_t;

typedef esp_probe_recv_data_t esp_probe_flush_data_t;

esp_err_t esp_probe_priv_init_hardware(esp_probe_handle_t handle, esp_probe_config_t *config, int max_chan_id);

esp_err_t esp_probe_priv_deinit_hardware(esp_probe_handle_t handle);

esp_err_t esp_probe_priv_enable_hardware(esp_probe_handle_t handle);

esp_err_t esp_probe_priv_disable_hardware(esp_probe_handle_t handle);

#ifdef __cplusplus
}
#endif
