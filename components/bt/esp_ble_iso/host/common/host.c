/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zephyr/bluetooth/iso.h>

#include <../host/conn_internal.h>

#include "common/host.h"
#include "common/conn.h"
#include "common/app/gap.h"
#include "common/app/gatt.h"

#if CONFIG_BT_BLUEDROID_ENABLED
#include "bluedroid/gap.h"
#include "bluedroid/gatt.h"
#else
#include "nimble/gatt.h"
#endif

LOG_MODULE_REGISTER(ISO_HOST, CONFIG_BT_ISO_LOG_LEVEL);

static BT_ISO_CTRL_BSS_ATTR struct k_mutex host_mutex;

extern struct bt_conn iso_conns[CONFIG_BT_ISO_MAX_CHAN];

/* bt_le_host_lock() retries every HOST_LOCK_POLL_MS, logging LockFail with both
 * task names, and reboots once the wait reaches HOST_LOCK_ABORT_MS. Healthy
 * holds are microseconds, so 5s is unambiguous - and nothing else would notice:
 * both tasks are blocked rather than spinning, so idle keeps feeding the task
 * watchdog. Set HOST_LOCK_ABORT_MS 0 to keep a wedge alive for inspection. */
#define HOST_LOCK_POLL_MS   1000
#define HOST_LOCK_ABORT_MS  5000

#if HOST_LOCK_DEBUG
void bt_le_host_lock_debug(const char *func, int line)
#else /* HOST_LOCK_DEBUG */
void bt_le_host_lock(void)
#endif /* HOST_LOCK_DEBUG */
{
    /* LOG_DBG("%s: %d", func, line); */

    /* Never returns without the mutex: the caller's critical section would run
     * unprotected, trading a deadlock for silent state corruption, and this API
     * cannot report the failure anyway - it returns void, so every caller
     * proceeds as if it holds the mutex. Retries and warns instead, then
     * reboots - see HOST_LOCK_POLL_MS. */
    unsigned waited_ms = 0;

    while (k_mutex_lock(&host_mutex, HOST_LOCK_POLL_MS / portTICK_PERIOD_MS) != 0) {
        waited_ms += HOST_LOCK_POLL_MS;

        /* Usually an AB-BA inversion: two locks, two tasks, opposite orders.
         *
         *   iso_task: host_mutex held -> blocks on the application's lock
         *             (an app callback dispatched from under the mutex)
         *   app task: application's lock held -> blocks here on host_mutex
         *
         * Neither order is wrong by itself; the pair only deadlocks where the
         * two paths overlap, so it need not reproduce. k_mutex_lock's LockFail
         * on the line above names both tasks. */
        LOG_WRN("HostLockTimeout[%ums]", waited_ms);

#if HOST_LOCK_ABORT_MS
        if (waited_ms >= HOST_LOCK_ABORT_MS) {
            abort();
        }
#endif /* HOST_LOCK_ABORT_MS */
    }
}

int bt_le_host_lock_timeout(void)
{
    /* The bounded variant, for callers that can report the failure upward - see
     * BT_LE_HOST_LOCK_OR_RETURN. Bounded by K_MUTEX_SHORT rather than
     * HOST_LOCK_ABORT_MS so that disabling the abort does not turn this into a
     * try-lock that fails on any contention. */
    if (k_mutex_lock(&host_mutex, K_MUTEX_SHORT) != 0) {
        return -EBUSY;
    }

    return 0;
}

#if HOST_LOCK_DEBUG
void bt_le_host_unlock_debug(const char *func, int line)
#else /* HOST_LOCK_DEBUG */
void bt_le_host_unlock(void)
#endif /* HOST_LOCK_DEBUG */
{
    /* LOG_DBG("%s: %d", func, line); */

    /* Defense-in-depth: bt_le_host_lock never returns without the mutex,
     * so this branch is unreachable in normal flow. Keep the check to
     * catch any unbalanced unlock (callers releasing without prior lock).
     */
    if (xSemaphoreGetMutexHolder(host_mutex.handle) != xTaskGetCurrentTaskHandle()) {
        LOG_WRN("HostUnlockNotHolder");
        return;
    }

    k_mutex_unlock(&host_mutex);
}

int bt_le_host_check_idle(void)
{
    struct bt_iso_chan *chan;
    size_t busy = 0;

    /* Only what ISO created; the application's adv sets, sync and ACL are
     * dropped by bt_le_host_deinit() rather than blocked on. Counts every
     * offender so one attempt tells the caller the whole list. */

    bt_le_host_lock();

    for (size_t i = 0; i < ARRAY_SIZE(iso_conns); i++) {
        chan = iso_conns[i].iso.chan;
        if (chan && chan->state != BT_ISO_STATE_DISCONNECTED) {
            LOG_ERR("DeinitBusyIsoChan[%u][state=%u]", i, chan->state);
            busy++;
        }
    }

    busy += bt_le_iso_report_busy();

    bt_le_host_unlock();

    return busy ? -EBUSY : 0;
}

int bt_le_host_init(void)
{
    int err;

    LOG_DBG("HostInit");

    k_mutex_create(&host_mutex);

    bt_le_conn_reset();

    bt_le_iso_state_reset();

    err = bt_le_scan_init();
    if (err) {
        goto delete_mutex;
    }

#if CONFIG_BT_BLUEDROID_ENABLED
    err = bt_le_bluedroid_gap_init();
    if (err) {
        goto deinit_scan;
    }

    err = bt_le_bluedroid_gatt_init();
    if (err) {
        goto deinit_gatt;
    }
#else
    /* nimble: reset the per-conn GATT cache/NRP arrays (now .bss/PSRAM, no
     * static init) so this init - and any later deinit/re-init - starts clean. */
    bt_le_nimble_gattc_db_init();
    bt_le_nimble_gatt_nrp_init();
#endif /* CONFIG_BT_BLUEDROID_ENABLED */

    err = bt_le_iso_init();
    if (err) {
        goto deinit_gatt;
    }

    err = bt_le_iso_task_init();
    if (err) {
        goto deinit_iso;
    }

    return 0;

deinit_iso:
    bt_le_iso_deinit();
deinit_gatt:
#if CONFIG_BT_BLUEDROID_ENABLED
    bt_le_bluedroid_gatt_deinit();
deinit_scan:
#endif /* CONFIG_BT_BLUEDROID_ENABLED */
    bt_le_scan_deinit();
delete_mutex:
    k_mutex_delete(&host_mutex);

    return err;
}

int bt_le_host_deinit(void)
{
    int err;

    LOG_DBG("HostDeinit");

    /* Everything below frees state the task dispatches into, so bail out while
     * it is still alive rather than free underneath it. */
    err = bt_le_iso_task_deinit();
    if (err) {
        return err;
    }

    bt_le_iso_deinit();
#if CONFIG_BT_BLUEDROID_ENABLED
    /* No gap_deinit: BTM_BleGapRegisterCallback refuses NULL, so the callback
     * stays. Harmless - task_post rejects once the task is gone. */
    bt_le_bluedroid_gatt_deinit();
#else
    bt_le_nimble_gattc_db_deinit();
    bt_le_nimble_gatt_nrp_deinit();
#endif /* CONFIG_BT_BLUEDROID_ENABLED */
    bt_le_scan_deinit();

    /* Last: iso_task has exited and the callbacks above are gone, so nothing
     * can take the mutex any more. */
    k_mutex_delete(&host_mutex);

    return 0;
}
