/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_check.h"
#include "esp_clk_tree.h"
#include "esp_private/gptimer.h"
#include "gptimer_priv.h"

static const char *TAG = "gptimer";

typedef struct gptimer_platform_t {
    _lock_t mutex;                             // platform level mutex lock
    gptimer_group_t *groups[SOC_TIMER_GROUPS]; // timer group pool
    int group_ref_counts[SOC_TIMER_GROUPS];    // reference count used to protect group install/uninstall
} gptimer_platform_t;

// gptimer driver platform, it's always a singleton
static gptimer_platform_t s_platform;

#if GPTIMER_USE_RETENTION_LINK
static esp_err_t gptimer_create_sleep_retention_link_cb(void *arg)
{
    gptimer_group_t *group = (gptimer_group_t *)arg;
    int group_id = group->group_id;
    sleep_retention_module_t module = group->sleep_retention_module;
    esp_err_t err = sleep_retention_entries_create(tg_timer_reg_retention_info[group_id].regdma_entry_array,
                                                   tg_timer_reg_retention_info[group_id].array_size,
                                                   REGDMA_LINK_PRI_GPTIMER, module);
    ESP_RETURN_ON_ERROR(err, TAG, "create retention link failed");
    return ESP_OK;
}

void gptimer_create_retention_module(gptimer_group_t *group)
{
    sleep_retention_module_t module = group->sleep_retention_module;
    _lock_acquire(&s_platform.mutex);
    if (group->retention_link_created == false) {
        if (sleep_retention_module_allocate(module) != ESP_OK) {
            // even though the sleep retention module create failed, GPTimer driver should still work, so just warning here
            ESP_LOGW(TAG, "create retention module for group %d retention, power domain can't turn off", group->group_id);
        } else {
            group->retention_link_created = true;
        }
    }
    _lock_release(&s_platform.mutex);
}
#endif // GPTIMER_USE_RETENTION_LINK

gptimer_group_t *gptimer_acquire_group_handle(int group_id)
{
    bool new_group = false;
    gptimer_group_t *group = NULL;

    // prevent install timer group concurrently
    _lock_acquire(&s_platform.mutex);
    if (!s_platform.groups[group_id]) {
        group = heap_caps_calloc(1, sizeof(gptimer_group_t), GPTIMER_MEM_ALLOC_CAPS);
        if (group) {
            new_group = true;
            s_platform.groups[group_id] = group;
            // initialize timer group members
            group->group_id = group_id;
            group->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
        }
    } else {
        group = s_platform.groups[group_id];
    }
    if (group) {
        // someone acquired the group handle means we have a new object that refer to this group
        s_platform.group_ref_counts[group_id]++;
    }
    _lock_release(&s_platform.mutex);

    if (new_group) {
        // !!! HARDWARE SHARED RESOURCE !!!
        // the gptimer and watchdog reside in the same the timer group
        // we need to increase/decrease the reference count before enable/disable/reset the peripheral
        PERIPH_RCC_ACQUIRE_ATOMIC(timer_group_periph_signals.groups[group_id].module, ref_count) {
            if (ref_count == 0) {
                timer_ll_enable_bus_clock(group_id, true);
                timer_ll_reset_register(group_id);
            }
        }
#if GPTIMER_USE_RETENTION_LINK
        sleep_retention_module_t module = TIMER_LL_SLEEP_RETENTION_MODULE_ID(group_id);
        sleep_retention_module_init_param_t init_param = {
            .cbs = {
                .create = {
                    .handle = gptimer_create_sleep_retention_link_cb,
                    .arg = group
                },
            },
            .depends = BIT(SLEEP_RETENTION_MODULE_CLOCK_SYSTEM)
        };
        if (sleep_retention_module_init(module, &init_param) == ESP_OK) {
            group->sleep_retention_module = module;
        } else {
            // even though the sleep retention module init failed, RMT driver should still work, so just warning here
            ESP_LOGW(TAG, "init sleep retention failed %d, power domain may be turned off during sleep", group_id);
        }
#endif // GPTIMER_USE_RETENTION_LINK
        ESP_LOGD(TAG, "new group (%d) @%p", group_id, group);
    }

    return group;
}

void gptimer_release_group_handle(gptimer_group_t *group)
{
    int group_id = group->group_id;
    bool do_deinitialize = false;

    _lock_acquire(&s_platform.mutex);
    s_platform.group_ref_counts[group_id]--;
    if (s_platform.group_ref_counts[group_id] == 0) {
        assert(s_platform.groups[group_id]);
        do_deinitialize = true;
        s_platform.groups[group_id] = NULL;
    }
    _lock_release(&s_platform.mutex);

    if (do_deinitialize) {
        // disable bus clock for the timer group
        PERIPH_RCC_RELEASE_ATOMIC(timer_group_periph_signals.groups[group_id].module, ref_count) {
            if (ref_count == 0) {
                timer_ll_enable_bus_clock(group_id, false);
            }
        }
#if GPTIMER_USE_RETENTION_LINK
        if (group->sleep_retention_module) {
            if (group->retention_link_created) {
                sleep_retention_module_free(group->sleep_retention_module);
            }
            sleep_retention_module_deinit(group->sleep_retention_module);
        }
#endif
        free(group);
        ESP_LOGD(TAG, "del group (%d)", group_id);
    }
}

