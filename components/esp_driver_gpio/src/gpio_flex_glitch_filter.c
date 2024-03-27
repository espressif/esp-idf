/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/cdefs.h>
#include "freertos/FreeRTOS.h"
#include "esp_check.h"
#include "esp_private/glitch_filter_priv.h"
#include "esp_private/io_mux.h"
#include "soc/soc_caps.h"
#include "hal/gpio_glitch_filter_ll.h"
#include "esp_pm.h"
#include "esp_clk_tree.h"

static const char *TAG = "gpio-filter";

typedef struct gpio_flex_glitch_filter_t gpio_flex_glitch_filter_t;

typedef struct gpio_flex_glitch_filter_group_t {
    gpio_glitch_filter_dev_t *hw;
    gpio_flex_glitch_filter_t *filters[SOC_GPIO_FLEX_GLITCH_FILTER_NUM];
    portMUX_TYPE spinlock;
} gpio_flex_glitch_filter_group_t;

struct gpio_flex_glitch_filter_t {
    gpio_glitch_filter_t base;
    gpio_flex_glitch_filter_group_t *group;
    uint32_t filter_id;
    esp_pm_lock_handle_t pm_lock;
#if CONFIG_PM_ENABLE
    char pm_lock_name[GLITCH_FILTER_PM_LOCK_NAME_LEN_MAX]; // pm lock name
#endif
};

static gpio_flex_glitch_filter_group_t s_gpio_glitch_filter_group = {
    .hw = &GLITCH_FILTER,
    .spinlock = portMUX_INITIALIZER_UNLOCKED,
};

static esp_err_t gpio_filter_register_to_group(gpio_flex_glitch_filter_t *filter)
{
    gpio_flex_glitch_filter_group_t *group = &s_gpio_glitch_filter_group;
    int filter_id = -1;
    // loop to search free one in the group
    portENTER_CRITICAL(&group->spinlock);
    for (int j = 0; j < SOC_GPIO_FLEX_GLITCH_FILTER_NUM; j++) {
        if (!group->filters[j]) {
            filter_id = j;
            group->filters[j] = filter;
            break;
        }
    }
    portEXIT_CRITICAL(&group->spinlock);

    ESP_RETURN_ON_FALSE(filter_id != -1, ESP_ERR_NOT_FOUND, TAG, "no free gpio glitch filter");
    filter->filter_id = filter_id;
    filter->group = group;
    return ESP_OK;
}

static esp_err_t gpio_filter_destroy(gpio_flex_glitch_filter_t *filter)
{
    gpio_flex_glitch_filter_group_t *group = &s_gpio_glitch_filter_group;
    int filter_id = filter->filter_id;

    // unregister the filter from the group
    if (filter->group) {
        portENTER_CRITICAL(&group->spinlock);
        group->filters[filter_id] = NULL;
        portEXIT_CRITICAL(&group->spinlock);
    }

    if (filter->pm_lock) {
        esp_pm_lock_delete(filter->pm_lock);
    }

    free(filter);
    return ESP_OK;
}

static esp_err_t gpio_flex_glitch_filter_del(gpio_glitch_filter_t *filter)
{
    ESP_RETURN_ON_FALSE(filter->fsm == GLITCH_FILTER_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "filter not in init state");
    gpio_flex_glitch_filter_t *flex_filter = __containerof(filter, gpio_flex_glitch_filter_t, base);
    return gpio_filter_destroy(flex_filter);
}

static esp_err_t gpio_flex_glitch_filter_enable(gpio_glitch_filter_t *filter)
{
    ESP_RETURN_ON_FALSE(filter->fsm == GLITCH_FILTER_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "filter not in init state");
    gpio_flex_glitch_filter_t *flex_filter = __containerof(filter, gpio_flex_glitch_filter_t, base);

    // acquire pm lock
    if (flex_filter->pm_lock) {
        esp_pm_lock_acquire(flex_filter->pm_lock);
    }

    int filter_id = flex_filter->filter_id;
    gpio_ll_glitch_filter_enable(s_gpio_glitch_filter_group.hw, filter_id, true);
    filter->fsm = GLITCH_FILTER_FSM_ENABLE;
    return ESP_OK;
}

