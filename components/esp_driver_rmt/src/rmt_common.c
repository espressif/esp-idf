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

#if SOC_PERIPH_CLK_CTRL_SHARED
#define RMT_CLOCK_SRC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define RMT_CLOCK_SRC_ATOMIC()
#endif

#if !SOC_RCC_IS_INDEPENDENT
#define RMT_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define RMT_RCC_ATOMIC()
#endif

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
            // group interrupt priority is shared between all channels, it will be set when allocate the first channel
            group->intr_priority = RMT_GROUP_INTR_PRIORITY_UNINITIALIZED;
            // enable the bus clock for the RMT peripheral
            RMT_RCC_ATOMIC() {
                rmt_ll_enable_bus_clock(group_id, true);
                rmt_ll_reset_register(group_id);
            }
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
    rmt_hal_context_t *hal = &group->hal;

    _lock_acquire(&s_platform.mutex);
    s_platform.group_ref_counts[group_id]--;
    if (s_platform.group_ref_counts[group_id] == 0) {
        do_deinitialize = true;
        s_platform.groups[group_id] = NULL;
        // disable core clock
        RMT_CLOCK_SRC_ATOMIC() {
            rmt_ll_enable_group_clock(hal->regs, false);
        }
        // hal layer deinitialize
        rmt_hal_deinit(hal);
        // disable bus clock
        RMT_RCC_ATOMIC() {
            rmt_ll_enable_bus_clock(group_id, false);
        }
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
    // if DMA is not used, we're using CPU to push the data to the RMT FIFO
    // if the CPU frequency goes down, the transfer+encoding scheme could be unstable because CPU can't fill the data in time
    // so, choose ESP_PM_CPU_FREQ_MAX lock for non-dma mode
    // otherwise, chose lock type based on the clock source
    esp_pm_lock_type_t pm_lock_type = chan->dma_chan ? ESP_PM_NO_LIGHT_SLEEP : ESP_PM_CPU_FREQ_MAX;

#if SOC_RMT_SUPPORT_APB
    if (clk_src == RMT_CLK_SRC_APB) {
        // APB clock frequency can be changed during DFS
        pm_lock_type = ESP_PM_APB_FREQ_MAX;
    }
#endif // SOC_RMT_SUPPORT_APB

    sprintf(chan->pm_lock_name, "rmt_%d_%d", group->group_id, channel_id); // e.g. rmt_0_0
    ret  = esp_pm_lock_create(pm_lock_type, 0, chan->pm_lock_name, &chan->pm_lock);
    ESP_RETURN_ON_ERROR(ret, TAG, "create pm lock failed");
#endif // CONFIG_PM_ENABLE

    // no division for group clock source, to achieve highest resolution
    RMT_CLOCK_SRC_ATOMIC() {
        rmt_ll_set_group_clock_src(group->hal.regs, channel_id, clk_src, 1, 1, 0);
        rmt_ll_enable_group_clock(group->hal.regs, true);
    }
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
    gpio_reset_pin(channel->gpio_num);
    return channel->del(channel);
}

esp_err_t rmt_enable(rmt_channel_handle_t channel)
{
    ESP_RETURN_ON_FALSE(channel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return channel->enable(channel);
}

esp_err_t rmt_disable(rmt_channel_handle_t channel)
{
    ESP_RETURN_ON_FALSE(channel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return channel->disable(channel);
}

bool rmt_set_intr_priority_to_group(rmt_group_t *group, int intr_priority)
{
    bool priority_conflict = false;
    portENTER_CRITICAL(&group->spinlock);
    if (group->intr_priority == RMT_GROUP_INTR_PRIORITY_UNINITIALIZED) {
        // intr_priority never allocated, accept user's value unconditionally
        // intr_priority could only be set once here
        group->intr_priority = intr_priority;
    } else {
        // group intr_priority already specified
        // If interrupt priority specified before, it CANNOT BE CHANGED until `rmt_release_group_handle()` called
        // So we have to check if the new priority specified conflicts with the old one
        if (intr_priority) {
            // User specified intr_priority, check if conflict or not
            // Even though the `group->intr_priority` is 0, an intr_priority must have been specified automatically too,
            // although we do not know it exactly now, so specifying the intr_priority again might also cause conflict.
            // So no matter if `group->intr_priority` is 0 or not, we have to check.
            // Value `0` of `group->intr_priority` means "unknown", NOT "unspecified"!
            if (intr_priority != (group->intr_priority)) {
                // intr_priority conflicts!
                priority_conflict = true;
            }
        }
        // else do nothing
        // user did not specify intr_priority, then keep the old priority
        // We'll use the `RMT_INTR_ALLOC_FLAG | RMT_ALLOW_INTR_PRIORITY_MASK`, which should always success
    }
    // The `group->intr_priority` will not change any longer, even though another task tries to modify it.
    // So we could exit critical here safely.
    portEXIT_CRITICAL(&group->spinlock);
    return priority_conflict;
}

int rmt_get_isr_flags(rmt_group_t *group)
{
    int isr_flags = RMT_INTR_ALLOC_FLAG;
    if (group->intr_priority) {
        // Use user-specified priority bit
        isr_flags |= (1 << (group->intr_priority));
    } else {
        // Allow all LOWMED priority bits
        isr_flags |= RMT_ALLOW_INTR_PRIORITY_MASK;
    }
    return isr_flags;
}
