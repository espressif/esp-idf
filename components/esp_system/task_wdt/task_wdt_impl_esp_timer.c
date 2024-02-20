/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "hal/wdt_hal.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_debug_helpers.h"
#include "esp_timer.h"
#include "esp_private/esp_task_wdt_impl.h"

/**
 * Context for the software implementation of the Task WatchDog Timer.
 * This will be passed as a parameter to public functions below. */
typedef struct {
    esp_timer_handle_t sw_timer;
    uint32_t period_ms;
} twdt_ctx_soft_t;

/**
 * Declare the initial context as static. It will be passed to the
 * task_wdt implementation as the implementation context in the
 * init function. */
static twdt_ctx_soft_t init_context;

static const char *TAG = "task_wdt_impl_soft";

esp_err_t esp_task_wdt_impl_timer_allocate(const esp_task_wdt_config_t *config,
                                           twdt_isr_callback callback,
                                           twdt_ctx_t *obj)
{
    twdt_ctx_soft_t *ctx = &init_context;
    const esp_timer_create_args_t timer_args = {
        .callback = callback,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_ISR,
        .name = "Task software watchdog",
        .skip_unhandled_events = true
    };

    /* Software Task timer. As we don't have a spare hardware watchdog timer, we will use esp_timer to simulate one */
    esp_err_t ret = esp_timer_create(&timer_args, &ctx->sw_timer);
    ESP_GOTO_ON_FALSE((ret == ESP_OK), ret, reterr, TAG, "could not start periodic timer");

    /* Configure it as a periodic timer, so that we check the Tasks everytime it is triggered.
     * No need to start the timer here, it will be started later with `esp_task_wdt_impl_timer_restart` */
    ctx->period_ms = config->timeout_ms;

    /* Return our context to the caller */
    *obj = (twdt_ctx_t) ctx;

reterr:
    return ret;
}

esp_err_t esp_task_wdt_impl_timer_reconfigure(twdt_ctx_t obj, const esp_task_wdt_config_t *config)
{
    esp_err_t ret = ESP_OK;
    twdt_ctx_soft_t* ctx = (twdt_ctx_soft_t*) obj;

    if (config == NULL || ctx == NULL) {
        ret = ESP_ERR_INVALID_STATE;
    }

    if (ret == ESP_OK) {
        /* The timer is stopped, we only need to update the period in our context, next time we start the
         * timer with `esp_task_wdt_impl_timer_restart`, we will pass the context's period to the
         * underlying esp_timer instance. */
        ctx->period_ms = config->timeout_ms;
    }

    return ret;
}

void esp_task_wdt_impl_timer_free(twdt_ctx_t obj)
{
    const twdt_ctx_soft_t* ctx = (twdt_ctx_soft_t*) obj;

    if (ctx != NULL && ctx->sw_timer != NULL) {
        ESP_ERROR_CHECK(esp_timer_delete(ctx->sw_timer));
    }
}

esp_err_t esp_task_wdt_impl_timer_feed(twdt_ctx_t obj)
{
    esp_err_t ret = ESP_OK;
    const twdt_ctx_soft_t* ctx = (twdt_ctx_soft_t*) obj;

    if (ctx == NULL) {
        ret = ESP_ERR_INVALID_STATE;
    }

    if (ret == ESP_OK) {
        /* Feed the periodic timer by restarting it, specifying the same period */
        ret = esp_timer_restart(ctx->sw_timer, ctx->period_ms * 1000);
    }

    return ret;
}

void esp_task_wdt_impl_timeout_triggered(twdt_ctx_t obj)
{
    (void) obj;
}

esp_err_t esp_task_wdt_impl_timer_stop(twdt_ctx_t obj)
{
    esp_err_t ret = ESP_OK;
    const twdt_ctx_soft_t* ctx = (twdt_ctx_soft_t*) obj;

    if (ctx == NULL || ctx->sw_timer == NULL) {
        ret = ESP_ERR_INVALID_STATE;
    }

    if (ret == ESP_OK) {
        ret = esp_timer_stop(ctx->sw_timer);
    }

    return ret;
}

esp_err_t esp_task_wdt_impl_timer_restart(twdt_ctx_t obj)
{
    esp_err_t ret = ESP_OK;
    twdt_ctx_soft_t* ctx = (twdt_ctx_soft_t*) obj;

    if (ctx == NULL || ctx->sw_timer == NULL) {
        ret = ESP_ERR_INVALID_STATE;
    }

    if (ret == ESP_OK) {
        esp_timer_start_periodic(ctx->sw_timer, ctx->period_ms * 1000);
    }

    return ret;
}
