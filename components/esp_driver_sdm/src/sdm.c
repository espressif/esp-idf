/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdatomic.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#if CONFIG_SDM_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
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
#include "hal/sdm_caps.h"
#include "hal/sdm_periph.h"
#include "hal/sdm_hal.h"
#include "hal/sdm_ll.h"
#include "hal/hal_utils.h"
#include "esp_private/esp_clk.h"
#include "esp_private/io_mux.h"
#include "esp_private/gpio.h"
#include "esp_private/sleep_retention.h"
#include "esp_private/esp_gpio_reserve.h"

#if CONFIG_SDM_OBJ_CACHE_SAFE
#define SDM_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define SDM_MEM_ALLOC_CAPS      MALLOC_CAP_DEFAULT
#endif

#define SDM_USE_RETENTION_LINK  (SOC_SDM_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP)

///!< Logging settings
#define TAG "sdm"

typedef struct sdm_platform_t sdm_platform_t;
typedef struct sdm_group_t sdm_group_t;
typedef struct sdm_channel_t sdm_channel_t;

struct sdm_platform_t {
    _lock_t mutex;                               // platform level mutex lock
    sdm_group_t *groups[SDM_CAPS_GET(INST_NUM)]; // sdm group pool
    int group_ref_counts[SDM_CAPS_GET(INST_NUM)];// reference count used to protect group install/uninstall
};

struct sdm_group_t {
    int group_id;          // Group ID, index from 0
    portMUX_TYPE spinlock; // to protect per-group register level concurrent access
    sdm_hal_context_t hal; // hal context
    sdm_channel_t *channels[SDM_CAPS_GET(CHANS_PER_INST)]; // array of sdm channels
    sdm_clock_source_t clk_src; // Clock source
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock; // PM lock, to prevent the system going into light sleep when SDM is running
#endif
};

typedef enum {
    SDM_FSM_INIT,
    SDM_FSM_ENABLE,
    SDM_FSM_WAIT,
} sdm_fsm_t;

struct sdm_channel_t {
    sdm_group_t *group;      // which group the sdm channel belongs to
    uint32_t chan_id;        // allocated channel numerical ID
    gpio_num_t gpio_num;     // GPIO number
    uint32_t sample_rate_hz; // Sample rate, in Hz
    portMUX_TYPE spinlock;   // to protect per-channels resources concurrently accessed by tasks
    _Atomic sdm_fsm_t fsm;   // state machine, to control the API is called in the correct order
};

// sdm driver platform, it's always a singleton
static sdm_platform_t s_platform;

#if SDM_USE_RETENTION_LINK
static esp_err_t sdm_create_sleep_retention_link_cb(void *group)
{
    int group_id = ((sdm_group_t *)group)->group_id;
    esp_err_t err = sleep_retention_entries_create(soc_sdm_retention_infos[group_id].regdma_entry_array,
                                                   soc_sdm_retention_infos[group_id].array_size,
                                                   REGDMA_LINK_PRI_SDM, soc_sdm_retention_infos[group_id].module);
    return err;
}

static void sdm_create_retention_module(sdm_group_t *group)
{
    int group_id = group->group_id;
    sleep_retention_module_t module = soc_sdm_retention_infos[group_id].module;
    _lock_acquire(&s_platform.mutex);
    if (sleep_retention_is_module_inited(module) && !sleep_retention_is_module_created(module)) {
        if (sleep_retention_module_allocate(module) != ESP_OK) {
            // even though the sleep retention module create failed, SDM driver should still work, so just warning here
            ESP_LOGW(TAG, "create retention link failed on SDM Group%d, power domain won't be turned off during sleep", group_id);
        }
    }
    _lock_release(&s_platform.mutex);
}
#endif // SDM_USE_RETENTION_LINK

static sdm_group_t *sdm_acquire_group_handle(int group_id, sdm_clock_source_t clk_src)
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
            group->clk_src = clk_src;

