/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_log.h"
#include "esp_err.h"

/**
 * @brief  Start the IEEE802154 MAC internal timer0.
 *
 */
void ieee802154_timer0_start(void);

/**
 * @brief  Stop the IEEE802154 MAC internal timer0.
 *
 */
void ieee802154_timer0_stop(void);

/**
 * @brief  Set the overflow threshold to the IEEE802154 MAC internal timer0.
 *
 * @param[in]  value  The threshold.
 *
 * @return
 *          ESP_OK if the threshold is valid.
 *          ESP_ERR_INVALID_ARG if the threshold is invalid.
 *
 */
esp_err_t ieee802154_timer0_set_threshold(uint32_t value);

/**
 * @brief  Get the current IEEE802154 MAC internal timer0 count.
 *
 * @return
 *          The timer0 count.
 *
 */
uint32_t ieee802154_timer0_get_value(void);

/**
 * @brief  Start the IEEE802154 MAC internal timer1.
 *
 */
void ieee802154_timer1_start(void);

/**
 * @brief  Stop the IEEE802154 MAC internal timer1.
 *
 */
void ieee802154_timer1_stop(void);

/**
 * @brief  Set the overflow threshold to the IEEE802154 MAC internal timer1.
 *
 * @param[in]  value  The threshold.
 *
 * @return
 *          ESP_OK if the threshold is valid.
 *          ESP_ERR_INVALID_ARG if the threshold is invalid.
 *
 */
esp_err_t ieee802154_timer1_set_threshold(uint32_t value);

/**
 * @brief  Get the current IEEE802154 MAC internal timer1 count.
 *
 * @return
 *          The timer1 count.
 *
 */
uint32_t ieee802154_timer1_get_value(void);

/**
 * @brief  Set timer0 to fire at specific time.
 *
 */
void ieee802154_timer0_fire_at(uint32_t fire_time);

/**
 * @brief  Set timer1 to fire at specific time.
 *
 */
void ieee802154_timer1_fire_at(uint32_t fire_time);

typedef void (*timer_callback_t)(void* ctx);

/**
 * @brief  Set timer0 callback.
 *
 */
void ieee802154_timer0_set_callback(timer_callback_t timer0_cb, void *timer0_ctx);

/**
 * @brief  Set timer1 callback.
 *
 */
void ieee802154_timer1_set_callback(timer_callback_t timer1_cb, void *timer1_ctx);

/**
 * @brief  Set timer0 to fire at specific time with callback.
 *
 */
void ieee802154_timer0_fire_at_with_callback(uint32_t fire_time, timer_callback_t timer0_callback, void *timer0_ctx);

/**
 * @brief  Set timer1 to fire at specific time with callback.
 *
 */
void ieee802154_timer1_fire_at_with_callback(uint32_t fire_time, timer_callback_t timer1_callback, void *timer1_ctx);

/**
 * @brief  ISR handle for timer0.
 *
 */
void isr_handle_timer0_done(void);

/**
 * @brief  ISR handle for timer1.
 *
 */
void isr_handle_timer1_done(void);

#ifdef __cplusplus
}
#endif
