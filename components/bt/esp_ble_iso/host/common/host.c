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

#include "common/host.h"
#include "common/app/gap.h"
#include "common/app/gatt.h"

LOG_MODULE_REGISTER(ISO_HOST, CONFIG_BT_ISO_LOG_LEVEL);

static struct k_mutex host_mutex;

#define TIMEOUT_MS  (5000 / portTICK_PERIOD_MS) /* 5s */

#if HOST_LOCK_DEBUG
void bt_le_host_lock_debug(const char *func, int line)
#else /* HOST_LOCK_DEBUG */
void bt_le_host_lock(void)
#endif /* HOST_LOCK_DEBUG */
{
    /* LOG_DBG("%s: %d", func, line); */

    int err = k_mutex_lock(&host_mutex, TIMEOUT_MS);
    if (err) {
        /* 5s wait failed: the host stack is wedged. k_mutex_lock has
         * already logged self/holder task names. Use libc abort() rather
         * than assert(0) — assert is a no-op under NDEBUG, which would
         * let the caller enter the critical section without the mutex
         * held and cause races. abort() halts in every build.
         */
        LOG_ERR("HostLockTimeout");
        abort();
    }
}

#if HOST_LOCK_DEBUG
void bt_le_host_unlock_debug(const char *func, int line)
#else /* HOST_LOCK_DEBUG */
void bt_le_host_unlock(void)
#endif /* HOST_LOCK_DEBUG */
{
    /* LOG_DBG("%s: %d", func, line); */

    /* Defense-in-depth: bt_le_host_lock now aborts on timeout, so this
     * branch is unreachable in normal flow. Keep the check to catch any
     * unbalanced unlock (callers releasing without prior lock).
     */
    if (xSemaphoreGetMutexHolder(host_mutex.handle) != xTaskGetCurrentTaskHandle()) {
        LOG_WRN("HostUnlockNotHolder");
        return;
    }

    k_mutex_unlock(&host_mutex);
}

int bt_le_host_init(void)
{
    int err;

    LOG_DBG("HostInit");

    k_mutex_create(&host_mutex);

    err = bt_le_scan_init();
    if (err) {
        goto delete_mutex;
    }

#if CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT
    err = bt_le_l2cap_init();
    if (err) {
        goto deinit_scan;
    }
#endif /* CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT */

    err = bt_le_iso_init();
    if (err) {
        goto deinit_l2cap;
    }

    err = bt_le_iso_task_init();
    if (err) {
        goto deinit_iso;
    }

    return 0;

deinit_iso:
    bt_le_iso_deinit();
deinit_l2cap:
#if CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT
    bt_le_l2cap_deinit();
deinit_scan:    /* only reachable when OTS path is compiled in */
#endif /* CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT */
    bt_le_scan_deinit();
delete_mutex:
    k_mutex_delete(&host_mutex);

    return err;
}

void bt_le_host_deinit(void)
{
    LOG_DBG("HostDeinit");

    bt_le_iso_task_deinit();
    bt_le_iso_deinit();
#if CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT
    bt_le_l2cap_deinit();
#endif /* CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT */
    bt_le_scan_deinit();

    k_mutex_delete(&host_mutex);
}
