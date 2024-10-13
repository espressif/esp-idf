/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/idf_additions.h"
#include "sdkconfig.h"
#include "esp_probe_private.h"
#include "esp_probe.h"
#include "esp_check.h"
#include "esp_cpu.h"

static const char *TAG = "esp_probe";

#define GET_US_BY_CCOUNT(t) ((float)(t)/CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)

static void s_esp_probe_flush_task(void *args)
{
    esp_probe_handle_t handle = (esp_probe_handle_t)args;
    esp_probe_flush_data_t flush_data;

    // Suspend if not start probing yet
    if (!handle->flags.is_started) {
        vTaskSuspend(NULL);
    }

    while (1) {
        if (xQueueReceive(handle->flush_que, &flush_data, portMAX_DELAY) == pdTRUE) {
            if (flush_data.size) {
                fwrite(flush_data.data, flush_data.size, 1, handle->out_stream);
            }
        }
        // If probe is stopped and no data to flush, suspend the task
        if (!handle->flags.is_started && uxQueueSpacesAvailable(handle->flush_que) == 2) {
            vTaskSuspend(NULL);
        }
    }
}

static void s_esp_probe_push_flush_queue(esp_probe_handle_t handle, void *data, uint32_t size, bool reset_ptr)
{
    esp_probe_flush_data_t flush_data = {
        .data = data,
        .size = size,
    };
    TickType_t timeout_tick = 0;
    if (!size) {
        // If no data to flush but only inform the flush task to suspend
        // then wait until data flushed
        timeout_tick = portMAX_DELAY;
    }
    if (xQueueSend(handle->flush_que, &flush_data, timeout_tick) != pdTRUE) {
        esp_probe_flush_data_t dummy;
        xQueueReceive(handle->flush_que, &dummy, 0);
        xQueueSend(handle->flush_que, &flush_data, 0);
        ESP_LOGW(TAG, "data dropped");
    }
    if (reset_ptr) {
        handle->w_ptr = 0;
        handle->f_ptr = 0;
    } else {
        handle->f_ptr = handle->w_ptr; // Update the flush pointer to the current write pointer
    }
}

static void s_esp_probe_dump_task(void *args)
{
    esp_probe_handle_t handle = (esp_probe_handle_t)args;
    esp_probe_recv_data_t recv_data;
    bool need_suspend = false;
    uint32_t byte_dump = 0;

    // Suspend if not start probing yet
    if (!handle->flags.is_started) {
        vTaskSuspend(NULL);
    }

    while (1) {
        // Receive the data that from data receive callback
        if (xQueueReceive(handle->recv_que, &recv_data, portMAX_DELAY) == pdTRUE) {
            if (handle->buf_size) {  // Buffer mode, adopt ping-pong buffer strategy to flush data
                if (unlikely(handle->w_ptr + recv_data.size > handle->max_dump_size)) {
                    // If the received in buffer has exceeded the max_dump_size but not reached the buffer size
                    // Only dump the rest piece of data
                    byte_dump = handle->max_dump_size - handle->w_ptr;
                } else {
                    // If the rest buffer is not sufficient for the received data
                    // Flush the second half of the buffer
                    if (handle->w_ptr + recv_data.size > handle->buf_size) {
                        s_esp_probe_push_flush_queue(handle, handle->buffer + handle->f_ptr,
                                                     handle->w_ptr - handle->f_ptr, true);
                    }
                    byte_dump = recv_data.size;
                }
                memcpy(handle->buffer + handle->w_ptr, recv_data.data, byte_dump);
                handle->w_ptr += byte_dump;

                // If the last flush pointer is 0 and the stored data has exceed the half of the buffer
                // Flush the first half of the buffer
                // Here minus some extra bytes to trigger the flush more timely
                if (!handle->f_ptr && handle->w_ptr >= handle->buf_size / 2 - 32) {
                    s_esp_probe_push_flush_queue(handle, handle->buffer, handle->w_ptr, false);
                }

            } else {  // Stream mode, write directly
                if (handle->dump_data_size + recv_data.size > handle->max_dump_size) {
                    byte_dump = handle->max_dump_size - handle->dump_data_size;
                } else {
                    byte_dump = recv_data.size;
                }
                fwrite(recv_data.data, byte_dump, 1, handle->out_stream);
            }
            // Update the dump data size statistic
            handle->dump_data_size += byte_dump;
        }
        uint32_t exceed_bytes = 0;
        // If reach the max dump size, suspend the task
        if (handle->dump_data_size >= handle->max_dump_size) {
            ESP_LOGW(TAG, "Dump data size reached the max dump size");
            need_suspend = true;
            exceed_bytes = handle->dump_data_size - handle->max_dump_size;
        }
        // If probe is stopped, suspend the task
        need_suspend |= !handle->flags.is_started;

        if (need_suspend) {
            // Flush the legacy data before suspend
            if (handle->buf_size) {
                s_esp_probe_push_flush_queue(handle, handle->buffer + handle->f_ptr,
                                             handle->w_ptr - handle->f_ptr - exceed_bytes, true);
            }
            vTaskSuspend(NULL);
        }
    }
}

static void s_esp_probe_destroy_object(esp_probe_handle_t handle)
{
    if (handle) {
        if (handle->buffer) {
            free(handle->buffer);
            handle->buffer = NULL;
        }
        if (handle->recv_que) {
            vQueueDeleteWithCaps(handle->recv_que);
            handle->recv_que = NULL;
        }
        if (handle->dump_task) {
            vTaskDelete(handle->dump_task);
            handle->dump_task = NULL;
        }
        if (handle->flush_que) {
            vQueueDeleteWithCaps(handle->flush_que);
            handle->flush_que = NULL;
        }
        if (handle->flush_task) {
            vTaskDelete(handle->flush_task);
            handle->flush_task = NULL;
        }
        free(handle);
    }
}