#if SDM_USE_RETENTION_LINK
            sleep_retention_module_t module = soc_sdm_retention_infos[group_id].module;
            sleep_retention_module_init_param_t init_param = {
                .cbs = {
                    .create = {
                        .handle = sdm_create_sleep_retention_link_cb,
                        .arg = group,
                    },
                },
                .depends = RETENTION_MODULE_BITMAP_INIT(CLOCK_SYSTEM)
            };
            // retention module init must be called BEFORE the hal init
            if (sleep_retention_module_init(module, &init_param) != ESP_OK) {
                ESP_LOGW(TAG, "init sleep retention failed on SDM Group%d, power domain may be turned off during sleep", group_id);
            }
#endif // SDM_USE_RETENTION_LINK

            // [IDF-12975]: enable APB register clock explicitly
            // initialize HAL context
            sdm_hal_init_config_t hal_config = {
                .group_id = group_id,
            };
            sdm_hal_init(&group->hal, &hal_config);
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
#if CONFIG_PM_ENABLE
        esp_pm_lock_type_t pm_type = ESP_PM_NO_LIGHT_SLEEP;
#if SDM_CAPS_GET(FUNC_CLOCK_SUPPORT_APB)
        if (clk_src == SDM_CLK_SRC_APB) {
            pm_type = ESP_PM_APB_FREQ_MAX;
        }
#endif // SDM_CAPS_GET(FUNC_CLOCK_SUPPORT_APB)
        if (esp_pm_lock_create(pm_type, 0, soc_sdm_signals[group_id].module_name, &group->pm_lock) != ESP_OK) {
            ESP_LOGE(TAG, "fail to create PM lock for group %d", group_id);
        }
#endif // CONFIG_PM_ENABLE
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
        sdm_hal_deinit(&group->hal);

#if SDM_USE_RETENTION_LINK
        sleep_retention_module_t module = soc_sdm_retention_infos[group_id].module;
        if (sleep_retention_is_module_created(module)) {
            sleep_retention_module_free(module);
        }
        if (sleep_retention_is_module_inited(module)) {
            sleep_retention_module_deinit(module);
        }
#endif // SDM_USE_RETENTION_LINK
    }
    _lock_release(&s_platform.mutex);

    if (do_deinitialize) {
#if CONFIG_PM_ENABLE
        if (group->pm_lock) {
            esp_pm_lock_delete(group->pm_lock);
        }
#endif
        free(group);
        ESP_LOGD(TAG, "del group (%d)", group_id);
    }
}

