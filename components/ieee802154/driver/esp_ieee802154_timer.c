/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "hal/ieee802154_ll.h"
#include "esp_check.h"
#include "esp_ieee802154_timer.h"
#include "esp_ieee802154_util.h"
#include <stdbool.h>

static timer_callback_t s_timer0_callback = NULL;
static timer_callback_t s_timer1_callback = NULL;
static void* s_timer0_ctx = NULL;
static void* s_timer1_ctx = NULL;

void ieee802154_timer0_start(void)
{
    ieee802154_ll_set_cmd(IEEE802154_CMD_TIMER0_START);
}

void ieee802154_timer0_stop(void)
{
    s_timer0_callback = NULL;
    s_timer0_ctx = NULL;
    ieee802154_ll_set_cmd(IEEE802154_CMD_TIMER0_STOP);
}

esp_err_t ieee802154_timer0_set_threshold(uint32_t value)
{
    ESP_RETURN_ON_FALSE((value < IEEE802154_TIMER0_THRESHOLD), ESP_ERR_INVALID_ARG, IEEE802154_TAG, "invalid timer0 threshold");

    ieee802154_ll_timer0_set_threshold(value);

    return ESP_OK;
}

uint32_t ieee802154_timer0_get_value(void)
{
    return ieee802154_ll_timer0_get_value();
}

void ieee802154_timer1_start(void)
{
    ieee802154_ll_set_cmd(IEEE802154_CMD_TIMER1_START);
}

void ieee802154_timer1_stop(void)
{
    s_timer1_callback = NULL;
    s_timer1_ctx = NULL;
    ieee802154_ll_set_cmd(IEEE802154_CMD_TIMER1_STOP);
}

esp_err_t ieee802154_timer1_set_threshold(uint32_t value)
{
    ESP_RETURN_ON_FALSE((value < IEEE802154_TIMER1_THRESHOLD), ESP_ERR_INVALID_ARG, IEEE802154_TAG, "invalid timer1 threshold");

    ieee802154_ll_timer1_set_threshold(value);

    return ESP_OK;
}

uint32_t ieee802154_timer1_get_value(void)
{
    return ieee802154_ll_timer1_get_value();
}

FORCE_INLINE_ATTR bool is_target_time_expired(uint32_t target, uint32_t now)
{
    return (((now - target) & (1 << 31)) == 0);
}

void ieee802154_timer0_fire_at(uint32_t fire_time)
{
    uint32_t current_time = (uint32_t)esp_timer_get_time();
    ieee802154_timer0_set_threshold((is_target_time_expired(fire_time, current_time) ? 0 : (fire_time - current_time))); //uint: 1us
    ieee802154_timer0_start();
}

void ieee802154_timer1_fire_at(uint32_t fire_time)
{
    uint32_t current_time = (uint32_t)esp_timer_get_time();
    ieee802154_timer1_set_threshold((is_target_time_expired(fire_time, current_time) ? 0 : (fire_time - current_time))); //uint: 1us
    ieee802154_timer1_start();
}

void ieee802154_timer0_set_callback(timer_callback_t timer0_cb, void *timer0_ctx)
{
    s_timer0_callback = timer0_cb;
    s_timer0_ctx = timer0_ctx;
}

void ieee802154_timer1_set_callback(timer_callback_t timer1_cb, void *timer1_ctx)
{
    s_timer1_callback = timer1_cb;
    s_timer1_ctx = timer1_ctx;
}

void ieee802154_timer0_fire_at_with_callback(uint32_t fire_time, timer_callback_t timer0_cb, void *timer0_ctx)
{
    ieee802154_timer0_set_callback(timer0_cb, timer0_ctx);
    ieee802154_timer0_fire_at(fire_time);
}

void ieee802154_timer1_fire_at_with_callback(uint32_t fire_time, timer_callback_t timer1_cb, void *timer1_ctx)
{
    ieee802154_timer1_set_callback(timer1_cb, timer1_ctx);
    ieee802154_timer1_fire_at(fire_time);
}

void isr_handle_timer0_done(void)
{
    if (s_timer0_callback) {
        timer_callback_t callback_tmp = s_timer0_callback; // tmp function used to allow new callback function to set a new callback without overwriting it
        void* ctx_tmp = s_timer0_ctx;

        s_timer0_callback = NULL;
        s_timer0_ctx = NULL;

        callback_tmp(ctx_tmp);
    }
}

void isr_handle_timer1_done(void)
{
    if (s_timer1_callback) {
        timer_callback_t callback_tmp = s_timer1_callback; // tmp function used to allow new callback function to set a new callback without overwriting it
        void* ctx_tmp = s_timer1_ctx;

        s_timer1_callback = NULL;
        s_timer1_ctx = NULL;

        callback_tmp(ctx_tmp);
    }
}
