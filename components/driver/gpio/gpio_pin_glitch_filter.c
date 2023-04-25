/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/cdefs.h>
#include "freertos/FreeRTOS.h"
#include "esp_check.h"
#include "esp_pm.h"
#include "glitch_filter_priv.h"
#include "hal/gpio_ll.h"
#include "esp_clk_tree.h"
#include "esp_private/io_mux.h"

static const char *TAG = "gpio-filter";

/**
 * @brief Type of GPIO pin glitch filter
 */
typedef struct gpio_pin_glitch_filter_t {
    gpio_glitch_filter_t base;
    esp_pm_lock_handle_t pm_lock;
#if CONFIG_PM_ENABLE
    char pm_lock_name[GLITCH_FILTER_PM_LOCK_NAME_LEN_MAX]; // pm lock name
#endif
} gpio_pin_glitch_filter_t;

static esp_err_t gpio_filter_destroy(gpio_pin_glitch_filter_t *filter)
{
    if (filter->pm_lock) {
        esp_pm_lock_delete(filter->pm_lock);
    }

    free(filter);
    return ESP_OK;
}

static esp_err_t gpio_pin_glitch_filter_del(gpio_glitch_filter_t *filter)
{
    ESP_RETURN_ON_FALSE(filter->fsm == GLITCH_FILTER_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "filter not in init state");
    gpio_pin_glitch_filter_t *pin_filter = __containerof(filter, gpio_pin_glitch_filter_t, base);
    return gpio_filter_destroy(pin_filter);
}

static esp_err_t gpio_pin_glitch_filter_enable(gpio_glitch_filter_t *filter)
{
    ESP_RETURN_ON_FALSE(filter->fsm == GLITCH_FILTER_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "filter not in init state");
    gpio_pin_glitch_filter_t *pin_filter = __containerof(filter, gpio_pin_glitch_filter_t, base);

    // acquire pm lock
    if (pin_filter->pm_lock) {
        esp_pm_lock_acquire(pin_filter->pm_lock);
    }

    gpio_ll_pin_filter_enable(NULL, filter->gpio_num);
    filter->fsm = GLITCH_FILTER_FSM_ENABLE;
    return ESP_OK;
}

static esp_err_t gpio_pin_glitch_filter_disable(gpio_glitch_filter_t *filter)
{
    ESP_RETURN_ON_FALSE(filter->fsm == GLITCH_FILTER_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "filter not in enable state");
    gpio_pin_glitch_filter_t *pin_filter = __containerof(filter, gpio_pin_glitch_filter_t, base);

    gpio_ll_pin_filter_disable(NULL, filter->gpio_num);

    // release pm lock
    if (pin_filter->pm_lock) {
        esp_pm_lock_release(pin_filter->pm_lock);
    }

    filter->fsm = GLITCH_FILTER_FSM_INIT;
    return ESP_OK;
}

esp_err_t gpio_new_pin_glitch_filter(const gpio_pin_glitch_filter_config_t *config, gpio_glitch_filter_handle_t *ret_filter)
{
    esp_err_t ret = ESP_OK;
    gpio_pin_glitch_filter_t *filter = NULL;
    ESP_GOTO_ON_FALSE(config && ret_filter, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(GPIO_IS_VALID_GPIO(config->gpio_num), ESP_ERR_INVALID_ARG, err, TAG, "invalid gpio number");

    filter = heap_caps_calloc(1, sizeof(gpio_pin_glitch_filter_t), FILTER_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(filter, ESP_ERR_NO_MEM, err, TAG, "no memory for pin glitch filter");

    // create pm lock according to different clock source
#if CONFIG_PM_ENABLE
    esp_pm_lock_type_t lock_type = ESP_PM_NO_LIGHT_SLEEP;
#if SOC_GPIO_FILTER_CLK_SUPPORT_APB
    if (config->clk_src == GLITCH_FILTER_CLK_SRC_APB) {
        lock_type = ESP_PM_APB_FREQ_MAX;
    }
#endif // SOC_GPIO_FILTER_CLK_SUPPORT_APB
    sprintf(filter->pm_lock_name, "filter_io_%d", config->gpio_num); // e.g. filter_io_0
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(lock_type, 0, filter->pm_lock_name, &filter->pm_lock),
                      err, TAG, "create pm_lock failed");
#endif // CONFIG_PM_ENABLE

    // Glitch filter's clock source is same to the IOMUX clock
    ESP_GOTO_ON_ERROR(io_mux_set_clock_source((soc_module_clk_t)(config->clk_src)), err, TAG, "set IO MUX clock source failed");

    filter->base.gpio_num = config->gpio_num;
    filter->base.fsm = GLITCH_FILTER_FSM_INIT;
    filter->base.del = gpio_pin_glitch_filter_del;
    filter->base.enable = gpio_pin_glitch_filter_enable;
    filter->base.disable = gpio_pin_glitch_filter_disable;

    *ret_filter = &(filter->base);
    return ESP_OK;
err:
    if (filter) {
        gpio_filter_destroy(filter);
    }
    return ret;
}