esp_err_t esp_new_probe(esp_probe_config_t *config, esp_probe_handle_t* ret_handle)
{
    ESP_RETURN_ON_FALSE(config && ret_handle, ESP_ERR_INVALID_ARG, TAG, "input parameter is NULL");
    // Get the max channel id and check whether any gpio specified
    int max_chan_id = ESP_PROBE_MAX_CHANNEL_NUM - 1;
    while (config->probe_gpio[max_chan_id] < 0) {
        max_chan_id--;
        ESP_RETURN_ON_FALSE(max_chan_id > 0, ESP_ERR_INVALID_ARG, TAG, "no gpio to probe");
    }
    // Create the probe object
    esp_err_t ret = ESP_OK;
    esp_probe_handle_t probe = heap_caps_calloc(1, sizeof(struct esp_probe_t), ESP_PROBE_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(probe, ESP_ERR_NO_MEM, TAG, "no memory for probe object");
    probe->max_dump_size = config->max_dump_size_kb ? (config->max_dump_size_kb << 10) : UINT32_MAX;

    // Allocate the storage buffer if the data should be cached
    if (config->storage_depth_kb) {
        if (config->storage_depth_kb < 8) {
            ESP_LOGW(TAG, "the storage depth is at least 8 KB, set to 8 KB instead");
            probe->buf_size = 8192;
        } else {
            probe->buf_size = config->storage_depth_kb * 1024;
        }
        probe->buffer = calloc(1, probe->buf_size);
        ESP_GOTO_ON_FALSE(probe->buffer, ESP_ERR_NO_MEM, err, TAG, "no memory for data storage");
        // Create flush task
        xTaskCreate(s_esp_probe_flush_task, "esp_probe_flush_task", 4096, probe,
                    (UBaseType_t)config->dump_task_priority, &(probe->flush_task));
        // Create the flush queue
        probe->flush_que = xQueueCreateWithCaps(2, sizeof(esp_probe_flush_data_t), MALLOC_CAP_DEFAULT);
        ESP_GOTO_ON_FALSE(probe->flush_que, ESP_ERR_NO_MEM, err, TAG, "no memory for flush queue");
    }

    // Create the receive queue
    probe->recv_que = xQueueCreateWithCaps(ESP_PROBE_DEFAULT_Q_DEPTH, sizeof(esp_probe_recv_data_t), ESP_PROBE_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(probe->recv_que, ESP_ERR_NO_MEM, err, TAG, "no memory for receive queue");

    // Create the dump task and suspend it
    xTaskCreate(s_esp_probe_dump_task, "esp_probe_dump_task", 4096, probe,
                (UBaseType_t)config->dump_task_priority, &(probe->dump_task));

    // Initialize the hardware peripheral
    ESP_GOTO_ON_ERROR(esp_probe_priv_init_hardware(probe, config, max_chan_id), err, TAG, "fail to init the hardware peripheral");

    *ret_handle = probe;
    return ret;

err:
    s_esp_probe_destroy_object(probe);
    probe = NULL;
    return ret;
}

esp_err_t esp_del_probe(esp_probe_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "input parameter is NULL");
    ESP_RETURN_ON_FALSE(!handle->flags.is_started, ESP_ERR_INVALID_STATE, TAG, "probe is still running");
    esp_probe_priv_deinit_hardware(handle);
    s_esp_probe_destroy_object(handle);
    return ESP_OK;
}

esp_err_t esp_probe_start(esp_probe_handle_t handle, FILE *out_stream)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "input parameter is NULL");
    ESP_RETURN_ON_FALSE(!handle->flags.is_started, ESP_ERR_INVALID_STATE, TAG, "probe is still running");
    handle->flags.is_started = true;

    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_ERROR(esp_probe_priv_enable_hardware(handle), err, TAG, "enable the hardware failed");
    out_stream = out_stream ? out_stream : stdout;
    if (handle->out_stream != out_stream) {
        handle->out_stream = out_stream;
        // io stream changed, reset timestamp and dump data size
        handle->dump_data_size = 0;
    }
    if (handle->buf_size) {
        // Reset the queue before resume the task
        xQueueReset(handle->flush_que);
        vTaskResume(handle->flush_task);
    }
    // Reset the queue before resume the task
    xQueueReset(handle->recv_que);
    vTaskResume(handle->dump_task);

    return ret;
err:
    handle->flags.is_started = false;
    return ret;
}

esp_err_t esp_probe_stop(esp_probe_handle_t handle, uint32_t *dump_data_size)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "input parameter is NULL");
    if (!handle->flags.is_started) {
        return ESP_OK;
    }
    // Set start flag to false
    handle->flags.is_started = false;

    // Waiting for the dump task be suspended after detecting the start flag is cleared
    while (eTaskGetState(handle->dump_task) != eSuspended) {
        vTaskDelay(1);
    }
    ESP_RETURN_ON_ERROR(esp_probe_priv_disable_hardware(handle), TAG, "disable the hardware failed");
    // Waiting for the flush task to flush the legacy data
    if (handle->buf_size) {
        // Push an empty data to inform the flush task to suspend
        s_esp_probe_push_flush_queue(handle, NULL, 0, true);
        while (eTaskGetState(handle->flush_task) != eSuspended) {
            vTaskDelay(1);
        }
    }
    if (dump_data_size) {
        *dump_data_size = handle->dump_data_size;
    }
    return ESP_OK;
}
