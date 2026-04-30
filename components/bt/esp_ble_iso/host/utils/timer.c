/*
 * SPDX-FileCopyrightText: 2016 Intel Corporation
 * SPDX-FileCopyrightText: 2016 Wind River Systems, Inc.
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "esp_timer.h"

#include <zephyr/kernel.h>
#include <zephyr/autoconf.h>
#include <zephyr/toolchain.h>
#include <zephyr/logging/log.h>

#include "common/host.h"

static void iso_timer_cb(void *arg)
{
    struct k_work *work = arg;
    int err;

    LOG_DBG("IsoTimerCb[%p]", work);

    assert(work);
    assert(work->timer);
    assert(work->handler);

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_TIMER_EVENT, work, 0);
    if (err) {
        LOG_ERR("IsoTimerPostFail[%d]", err);
    }
}

int k_work_submit(struct k_work *work)
{
    LOG_DBG("WorkSubmit[%p]", work);

    assert(work);

    if (work->handler == NULL) {
        LOG_WRN("WorkHdlrNull");
        return -EINVAL;
    }

    work->handler(work);

    return 0;
}

int k_work_submit_safe(struct k_work *work)
{
    int err;
    bt_le_host_lock();
    err = k_work_submit(work);
    bt_le_host_unlock();
    return err;
}

bool k_work_is_pending(struct k_work *work)
{
    bool is_pending;

    LOG_DBG("WorkIsPending[%p]", work);

    assert(work);

    if (work->handler == NULL) {
        LOG_WRN("WorkHdlrNull");
        return false;
    }

    is_pending = (work->timer ? esp_timer_is_active(work->timer) : false);

    LOG_DBG("%sPending", is_pending ? "Is" : "Not");

    return is_pending;
}

void k_work_init(struct k_work *work, k_work_handler_t handler)
{
    LOG_DBG("WorkInit[%p]", work);

    assert(work);

    work->handler = handler;
}

struct k_work_delayable *k_work_delayable_from_work(struct k_work *work)
{
    LOG_DBG("DworkFromWork[%p]", work);

    assert(work);

    return (struct k_work_delayable *)work;
}

void k_work_init_delayable(struct k_work_delayable *dwork,
                           k_work_handler_t handler)
{
    LOG_DBG("DworkInit[%p]", dwork);

    assert(dwork);

    const esp_timer_create_args_t timer_args = {
        .callback = &iso_timer_cb,
        .arg = &dwork->work,
        .name = "iso_timer",
    };
    int err;

    if (dwork->work.timer) {
        LOG_WRN("TimerCreated");
        return;
    }

    err = esp_timer_create(&timer_args, (esp_timer_handle_t *)&dwork->work.timer);
    if (err) {
        LOG_ERR("CreateTimerFail[%d]", err);
        /* Reset handler so the object remains in a clean uninitialized state */
        dwork->work.handler = NULL;
        return;
    }

    dwork->work.handler = handler;
}

void k_work_deinit_delayable(struct k_work_delayable *dwork)
{
    int err;

    LOG_DBG("DworkDeinit[%p]", dwork);

    assert(dwork);

    if (dwork->work.timer == NULL) {
        LOG_INF("TimerNotCreated");
        return;
    }

    err = esp_timer_delete(dwork->work.timer);
    if (err) {
        LOG_ERR("DeleteTimerFail[%d]", err);
        return;
    }

    memset(&dwork->work, 0, sizeof(dwork->work));
}

int k_work_cancel_delayable(struct k_work_delayable *dwork)
{
    LOG_DBG("DworkCancel[%p]", dwork);

    assert(dwork);

    if (dwork->work.timer == NULL) {
        LOG_INF("TimerNotCreated");
        return -EINVAL;
    }

    esp_timer_stop(dwork->work.timer);

    return 0;
}

bool k_work_cancel_delayable_sync(struct k_work_delayable *dwork,
                                  struct k_work_sync *sync)
{
    LOG_DBG("DworkCancelSync[%p]", dwork);

    assert(dwork);

    ARG_UNUSED(sync);

    if (dwork->work.timer == NULL) {
        LOG_INF("TimerNotCreated");
        return false;
    }

    esp_timer_stop(dwork->work.timer);

    /* TODO: check the return result */
    return false;
}

int k_work_schedule(struct k_work_delayable *dwork, k_timeout_t ms)
{
    int err;

    LOG_DBG("WorkSchedule[%p][%u]", dwork, ms);

    assert(dwork);

    if (dwork->work.timer == NULL) {
        LOG_WRN("TimerNotCreated");
        return -EINVAL;
    }

    esp_timer_stop(dwork->work.timer);

    dwork->work.timeout_us = esp_timer_get_time() + (int64_t)ms * 1000;

    if (ms == K_NO_WAIT) {
        k_work_submit(&dwork->work);
        return 0;
    }

    err = esp_timer_start_once(dwork->work.timer, ms * 1000);
    if (err) {
        LOG_ERR("StartTimerFail[%d]", err);
        return -EIO;
    }

    return 0;
}

int k_work_reschedule(struct k_work_delayable *dwork, k_timeout_t ms)
{
    int err;

    LOG_DBG("WorkReschedule[%p][%u]", dwork, ms);

    assert(dwork);

    if (dwork->work.timer == NULL) {
        LOG_WRN("TimerNotCreated");
        return -EINVAL;
    }

    esp_timer_stop(dwork->work.timer);

    dwork->work.timeout_us = esp_timer_get_time() + (int64_t)ms * 1000;

    if (ms == K_NO_WAIT) {
        k_work_submit(&dwork->work);
        return 0;
    }

    err = esp_timer_start_once(dwork->work.timer, ms * 1000);
    if (err) {
        LOG_ERR("RestartTimerFail[%d]", err);
        return -EIO;
    }

    return 0;
}

int k_work_schedule_periodic(struct k_work_delayable *dwork, k_timeout_t period_ms)
{
    int err;

    LOG_DBG("WorkSchedulePeriodic[%p][%u]", dwork, period_ms);

    assert(dwork);
    assert(period_ms > 0);

    if (dwork->work.timer == NULL) {
        LOG_WRN("TimerNotCreated");
        return -EINVAL;
    }

    esp_timer_stop(dwork->work.timer);

    err = esp_timer_start_periodic(dwork->work.timer, (uint64_t)period_ms * 1000);
    if (err) {
        LOG_ERR("StartPeriodicTimerFail[%d]", err);
        return -EIO;
    }

    return 0;
}

k_timeout_t k_work_delayable_remaining_get(struct k_work_delayable *dwork)
{
    k_timeout_t timeout;
    int64_t delta_us;

    LOG_DBG("DworkRemainingGet[%p]", dwork);

    assert(dwork);

    if (dwork->work.timer == NULL) {
        LOG_WRN("TimerNotCreated");
        return 0;
    }

    if (dwork->work.timeout_us == 0) {
        LOG_DBG("WorkTimeoutZero");
        return 0;
    }

    delta_us = dwork->work.timeout_us - esp_timer_get_time();

    timeout = (delta_us > 0 ? (k_timeout_t)(delta_us / 1000) : 0);

    LOG_DBG("Timeout[%u]", timeout);

    return timeout;
}

void bt_le_timer_handle_event(void *arg)
{
    struct k_work *work = arg;

    LOG_DBG("HdlTimerEvt[%p]", work);

    k_work_submit_safe(work);
}
