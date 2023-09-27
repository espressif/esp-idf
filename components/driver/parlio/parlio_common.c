/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/lock.h>
#include "sdkconfig.h"
#if CONFIG_PARLIO_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "esp_check.h"
#include "clk_ctrl_os.h"
#include "soc/rtc.h"
#include "soc/parlio_periph.h"
#include "hal/parlio_ll.h"
#include "esp_private/esp_clk.h"
#include "parlio_private.h"

static const char *TAG = "parlio";

typedef struct parlio_platform_t {
    _lock_t mutex;                             // platform level mutex lock
    parlio_group_t *groups[SOC_PARLIO_GROUPS]; // array of parallel IO group instances
    int group_ref_counts[SOC_PARLIO_GROUPS];   // reference count used to protect group install/uninstall
} parlio_platform_t;

static parlio_platform_t s_platform; // singleton platform

parlio_group_t *parlio_acquire_group_handle(int group_id)
{
    bool new_group = false;
    parlio_group_t *group = NULL;

    // prevent install parlio group concurrently
    _lock_acquire(&s_platform.mutex);
    if (!s_platform.groups[group_id]) {
        group = heap_caps_calloc(1, sizeof(parlio_group_t), PARLIO_MEM_ALLOC_CAPS);
        if (group) {
            new_group = true;
            s_platform.groups[group_id] = group;
            group->group_id = group_id;
            group->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
            PARLIO_RCC_ATOMIC() {
                parlio_ll_enable_bus_clock(group_id, true);
                parlio_ll_reset_register(group_id);
            }
            // hal layer initialize
            parlio_hal_init(&group->hal);
        }
    } else { // group already install
        group = s_platform.groups[group_id];
    }
    if (group) {
        // someone acquired the group handle means we have a new object that refer to this group
        s_platform.group_ref_counts[group_id]++;
    }
    _lock_release(&s_platform.mutex);

    if (new_group) {
        ESP_LOGD(TAG, "new group(%d) at %p", group_id, group);
    }
    return group;
}

void parlio_release_group_handle(parlio_group_t *group)
{
    int group_id = group->group_id;
    bool do_deinitialize = false;

    _lock_acquire(&s_platform.mutex);
    s_platform.group_ref_counts[group_id]--;
    if (s_platform.group_ref_counts[group_id] == 0) {
        do_deinitialize = true;
        s_platform.groups[group_id] = NULL;
        // hal layer deinitialize
        parlio_hal_deinit(&group->hal);
        PARLIO_RCC_ATOMIC() {
            parlio_ll_enable_bus_clock(group_id, false);
        }
        free(group);
    }
    _lock_release(&s_platform.mutex);

    if (do_deinitialize) {
        ESP_LOGD(TAG, "del group(%d)", group_id);
    }
}
