/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "hal/wdt_hal.h"
#include "hal/mwdt_ll.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_private/system_internal.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_task_wdt_impl.h"

#define TWDT_INSTANCE           WDT_MWDT0
#define TWDT_TICKS_PER_US       500
#define TWDT_PRESCALER          MWDT_LL_DEFAULT_CLK_PRESCALER   // Tick period of 500us if WDT source clock is 80MHz
#define TWDT_PERIPH_MODULE      PERIPH_TIMG0_MODULE
#define TWDT_INTR_SOURCE        ETS_TG0_WDT_LEVEL_INTR_SOURCE

/**
 * Context for the software implementation of the Task WatchDog Timer.
 * This will be passed as a parameter to public functions below. */
typedef struct {
    wdt_hal_context_t hal;
    intr_handle_t intr_handle;
} twdt_ctx_hard_t;

/**
 * Declare the initial context as static. It will be passed to the
 * task_wdt implementation as the implementation context in the
 * init function. */
static twdt_ctx_hard_t init_context;



esp_err_t esp_task_wdt_impl_timer_allocate(const esp_task_wdt_config_t *config,
                                           twdt_isr_callback callback,
                                           twdt_ctx_t *obj)
{
    esp_err_t ret = ESP_OK;
    twdt_ctx_hard_t *ctx = &init_context;

    if (config == NULL || obj == NULL) {
        ret = ESP_ERR_INVALID_STATE;
    }

    if (ret == ESP_OK) {
        esp_intr_alloc(TWDT_INTR_SOURCE, 0, callback, NULL, &ctx->intr_handle);
    }

    if (ret == ESP_OK) {
        periph_module_enable(TWDT_PERIPH_MODULE);
        wdt_hal_init(&ctx->hal, TWDT_INSTANCE, TWDT_PRESCALER, true);

        wdt_hal_write_protect_disable(&ctx->hal);
        // Configure 1st stage timeout and behavior
        wdt_hal_config_stage(&ctx->hal, WDT_STAGE0, config->timeout_ms * (1000 / TWDT_TICKS_PER_US), WDT_STAGE_ACTION_INT);
        // Configure 2nd stage timeout and behavior
        wdt_hal_config_stage(&ctx->hal, WDT_STAGE1, config->timeout_ms * (2 * 1000 / TWDT_TICKS_PER_US), WDT_STAGE_ACTION_RESET_SYSTEM);
        // No need to enable to enable the WDT here, it will be enabled with `esp_task_wdt_impl_timer_restart`
        wdt_hal_write_protect_enable(&ctx->hal);

        /* Return the implementation context to the caller */
        *obj = (twdt_ctx_t) ctx;
    }

    return ret;
}


esp_err_t esp_task_wdt_impl_timer_reconfigure(twdt_ctx_t obj, const esp_task_wdt_config_t *config)
{
    esp_err_t ret = ESP_OK;

    twdt_ctx_hard_t* ctx = (twdt_ctx_hard_t*) obj;

    if (config == NULL || ctx == NULL) {
        ret = ESP_ERR_INVALID_STATE;
    }

    if (ret == ESP_OK) {
        wdt_hal_write_protect_disable(&ctx->hal);
        /* Reconfigure the 1st and 2nd stage timeout */
        wdt_hal_config_stage(&ctx->hal, WDT_STAGE0, config->timeout_ms * (1000 / TWDT_TICKS_PER_US), WDT_STAGE_ACTION_INT);
        wdt_hal_config_stage(&ctx->hal, WDT_STAGE1, config->timeout_ms * (2 * 1000 / TWDT_TICKS_PER_US), WDT_STAGE_ACTION_RESET_SYSTEM);
        wdt_hal_write_protect_enable(&ctx->hal);
    }

    return ret;
}


void esp_task_wdt_impl_timer_free(twdt_ctx_t obj)
{
    twdt_ctx_hard_t* ctx = (twdt_ctx_hard_t*) obj;

    if (ctx != NULL) {
        /* Stop hardware timer and the interrupt associated */
        wdt_hal_deinit(&ctx->hal);
        ESP_ERROR_CHECK(esp_intr_disable(ctx->intr_handle));

        /* Disable the Timer Group module */
        periph_module_disable(TWDT_PERIPH_MODULE);

        /* Deregister interrupt */
        ESP_ERROR_CHECK(esp_intr_free(ctx->intr_handle));
    }
}


esp_err_t esp_task_wdt_impl_timer_feed(twdt_ctx_t obj)
{
    esp_err_t ret = ESP_OK;
    twdt_ctx_hard_t* ctx = (twdt_ctx_hard_t*) obj;

    if (ctx == NULL) {
        ret = ESP_ERR_INVALID_STATE;
    }

    if (ret == ESP_OK) {
        wdt_hal_write_protect_disable(&ctx->hal);
        wdt_hal_feed(&ctx->hal);
        wdt_hal_write_protect_enable(&ctx->hal);
    }

    return ret;
}


void esp_task_wdt_impl_timeout_triggered(twdt_ctx_t obj)
{
    twdt_ctx_hard_t* ctx = (twdt_ctx_hard_t*) obj;

    if (ctx != NULL) {
        /* Reset hardware timer so that 2nd stage timeout is not reached (will trigger system reset) */
        wdt_hal_write_protect_disable(&ctx->hal);
        wdt_hal_handle_intr(&ctx->hal);  // Feeds WDT and clears acknowledges interrupt
        wdt_hal_write_protect_enable(&ctx->hal);
    }
}


esp_err_t esp_task_wdt_impl_timer_stop(twdt_ctx_t obj)
{
    esp_err_t ret = ESP_OK;
    twdt_ctx_hard_t* ctx = (twdt_ctx_hard_t*) obj;

    if (ctx == NULL) {
        ret = ESP_ERR_INVALID_STATE;
    }

    if (ret == ESP_OK) {
        wdt_hal_write_protect_disable(&ctx->hal);
        wdt_hal_disable(&ctx->hal);
        wdt_hal_write_protect_enable(&ctx->hal);
    }

    return ret;
}


esp_err_t esp_task_wdt_impl_timer_restart(twdt_ctx_t obj)
{
    esp_err_t ret = ESP_OK;
    twdt_ctx_hard_t* ctx = (twdt_ctx_hard_t*) obj;

    if (ctx == NULL) {
        ret = ESP_ERR_INVALID_STATE;
    }

    if (ret == ESP_OK) {
        wdt_hal_write_protect_disable(&ctx->hal);
        wdt_hal_enable(&ctx->hal);
        wdt_hal_feed(&ctx->hal);
        wdt_hal_write_protect_enable(&ctx->hal);
    }

    return ret;
}
