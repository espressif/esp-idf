/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
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
#include "esp_private/esp_clk.h"
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

    // [clk_tree] TODO: replace the following switch table by clk_tree API
    switch (clk_src) {
#if SOC_RMT_SUPPORT_APB
    case RMT_CLK_SRC_APB:
        periph_src_clk_hz = esp_clk_apb_freq();
#if CONFIG_PM_ENABLE
        sprintf(chan->pm_lock_name, "rmt_%d_%d", group->group_id, channel_id); // e.g. rmt_0_0
        ret  = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, chan->pm_lock_name, &chan->pm_lock);
        ESP_RETURN_ON_ERROR(ret, TAG, "create APB_FREQ_MAX lock failed");
        ESP_LOGD(TAG, "install APB_FREQ_MAX lock for RMT channel (%d,%d)", group->group_id, channel_id);
#endif // CONFIG_PM_ENABLE
#endif // SOC_RMT_SUPPORT_APB
        break;
#if SOC_RMT_SUPPORT_AHB
    case RMT_CLK_SRC_AHB:
        // TODO: decide which kind of PM lock we should use for such clock
        periph_src_clk_hz = 48 * 1000 * 1000;
        break;
#endif // SOC_RMT_SUPPORT_AHB
#if SOC_RMT_SUPPORT_XTAL
    case RMT_CLK_SRC_XTAL:
        periph_src_clk_hz = esp_clk_xtal_freq();
        break;
#endif // SOC_RMT_SUPPORT_XTAL
#if SOC_RMT_SUPPORT_REF_TICK
    case RMT_CLK_SRC_REF_TICK:
        periph_src_clk_hz = REF_CLK_FREQ;
        break;
#endif // SOC_RMT_SUPPORT_REF_TICK
#if SOC_RMT_SUPPORT_RC_FAST
    case RMT_CLK_SRC_RC_FAST:
        periph_rtc_dig_clk8m_enable();
        periph_src_clk_hz = periph_rtc_dig_clk8m_get_freq();
        break;
#endif // SOC_RMT_SUPPORT_RC_FAST
    default:
        ESP_RETURN_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, TAG, "clock source %d is not supported", clk_src);
        break;
    }
    // no division for group clock source, to achieve highest resolution
    rmt_ll_set_group_clock_src(group->hal.regs, channel_id, clk_src, 1, 1, 0);
    group->resolution_hz = periph_src_clk_hz;
    ESP_LOGD(TAG, "group clock resolution:%"PRIu32, group->resolution_hz);
    return ret;
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
