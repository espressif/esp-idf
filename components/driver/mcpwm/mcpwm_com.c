/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#if CONFIG_MCPWM_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "esp_check.h"
#include "esp_private/periph_ctrl.h"
#include "soc/mcpwm_periph.h"
#include "hal/mcpwm_ll.h"
#include "mcpwm_private.h"

static const char *TAG = "mcpwm";

typedef struct {
    _lock_t mutex;                           // platform level mutex lock
    mcpwm_group_t *groups[SOC_MCPWM_GROUPS]; // array of MCPWM group instances
    int group_ref_counts[SOC_MCPWM_GROUPS];  // reference count used to protect group install/uninstall
} mcpwm_platform_t;

static mcpwm_platform_t s_platform; // singleton platform

mcpwm_group_t *mcpwm_acquire_group_handle(int group_id)
{
    bool new_group = false;
    mcpwm_group_t *group = NULL;

    // prevent install mcpwm group concurrently
    _lock_acquire(&s_platform.mutex);
    if (!s_platform.groups[group_id]) {
        group = heap_caps_calloc(1, sizeof(mcpwm_group_t), MCPWM_MEM_ALLOC_CAPS);
        if (group) {
            new_group = true;
            s_platform.groups[group_id] = group;
            group->group_id = group_id;
            group->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
            // enable APB to access MCPWM registers
            periph_module_enable(mcpwm_periph_signals.groups[group_id].module);
            periph_module_reset(mcpwm_periph_signals.groups[group_id].module);
            // initialize HAL context
            mcpwm_hal_init_config_t hal_config = {
                .group_id = group_id
            };
            mcpwm_hal_context_t *hal = &group->hal;
            mcpwm_hal_init(hal, &hal_config);
            // disable all interrupts and clear pending status
            mcpwm_ll_intr_enable(hal->dev, UINT32_MAX, false);
            mcpwm_ll_intr_clear_status(hal->dev, UINT32_MAX);
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

void mcpwm_release_group_handle(mcpwm_group_t *group)
{
    int group_id = group->group_id;
    bool do_deinitialize = false;

    _lock_acquire(&s_platform.mutex);
    s_platform.group_ref_counts[group_id]--;
    if (s_platform.group_ref_counts[group_id] == 0) {
        do_deinitialize = true;
        s_platform.groups[group_id] = NULL; // deregister from platfrom
        // hal layer deinitialize
        mcpwm_hal_deinit(&group->hal);
        periph_module_disable(mcpwm_periph_signals.groups[group_id].module);
        free(group);
    }
    _lock_release(&s_platform.mutex);

    if (do_deinitialize) {
        ESP_LOGD(TAG, "del group(%d)", group_id);
    }
}

esp_err_t mcpwm_select_periph_clock(mcpwm_group_t *group, mcpwm_timer_clock_source_t clk_src)
{
    esp_err_t ret = ESP_OK;
    uint32_t periph_src_clk_hz = 0;
    bool clock_selection_conflict = false;
    bool do_clock_init = false;
    // check if we need to update the group clock source, group clock source is shared by all mcpwm objects
    portENTER_CRITICAL(&group->spinlock);
    if (group->clk_src == 0) {
        group->clk_src = clk_src;
        do_clock_init = true;
    } else {
        clock_selection_conflict = (group->clk_src != clk_src);
    }
    portEXIT_CRITICAL(&group->spinlock);
    ESP_RETURN_ON_FALSE(!clock_selection_conflict, ESP_ERR_INVALID_STATE, TAG,
                        "group clock conflict, already is %d but attempt to %d", group->clk_src, clk_src);

    if (do_clock_init) {
        // [clk_tree] ToDo: replace the following switch-case table by clock_tree APIs
        switch (clk_src) {
        case MCPWM_TIMER_CLK_SRC_DEFAULT:
            periph_src_clk_hz = 160000000;
#if CONFIG_PM_ENABLE
            sprintf(group->pm_lock_name, "mcpwm_%d", group->group_id); // e.g. mcpwm_0
            ret  = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, group->pm_lock_name, &group->pm_lock);
            ESP_RETURN_ON_ERROR(ret, TAG, "create ESP_PM_APB_FREQ_MAX lock failed");
            ESP_LOGD(TAG, "install ESP_PM_APB_FREQ_MAX lock for MCPWM group(%d)", group->group_id);
#endif // CONFIG_PM_ENABLE
            break;
        default:
            ESP_RETURN_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, TAG, "clock source %d is not supported", clk_src);
            break;
        }
        mcpwm_ll_group_set_clock_prescale(group->hal.dev, MCPWM_PERIPH_CLOCK_PRE_SCALE);
        group->resolution_hz = periph_src_clk_hz / MCPWM_PERIPH_CLOCK_PRE_SCALE;
        ESP_LOGD(TAG, "group (%d) clock resolution:%"PRIu32"Hz", group->group_id, group->resolution_hz);
    }
    return ret;
}
