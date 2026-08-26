/*
 * SPDX-FileCopyrightText: 2018-2020 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2020-2022 Espressif Systems (Shanghai) CO LTD
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "esp_log.h"

#include "genie_mesh.h"
#include "genie_util.h"

static const char *TAG = "genie_util";

void util_timer_init(util_timer_t *timer, util_timer_handler_t handle, void *args)
{
    ENTER_FUNC();
    int ret = 0;

    if (NULL == timer) {
        ESP_LOGE(TAG, "timer is NULL");
        return;
    }

    ESP_LOGD(TAG, "timer: %p, handle: %p, args: %p", timer, handle, args);

    esp_timer_create_args_t create_args = {
        .callback = handle,
        .arg      = args,
        .name     = "genie_timer"
    };

    timer->handler = handle;
    timer->args    = args;
    timer->timeout = 0;

    ret = esp_timer_create(&create_args, &timer->timer);

    if (ret) {
        ESP_LOGE(TAG, "fail to create a timer, err: %d", ret);
    }
}

void util_timer_start(util_timer_t *timer, uint32_t timeout)
{
    ENTER_FUNC();
    int ret = 0;

    if (NULL == timer) {
        ESP_LOGE(TAG, "timer is NULL");
        return;
    }

    ESP_LOGD(TAG, "timer: %p, timeout: %" PRIu32, timer, timeout);

    util_timer_stop(timer);

    timer->timeout  = timeout;
    timer->start_ms = (uint32_t)esp_timer_get_time();

    ret = esp_timer_start_once(timer->timer, timeout * 1000);

    if (ret) {
        ESP_LOGE(TAG, "fail to change timeout and start timer, err: %d", ret);
    }
}

void util_timer_stop(util_timer_t *timer)
{
    ENTER_FUNC();
    int ret = 0;

    if (NULL == timer) {
        ESP_LOGE(TAG, "timer is NULL");
        return;
    }
    /**
     * Timer may be reused, so its timeout value
     * should be cleared when stopped.
     */
    if (!timer->timeout) {
        return;
    }

    ESP_LOGD(TAG, "timer: %p", timer);
    ret = esp_timer_stop(timer->timer);
    if (ret) {
        ESP_LOGD(TAG, "fail to stop timer, err: %d", ret);
    }

    timer->timeout = 0;
}

bool util_timer_is_started(util_timer_t *timer)
{
    ENTER_FUNC();

    if (NULL == timer) {
        ESP_LOGE(TAG, "timer is NULL");
        return false;
    }

    return timer->timeout ? true : false;
}

int util_semaphore_init(util_semaphore_t *sem, uint32_t initial_count,
               uint32_t limit)
{
    ENTER_FUNC();
    int ret = 0;

    if (NULL == sem) {
        ESP_LOGE(TAG, "sem is NULL");
        return -EINVAL;
    }

    sem->sem = xSemaphoreCreateCounting(limit, initial_count);

    return ret;
}

int util_semaphore_take(util_semaphore_t *sem, uint32_t timeout)
{
    ENTER_FUNC();
    TickType_t ticks = 0;

    if (timeout == K_FOREVER) {
        ticks = portMAX_DELAY;
    } else {
        ticks = pdMS_TO_TICKS(timeout);
    }

    return xSemaphoreTake(sem->sem, ticks);
}

int util_semaphore_give(util_semaphore_t *sem)
{
    ENTER_FUNC();
    if (NULL == sem) {
        ESP_LOGE(TAG, "sem is NULL");
        return -EINVAL;
    }

    xSemaphoreGive(sem->sem);

    return 0;
}

int util_semaphore_delete(util_semaphore_t *sem)
{
    ENTER_FUNC();
    if (NULL == sem) {
        ESP_LOGE(TAG, "sem is NULL");
        return -EINVAL;
    }

    vSemaphoreDelete(sem->sem);

    return 0;
}

uint32_t util_semaphore_count_get(util_semaphore_t *sem)
{
    ENTER_FUNC();
    uint32_t count = 0;

    count = uxSemaphoreGetCount(sem->sem);

    return count;
}

const char *util_hex2str(const void *buf, size_t len)
{
    ENTER_FUNC();
    int    i                = 0;
    const  uint8_t *b       = buf;
    static char str[129]    = {0};
    static const char hex[] = "0123456789abcdef";

    len = MIN(len, (sizeof(str) - 1) / 2);

    for (i = 0; i < len; i++) {
        str[i * 2]     = hex[b[i] >> 4];
        str[i * 2 + 1] = hex[b[i] & 0xf];
    }

    str[i * 2] = '\0';

    return str;
}
