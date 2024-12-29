/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_clk_tree.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/periph_ctrl.h"
#include "soc/mcpwm_periph.h"
#include "soc/soc_caps.h"
#include "hal/mcpwm_ll.h"
#include "mcpwm_private.h"
#include "esp_private/rtc_clk.h"

#if SOC_PERIPH_CLK_CTRL_SHARED
#define MCPWM_CLOCK_SRC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define MCPWM_CLOCK_SRC_ATOMIC()
#endif

#if !SOC_RCC_IS_INDEPENDENT
#define MCPWM_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define MCPWM_RCC_ATOMIC()
#endif

#if MCPWM_USE_RETENTION_LINK
static esp_err_t mcpwm_create_sleep_retention_link_cb(void *arg);
#endif

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
            group->intr_priority = -1;
            group->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
#if MCPWM_USE_RETENTION_LINK
            sleep_retention_module_t module = mcpwm_reg_retention_info[group_id].retention_module;
            sleep_retention_module_init_param_t init_param = {
                .cbs = {
                    .create = {
                        .handle = mcpwm_create_sleep_retention_link_cb,
                        .arg = group,
                    },
                },
                .depends = RETENTION_MODULE_BITMAP_INIT(CLOCK_SYSTEM)
            };
            // we only do retention init here. Allocate retention module in the unit initialization
            if (sleep_retention_module_init(module, &init_param) != ESP_OK) {
                // even though the sleep retention module init failed, MCPWM driver should still work, so just warning here
                ESP_LOGW(TAG, "init sleep retention failed %d, power domain may be turned off during sleep", group_id);
            }
#endif // MCPWM_USE_RETENTION_LINK
            // enable APB to access MCPWM registers
            MCPWM_RCC_ATOMIC() {
                mcpwm_ll_enable_bus_clock(group_id, true);
                mcpwm_ll_reset_register(group_id);
            }
            // enable function clock before initialize HAL context
            // MCPWM registers are in the core clock domain, there's a bridge between APB and the Core clock domain
            // if the core clock is not enabled, then even the APB clock is enabled, the MCPWM registers are still not accessible
            MCPWM_CLOCK_SRC_ATOMIC() {
                mcpwm_ll_group_enable_clock(group_id, true);
            }
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
        s_platform.groups[group_id] = NULL; // deregister from platform
        MCPWM_CLOCK_SRC_ATOMIC() {
            mcpwm_ll_group_enable_clock(group_id, false);
        }
        // hal layer deinitialize
        mcpwm_hal_deinit(&group->hal);
        MCPWM_RCC_ATOMIC() {
            mcpwm_ll_enable_bus_clock(group_id, false);
        }
        if (group->pm_lock) {
            esp_pm_lock_delete(group->pm_lock);
        }
#if MCPWM_USE_RETENTION_LINK
        const periph_retention_module_t module_id = mcpwm_reg_retention_info[group_id].retention_module;
        if (sleep_retention_is_module_created(module_id)) {
            sleep_retention_module_free(module_id);
        }
        if (sleep_retention_is_module_inited(module_id)) {
            sleep_retention_module_deinit(module_id);
        }
#endif // MCPWM_USE_RETENTION_LINK
        free(group);
    }
    _lock_release(&s_platform.mutex);

    if (do_deinitialize) {
        ESP_LOGD(TAG, "del group(%d)", group_id);
    }
}

esp_err_t mcpwm_check_intr_priority(mcpwm_group_t *group, int intr_priority)
{
    esp_err_t ret = ESP_OK;
    bool intr_priority_conflict = false;
    portENTER_CRITICAL(&group->spinlock);
    if (group->intr_priority == -1) {
        group->intr_priority = intr_priority;
    } else if (intr_priority != 0) {
        intr_priority_conflict = (group->intr_priority != intr_priority);
    }
    portEXIT_CRITICAL(&group->spinlock);
    ESP_RETURN_ON_FALSE(!intr_priority_conflict, ESP_ERR_INVALID_STATE, TAG, "intr_priority conflict, already is %d but attempt to %d", group->intr_priority, intr_priority);
    return ret;
}

int mcpwm_get_intr_priority_flag(mcpwm_group_t *group)
{
    int isr_flags = 0;
    if (group->intr_priority) {
        isr_flags |= 1 << (group->intr_priority);
    } else {
        isr_flags |= MCPWM_ALLOW_INTR_PRIORITY_MASK;
    }
    return isr_flags;
}

esp_err_t mcpwm_select_periph_clock(mcpwm_group_t *group, soc_module_clk_t clk_src)
{
    esp_err_t ret = ESP_OK;
    bool clock_selection_conflict = false;
    bool do_clock_init = false;
    // check if we need to update the group clock source, group clock source is shared by all mcpwm modules
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

#if CONFIG_PM_ENABLE
        sprintf(group->pm_lock_name, "mcpwm_%d", group->group_id); // e.g. mcpwm_0
        ret  = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, group->pm_lock_name, &group->pm_lock);
        ESP_RETURN_ON_ERROR(ret, TAG, "create pm lock failed");
        ESP_LOGD(TAG, "install NO_LIGHT_SLEEP lock for MCPWM group(%d)", group->group_id);
#endif // CONFIG_PM_ENABLE

        esp_clk_tree_enable_src((soc_module_clk_t)clk_src, true);
        MCPWM_CLOCK_SRC_ATOMIC() {
            mcpwm_ll_group_set_clock_source(group->group_id, clk_src);
        }
    }
    return ret;
}

