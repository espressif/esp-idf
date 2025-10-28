/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/lock.h>
#include "esp_clk_tree.h"
#include "esp_private/gptimer.h"
#include "gptimer_priv.h"
#include "esp_private/esp_clk_tree_common.h"

typedef struct gptimer_platform_t {
    _lock_t mutex;                             // platform level mutex lock
    gptimer_group_t *groups[TIMG_LL_GET(INST_NUM)]; // timer group pool
    int group_ref_counts[TIMG_LL_GET(INST_NUM)];    // reference count used to protect group install/uninstall
} gptimer_platform_t;

// gptimer driver platform, it's always a singleton
static gptimer_platform_t s_platform;

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
        PERIPH_RCC_ACQUIRE_ATOMIC(soc_timg_gptimer_signals[group_id][0].parent_module, ref_count) {
            if (ref_count == 0) {
                timg_ll_enable_bus_clock(group_id, true);
                timg_ll_reset_register(group_id);
            }
        }
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
        PERIPH_RCC_RELEASE_ATOMIC(soc_timg_gptimer_signals[group_id][0].parent_module, ref_count) {
            if (ref_count == 0) {
                timg_ll_enable_bus_clock(group_id, false);
            }
        }
        free(group);
        ESP_LOGD(TAG, "del group (%d)", group_id);
    }
}

esp_err_t gptimer_select_periph_clock(gptimer_t *timer, gptimer_clock_source_t src_clk, uint32_t resolution_hz)
{
    uint32_t counter_src_hz = 0;
    int timer_id = timer->timer_id;
    int group_id = timer->group->group_id;
    // TODO: [clk_tree] to use a generic clock enable/disable or acquire/release function for all clock source
#if TIMER_LL_FUNC_CLOCK_SUPPORT_RC_FAST
    if (src_clk == GPTIMER_CLK_SRC_RC_FAST) {
        // RC_FAST clock is not enabled automatically on start up, we enable it here manually.
        // Note there's a ref count in the enable/disable function, we must call them in pair in the driver.
        periph_rtc_dig_clk8m_enable();
    }
#endif // TIMER_LL_FUNC_CLOCK_SUPPORT_RC_FAST
    timer->clk_src = src_clk;

    // get clock source frequency
    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)src_clk, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &counter_src_hz),
                        TAG, "get clock source frequency failed");

#if CONFIG_PM_ENABLE
    bool need_pm_lock = true;
    // to make the gptimer work reliable, the source clock must stay alive and unchanged
    // driver will create different pm lock for that purpose, according to different clock source
    esp_pm_lock_type_t pm_lock_type = ESP_PM_NO_LIGHT_SLEEP;

#if TIMER_LL_FUNC_CLOCK_SUPPORT_RC_FAST
    if (src_clk == GPTIMER_CLK_SRC_RC_FAST) {
        // RC_FAST won't be turn off in sleep and won't change its frequency during DFS
        need_pm_lock = false;
    }
#endif // TIMER_LL_FUNC_CLOCK_SUPPORT_RC_FAST

#if TIMER_LL_FUNC_CLOCK_SUPPORT_APB
    if (src_clk == GPTIMER_CLK_SRC_APB) {
        // APB clock frequency can be changed during DFS
        pm_lock_type = ESP_PM_APB_FREQ_MAX;
    }
#endif // TIMER_LL_FUNC_CLOCK_SUPPORT_APB

#if CONFIG_IDF_TARGET_ESP32C2
    if (src_clk == GPTIMER_CLK_SRC_PLL_F40M) {
        // although PLL_F40M clock is a fixed PLL clock, which is unchangeable
        // on ESP32C2, PLL_F40M can be turned off even during DFS (unlike other PLL clocks)
        // so we're acquiring a fake "APB" lock here to prevent the system from doing DFS
        pm_lock_type = ESP_PM_APB_FREQ_MAX;
    }
#endif // CONFIG_IDF_TARGET_ESP32C2

    if (need_pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_create(pm_lock_type, 0, soc_timg_gptimer_signals[group_id][timer_id].module_name, &timer->pm_lock),
                            TAG, "create pm lock failed");
    }
#endif // CONFIG_PM_ENABLE

    // !!! HARDWARE SHARED RESOURCE !!!
    // on some ESP chip, different peripheral's clock source setting are mixed in the same register
    // so we need to make this done in an atomic way
    GPTIMER_CLOCK_SRC_ATOMIC() {
        timer_ll_set_clock_source(group_id, timer_id, src_clk);
        timer_ll_enable_clock(group_id, timer_id, true);
    }
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

#if CONFIG_PM_ENABLE
esp_err_t gptimer_get_pm_lock(gptimer_handle_t timer, esp_pm_lock_handle_t *ret_pm_lock)
{
    ESP_RETURN_ON_FALSE(timer && ret_pm_lock, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    *ret_pm_lock = timer->pm_lock;
    return ESP_OK;
}
#endif // CONFIG_PM_ENABLE

int gptimer_get_group_id(gptimer_handle_t timer, int *group_id)
{
    ESP_RETURN_ON_FALSE(timer && group_id, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    *group_id = timer->group->group_id;
    return ESP_OK;
}

#if CONFIG_GPTIMER_ENABLE_DEBUG_LOG
__attribute__((constructor))
static void gptimer_override_default_log_level(void)
{
    esp_log_level_set(TAG, ESP_LOG_VERBOSE);
}
#endif
