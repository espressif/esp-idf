/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdio.h>
#include "driver/parlio_rx.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_PROBE_MAX_CHANNEL_NUM   16                  /*!< Max supported probe channel number.
                                                             Note that not all targets can reach the max channel, for example, ESP32-H2 has only 8 channels */
#define ESP_PROBE_MAX_SAMPLE_RATE   0                   /*!< The maximum sample rates are different among different targets,
                                                             Set 0 to adopt the maximum sample rate of the current target */

/**
 * @brief ESP Probe stream mode default configuration
 * @note  The stream mode relies heavily on the bandwidth of the output stream
 *        Stream mode can only achieve a relatively low sample rate to guarantee no samples are dropped,
 *        But it can keep sampling continuously for a long time
 */
#define ESP_PROBE_DEFAULT_STREAM_CONFIG(rate_hz) {  \
    .sample_rate_hz = rate_hz,                      \
    .dump_task_priority = 6,                        \
    .storage_depth_kb = 0,                          \
    .max_dump_size_kb = 0,                          \
    .probe_gpio = {                                 \
        [0 ... ESP_PROBE_MAX_CHANNEL_NUM - 1] = -1, \
    },                                              \
}

/**
 * @brief ESP Probe buffer mode default configuration
 * @note  The buffer mode will store the data onto heap storage first,
 *        and output all of them to the stream in once.
 * @note  The probe will stop if run out of the given storage on heap.
 *        So that to guarantee a reliable data storing during the sampling period.
 *        However, the disadvantage is that, the probing time is limited by the available heap size,
 *        the higher the sample rate is, the shorter time the sampling last.
 */
#define ESP_PROBE_DEFAULT_BUFFER_CONFIG(rate_hz, depth_kb) {    \
    .sample_rate_hz = rate_hz,                      \
    .dump_task_priority = 6,                        \
    .storage_depth_kb = depth_kb,                   \
    .max_dump_size_kb = depth_kb,                   \
    .probe_gpio = {                                 \
        [0 ... ESP_PROBE_MAX_CHANNEL_NUM - 1] = -1, \
    },                                              \
}

/**
 * @brief ESP Probe buffer stream mode default configuration
 * @note  The buffer stream mode will store the data onto heap storage first,
 *        and output them to the stream every half of the storage
 * @note  Buffer stream mode means the probe will loop to use the given storage on heap.
 *        So that we can also sample the data continuously for a long time.
 *        However, while the data outputting rate can't catch the data producing rate,
 *        data will still be dropped.
 *        The difference comparing to the stream mode is that, it sends a larger block in every output,
 *        i.e., it can hold more reliable samples for a time
 */
#define ESP_PROBE_DEFAULT_BUFFER_STREAM_CONFIG(rate_hz, depth_kb) {   \
    .sample_rate_hz = rate_hz,                      \
    .dump_task_priority = 6,                        \
    .storage_depth_kb = depth_kb,                   \
    .max_dump_size_kb = 0,                          \
    .probe_gpio = {                                 \
        [0 ... ESP_PROBE_MAX_CHANNEL_NUM - 1] = -1, \
    },                                              \
}

typedef struct {
    uint32_t    sample_rate_hz;                         /*!< The sample rate of the probe, unit: Hz, set '0' to use the max sample rate */
    int         probe_gpio[ESP_PROBE_MAX_CHANNEL_NUM];  /*!< The GPIO of each probe channel, please set '-1' for unused channels,
                                                             some targets like ESP32-H2 only support up to 8 channels */
    uint32_t    storage_depth_kb;                       /*!< The max heap storage depth for probed data (unit: kilobytes).
                                                             0: the probed data won't be stored but output in realtime, i.e. stream mode;
                                                             others: the probed data will be stored in a ping-pong buffer,
                                                             The ping-pong buffer will be flushed alternately, i.e. buffer mode.
                                                             ATTENTION: storage depth at lease be 8 KB, otherwise there is no difference with stream mode. */
    uint32_t    max_dump_size_kb;                       /*!< The max dump size for the limited storage (like flash/ram) (unit: kilobytes),
                                                             The probe will stop if the dump data size reach this value.
                                                             set 0 for no dump size limitation (like dumping to the host via UART/USB/Network),
                                                             set equals to `storage_depth_kb` to guarantee no sample lost in a short time */
    uint32_t    dump_task_priority;                     /*!< The priority of the dump task, which used for dumping the probed data via the out stream */
} esp_probe_config_t;

typedef struct esp_probe_t  *esp_probe_handle_t;        /*!< The handle of probe instance */

/**
 * @brief Allocate a new probe instance
 *
 * @param[in]  config           The probe configuration
 * @param[out] ret_handle       The returned probe handle
 * @return
 *      - ESP_OK                Success to allocate the probe instance
 *      - ESP_NO_MEM            No memory for probe instance
 *      - ESP_ERR_INVALID_ARG   NULL pointer or invalid configuration
 *      - ESP_ERR_NOT_SUPPORTED Exceed the max supported channels
 */
esp_err_t esp_new_probe(esp_probe_config_t *config, esp_probe_handle_t* ret_handle);

/**
 * @brief Delete the probe instance
 *
 * @param[in]  handle           The probe handle
 * @return
 *      - ESP_OK                Success to delete the probe instance
 *      - ESP_ERR_INVALID_ARG   NULL pointer
 *      - ESP_ERR_INVALID_STATE The probe has not stopped yet
 */
esp_err_t esp_del_probe(esp_probe_handle_t handle);

/**
 * @brief Start sampling on the probe channels
 *
 * @param[in]  handle           The probe handle
 * @param[in]  out_stream       The output stream for the VCD data, set NULL will use 'stdout' by default
 * @return
 *      - ESP_OK                Success to start the sampling on the probe channels
 *      - ESP_ERR_INVALID_ARG   NULL pointer
 *      - ESP_ERR_INVALID_STATE The probe has already started
 */
esp_err_t esp_probe_start(esp_probe_handle_t handle, FILE *out_stream);

/**
 * @brief Stop sampling on the probe channels
 * @note  If the 'esp_probe_config_t::storage_depth_kb' is set (i.e. buffer mode),
 *        The data in the buffer will be popped to the out stream after this function is called
 * @note  This function is
 *
 * @param[in]  handle           The probe handle
 * @param[out] dump_data_size   The dumped data size, can be NULL if not needed
 * @return
 *      - ESP_OK            Success to stop the sampling on the probe channels
 *      - Others            Failed to stop the sampling
 */
esp_err_t esp_probe_stop(esp_probe_handle_t handle, uint32_t *dump_data_size);

#ifdef __cplusplus
}
#endif