esp_err_t mcpwm_set_prescale(mcpwm_group_t *group, uint32_t expect_module_resolution_hz, uint32_t module_prescale_max, uint32_t *ret_module_prescale)
{
    ESP_RETURN_ON_FALSE(group && expect_module_resolution_hz && module_prescale_max, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    uint32_t periph_src_clk_hz = 0;
    int group_id = group->group_id;
    uint32_t group_resolution_hz = 0;
    uint32_t group_prescale = group->prescale > 0 ? group->prescale : MCPWM_GROUP_CLOCK_DEFAULT_PRESCALE;     // range: 1~256, 0 means not calculated
    uint32_t module_prescale = 0;       // range: 1~256 (for timer) or 1~16 (for carrier) or 1 (for capture)

    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz(group->clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &periph_src_clk_hz), TAG, "get clock source freq failed");

    // calc the group prescale

    group_resolution_hz = periph_src_clk_hz / group_prescale;
    module_prescale = group_resolution_hz / expect_module_resolution_hz;

    // default prescale cannot match
    // try to ensure accurate division. If none of the division factors can be guaranteed to be integers, then allocate the clock frequency to the highest divisor
    uint32_t fit_module_prescale = 0;
    uint32_t fit_group_prescale = 0;
    if (!(module_prescale >= 1 && module_prescale <= module_prescale_max)) {
        group_prescale = 0;
        while (++group_prescale <= MCPWM_LL_MAX_GROUP_PRESCALE) {
            group_resolution_hz = periph_src_clk_hz / group_prescale;
            module_prescale = group_resolution_hz / expect_module_resolution_hz;
            if (module_prescale >= 1 && module_prescale <= module_prescale_max) {
                // maintain the first value found during the search that satisfies the division requirement (highest frequency), applicable for cases where integer division is not possible."
                fit_module_prescale = fit_module_prescale ? fit_module_prescale : module_prescale;
                fit_group_prescale = fit_group_prescale ? fit_group_prescale : group_prescale;
                // find accurate division
                if (group_resolution_hz == expect_module_resolution_hz * module_prescale) {
                    fit_module_prescale = module_prescale;
                    fit_group_prescale = group_prescale;
                    break;
                }
            }
        }
        module_prescale = fit_module_prescale;
        group_prescale = fit_group_prescale;
        group_resolution_hz = periph_src_clk_hz / group_prescale;
    }

    ESP_LOGD(TAG, "group (%d) calc prescale:%"PRIu32", module calc prescale:%"PRIu32"", group_id, group_prescale, module_prescale);
    ESP_RETURN_ON_FALSE(group_prescale > 0 && group_prescale <= MCPWM_LL_MAX_GROUP_PRESCALE, ESP_ERR_INVALID_STATE, TAG,
                        "set group prescale failed, group clock cannot match the resolution");

    // check if we need to update the group prescale, group prescale is shared by all mcpwm modules
    bool prescale_conflict = false;
    portENTER_CRITICAL(&group->spinlock);
    if (group->prescale == 0) {
        group->prescale = group_prescale;
        group->resolution_hz = group_resolution_hz;
        MCPWM_CLOCK_SRC_ATOMIC() {
            mcpwm_ll_group_set_clock_prescale(group_id, group_prescale);
        }
    } else {
        prescale_conflict = (group->prescale != group_prescale);
    }
    portEXIT_CRITICAL(&group->spinlock);

    ESP_RETURN_ON_FALSE(!prescale_conflict, ESP_ERR_INVALID_STATE, TAG,
                        "group prescale conflict, already is %"PRIu32" but attempt to %"PRIu32"", group->prescale, group_prescale);

    ESP_LOGD(TAG, "group (%d) clock resolution:%"PRIu32"Hz", group_id, group->resolution_hz);

    // set module resolution
    if (ret_module_prescale) {
        *ret_module_prescale = module_prescale;
    }

    return ESP_OK;
}

#if MCPWM_USE_RETENTION_LINK
static esp_err_t mcpwm_create_sleep_retention_link_cb(void *arg)
{
    mcpwm_group_t *group = (mcpwm_group_t *)arg;
    int group_id = group->group_id;
    sleep_retention_module_t module_id = mcpwm_reg_retention_info[group_id].retention_module;
    esp_err_t err = sleep_retention_entries_create(mcpwm_reg_retention_info[group_id].regdma_entry_array,
                                                   mcpwm_reg_retention_info[group_id].array_size,
                                                   REGDMA_LINK_PRI_MCPWM, module_id);
    return err;
}
void mcpwm_create_retention_module(mcpwm_group_t *group)
{
    int group_id = group->group_id;
    sleep_retention_module_t module_id = mcpwm_reg_retention_info[group_id].retention_module;
    _lock_acquire(&s_platform.mutex);
    if (sleep_retention_is_module_inited(module_id) && !sleep_retention_is_module_created(module_id)) {
        if (sleep_retention_module_allocate(module_id) != ESP_OK) {
            // even though the sleep retention module create failed, MCPWM driver should still work, so just warning here
            ESP_LOGW(TAG, "create retention module failed, power domain can't turn off");
        }
    }
    _lock_release(&s_platform.mutex);
}
#endif // MCPWM_USE_RETENTION_LINK