static esp_err_t sdm_register_to_group(sdm_channel_t *chan, sdm_clock_source_t clk_src)
{
    sdm_group_t *group = NULL;
    int chan_id = -1;
    for (int i = 0; i < SDM_CAPS_GET(INST_NUM); i++) {
        group = sdm_acquire_group_handle(i, clk_src);
        ESP_RETURN_ON_FALSE(group, ESP_ERR_NO_MEM, TAG, "no mem for group (%d)", i);
        // loop to search free unit in the group
        portENTER_CRITICAL(&group->spinlock);
        for (int j = 0; j < SDM_CAPS_GET(CHANS_PER_INST); j++) {
            if (!group->channels[j]) {
                chan_id = j;
                group->channels[j] = chan;
                chan->group = group;
                chan->chan_id = chan_id;
                break;
            }
        }
        portEXIT_CRITICAL(&group->spinlock);
        if (chan_id < 0) {
            sdm_release_group_handle(group);
        } else {
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
    if (chan->group) {
        sdm_unregister_from_group(chan);
    }
    if (chan->gpio_num >= 0) {
        gpio_output_disable(chan->gpio_num);
        esp_gpio_revoke(BIT64(chan->gpio_num));
    }
    free(chan);
    return ESP_OK;
}

esp_err_t sdm_new_channel(const sdm_config_t *config, sdm_channel_handle_t *ret_chan)
{
    esp_err_t ret = ESP_OK;
    sdm_channel_t *chan = NULL;
    ESP_RETURN_ON_FALSE(config && ret_chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_OUTPUT_GPIO(config->gpio_num), ESP_ERR_INVALID_ARG, TAG, "invalid GPIO number");

    [[maybe_unused]] bool allow_pd = config->flags.allow_pd == 1;
#if !SOC_SDM_SUPPORT_SLEEP_RETENTION
    ESP_RETURN_ON_FALSE(allow_pd == false, ESP_ERR_NOT_SUPPORTED, TAG, "not able to power down in light sleep");
#endif // SOC_SDM_SUPPORT_SLEEP_RETENTION

    // allocate channel memory from internal memory because it contains atomic variable
    chan = heap_caps_calloc(1, sizeof(sdm_channel_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_NO_MEM, TAG, "no mem for channel");
    chan->gpio_num = GPIO_NUM_NC; // default to NC, will be set later

    sdm_clock_source_t clk_src = config->clk_src ? config->clk_src : SDM_CLK_SRC_DEFAULT;
    // register channel to the group
    ESP_GOTO_ON_ERROR(sdm_register_to_group(chan, clk_src), err, TAG, "register to group failed");
    sdm_group_t *group = chan->group;
    int group_id = group->group_id;
    int chan_id = chan->chan_id;

    ESP_GOTO_ON_FALSE(group->clk_src == clk_src, ESP_ERR_INVALID_ARG, err, TAG, "clock source conflict");

    // SDM clock comes from IO MUX, but IO MUX clock might be shared with other submodules as well
    ESP_GOTO_ON_ERROR(io_mux_set_clock_source((soc_module_clk_t)clk_src), err, TAG, "set IO MUX clock source failed");

    uint32_t src_clk_hz = 0;
    ESP_GOTO_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)clk_src,
                                                   ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &src_clk_hz), err, TAG, "get source clock frequency failed");

    // Reserve the new GPIO
    uint64_t old_gpio_rsv_mask = esp_gpio_reserve(BIT64(config->gpio_num));
    if (old_gpio_rsv_mask & BIT64(config->gpio_num)) {
        ESP_LOGW(TAG, "GPIO %d is not usable, maybe conflict with others", config->gpio_num);
    }
    // connect the signal to the GPIO by matrix
    gpio_matrix_output(config->gpio_num, soc_sdm_signals[group_id].channels[chan_id].sig_id_matrix, config->flags.invert_out, false);
    chan->gpio_num = config->gpio_num;

    // set prescale based on sample rate
    uint32_t prescale = 0;
    hal_utils_clk_info_t clk_info = {
        .src_freq_hz = src_clk_hz,
        .exp_freq_hz = config->sample_rate_hz,
        .max_integ = SDM_LL_PRESCALE_MAX + 1,
        .min_integ = 1,
        .round_opt = HAL_DIV_ROUND,
    };
    uint32_t actual_freq = hal_utils_calc_clk_div_integer(&clk_info, &prescale);
    ESP_GOTO_ON_FALSE(actual_freq, ESP_ERR_INVALID_ARG, err, TAG,
                      "sample rate out of range [%"PRIu32", %"PRIu32"] Hz", src_clk_hz / SDM_LL_PRESCALE_MAX, src_clk_hz);
    if (actual_freq != config->sample_rate_hz) {
        ESP_LOGW(TAG, "precision loss, expected sample rate %"PRIu32" Hz runs at %"PRIu32" Hz", config->sample_rate_hz, actual_freq);
    }
    sdm_ll_set_prescale(group->hal.dev, chan_id, prescale);
    chan->sample_rate_hz = src_clk_hz / prescale;
    // preset the duty cycle to zero
    sdm_ll_set_pulse_density(group->hal.dev, chan_id, 0);

    // initialize other members
    chan->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    atomic_init(&chan->fsm, SDM_FSM_INIT); // set the initial state to INIT

#if SDM_USE_RETENTION_LINK
    if (allow_pd) {
        sdm_create_retention_module(group);
    }
#endif // SDM_USE_RETENTION_LINK

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
    sdm_fsm_t expected_fsm = SDM_FSM_INIT;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&chan->fsm, &expected_fsm, SDM_FSM_WAIT),
                        ESP_ERR_INVALID_STATE, TAG, "channel not in init state");
    ESP_LOGD(TAG, "del channel (%d,%d)", chan->group->group_id, chan->chan_id);
    // recycle memory resource
    ESP_RETURN_ON_ERROR(sdm_destroy(chan), TAG, "destroy channel failed");
    return ESP_OK;
}

