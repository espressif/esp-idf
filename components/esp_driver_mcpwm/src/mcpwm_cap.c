/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mcpwm_private.h"
#include "esp_private/esp_clk.h"
#include "esp_clk_tree.h"
#include "esp_memory_utils.h"
#include "driver/mcpwm_cap.h"
#include "driver/gpio.h"
#include "esp_private/gpio.h"

static void mcpwm_capture_default_isr(void *args);

static esp_err_t mcpwm_cap_timer_register_to_group(mcpwm_cap_timer_t *cap_timer, int group_id)
{
    mcpwm_group_t *group = mcpwm_acquire_group_handle(group_id);
    ESP_RETURN_ON_FALSE(group, ESP_ERR_NO_MEM, TAG, "no mem for group (%d)", group_id);

    bool new_timer = false;
    portENTER_CRITICAL(&group->spinlock);
    if (!group->cap_timer) {
        group->cap_timer = cap_timer;
        new_timer = true;
    }
    portEXIT_CRITICAL(&group->spinlock);

    if (!new_timer) {
        mcpwm_release_group_handle(group);
        group = NULL;
    } else {
        cap_timer->group = group;
    }
    ESP_RETURN_ON_FALSE(new_timer, ESP_ERR_NOT_FOUND, TAG, "no free cap timer in group (%d)", group_id);
    return ESP_OK;
}

static void mcpwm_cap_timer_unregister_from_group(mcpwm_cap_timer_t *cap_timer)
{
    mcpwm_group_t *group = cap_timer->group;

    portENTER_CRITICAL(&group->spinlock);
    group->cap_timer = NULL;
    portEXIT_CRITICAL(&group->spinlock);

    // capture timer has a reference on group, release it now
    mcpwm_release_group_handle(group);
}

static esp_err_t mcpwm_cap_timer_destroy(mcpwm_cap_timer_t *cap_timer)
{
#if !SOC_MCPWM_CAPTURE_CLK_FROM_GROUP && CONFIG_PM_ENABLE
    if (cap_timer->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_delete(cap_timer->pm_lock), TAG, "delete pm_lock failed");
    }
#endif
    if (cap_timer->group) {
        mcpwm_cap_timer_unregister_from_group(cap_timer);
    }
    free(cap_timer);
    return ESP_OK;
}

