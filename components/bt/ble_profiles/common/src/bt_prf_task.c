/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>

#include "sdkconfig.h"
#include "esp_log.h"
#include "bt_osal.h"
#include "bt_osal_freertos.h"
#include "bt_prf_task.h"

static const char *TAG = "bt_prf_task";

/**
 * @brief Internal state of the shared BLE profile task
 *
 * Allocated in bt_prf_task_init() and freed in bt_prf_task_deinit(). The
 * pointer @ref s_prf_task is written only during single-threaded bring-up and
 * tear-down and read (never written) by bt_prf_task_post(); the documented
 * lifecycle contract (init happens-before any post, deinit happens-after the
 * last post) makes it safe without a lock.
 */
typedef struct {
    struct bt_osal_eventq evq; /*!< Shared event queue served by the worker task */
    bool running;             /*!< true once the worker task has been started */
} bt_prf_task_ctx_t;

static bt_prf_task_ctx_t *s_prf_task;

bt_osal_error_t bt_prf_task_init(void)
{
    bt_prf_task_ctx_t *ctx;
    bt_osal_error_t rc;

    /* Idempotent: a second bring-up while already running is a no-op. */
    if (s_prf_task != NULL) {
        return BT_OSAL_OK;
    }

    ctx = calloc(1, sizeof(*ctx));
    if (ctx == NULL) {
        ESP_LOGE(TAG, "no memory for profile task context");
        return BT_OSAL_ENOMEM;
    }

    /* Allocates the queue's backing storage; asserts internally on OOM. */
    bt_osal_eventq_init(&ctx->evq);

    /* Kconfig encodes "any core" as -1; the OSAL wants its own sentinel. */
    struct bt_osal_task_info task_info = {
        .name = "bt_profiles",
        .prio = CONFIG_BT_PRF_TASK_PRIORITY,
        .stack_size = CONFIG_BT_PRF_TASK_STACK_SIZE,
        .core_id = (CONFIG_BT_PRF_TASK_CORE_ID < 0) ? BT_OSAL_TASK_NO_AFFINITY
                                                    : CONFIG_BT_PRF_TASK_CORE_ID,
    };
    rc = bt_osal_eventq_start(&ctx->evq, &task_info);
    if (rc != BT_OSAL_OK) {
        ESP_LOGE(TAG, "failed to start profile task: %d", rc);
        bt_osal_eventq_deinit(&ctx->evq);
        free(ctx);
        return rc;
    }

    ctx->running = true;
    s_prf_task = ctx;

    ESP_LOGI(TAG, "profile task started (stack %d, prio %d)",
             CONFIG_BT_PRF_TASK_STACK_SIZE, CONFIG_BT_PRF_TASK_PRIORITY);
    return BT_OSAL_OK;
}

bt_osal_error_t bt_prf_task_deinit(void)
{
    bt_prf_task_ctx_t *ctx = s_prf_task;

    /* Symmetric no-op if bring-up never happened. */
    if (ctx == NULL) {
        return BT_OSAL_OK;
    }

    /* Publish the torn-down state before releasing resources so a stray
     * bt_prf_task_post() observes "not running" rather than a dangling queue. */
    s_prf_task = NULL;

    /* Stops (deletes) the worker task, then frees the queue's storage. */
    bt_osal_eventq_deinit(&ctx->evq);
    free(ctx);

    ESP_LOGI(TAG, "profile task stopped");
    return BT_OSAL_OK;
}

struct bt_osal_eventq *bt_prf_task_eventq(void)
{
    bt_prf_task_ctx_t *ctx = s_prf_task;

    return (ctx != NULL && ctx->running) ? &ctx->evq : NULL;
}

bool bt_prf_task_is_running(void)
{
    bt_prf_task_ctx_t *ctx = s_prf_task;

    return ctx != NULL && ctx->running;
}