esp_err_t sdm_channel_enable(sdm_channel_handle_t chan)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    sdm_fsm_t expected_fsm = SDM_FSM_INIT;
    if (atomic_compare_exchange_strong(&chan->fsm, &expected_fsm, SDM_FSM_WAIT)) {
#if CONFIG_PM_ENABLE
        // acquire power manager lock
        if (chan->group->pm_lock) {
            esp_pm_lock_acquire(chan->group->pm_lock);
        }
#endif
        // enable the channel
        atomic_store(&chan->fsm, SDM_FSM_ENABLE); // change state to ENABLE
        ESP_LOGD(TAG, "channel (%d,%d) enabled", chan->group->group_id, chan->chan_id);
    } else {
        ESP_RETURN_ON_ERROR(ESP_ERR_INVALID_STATE, TAG, "channel not in init state");
    }

    return ESP_OK;
}

esp_err_t sdm_channel_disable(sdm_channel_handle_t chan)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    sdm_fsm_t expected_fsm = SDM_FSM_ENABLE;
    if (atomic_compare_exchange_strong(&chan->fsm, &expected_fsm, SDM_FSM_WAIT)) {
#if CONFIG_PM_ENABLE
        // release power manager lock
        if (chan->group->pm_lock) {
            esp_pm_lock_release(chan->group->pm_lock);
        }
#endif
        atomic_store(&chan->fsm, SDM_FSM_INIT); // change state to INIT
        ESP_LOGD(TAG, "channel (%d,%d) disabled", chan->group->group_id, chan->chan_id);
    } else {
        ESP_RETURN_ON_ERROR(ESP_ERR_INVALID_STATE, TAG, "channel not in enable state");
    }

    return ESP_OK;
}

esp_err_t sdm_channel_set_pulse_density(sdm_channel_handle_t chan, int8_t density)
{
    if (!chan) {
        return ESP_ERR_INVALID_ARG;
    }
    bool valid_state = false;
    sdm_fsm_t expected_fsm = SDM_FSM_INIT;
    sdm_fsm_t restore_fsm = SDM_FSM_INIT;
    // this function can be called only when the channel is in init or enable state
    if (atomic_compare_exchange_strong(&chan->fsm, &expected_fsm, SDM_FSM_WAIT)) {
        valid_state = true;
        restore_fsm = SDM_FSM_INIT;
    } else {
        expected_fsm = SDM_FSM_ENABLE;
        if (atomic_compare_exchange_strong(&chan->fsm, &expected_fsm, SDM_FSM_WAIT)) {
            valid_state = true;
            restore_fsm = SDM_FSM_ENABLE;
        }
    }
    if (!valid_state) {
        return ESP_ERR_INVALID_STATE;
    }

    sdm_group_t *group = chan->group;
    int chan_id = chan->chan_id;

    portENTER_CRITICAL_SAFE(&chan->spinlock);
    sdm_ll_set_pulse_density(group->hal.dev, chan_id, density);
    portEXIT_CRITICAL_SAFE(&chan->spinlock);

    // restore the state
    atomic_store(&chan->fsm, restore_fsm);

    return ESP_OK;
}

#if CONFIG_SDM_ENABLE_DEBUG_LOG
__attribute__((constructor))
static void sdm_override_default_log_level(void)
{
    esp_log_level_set(TAG, ESP_LOG_VERBOSE);
}
#endif