esp_err_t mcpwm_new_capture_timer(const mcpwm_capture_timer_config_t *config, mcpwm_cap_timer_handle_t *ret_cap_timer)
{
    esp_err_t ret = ESP_OK;
    mcpwm_cap_timer_t *cap_timer = NULL;
    ESP_GOTO_ON_FALSE(config && ret_cap_timer, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(config->group_id < SOC_MCPWM_GROUPS && config->group_id >= 0, ESP_ERR_INVALID_ARG,
                      err, TAG, "invalid group ID:%d", config->group_id);

#if !SOC_MCPWM_SUPPORT_SLEEP_RETENTION
    ESP_RETURN_ON_FALSE(config->flags.allow_pd == 0, ESP_ERR_NOT_SUPPORTED, TAG, "register back up is not supported");
#endif // SOC_MCPWM_SUPPORT_SLEEP_RETENTION

    cap_timer = heap_caps_calloc(1, sizeof(mcpwm_cap_timer_t), MCPWM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(cap_timer, ESP_ERR_NO_MEM, err, TAG, "no mem for capture timer");

    ESP_GOTO_ON_ERROR(mcpwm_cap_timer_register_to_group(cap_timer, config->group_id), err, TAG, "register timer failed");
    mcpwm_group_t *group = cap_timer->group;
    int group_id = group->group_id;

    mcpwm_capture_clock_source_t clk_src = config->clk_src ? config->clk_src : MCPWM_CAPTURE_CLK_SRC_DEFAULT;
#if SOC_MCPWM_CAPTURE_CLK_FROM_GROUP
    // capture timer clock source is same as the MCPWM group
    ESP_GOTO_ON_ERROR(mcpwm_select_periph_clock(group, (soc_module_clk_t)clk_src), err, TAG, "set group clock failed");
#if CONFIG_PM_ENABLE
    cap_timer->pm_lock = group->pm_lock;
#endif
    uint32_t periph_src_clk_hz = 0;
    ESP_GOTO_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &periph_src_clk_hz), err, TAG, "get clock source freq failed");
    ESP_LOGD(TAG, "periph_src_clk_hz %"PRIu32"", periph_src_clk_hz);
    // when resolution_hz set to zero, use default resolution_hz
    uint32_t cap_timer_prescale = group->prescale > 0 ? group->prescale : MCPWM_GROUP_CLOCK_DEFAULT_PRESCALE;
    uint32_t resolution_hz = config->resolution_hz ? config->resolution_hz : periph_src_clk_hz / cap_timer_prescale;

    ESP_GOTO_ON_ERROR(mcpwm_set_prescale(group, resolution_hz, MCPWM_LL_MAX_CAPTURE_TIMER_PRESCALE, NULL), err, TAG, "set prescale failed");
    cap_timer->resolution_hz = group->resolution_hz;
    if (cap_timer->resolution_hz != resolution_hz) {
        ESP_LOGW(TAG, "adjust cap_timer resolution to %"PRIu32"Hz", cap_timer->resolution_hz);
    }
#else
    // capture timer has independent clock source selection
    switch (clk_src) {
    case MCPWM_CAPTURE_CLK_SRC_APB:
        cap_timer->resolution_hz = esp_clk_apb_freq();
        if (config->resolution_hz) {
            ESP_LOGW(TAG, "cap_timer resolution is not adjustable in current target, still %"PRIu32"Hz", cap_timer->resolution_hz);
        }
#if CONFIG_PM_ENABLE
        ret  = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "mcpwm_cap_timer", &cap_timer->pm_lock);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "create ESP_PM_APB_FREQ_MAX lock failed");
#endif // CONFIG_PM_ENABLE
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "invalid clock source:%d", config->clk_src);
    }
#endif

    // fill in other capture timer specific members
    cap_timer->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    cap_timer->fsm = MCPWM_CAP_TIMER_FSM_INIT;
    *ret_cap_timer = cap_timer;

#if MCPWM_USE_RETENTION_LINK
    if (config->flags.allow_pd != 0) {
        mcpwm_create_retention_module(group);
    }
#endif // MCPWM_USE_RETENTION_LINK

    ESP_LOGD(TAG, "new capture timer at %p, in group (%d), resolution %"PRIu32, cap_timer, group_id, cap_timer->resolution_hz);
    return ESP_OK;

err:
    if (cap_timer) {
        mcpwm_cap_timer_destroy(cap_timer);
    }
    return ret;
}

