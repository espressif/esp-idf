/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#if CONFIG_SDM_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "freertos/FreeRTOS.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_pm.h"
#include "esp_clk_tree.h"
#include "driver/gpio.h"
#include "driver/sdm.h"
#include "hal/gpio_hal.h"
#include "hal/sdm_hal.h"
#include "hal/sdm_ll.h"
#include "soc/sdm_periph.h"
#include "esp_private/esp_clk.h"
#include "esp_private/io_mux.h"

#if CONFIG_SDM_CTRL_FUNC_IN_IRAM
#define SDM_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define SDM_MEM_ALLOC_CAPS      MALLOC_CAP_DEFAULT
#endif

#define SDM_PM_LOCK_NAME_LEN_MAX 16

static const char *TAG = "sdm";

typedef struct sdm_platform_t sdm_platform_t;
typedef struct sdm_group_t sdm_group_t;
typedef struct sdm_channel_t sdm_channel_t;

struct sdm_platform_t {
    _lock_t mutex;                                 // platform level mutex lock
    sdm_group_t *groups[SOC_SDM_GROUPS]; // sdm group pool
    int group_ref_counts[SOC_SDM_GROUPS];     // reference count used to protect group install/uninstall
};

struct sdm_group_t {
    int group_id;               // Group ID, index from 0
    portMUX_TYPE spinlock;      // to protect per-group register level concurrent access
    sdm_hal_context_t hal; // hal context
    sdm_channel_t *channels[SOC_SDM_CHANNELS_PER_GROUP]; // array of sdm channels
    sdm_clock_source_t clk_src; // Clock source
};

typedef enum {
    SDM_FSM_INIT,
    SDM_FSM_ENABLE,
} sdm_fsm_t;

struct sdm_channel_t {
    sdm_group_t *group;         // which group the sdm channel belongs to
    uint32_t chan_id;                // allocated channel numerical ID
    int gpio_num;                    // GPIO number
    uint32_t sample_rate_hz;         // Sample rate, in Hz
    portMUX_TYPE spinlock;           // to protect per-channels resources concurrently accessed by task and ISR handler
    esp_pm_lock_handle_t pm_lock;    // PM lock, for glitch filter, as that module can only be functional under APB
    sdm_fsm_t fsm;              // FSM state
#if CONFIG_PM_ENABLE
    char pm_lock_name[SDM_PM_LOCK_NAME_LEN_MAX]; // pm lock name
#endif
};

// sdm driver platform, it's always a singleton
static sdm_platform_t s_platform;

