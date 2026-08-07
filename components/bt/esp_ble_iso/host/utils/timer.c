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

LOG_MODULE_REGISTER(ISO_TIMER, CONFIG_BT_ISO_LOG_LEVEL);

static void iso_timer_cb(void *arg)
{
    struct k_work *work = arg;
    int err;

    BT_LE_ASSERT(work);
    BT_LE_ASSERT(work->timer);
    BT_LE_ASSERT(work->handler);

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_TIMER_EVENT, work, work->gen);
    if (err) {
        ISO_POST_FAIL_LOG(err, "TimerCbPostFail[%d]", err);
    }
}

int k_work_submit(struct k_work *work)
{
    BT_LE_ASSERT(work);

    if (work->handler == NULL) {
        LOG_WRN("TimerSubmitHdlrNull");
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

    BT_LE_ASSERT(work);

    if (work->handler == NULL) {
        LOG_WRN("TimerIsPendingHdlrNull");
        return false;
    }

    is_pending = (work->timer ? esp_timer_is_active(work->timer) : false);

    LOG_DBG("TimerIsPendingRet[%s]", is_pending ? "Is" : "Not");

    return is_pending;
}

void k_work_init(struct k_work *work, k_work_handler_t handler)
{
    BT_LE_ASSERT(work);

    /* Clear timer/timeout_us/gen too: callers must not have to zero-init */
    memset(work, 0, sizeof(*work));

    work->handler = handler;
}

struct k_work_delayable *k_work_delayable_from_work(struct k_work *work)
{
    BT_LE_ASSERT(work);
    return (struct k_work_delayable *)work;
}

void k_work_init_delayable(struct k_work_delayable *dwork,
                           k_work_handler_t handler)
{
    BT_LE_ASSERT(dwork);

    const esp_timer_create_args_t timer_args = {
        .callback = &iso_timer_cb,
        .arg = &dwork->work,
        .name = "IsoTimer",
    };
    int err;

    if (dwork->work.timer) {
        LOG_WRN("TimerInitAlreadyCreated");
        return;
    }

    err = esp_timer_create(&timer_args, (esp_timer_handle_t *)&dwork->work.timer);
    if (err) {
        LOG_ERR("TimerInitCreateFail[%d]", err);
        /* Reset handler so the object remains in a clean uninitialized state */
        dwork->work.handler = NULL;
        return;
    }

    dwork->work.handler = handler;
}

void k_work_deinit_delayable(struct k_work_delayable *dwork)
{
    int err;

    BT_LE_ASSERT(dwork);

    if (dwork->work.timer == NULL) {
        LOG_INF("TimerDeinitNotCreated");
        return;
    }

    dwork->work.gen++;

    /* esp_timer_delete rejects a running timer */
    esp_timer_stop(dwork->work.timer);

    err = esp_timer_delete(dwork->work.timer);
    if (err) {
        LOG_ERR("TimerDeinitDelFail[%d]", err);
        return;
    }

    memset(&dwork->work, 0, sizeof(dwork->work));
}

int k_work_cancel_delayable(struct k_work_delayable *dwork)
{
    BT_LE_ASSERT(dwork);

    if (dwork->work.timer == NULL) {
        LOG_INF("TimerCancelNotCreated");
        return -EINVAL;
    }

    esp_timer_stop(dwork->work.timer);
    dwork->work.gen++;

    return 0;
}

bool k_work_cancel_delayable_sync(struct k_work_delayable *dwork,
                                  struct k_work_sync *sync)
{
    BT_LE_ASSERT(dwork);

    ARG_UNUSED(sync);

    if (dwork->work.timer == NULL) {
        LOG_INF("TimerCancelSyncNotCreated");
        return false;
    }

    esp_timer_stop(dwork->work.timer);

    dwork->work.gen++;

    /* TODO: check the return result */
    return false;
}

int k_work_schedule(struct k_work_delayable *dwork, k_timeout_t ms)
{
    int err;

    BT_LE_ASSERT(dwork);

    if (dwork->work.timer == NULL) {
        LOG_WRN("TimerSchNotCreated");
        return -EINVAL;
    }

    esp_timer_stop(dwork->work.timer);

    dwork->work.gen++;

    dwork->work.timeout_us = esp_timer_get_time() + (int64_t)ms * 1000;

    if (ms == K_NO_WAIT) {
        k_work_submit(&dwork->work);
        return 0;
    }

    err = esp_timer_start_once(dwork->work.timer, ms * 1000);
    if (err) {
        LOG_ERR("TimerSchStartFail[%d]", err);
        return -EIO;
    }

    return 0;
}

int k_work_reschedule(struct k_work_delayable *dwork, k_timeout_t ms)
{
    int err;

    BT_LE_ASSERT(dwork);

    if (dwork->work.timer == NULL) {
        LOG_WRN("TimerReschNotCreated");
        return -EINVAL;
    }

    esp_timer_stop(dwork->work.timer);

    dwork->work.gen++;

    dwork->work.timeout_us = esp_timer_get_time() + (int64_t)ms * 1000;

    if (ms == K_NO_WAIT) {
        k_work_submit(&dwork->work);
        return 0;
    }

    err = esp_timer_start_once(dwork->work.timer, ms * 1000);
    if (err) {
        LOG_ERR("TimerReschStartFail[%d]", err);
        return -EIO;
    }

    return 0;
}

int k_work_schedule_periodic_us(struct k_work_delayable *dwork, uint64_t period_us)
{
    int err;

    BT_LE_ASSERT(dwork);
    BT_LE_ASSERT(period_us > 0);

    if (dwork->work.timer == NULL) {
        LOG_WRN("TimerPeriodicNotCreated");
        return -EINVAL;
    }

    esp_timer_stop(dwork->work.timer);

    dwork->work.gen++;

    err = esp_timer_start_periodic(dwork->work.timer, period_us);
    if (err) {
        LOG_ERR("TimerPeriodicStartFail[%d]", err);
        return -EIO;
    }

    return 0;
}

int k_work_schedule_periodic(struct k_work_delayable *dwork, k_timeout_t period_ms)
{
    /* Sub-millisecond ISO SDU intervals (e.g. 7500, 8163, 10884 us) lose
     * precision when forced to whole milliseconds; callers that need the exact
     * interval should use k_work_schedule_periodic_us(). */
    return k_work_schedule_periodic_us(dwork, (uint64_t)period_ms * 1000);
}

k_timeout_t k_work_delayable_remaining_get(struct k_work_delayable *dwork)
{
    k_timeout_t timeout;
    int64_t delta_us;

    BT_LE_ASSERT(dwork);

    if (dwork->work.timer == NULL) {
        LOG_WRN("TimerRemainingNotCreated");
        return 0;
    }

    if (dwork->work.timeout_us == 0) {
        LOG_DBG("TimerRemainingTimeoutZero");
        return 0;
    }

    delta_us = dwork->work.timeout_us - esp_timer_get_time();

    timeout = (delta_us > 0 ? (k_timeout_t)(delta_us / 1000) : 0);

    LOG_DBG("TimerRemaining[%u]", timeout);

    return timeout;
}

void bt_le_timer_handle_event(void *arg, size_t gen)
{
    struct k_work *work = arg;

    bt_le_host_lock();
    /* Drop the event if the work was re-armed or cancelled after it fired */
    if ((uint32_t)gen == work->gen) {
        k_work_submit(work);
    }
    bt_le_host_unlock();
}
