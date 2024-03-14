/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
        portMUX_INITIALIZE(&group->spinlock);
        group->group_id = group_id;
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
    }
    _lock_release(&s_platform.mutex);

    if (do_deinitialize) {
        free(group);
        ESP_LOGD(TAG, "del group(%d)", group_id);
    }
}

esp_err_t parlio_register_unit_to_group(parlio_unit_base_handle_t unit)
{
    parlio_group_t *group = NULL;
    int unit_id = -1;
    for (int i = 0; i < SOC_PARLIO_GROUPS; i++) {
        group = parlio_acquire_group_handle(i);
        ESP_RETURN_ON_FALSE(group, ESP_ERR_NO_MEM, TAG, "no memory for group (%d)", i);
        portENTER_CRITICAL(&group->spinlock);
        if (unit->dir == PARLIO_DIR_TX) {
            for (int j = 0; j < SOC_PARLIO_TX_UNITS_PER_GROUP; j++) {
                if (!group->tx_units[j]) {
                    group->tx_units[j] = unit;
                    unit_id = j;
                    break;
                }
            }
        } else {
            for (int j = 0; j < SOC_PARLIO_RX_UNITS_PER_GROUP; j++) {
                if (!group->rx_units[j]) {
                    group->rx_units[j] = unit;
                    unit_id = j;
                    break;
                }
            }
        }
        portEXIT_CRITICAL(&group->spinlock);
        if (unit_id < 0) {
            /* didn't find a free unit slot in the group */
            parlio_release_group_handle(group);
        } else {
            unit->unit_id = unit_id;
            unit->group = group;
            break;
        }
    }
    ESP_RETURN_ON_FALSE(unit_id >= 0, ESP_ERR_NOT_FOUND, TAG,
                        "no free %s unit", unit->dir == PARLIO_DIR_TX ? "tx" : "rx");
    return ESP_OK;
}

void parlio_unregister_unit_from_group(parlio_unit_base_handle_t unit)
{
    assert(unit);
    parlio_group_t *group = unit->group;
    int unit_id = unit->unit_id;
    portENTER_CRITICAL(&group->spinlock);
    if (unit->dir == PARLIO_DIR_TX) {
        group->tx_units[unit_id] = NULL;
    } else {
        group->rx_units[unit_id] = NULL;
    }
    portEXIT_CRITICAL(&group->spinlock);
    /* the parlio unit has a reference of the group, release it now */
    parlio_release_group_handle(group);
}