static sdm_group_t *sdm_acquire_group_handle(int group_id)
{
    bool new_group = false;
    sdm_group_t *group = NULL;

    // prevent install sdm group concurrently
    _lock_acquire(&s_platform.mutex);
    if (!s_platform.groups[group_id]) {
        group = heap_caps_calloc(1, sizeof(sdm_group_t), SDM_MEM_ALLOC_CAPS);
        if (group) {
            new_group = true;
            s_platform.groups[group_id] = group; // register to platform
            // initialize sdm group members
            group->group_id = group_id;
            group->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
            group->clk_src = 0;
            // initialize HAL context
            sdm_hal_init(&group->hal, group_id);
            // enable clock
            // note that, this will enables all the channels' output, and channel can't be disable/enable separately
            sdm_ll_enable_clock(group->hal.dev, true);
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
        ESP_LOGD(TAG, "new group (%d) at %p", group_id, group);
    }

    return group;
}

static void sdm_release_group_handle(sdm_group_t *group)
{
    int group_id = group->group_id;
    bool do_deinitialize = false;

    _lock_acquire(&s_platform.mutex);
    s_platform.group_ref_counts[group_id]--;
    if (s_platform.group_ref_counts[group_id] == 0) {
        assert(s_platform.groups[group_id]);
        do_deinitialize = true;
        s_platform.groups[group_id] = NULL; // deregister from platform
        sdm_ll_enable_clock(group->hal.dev, false);
    }
    _lock_release(&s_platform.mutex);

    if (do_deinitialize) {
        free(group);
        ESP_LOGD(TAG, "del group (%d)", group_id);
    }
}

static esp_err_t sdm_register_to_group(sdm_channel_t *chan)
{
    sdm_group_t *group = NULL;
    int chan_id = -1;
    for (int i = 0; i < SOC_SDM_GROUPS; i++) {
        group = sdm_acquire_group_handle(i);
        ESP_RETURN_ON_FALSE(group, ESP_ERR_NO_MEM, TAG, "no mem for group (%d)", i);
        // loop to search free unit in the group
        portENTER_CRITICAL(&group->spinlock);
        for (int j = 0; j < SOC_SDM_CHANNELS_PER_GROUP; j++) {
            if (!group->channels[j]) {
                chan_id = j;
                group->channels[j] = chan;
                break;
            }
        }
        portEXIT_CRITICAL(&group->spinlock);
        if (chan_id < 0) {
            sdm_release_group_handle(group);
        } else {
            chan->group = group;
            chan->chan_id = chan_id;
            break;
        }
    }
    ESP_RETURN_ON_FALSE(chan_id != -1, ESP_ERR_NOT_FOUND, TAG, "no free channels");
    return ESP_OK;
}

static void sdm_unregister_from_group(sdm_channel_t *chan)
{
    sdm_group_t *group = chan->group;
    int chan_id = chan->chan_id;
    portENTER_CRITICAL(&group->spinlock);
    group->channels[chan_id] = NULL;
    portEXIT_CRITICAL(&group->spinlock);
    // channel has a reference on group, release it now
    sdm_release_group_handle(group);
}

static esp_err_t sdm_destroy(sdm_channel_t *chan)
{
    if (chan->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_delete(chan->pm_lock), TAG, "delete pm lock failed");
    }
    if (chan->group) {
        sdm_unregister_from_group(chan);
    }
    free(chan);
    return ESP_OK;
}

esp_err_t sdm_new_channel(const sdm_config_t *config, sdm_channel_handle_t *ret_chan)
{
#if CONFIG_SDM_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    sdm_channel_t *chan = NULL;
    ESP_GOTO_ON_FALSE(config && ret_chan, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(GPIO_IS_VALID_OUTPUT_GPIO(config->gpio_num), ESP_ERR_INVALID_ARG, err, TAG, "invalid GPIO number");

    chan = heap_caps_calloc(1, sizeof(sdm_channel_t), SDM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(chan, ESP_ERR_NO_MEM, err, TAG, "no mem for channel");
    // register channel to the group
    ESP_GOTO_ON_ERROR(sdm_register_to_group(chan), err, TAG, "register to group failed");
    sdm_group_t *group = chan->group;
    int group_id = group->group_id;
    int chan_id = chan->chan_id;

    ESP_GOTO_ON_FALSE(group->clk_src == 0 || group->clk_src == config->clk_src, ESP_ERR_INVALID_ARG, err, TAG, "clock source conflict");
    uint32_t src_clk_hz = 0;
    ESP_GOTO_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)config->clk_src,
                                                   ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &src_clk_hz), err, TAG, "get source clock frequency failed");

#if CONFIG_PM_ENABLE
    esp_pm_lock_type_t pm_type = ESP_PM_NO_LIGHT_SLEEP;
#if SOC_SDM_CLK_SUPPORT_APB
    if (config->clk_src == SDM_CLK_SRC_APB) {
        pm_type = ESP_PM_APB_FREQ_MAX;
    }
#endif // SOC_SDM_CLK_SUPPORT_APB
    sprintf(chan->pm_lock_name, "sdm_%d_%d", group->group_id, chan_id); // e.g. sdm_0_0
    ret = esp_pm_lock_create(pm_type, 0, chan->pm_lock_name, &chan->pm_lock);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "create %s lock failed", chan->pm_lock_name);