esp_err_t gptimer_select_periph_clock(gptimer_t *timer, gptimer_clock_source_t src_clk, uint32_t resolution_hz)
{
    uint32_t counter_src_hz = 0;
    int timer_id = timer->timer_id;

    // TODO: [clk_tree] to use a generic clock enable/disable or acquire/release function for all clock source
#if SOC_TIMER_GROUP_SUPPORT_RC_FAST
    if (src_clk == GPTIMER_CLK_SRC_RC_FAST) {
        // RC_FAST clock is not enabled automatically on start up, we enable it here manually.
        // Note there's a ref count in the enable/disable function, we must call them in pair in the driver.
        periph_rtc_dig_clk8m_enable();
    }
#endif // SOC_TIMER_GROUP_SUPPORT_RC_FAST

    // get clock source frequency
    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)src_clk, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &counter_src_hz),
                        TAG, "get clock source frequency failed");

#if CONFIG_PM_ENABLE
    bool need_pm_lock = true;
    // to make the gptimer work reliable, the source clock must stay alive and unchanged
    // driver will create different pm lock for that purpose, according to different clock source
    esp_pm_lock_type_t pm_lock_type = ESP_PM_NO_LIGHT_SLEEP;

#if SOC_TIMER_GROUP_SUPPORT_RC_FAST
    if (src_clk == GPTIMER_CLK_SRC_RC_FAST) {
        // RC_FAST won't be turn off in sleep and won't change its frequency during DFS
        need_pm_lock = false;
    }
#endif // SOC_TIMER_GROUP_SUPPORT_RC_FAST

#if SOC_TIMER_GROUP_SUPPORT_APB
    if (src_clk == GPTIMER_CLK_SRC_APB) {
        // APB clock frequency can be changed during DFS
        pm_lock_type = ESP_PM_APB_FREQ_MAX;
    }
#endif // SOC_TIMER_GROUP_SUPPORT_APB

#if CONFIG_IDF_TARGET_ESP32C2
    if (src_clk == GPTIMER_CLK_SRC_PLL_F40M) {
        // although PLL_F40M clock is a fixed PLL clock, which is unchangeable
        // on ESP32C2, PLL_F40M can be turned off even during DFS (unlike other PLL clocks)
        // so we're acquiring a fake "APB" lock here to prevent the system from doing DFS
        pm_lock_type = ESP_PM_APB_FREQ_MAX;
    }
#endif // CONFIG_IDF_TARGET_ESP32C2

    if (need_pm_lock) {
        sprintf(timer->pm_lock_name, "gptimer_%d_%d", timer->group->group_id, timer_id); // e.g. gptimer_0_0
        ESP_RETURN_ON_ERROR(esp_pm_lock_create(pm_lock_type, 0, timer->pm_lock_name, &timer->pm_lock),
                            TAG, "create pm lock failed");
    }
#endif // CONFIG_PM_ENABLE

    // !!! HARDWARE SHARED RESOURCE !!!
    // on some ESP chip, different peripheral's clock source setting are mixed in the same register
    // so we need to make this done in an atomic way
    GPTIMER_CLOCK_SRC_ATOMIC() {
        timer_ll_set_clock_source(timer->hal.dev, timer_id, src_clk);
        timer_ll_enable_clock(timer->hal.dev, timer_id, true);
    }
    timer->clk_src = src_clk;
    uint32_t prescale = counter_src_hz / resolution_hz; // potential resolution loss here
    timer_ll_set_clock_prescale(timer->hal.dev, timer_id, prescale);
    timer->resolution_hz = counter_src_hz / prescale; // this is the real resolution
    if (timer->resolution_hz != resolution_hz) {
        ESP_LOGW(TAG, "resolution lost, expect %"PRIu32", real %"PRIu32, resolution_hz, timer->resolution_hz);
    }
    return ESP_OK;
}

esp_err_t gptimer_get_intr_handle(gptimer_handle_t timer, intr_handle_t *ret_intr_handle)
{
    ESP_RETURN_ON_FALSE(timer && ret_intr_handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    *ret_intr_handle = timer->intr;
    return ESP_OK;
}

esp_err_t gptimer_get_pm_lock(gptimer_handle_t timer, esp_pm_lock_handle_t *ret_pm_lock)
{
    ESP_RETURN_ON_FALSE(timer && ret_pm_lock, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    *ret_pm_lock = timer->pm_lock;
    return ESP_OK;
}

int gptimer_get_group_id(gptimer_handle_t timer, int *group_id)
{
    ESP_RETURN_ON_FALSE(timer && group_id, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    *group_id = timer->group->group_id;
    return ESP_OK;
}