static esp_err_t gpio_flex_glitch_filter_disable(gpio_glitch_filter_t *filter)
{
    ESP_RETURN_ON_FALSE(filter->fsm == GLITCH_FILTER_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "filter not in enable state");
    gpio_flex_glitch_filter_t *flex_filter = __containerof(filter, gpio_flex_glitch_filter_t, base);

    int filter_id = flex_filter->filter_id;
    gpio_ll_glitch_filter_enable(s_gpio_glitch_filter_group.hw, filter_id, false);

    // release pm lock
    if (flex_filter->pm_lock) {
        esp_pm_lock_release(flex_filter->pm_lock);
    }

    filter->fsm = GLITCH_FILTER_FSM_INIT;
    return ESP_OK;
}

esp_err_t gpio_new_flex_glitch_filter(const gpio_flex_glitch_filter_config_t *config, gpio_glitch_filter_handle_t *ret_filter)
{
    esp_err_t ret = ESP_OK;
    gpio_flex_glitch_filter_t *filter = NULL;
    ESP_GOTO_ON_FALSE(config && ret_filter, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(GPIO_IS_VALID_GPIO(config->gpio_num), ESP_ERR_INVALID_ARG, err, TAG, "invalid gpio number");

    // allocate driver object
    filter = heap_caps_calloc(1, sizeof(gpio_flex_glitch_filter_t), FILTER_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(filter, ESP_ERR_NO_MEM, err, TAG, "no memory for flex glitch filter");

    // register the filter to the group
    ESP_GOTO_ON_ERROR(gpio_filter_register_to_group(filter), err, TAG, "register filter to group failed");
    int filter_id = filter->filter_id;

    // set clock source
    uint32_t clk_freq_hz = 0;
    ESP_GOTO_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)config->clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_freq_hz),
                      err, TAG, "get clock source frequency failed");

    // create pm_lock according to different clock source
#if CONFIG_PM_ENABLE
    esp_pm_lock_type_t lock_type = ESP_PM_NO_LIGHT_SLEEP;
    sprintf(filter->pm_lock_name, "filter_%d", filter_id); // e.g. filter_0
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(lock_type, 0, filter->pm_lock_name, &filter->pm_lock),
                      err, TAG, "create pm_lock failed");
#endif

    uint32_t window_thres_ticks = clk_freq_hz / 1000000 * config->window_thres_ns / 1000;
    uint32_t window_width_ticks = clk_freq_hz / 1000000 * config->window_width_ns / 1000;
    ESP_GOTO_ON_FALSE(window_thres_ticks && window_thres_ticks <= window_width_ticks && window_width_ticks <= GPIO_LL_GLITCH_FILTER_MAX_WINDOW,
                      ESP_ERR_INVALID_ARG, err, TAG, "invalid or out of range window width/threshold");

    // Glitch filter's clock source is same to the IOMUX clock
    ESP_GOTO_ON_ERROR(io_mux_set_clock_source((soc_module_clk_t)(config->clk_src)), err, TAG, "set IO MUX clock source failed");

    // make sure the filter is disabled
    gpio_ll_glitch_filter_enable(s_gpio_glitch_filter_group.hw, filter_id, false);
    // apply the filter to the GPIO
    gpio_ll_glitch_filter_set_gpio(s_gpio_glitch_filter_group.hw, filter_id, config->gpio_num);
    // set filter coefficient
    gpio_ll_glitch_filter_set_window_coeff(s_gpio_glitch_filter_group.hw, filter_id, window_width_ticks, window_thres_ticks);

    filter->base.gpio_num = config->gpio_num;
    filter->base.fsm = GLITCH_FILTER_FSM_INIT;
    filter->base.del = gpio_flex_glitch_filter_del;
    filter->base.enable = gpio_flex_glitch_filter_enable;
    filter->base.disable = gpio_flex_glitch_filter_disable;

    *ret_filter = &(filter->base);
    return ESP_OK;
err:
    if (filter) {
        gpio_filter_destroy(filter);
    }
    return ret;
}