#endif // CONFIG_PM_ENABLE
    group->clk_src = config->clk_src;

    // SDM clock comes from IO MUX, but IO MUX clock might be shared with other submodules as well
    ESP_GOTO_ON_ERROR(io_mux_set_clock_source((soc_module_clk_t)(group->clk_src)), err, TAG, "set IO MUX clock source failed");

    // GPIO configuration
    gpio_config_t gpio_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        // also enable the input path is `io_loop_back` is on, this is useful for debug
        .mode = GPIO_MODE_OUTPUT | (config->flags.io_loop_back ? GPIO_MODE_INPUT : 0),
        .pull_down_en = false,
        .pull_up_en = true,
        .pin_bit_mask = 1ULL << config->gpio_num,
    };
    ESP_GOTO_ON_ERROR(gpio_config(&gpio_conf), err, TAG, "config GPIO failed");
    esp_rom_gpio_connect_out_signal(config->gpio_num, sigma_delta_periph_signals.channels[chan_id].sd_sig, config->flags.invert_out, false);
    chan->gpio_num = config->gpio_num;

    // set prescale based on sample rate
    uint32_t prescale = src_clk_hz / config->sample_rate_hz;
    sdm_ll_set_prescale(group->hal.dev, chan_id, prescale);
    chan->sample_rate_hz = src_clk_hz / prescale;
    // preset the duty cycle to zero
    sdm_ll_set_pulse_density(group->hal.dev, chan_id, 0);

    // initialize other members of timer
    chan->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    chan->fsm = SDM_FSM_INIT; // put the channel into init state

    ESP_LOGD(TAG, "new sdm channel (%d,%d) at %p, gpio=%d, sample rate=%"PRIu32"Hz", group_id, chan_id, chan, chan->gpio_num, chan->sample_rate_hz);
    *ret_chan = chan;
    return ESP_OK;
err:
    if (chan) {
        sdm_destroy(chan);
    }
    return ret;
}

esp_err_t sdm_del_channel(sdm_channel_handle_t chan)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(chan->fsm == SDM_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "channel not in init state");
    sdm_group_t *group = chan->group;
    int group_id = group->group_id;
    int chan_id = chan->chan_id;
    ESP_LOGD(TAG, "del channel (%d,%d)", group_id, chan_id);
    // recycle memory resource
    ESP_RETURN_ON_ERROR(sdm_destroy(chan), TAG, "destroy channel failed");
    return ESP_OK;
}

esp_err_t sdm_channel_enable(sdm_channel_handle_t chan)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(chan->fsm == SDM_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "channel not in init state");

    // acquire power manager lock
    if (chan->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(chan->pm_lock), TAG, "acquire pm_lock failed");
    }
    chan->fsm = SDM_FSM_ENABLE;
    return ESP_OK;
}

esp_err_t sdm_channel_disable(sdm_channel_handle_t chan)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(chan->fsm == SDM_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "channel not in enable state");

    // release power manager lock
    if (chan->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(chan->pm_lock), TAG, "release pm_lock failed");
    }
    chan->fsm = SDM_FSM_INIT;
    return ESP_OK;
}

esp_err_t sdm_channel_set_pulse_density(sdm_channel_handle_t chan, int8_t density)
{
    ESP_RETURN_ON_FALSE_ISR(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    sdm_group_t *group = chan->group;
    int chan_id = chan->chan_id;

    portENTER_CRITICAL_SAFE(&chan->spinlock);
    sdm_ll_set_pulse_density(group->hal.dev, chan_id, density);
    portEXIT_CRITICAL_SAFE(&chan->spinlock);

    return ESP_OK;
}

esp_err_t sdm_channel_set_duty(sdm_channel_handle_t chan, int8_t duty)
__attribute__((alias("sdm_channel_set_pulse_density")));
