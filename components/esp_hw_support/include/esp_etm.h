/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdio.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ETM channel handle
 */
typedef struct esp_etm_channel_t *esp_etm_channel_handle_t;

/**
 * @brief ETM event handle
 */
typedef struct esp_etm_event_t *esp_etm_event_handle_t;

/**
 * @brief ETM task handle
 */
typedef struct esp_etm_task_t *esp_etm_task_handle_t;

/**
 * @brief ETM channel configuration
 */
typedef struct {
    /// Extra configuration flags for ETM channel
    struct etm_chan_flags {
        uint32_t allow_pd : 1; /*!< If set, driver allows the power domain to be powered off when system enters sleep mode.
                                    This can save power, but at the expense of more RAM being consumed to save register context. */
    } flags; /*!< ETM channel flags */
} esp_etm_channel_config_t;

/**
 * @brief Allocate an ETM channel
 *
 * @note The channel can later be freed by `esp_etm_del_channel`
 *
 * @param[in] config ETM channel configuration
 * @param[out] ret_chan Returned ETM channel handle
 * @return
 *      - ESP_OK: Allocate ETM channel successfully
 *      - ESP_ERR_INVALID_ARG: Allocate ETM channel failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Allocate ETM channel failed because of out of memory
 *      - ESP_ERR_NOT_FOUND: Allocate ETM channel failed because all channels are used up and no more free one
 *      - ESP_FAIL: Allocate ETM channel failed because of other reasons
 */
esp_err_t esp_etm_new_channel(const esp_etm_channel_config_t *config, esp_etm_channel_handle_t *ret_chan);

/**
 * @brief Delete an ETM channel
 *
 * @param[in] chan ETM channel handle that created by `esp_etm_new_channel`
 * @return
 *      - ESP_OK: Delete ETM channel successfully
 *      - ESP_ERR_INVALID_ARG: Delete ETM channel failed because of invalid argument
 *      - ESP_FAIL: Delete ETM channel failed because of other reasons
 */
esp_err_t esp_etm_del_channel(esp_etm_channel_handle_t chan);

/**
 * @brief Enable ETM channel
 *
 * @note This function will transit the channel state from init to enable.
 *
 * @param[in] chan ETM channel handle that created by `esp_etm_new_channel`
 * @return
 *      - ESP_OK: Enable ETM channel successfully
 *      - ESP_ERR_INVALID_ARG: Enable ETM channel failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Enable ETM channel failed because the channel has been enabled already
 *      - ESP_FAIL: Enable ETM channel failed because of other reasons
 */
esp_err_t esp_etm_channel_enable(esp_etm_channel_handle_t chan);

/**
 * @brief Disable ETM channel
 *
 * @note This function will transit the channel state from enable to init.
 *
 * @param[in] chan ETM channel handle that created by `esp_etm_new_channel`
 * @return
 *      - ESP_OK: Disable ETM channel successfully
 *      - ESP_ERR_INVALID_ARG: Disable ETM channel failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Disable ETM channel failed because the channel is not enabled yet
 *      - ESP_FAIL: Disable ETM channel failed because of other reasons
 */
esp_err_t esp_etm_channel_disable(esp_etm_channel_handle_t chan);

/**
 * @brief Connect an ETM event to an ETM task via a previously allocated ETM channel
 *
 * @note Setting the ETM event/task handle to NULL means to disconnect the channel from any event/task
 *
 * @param[in] chan ETM channel handle that created by `esp_etm_new_channel`
 * @param[in] event ETM event handle obtained from a driver/peripheral, e.g. `xxx_new_etm_event`
 * @param[in] task ETM task handle obtained from a driver/peripheral, e.g. `xxx_new_etm_task`
 * @return
 *      - ESP_OK: Connect ETM event and task to the channel successfully
 *      - ESP_ERR_INVALID_ARG: Connect ETM event and task to the channel failed because of invalid argument
 *      - ESP_FAIL: Connect ETM event and task to the channel failed because of other reasons
 */
esp_err_t esp_etm_channel_connect(esp_etm_channel_handle_t chan, esp_etm_event_handle_t event, esp_etm_task_handle_t task);

/**
 * @brief Delete ETM event
 *
 * @note Although the ETM event comes from various peripherals, we provide the same user API to delete the event handle seamlessly.
 *
 * @param[in] event ETM event handle obtained from a driver/peripheral, e.g. `xxx_new_etm_event`
 * @return
 *      - ESP_OK: Delete ETM event successfully
 *      - ESP_ERR_INVALID_ARG: Delete ETM event failed because of invalid argument
 *      - ESP_FAIL: Delete ETM event failed because of other reasons
 */
esp_err_t esp_etm_del_event(esp_etm_event_handle_t event);

/**
 * @brief Delete ETM task
 *
 * @note Although the ETM task comes from various peripherals, we provide the same user API to delete the task handle seamlessly.
 *
 * @param[in] task ETM task handle obtained from a driver/peripheral, e.g. `xxx_new_etm_task`
 * @return
 *      - ESP_OK: Delete ETM task successfully
 *      - ESP_ERR_INVALID_ARG: Delete ETM task failed because of invalid argument
 *      - ESP_FAIL: Delete ETM task failed because of other reasons
 */
esp_err_t esp_etm_del_task(esp_etm_task_handle_t task);

/**
 * @brief Dump ETM channel usages to the given IO stream
 *
 * @param[in] out_stream IO stream (e.g. stdout)
 * @return
 *      - ESP_OK: Dump ETM channel usages successfully
 *      - ESP_ERR_INVALID_ARG: Dump ETM channel usages failed because of invalid argument
 *      - ESP_FAIL: Dump ETM channel usages failed because of other reasons
 */
esp_err_t esp_etm_dump(FILE *out_stream);

#ifdef __cplusplus
}
#endif
