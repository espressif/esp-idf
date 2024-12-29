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
#include "esp_private/sleep_retention.h"
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
#if PARLIO_USE_RETENTION_LINK
            sleep_retention_module_t module_id = parlio_reg_retention_info[group_id].retention_module;
            sleep_retention_module_init_param_t init_param = {
                .cbs = {
                    .create = {
                        .handle = parlio_create_sleep_retention_link_cb,
                        .arg = group,
                    },
                },
                .depends = RETENTION_MODULE_BITMAP_INIT(CLOCK_SYSTEM)
            };
            // we only do retention init here. Allocate retention module in the unit initialization
            if (sleep_retention_module_init(module_id, &init_param) != ESP_OK) {
                // even though the sleep retention module init failed, PARLIO driver should still work, so just warning here
                ESP_LOGW(TAG, "init sleep retention failed %d, power domain may be turned off during sleep", group_id);
            }
#endif // PARLIO_USE_RETENTION_LINK
            // hal layer initialize
            parlio_hal_init(&group->hal);
            group->dma_align = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
            group->dma_align = group->dma_align < 4 ? 4 : group->dma_align;
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
#if PARLIO_USE_RETENTION_LINK
        const periph_retention_module_t module_id = parlio_reg_retention_info[group_id].retention_module;
        if (sleep_retention_is_module_created(module_id)) {
            assert(sleep_retention_is_module_inited(module_id));
            sleep_retention_module_free(module_id);
        }
        if (sleep_retention_is_module_inited(module_id)) {
            sleep_retention_module_deinit(module_id);
        }
#endif // PARLIO_USE_RETENTION_LINK
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

#if PARLIO_USE_RETENTION_LINK
esp_err_t parlio_create_sleep_retention_link_cb(void *arg)
{
    parlio_group_t *group = (parlio_group_t *)arg;
    int group_id = group->group_id;
    sleep_retention_module_t module_id = parlio_reg_retention_info[group_id].retention_module;
    esp_err_t err = sleep_retention_entries_create(parlio_reg_retention_info[group_id].regdma_entry_array,
                                                   parlio_reg_retention_info[group_id].array_size,
                                                   REGDMA_LINK_PRI_PARLIO, module_id);
    ESP_RETURN_ON_ERROR(err, TAG, "create retention link failed");
    return ESP_OK;
}

void parlio_create_retention_module(parlio_group_t *group)
{
    int group_id = group->group_id;
    sleep_retention_module_t module_id = parlio_reg_retention_info[group_id].retention_module;

    _lock_acquire(&s_platform.mutex);
    if (sleep_retention_is_module_inited(module_id) && !sleep_retention_is_module_created(module_id)) {
        if (sleep_retention_module_allocate(module_id) != ESP_OK) {
            // even though the sleep retention module create failed, PARLIO driver should still work, so just warning here
            ESP_LOGW(TAG, "create retention module failed, power domain can't turn off");
        }
    }
    _lock_release(&s_platform.mutex);
}
#endif // PARLIO_USE_RETENTION_LINK