esp_err_t mcpwm_del_capture_timer(mcpwm_cap_timer_handle_t cap_timer)
{
    ESP_RETURN_ON_FALSE(cap_timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(cap_timer->fsm == MCPWM_CAP_TIMER_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "timer not in init state");
    for (int i = 0; i < SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER; i++) {
        ESP_RETURN_ON_FALSE(!cap_timer->cap_channels[i], ESP_ERR_INVALID_STATE, TAG, "cap channel still in working");
    }
    mcpwm_group_t *group = cap_timer->group;

    ESP_LOGD(TAG, "del capture timer in group %d", group->group_id);
    // recycle memory resource
    ESP_RETURN_ON_ERROR(mcpwm_cap_timer_destroy(cap_timer), TAG, "destroy capture timer failed");
    return ESP_OK;
}

esp_err_t mcpwm_capture_timer_enable(mcpwm_cap_timer_handle_t cap_timer)
{
    ESP_RETURN_ON_FALSE(cap_timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(cap_timer->fsm == MCPWM_CAP_TIMER_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "timer not in init state");
#if CONFIG_PM_ENABLE
    if (cap_timer->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(cap_timer->pm_lock), TAG, "acquire pm_lock failed");
    }
#endif
    cap_timer->fsm = MCPWM_CAP_TIMER_FSM_ENABLE;
    return ESP_OK;
}

esp_err_t mcpwm_capture_timer_disable(mcpwm_cap_timer_handle_t cap_timer)
{
    ESP_RETURN_ON_FALSE(cap_timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(cap_timer->fsm == MCPWM_CAP_TIMER_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "timer not in enable state");
#if CONFIG_PM_ENABLE
    if (cap_timer->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(cap_timer->pm_lock), TAG, "release pm_lock failed");
    }
#endif
    cap_timer->fsm = MCPWM_CAP_TIMER_FSM_INIT;
    return ESP_OK;
}

esp_err_t mcpwm_capture_timer_start(mcpwm_cap_timer_handle_t cap_timer)
{
    ESP_RETURN_ON_FALSE(cap_timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(cap_timer->fsm == MCPWM_CAP_TIMER_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "timer not enabled yet");
    mcpwm_group_t *group = cap_timer->group;

    portENTER_CRITICAL_SAFE(&cap_timer->spinlock);
    mcpwm_ll_capture_enable_timer(group->hal.dev, true);
    portEXIT_CRITICAL_SAFE(&cap_timer->spinlock);

    return ESP_OK;
}

esp_err_t mcpwm_capture_timer_stop(mcpwm_cap_timer_handle_t cap_timer)
{
    ESP_RETURN_ON_FALSE(cap_timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(cap_timer->fsm == MCPWM_CAP_TIMER_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "timer not enabled yet");
    mcpwm_group_t *group = cap_timer->group;

    portENTER_CRITICAL_SAFE(&cap_timer->spinlock);
    mcpwm_ll_capture_enable_timer(group->hal.dev, false);
    portEXIT_CRITICAL_SAFE(&cap_timer->spinlock);

    return ESP_OK;
}

esp_err_t mcpwm_capture_timer_get_resolution(mcpwm_cap_timer_handle_t cap_timer, uint32_t *out_resolution)
{
    ESP_RETURN_ON_FALSE(cap_timer && out_resolution, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    *out_resolution = cap_timer->resolution_hz;
    return ESP_OK;
}

static esp_err_t mcpwm_capture_channel_register_to_timer(mcpwm_cap_channel_t *cap_channel, mcpwm_cap_timer_t *cap_timer)
{
    int cap_chan_id = -1;
    portENTER_CRITICAL(&cap_timer->spinlock);
    for (int i = 0; i < SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER; i++) {
        if (!cap_timer->cap_channels[i]) {
            cap_timer->cap_channels[i] = cap_channel;
            cap_chan_id = i;
            break;
        }
    }
    portEXIT_CRITICAL(&cap_timer->spinlock);
    ESP_RETURN_ON_FALSE(cap_chan_id >= 0, ESP_ERR_NOT_FOUND, TAG, "no free channel in the timer (%d)", cap_timer->group->group_id);

    cap_channel->cap_chan_id = cap_chan_id;
    cap_channel->cap_timer = cap_timer;
    return ESP_OK;
}

static void mcpwm_capture_channel_unregister_from_timer(mcpwm_cap_channel_t *cap_chan)
{
    mcpwm_cap_timer_t *cap_timer = cap_chan->cap_timer;
    int cap_chan_id = cap_chan->cap_chan_id;

    portENTER_CRITICAL(&cap_timer->spinlock);
    cap_timer->cap_channels[cap_chan_id] = NULL;
    portEXIT_CRITICAL(&cap_timer->spinlock);
}

static esp_err_t mcpwm_capture_channel_destroy(mcpwm_cap_channel_t *cap_chan)
{
    if (cap_chan->intr) {
        ESP_RETURN_ON_ERROR(esp_intr_free(cap_chan->intr), TAG, "delete interrupt service failed");
    }
    if (cap_chan->cap_timer) {
        mcpwm_capture_channel_unregister_from_timer(cap_chan);
    }
    free(cap_chan);
    return ESP_OK;
}

esp_err_t mcpwm_new_capture_channel(mcpwm_cap_timer_handle_t cap_timer, const mcpwm_capture_channel_config_t *config, mcpwm_cap_channel_handle_t *ret_cap_channel)
{
    esp_err_t ret = ESP_OK;
    mcpwm_cap_channel_t *cap_chan = NULL;
    ESP_GOTO_ON_FALSE(cap_timer && config && ret_cap_channel, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(config->prescale && config->prescale <= MCPWM_LL_MAX_CAPTURE_PRESCALE, ESP_ERR_INVALID_ARG, err, TAG, "invalid prescale");
    if (config->intr_priority) {
        ESP_GOTO_ON_FALSE(1 << (config->intr_priority) & MCPWM_ALLOW_INTR_PRIORITY_MASK, ESP_ERR_INVALID_ARG, err,
                          TAG, "invalid interrupt priority:%d", config->intr_priority);
    }

    // create instance firstly, then install onto platform
    cap_chan = heap_caps_calloc(1, sizeof(mcpwm_cap_channel_t), MCPWM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(cap_chan, ESP_ERR_NO_MEM, err, TAG, "no mem for capture channel");

    ESP_GOTO_ON_ERROR(mcpwm_capture_channel_register_to_timer(cap_chan, cap_timer), err, TAG, "register channel failed");
    mcpwm_group_t *group = cap_timer->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int cap_chan_id = cap_chan->cap_chan_id;

    // if interrupt priority specified before, it cannot be changed until the group is released
    // check if the new priority specified consistents with the old one
    ESP_GOTO_ON_ERROR(mcpwm_check_intr_priority(group, config->intr_priority), err, TAG, "set group interrupt priority failed");

    mcpwm_ll_capture_enable_negedge(hal->dev, cap_chan_id, config->flags.neg_edge);
    mcpwm_ll_capture_enable_posedge(hal->dev, cap_chan_id, config->flags.pos_edge);
    mcpwm_ll_invert_input(hal->dev, cap_chan_id, config->flags.invert_cap_signal);
    mcpwm_ll_capture_set_prescale(hal->dev, cap_chan_id, config->prescale);

    if (config->gpio_num >= 0) {
        // GPIO configuration
        gpio_func_sel(config->gpio_num, PIN_FUNC_GPIO);
        gpio_input_enable(config->gpio_num);
        esp_rom_gpio_connect_in_signal(config->gpio_num, mcpwm_periph_signals.groups[group->group_id].captures[cap_chan_id].cap_sig, 0);
    }

    cap_chan->gpio_num = config->gpio_num;
    cap_chan->fsm = MCPWM_CAP_CHAN_FSM_INIT;

    *ret_cap_channel = cap_chan;
    ESP_LOGD(TAG, "new capture channel (%d,%d) at %p", group->group_id, cap_chan_id, cap_chan);
    return ESP_OK;
err:
    if (cap_chan) {
        mcpwm_capture_channel_destroy(cap_chan);
    }
    return ret;
}

esp_err_t mcpwm_del_capture_channel(mcpwm_cap_channel_handle_t cap_channel)
{
    ESP_RETURN_ON_FALSE(cap_channel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(cap_channel->fsm == MCPWM_CAP_CHAN_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "channel not in init state");
    mcpwm_cap_timer_t *cap_timer = cap_channel->cap_timer;
    mcpwm_group_t *group = cap_timer->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int cap_chan_id = cap_channel->cap_chan_id;

    ESP_LOGD(TAG, "del capture channel (%d,%d)", group->group_id, cap_channel->cap_chan_id);

    portENTER_CRITICAL(&group->spinlock);
    mcpwm_ll_intr_enable(hal->dev, MCPWM_LL_EVENT_CAPTURE(cap_chan_id), false);
    mcpwm_ll_intr_clear_status(hal->dev, MCPWM_LL_EVENT_CAPTURE(cap_chan_id));
    portEXIT_CRITICAL(&group->spinlock);

    // disconnect signal from the GPIO pin
    esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT,
                                   mcpwm_periph_signals.groups[group->group_id].captures[cap_chan_id].cap_sig, 0);

    // recycle memory resource
    ESP_RETURN_ON_ERROR(mcpwm_capture_channel_destroy(cap_channel), TAG, "destroy capture channel failed");
    return ESP_OK;
}

esp_err_t mcpwm_capture_channel_enable(mcpwm_cap_channel_handle_t cap_channel)
{
    ESP_RETURN_ON_FALSE(cap_channel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(cap_channel->fsm == MCPWM_CAP_CHAN_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "channel not in init state");
    mcpwm_hal_context_t *hal = &cap_channel->cap_timer->group->hal;

    // enable interrupt service
    if (cap_channel->intr) {
        ESP_RETURN_ON_ERROR(esp_intr_enable(cap_channel->intr), TAG, "enable interrupt service failed");
    }
    // enable channel
    mcpwm_ll_capture_enable_channel(hal->dev, cap_channel->cap_chan_id, true);

    cap_channel->fsm = MCPWM_CAP_CHAN_FSM_ENABLE;
    return ESP_OK;
}

esp_err_t mcpwm_capture_channel_disable(mcpwm_cap_channel_handle_t cap_channel)
{
    ESP_RETURN_ON_FALSE(cap_channel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(cap_channel->fsm == MCPWM_CAP_CHAN_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "channel not in enable state");
    mcpwm_hal_context_t *hal = &cap_channel->cap_timer->group->hal;

    // disable channel
    mcpwm_ll_capture_enable_channel(hal->dev, cap_channel->cap_chan_id, false);

    // disable interrupt service
    if (cap_channel->intr) {
        ESP_RETURN_ON_ERROR(esp_intr_disable(cap_channel->intr), TAG, "disable interrupt service failed");
    }

    cap_channel->fsm = MCPWM_CAP_CHAN_FSM_INIT;
    return ESP_OK;
}

esp_err_t mcpwm_capture_channel_register_event_callbacks(mcpwm_cap_channel_handle_t cap_channel, const mcpwm_capture_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(cap_channel && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    mcpwm_group_t *group = cap_channel->cap_timer->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int group_id = group->group_id;
    int cap_chan_id = cap_channel->cap_chan_id;

#if CONFIG_MCPWM_ISR_CACHE_SAFE
    if (cbs->on_cap) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_cap), ESP_ERR_INVALID_ARG, TAG, "on_cap callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif

    // lazy install interrupt service
    if (!cap_channel->intr) {
        ESP_RETURN_ON_FALSE(cap_channel->fsm == MCPWM_CAP_CHAN_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "channel not in init state");
        int isr_flags = MCPWM_INTR_ALLOC_FLAG;
        isr_flags |= mcpwm_get_intr_priority_flag(group);
        ESP_RETURN_ON_ERROR(esp_intr_alloc_intrstatus(mcpwm_periph_signals.groups[group_id].irq_id, isr_flags,
                                                      (uint32_t)mcpwm_ll_intr_get_status_reg(hal->dev), MCPWM_LL_EVENT_CAPTURE(cap_chan_id),
                                                      mcpwm_capture_default_isr, cap_channel, &cap_channel->intr), TAG, "install interrupt service for cap channel failed");
    }

    portENTER_CRITICAL(&group->spinlock);
    mcpwm_ll_intr_enable(hal->dev, MCPWM_LL_EVENT_CAPTURE(cap_chan_id), cbs->on_cap != NULL);
    portEXIT_CRITICAL(&group->spinlock);

    cap_channel->on_cap = cbs->on_cap;
    cap_channel->user_data = user_data;

    return ESP_OK;
}

esp_err_t mcpwm_capture_channel_trigger_soft_catch(mcpwm_cap_channel_handle_t cap_channel)
{
    ESP_RETURN_ON_FALSE(cap_channel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(cap_channel->fsm == MCPWM_CAP_CHAN_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "channel not enabled yet");
    mcpwm_cap_timer_t *cap_timer = cap_channel->cap_timer;
    mcpwm_group_t *group = cap_timer->group;

    // note: soft capture can also triggers the interrupt routine
    mcpwm_ll_trigger_soft_capture(group->hal.dev, cap_channel->cap_chan_id);
    return ESP_OK;
}

esp_err_t mcpwm_capture_timer_set_phase_on_sync(mcpwm_cap_timer_handle_t cap_timer, const mcpwm_capture_timer_sync_phase_config_t *config)
{
    ESP_RETURN_ON_FALSE(cap_timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    // capture timer only support count up
    ESP_RETURN_ON_FALSE(config->direction == MCPWM_TIMER_DIRECTION_UP, ESP_ERR_INVALID_ARG, TAG, "invalid sync direction");
    mcpwm_group_t *group = cap_timer->group;
    mcpwm_sync_t *sync_source = config->sync_src;

    // a non-NULL sync_src means to enable sync feature
    if (sync_source) {
        switch (sync_source->type) {
        case MCPWM_SYNC_TYPE_GPIO: {
            ESP_RETURN_ON_FALSE(group == sync_source->group, ESP_ERR_INVALID_ARG, TAG, "capture timer and sync source are not in the same group");
            mcpwm_gpio_sync_src_t *gpio_sync_src = __containerof(sync_source, mcpwm_gpio_sync_src_t, base);
            mcpwm_ll_capture_set_gpio_sync(group->hal.dev, gpio_sync_src->sync_id);
            ESP_LOGD(TAG, "enable sync to GPIO (%d,%d) for cap timer (%d)",
                     group->group_id, gpio_sync_src->sync_id, group->group_id);
            break;
        }
        case MCPWM_SYNC_TYPE_TIMER: {
            ESP_RETURN_ON_FALSE(group == sync_source->group, ESP_ERR_INVALID_ARG, TAG, "capture timer and sync source are not in the same group");
            mcpwm_timer_sync_src_t *timer_sync_src = __containerof(sync_source, mcpwm_timer_sync_src_t, base);
            mcpwm_ll_capture_set_timer_sync(group->hal.dev, timer_sync_src->timer->timer_id);
            ESP_LOGD(TAG, "enable sync to pwm timer (%d,%d) for cap timer (%d)",
                     group->group_id, timer_sync_src->timer->timer_id, group->group_id);
            break;
        }
        case MCPWM_SYNC_TYPE_SOFT: {
            mcpwm_soft_sync_src_t *soft_sync = __containerof(sync_source, mcpwm_soft_sync_src_t, base);
            soft_sync->soft_sync_from = MCPWM_SOFT_SYNC_FROM_CAP;
            soft_sync->cap_timer = cap_timer;
            soft_sync->base.group = group;
            break;
        }
        }
        mcpwm_ll_capture_enable_timer_sync(group->hal.dev, true);
        mcpwm_ll_capture_set_sync_phase_value(group->hal.dev, config->count_value);
    } else { // disable sync feature
        mcpwm_ll_capture_enable_timer_sync(group->hal.dev, false);
        ESP_LOGD(TAG, "disable sync for cap timer (%d)", group->group_id);
    }
    return ESP_OK;
}

static void mcpwm_capture_default_isr(void *args)
{
    mcpwm_cap_channel_t *cap_chan = (mcpwm_cap_channel_t *)args;
    mcpwm_group_t *group = cap_chan->cap_timer->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int cap_id = cap_chan->cap_chan_id;
    bool need_yield = false;

    uint32_t status = mcpwm_ll_intr_get_status(hal->dev);
    mcpwm_ll_intr_clear_status(hal->dev, status & MCPWM_LL_EVENT_CAPTURE(cap_id));

    // read capture value and pass to user
    mcpwm_capture_event_data_t data = {
        .cap_value = mcpwm_ll_capture_get_value(hal->dev, cap_id),
        .cap_edge = mcpwm_ll_capture_get_edge(hal->dev, cap_id),
    };
    if (status & MCPWM_LL_EVENT_CAPTURE(cap_id)) {
        mcpwm_capture_event_cb_t cb = cap_chan->on_cap;
        if (cb) {
            if (cb(cap_chan, &data, cap_chan->user_data)) {
                need_yield = true;
            }
        }
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}
