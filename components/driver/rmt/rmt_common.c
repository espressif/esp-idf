/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/lock.h>
#include "sdkconfig.h"
#if CONFIG_RMT_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "esp_check.h"
#include "rmt_private.h"
#include "clk_ctrl_os.h"
#include "soc/rtc.h"
#include "soc/rmt_periph.h"
#include "hal/rmt_ll.h"
#include "driver/gpio.h"
#include "esp_clk_tree.h"
#include "esp_private/periph_ctrl.h"

static const char *TAG = "rmt";

typedef struct rmt_platform_t {
    _lock_t mutex;                        // platform level mutex lock
    rmt_group_t *groups[SOC_RMT_GROUPS];  // array of RMT group instances
    int group_ref_counts[SOC_RMT_GROUPS]; // reference count used to protect group install/uninstall
} rmt_platform_t;

static rmt_platform_t s_platform; // singleton platform

rmt_group_t *rmt_acquire_group_handle(int group_id)
{
    bool new_group = false;
    rmt_group_t *group = NULL;

    // prevent install rmt group concurrently
    _lock_acquire(&s_platform.mutex);
    if (!s_platform.groups[group_id]) {
        group = heap_caps_calloc(1, sizeof(rmt_group_t), RMT_MEM_ALLOC_CAPS);
        if (group) {
            new_group = true;
            s_platform.groups[group_id] = group;
            group->group_id = group_id;
            group->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
            // initial occupy_mask: 1111...100...0
            group->occupy_mask = UINT32_MAX & ~((1 << SOC_RMT_CHANNELS_PER_GROUP) - 1);
            // group clock won't be configured at this stage, it will be set when allocate the first channel
            group->clk_src = 0;
            // enable APB access RMT registers
            periph_module_enable(rmt_periph_signals.groups[group_id].module);
            periph_module_reset(rmt_periph_signals.groups[group_id].module);
            // hal layer initialize
            rmt_hal_init(&group->hal);
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
        ESP_LOGD(TAG, "new group(%d) at %p, occupy=%"PRIx32, group_id, group, group->occupy_mask);
    }
    return group;
}

void rmt_release_group_handle(rmt_group_t *group)
{
    int group_id = group->group_id;
    rmt_clock_source_t clk_src = group->clk_src;
    bool do_deinitialize = false;

    _lock_acquire(&s_platform.mutex);
    s_platform.group_ref_counts[group_id]--;
    if (s_platform.group_ref_counts[group_id] == 0) {
        do_deinitialize = true;
        s_platform.groups[group_id] = NULL;
        // hal layer deinitialize
        rmt_hal_deinit(&group->hal);
        periph_module_disable(rmt_periph_signals.groups[group_id].module);
        free(group);
    }
    _lock_release(&s_platform.mutex);

    switch (clk_src) {
#if SOC_RMT_SUPPORT_RC_FAST
    case RMT_CLK_SRC_RC_FAST:
        periph_rtc_dig_clk8m_disable();
        break;
#endif // SOC_RMT_SUPPORT_RC_FAST
    default:
        break;
    }

    if (do_deinitialize) {
        ESP_LOGD(TAG, "del group(%d)", group_id);
    }
}

esp_err_t rmt_select_periph_clock(rmt_channel_handle_t chan, rmt_clock_source_t clk_src)
{
    esp_err_t ret = ESP_OK;
    rmt_group_t *group = chan->group;
    int channel_id = chan->channel_id;
    uint32_t periph_src_clk_hz = 0;
    bool clock_selection_conflict = false;
    // check if we need to update the group clock source, group clock source is shared by all channels
    portENTER_CRITICAL(&group->spinlock);
    if (group->clk_src == 0) {
        group->clk_src = clk_src;
    } else {
        clock_selection_conflict = (group->clk_src != clk_src);
    }
    portEXIT_CRITICAL(&group->spinlock);
    ESP_RETURN_ON_FALSE(!clock_selection_conflict, ESP_ERR_INVALID_STATE, TAG,
                        "group clock conflict, already is %d but attempt to %d", group->clk_src, clk_src);

    // TODO: [clk_tree] to use a generic clock enable/disable or acquire/release function for all clock source
#if SOC_RMT_SUPPORT_RC_FAST
    if (clk_src == RMT_CLK_SRC_RC_FAST) {
        // RC_FAST clock is not enabled automatically on start up, we enable it here manually.
        // Note there's a ref count in the enable/disable function, we must call them in pair in the driver.
        periph_rtc_dig_clk8m_enable();
    }
#endif // SOC_RMT_SUPPORT_RC_FAST

    // get clock source frequency
    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &periph_src_clk_hz),
                        TAG, "get clock source frequency failed");

#if CONFIG_PM_ENABLE
    bool need_pm_lock = true;
    // to make the RMT work reliable, the source clock must stay alive and unchanged
    // driver will create different pm lock for that purpose, according to different clock source
    esp_pm_lock_type_t pm_lock_type = ESP_PM_NO_LIGHT_SLEEP;

#if SOC_RMT_SUPPORT_RC_FAST
    if (clk_src == RMT_CLK_SRC_RC_FAST) {
        // RC_FAST won't be turn off in sleep and won't change its frequency during DFS
        need_pm_lock = false;
    }
#endif // SOC_RMT_SUPPORT_RC_FAST

#if SOC_RMT_SUPPORT_APB
    if (clk_src == RMT_CLK_SRC_APB) {
        // APB clock frequency can be changed during DFS
        pm_lock_type = ESP_PM_APB_FREQ_MAX;
    }
#endif // SOC_RMT_SUPPORT_APB

    if (need_pm_lock) {
        sprintf(chan->pm_lock_name, "rmt_%d_%d", group->group_id, channel_id); // e.g. rmt_0_0
        ret  = esp_pm_lock_create(pm_lock_type, 0, chan->pm_lock_name, &chan->pm_lock);
        ESP_RETURN_ON_ERROR(ret, TAG, "create pm lock failed");
    }
#endif // CONFIG_PM_ENABLE

    // no division for group clock source, to achieve highest resolution
    rmt_ll_set_group_clock_src(group->hal.regs, channel_id, clk_src, 1, 1, 0);
    group->resolution_hz = periph_src_clk_hz;
    ESP_LOGD(TAG, "group clock resolution:%"PRIu32, group->resolution_hz);
    return ret;
}

esp_err_t rmt_get_channel_id(rmt_channel_handle_t channel, int *ret_id)
{
    ESP_RETURN_ON_FALSE(channel && ret_id, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    *ret_id = channel->channel_id;
    return ESP_OK;
}

esp_err_t rmt_apply_carrier(rmt_channel_handle_t channel, const rmt_carrier_config_t *config)
{
    // specially, we allow config to be NULL, means to disable the carrier submodule
    ESP_RETURN_ON_FALSE(channel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return channel->set_carrier_action(channel, config);
}

esp_err_t rmt_del_channel(rmt_channel_handle_t channel)
{
    ESP_RETURN_ON_FALSE(channel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(channel->fsm == RMT_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "channel not in init state");
    gpio_reset_pin(channel->gpio_num);
    return channel->del(channel);
}

esp_err_t rmt_enable(rmt_channel_handle_t channel)
{
    ESP_RETURN_ON_FALSE(channel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(channel->fsm == RMT_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "channel not in init state");
    return channel->enable(channel);
}

esp_err_t rmt_disable(rmt_channel_handle_t channel)
{
    ESP_RETURN_ON_FALSE(channel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(channel->fsm == RMT_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "channel not in enable state");
    return channel->disable(channel);
}
