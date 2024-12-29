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
#include "hal/mwdt_ll.h"
#include "hal/timer_ll.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_private/system_internal.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_task_wdt_impl.h"

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_TIMER_SUPPORT_SLEEP_RETENTION
#include "esp_private/sleep_retention.h"
#endif

#define TWDT_INSTANCE           WDT_MWDT0
#define TWDT_TICKS_PER_US       500
#define TWDT_PRESCALER          MWDT_LL_DEFAULT_CLK_PRESCALER   // Tick period of 500us if WDT source clock is 80MHz
#define TWDT_PERIPH_MODULE      PERIPH_TIMG0_MODULE
#define TWDT_TIMER_GROUP        0
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

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_TIMER_SUPPORT_SLEEP_RETENTION
static const char* TAG = "task_wdt";
static esp_err_t sleep_task_wdt_retention_init(void *arg)
{
    uint32_t group_id = *(uint32_t *)arg;
    esp_err_t err = sleep_retention_entries_create(tg_wdt_regs_retention[group_id].link_list,
                                                   tg_wdt_regs_retention[group_id].link_num,
                                                   REGDMA_LINK_PRI_SYS_PERIPH_LOW,
                                                   (group_id == 0) ? SLEEP_RETENTION_MODULE_TG0_WDT : SLEEP_RETENTION_MODULE_TG1_WDT);
    if (err == ESP_OK) {
        ESP_LOGD(TAG, "Task watchdog timer retention initialization");
    }
    ESP_RETURN_ON_ERROR(err, TAG, "Failed to create sleep retention linked list for task watchdog timer");
    return err;
}

static esp_err_t esp_task_wdt_retention_enable(uint32_t group_id)
{
    sleep_retention_module_init_param_t init_param = {
        .cbs = { .create = { .handle = sleep_task_wdt_retention_init, .arg = &group_id } },
        .depends = RETENTION_MODULE_BITMAP_INIT(CLOCK_SYSTEM)
    };
    esp_err_t err = sleep_retention_module_init((group_id == 0) ? SLEEP_RETENTION_MODULE_TG0_WDT : SLEEP_RETENTION_MODULE_TG1_WDT, &init_param);
    if (err == ESP_OK) {
        err = sleep_retention_module_allocate((group_id == 0) ? SLEEP_RETENTION_MODULE_TG0_WDT : SLEEP_RETENTION_MODULE_TG1_WDT);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "Failed to allocate sleep retention linked list for task watchdog timer retention");
        }
    }
    return err;
}

static esp_err_t esp_task_wdt_retention_disable(uint32_t group_id)
{
    esp_err_t err = sleep_retention_module_free((group_id == 0) ? SLEEP_RETENTION_MODULE_TG0_WDT : SLEEP_RETENTION_MODULE_TG1_WDT);
    if (err == ESP_OK) {
        err = sleep_retention_module_deinit((group_id == 0) ? SLEEP_RETENTION_MODULE_TG0_WDT : SLEEP_RETENTION_MODULE_TG1_WDT);
    }
    return err;
}
#endif

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
        // enable bus clock for the timer group registers
        PERIPH_RCC_ACQUIRE_ATOMIC(TWDT_PERIPH_MODULE, ref_count) {
            if (ref_count == 0) {
                timer_ll_enable_bus_clock(TWDT_TIMER_GROUP, true);
                timer_ll_reset_register(TWDT_TIMER_GROUP);
            }
        }
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

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_TIMER_SUPPORT_SLEEP_RETENTION
        esp_task_wdt_retention_enable(TWDT_TIMER_GROUP);
#endif
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
        PERIPH_RCC_RELEASE_ATOMIC(TWDT_PERIPH_MODULE, ref_count) {
            if (ref_count == 0) {
                timer_ll_enable_bus_clock(TWDT_TIMER_GROUP, false);
            }
        }

        /* Deregister interrupt */
        ESP_ERROR_CHECK(esp_intr_free(ctx->intr_handle));

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_TIMER_SUPPORT_SLEEP_RETENTION
        ESP_ERROR_CHECK(esp_task_wdt_retention_disable(TWDT_TIMER_GROUP));
#endif
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
