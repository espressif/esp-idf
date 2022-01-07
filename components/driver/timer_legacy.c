/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "driver/timer_types_legacy.h"
#include "hal/timer_hal.h"
#include "hal/timer_ll.h"
#include "hal/check.h"
#include "soc/timer_periph.h"
#include "soc/rtc.h"
#include "soc/timer_group_reg.h"
#include "esp_private/periph_ctrl.h"

static const char *TIMER_TAG = "timer_group";

#define TIMER_GROUP_NUM_ERROR   "TIMER GROUP NUM ERROR"
#define TIMER_NUM_ERROR         "HW TIMER NUM ERROR"
#define TIMER_PARAM_ADDR_ERROR  "HW TIMER PARAM ADDR ERROR"
#define TIMER_NEVER_INIT_ERROR  "HW TIMER NEVER INIT ERROR"
#define TIMER_COUNT_DIR_ERROR   "HW TIMER COUNTER DIR ERROR"
#define TIMER_AUTORELOAD_ERROR  "HW TIMER AUTORELOAD ERROR"
#define TIMER_SCALE_ERROR       "HW TIMER SCALE ERROR"
#define TIMER_ALARM_ERROR       "HW TIMER ALARM ERROR"
#define DIVIDER_RANGE_ERROR     "HW TIMER divider outside of [2, 65536] range error"

#define TIMER_ENTER_CRITICAL(mux)      portENTER_CRITICAL_SAFE(mux);
#define TIMER_EXIT_CRITICAL(mux)       portEXIT_CRITICAL_SAFE(mux);

typedef struct {
    timer_isr_t fn;  /*!< isr function */
    void *args;      /*!< isr function args */
    timer_isr_handle_t timer_isr_handle;  /*!< interrupt handle */
    timer_group_t isr_timer_group;        /*!< timer group of interrupt triggered */
} timer_isr_func_t;

typedef struct {
    timer_hal_context_t hal;
    timer_isr_func_t timer_isr_fun;
    gptimer_clock_source_t clk_src;
    gptimer_count_direction_t direction;
    uint32_t divider;
    uint64_t alarm_value;
    bool alarm_en;
    bool auto_reload_en;
    bool counter_en;
} timer_obj_t;

static timer_obj_t *p_timer_obj[TIMER_GROUP_MAX][TIMER_MAX] = {0};
static portMUX_TYPE timer_spinlock[TIMER_GROUP_MAX] = { [0 ... TIMER_GROUP_MAX - 1] = portMUX_INITIALIZER_UNLOCKED, };

esp_err_t timer_get_counter_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t *timer_val)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_val != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_PARAM_ADDR_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    *timer_val = timer_ll_get_counter_value(p_timer_obj[group_num][timer_num]->hal.dev, timer_num);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_get_counter_time_sec(timer_group_t group_num, timer_idx_t timer_num, double *time)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(time != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_PARAM_ADDR_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    uint64_t timer_val = timer_ll_get_counter_value(p_timer_obj[group_num][timer_num]->hal.dev, timer_num);
    uint32_t div = p_timer_obj[group_num][timer_num]->divider;
    switch (p_timer_obj[group_num][timer_num]->clk_src) {
    case GPTIMER_CLK_SRC_APB:
        *time = (double)timer_val * div / rtc_clk_apb_freq_get();
        break;
#if SOC_TIMER_GROUP_SUPPORT_XTAL
    case GPTIMER_CLK_SRC_XTAL:
        *time = (double)timer_val * div / ((int)rtc_clk_xtal_freq_get() * MHZ);
        break;
#endif
    default:
        ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_ARG, TIMER_TAG, "invalid clock source");
        break;
    }
    return ESP_OK;
}

esp_err_t timer_set_counter_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t load_val)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_set_counter_value(&(p_timer_obj[group_num][timer_num]->hal), load_val);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_start(timer_group_t group_num, timer_idx_t timer_num)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_ll_enable_counter(p_timer_obj[group_num][timer_num]->hal.dev, timer_num, true);
    p_timer_obj[group_num][timer_num]->counter_en = true;
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_pause(timer_group_t group_num, timer_idx_t timer_num)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_ll_enable_counter(p_timer_obj[group_num][timer_num]->hal.dev, timer_num, false);
    p_timer_obj[group_num][timer_num]->counter_en = false;
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_set_counter_mode(timer_group_t group_num, timer_idx_t timer_num, timer_count_dir_t counter_dir)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(counter_dir < TIMER_COUNT_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_COUNT_DIR_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_ll_set_count_direction(p_timer_obj[group_num][timer_num]->hal.dev, timer_num, counter_dir);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_set_auto_reload(timer_group_t group_num, timer_idx_t timer_num, timer_autoreload_t reload)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(reload < TIMER_AUTORELOAD_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_AUTORELOAD_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_ll_enable_auto_reload(p_timer_obj[group_num][timer_num]->hal.dev, timer_num, reload);
    p_timer_obj[group_num][timer_num]->auto_reload_en = reload;
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_set_divider(timer_group_t group_num, timer_idx_t timer_num, uint32_t divider)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(divider > 1 && divider < 65537, ESP_ERR_INVALID_ARG, TIMER_TAG,  DIVIDER_RANGE_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_ll_set_clock_prescale(p_timer_obj[group_num][timer_num]->hal.dev, timer_num, divider);
    p_timer_obj[group_num][timer_num]->divider = divider;
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_set_alarm_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t alarm_value)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_ll_set_alarm_value(p_timer_obj[group_num][timer_num]->hal.dev, timer_num, alarm_value);
    p_timer_obj[group_num][timer_num]->alarm_value = alarm_value;
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_get_alarm_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t *alarm_value)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(alarm_value != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_PARAM_ADDR_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    *alarm_value = p_timer_obj[group_num][timer_num]->alarm_value;
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_set_alarm(timer_group_t group_num, timer_idx_t timer_num, timer_alarm_t alarm_en)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(alarm_en < TIMER_ALARM_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_ALARM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_ll_enable_alarm(p_timer_obj[group_num][timer_num]->hal.dev, timer_num, alarm_en);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

static void IRAM_ATTR timer_isr_default(void *arg)
{
    bool is_awoken = false;
    timer_obj_t *timer_obj = (timer_obj_t *)arg;
    if (timer_obj == NULL || timer_obj->timer_isr_fun.fn == NULL) {
        return;
    }
    uint32_t timer_id = timer_obj->hal.timer_id;
    timer_hal_context_t *hal = &timer_obj->hal;
    TIMER_ENTER_CRITICAL(&timer_spinlock[timer_obj->timer_isr_fun.isr_timer_group]);
    uint32_t intr_status = timer_ll_get_intr_status(hal->dev);
    uint64_t old_alarm_value = timer_obj->alarm_value;
    if (intr_status & TIMER_LL_EVENT_ALARM(timer_id)) {
        // Clear interrupt status
        timer_ll_clear_intr_status(hal->dev, TIMER_LL_EVENT_ALARM(timer_id));
        // call user registered callback
        is_awoken = timer_obj->timer_isr_fun.fn(timer_obj->timer_isr_fun.args);
        // reenable alarm if required
        uint64_t new_alarm_value = timer_obj->alarm_value;
        bool reenable_alarm = (new_alarm_value != old_alarm_value) || timer_obj->auto_reload_en;
        timer_ll_enable_alarm(hal->dev, timer_id, reenable_alarm);
    }
    TIMER_EXIT_CRITICAL(&timer_spinlock[timer_obj->timer_isr_fun.isr_timer_group]);

    if (is_awoken) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t timer_enable_intr(timer_group_t group_num, timer_idx_t timer_num)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_ll_enable_intr(p_timer_obj[group_num][timer_num]->hal.dev, TIMER_LL_EVENT_ALARM(timer_num), true);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_disable_intr(timer_group_t group_num, timer_idx_t timer_num)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_ll_enable_intr(p_timer_obj[group_num][timer_num]->hal.dev, TIMER_LL_EVENT_ALARM(timer_num), false);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_isr_register(timer_group_t group_num, timer_idx_t timer_num,
                             void (*fn)(void *), void *arg, int intr_alloc_flags, timer_isr_handle_t *handle)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(fn != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_PARAM_ADDR_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    timer_hal_context_t *hal = &p_timer_obj[group_num][timer_num]->hal;
    return esp_intr_alloc_intrstatus(timer_group_periph_signals.groups[group_num].timer_irq_id[timer_num],
                                     intr_alloc_flags,
                                     (uint32_t)timer_ll_get_intr_status_reg(hal->dev),
                                     TIMER_LL_EVENT_ALARM(timer_num), fn, arg, handle);
}

esp_err_t timer_isr_callback_add(timer_group_t group_num, timer_idx_t timer_num, timer_isr_t isr_handler, void *args, int intr_alloc_flags)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);

    timer_disable_intr(group_num, timer_num);
    p_timer_obj[group_num][timer_num]->timer_isr_fun.fn = isr_handler;
    p_timer_obj[group_num][timer_num]->timer_isr_fun.args = args;
    p_timer_obj[group_num][timer_num]->timer_isr_fun.isr_timer_group = group_num;
    timer_isr_register(group_num, timer_num, timer_isr_default, (void *)p_timer_obj[group_num][timer_num],
                       intr_alloc_flags, &(p_timer_obj[group_num][timer_num]->timer_isr_fun.timer_isr_handle));
    timer_enable_intr(group_num, timer_num);

    return ESP_OK;
}

esp_err_t timer_isr_callback_remove(timer_group_t group_num, timer_idx_t timer_num)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);

    timer_disable_intr(group_num, timer_num);
    p_timer_obj[group_num][timer_num]->timer_isr_fun.fn = NULL;
    p_timer_obj[group_num][timer_num]->timer_isr_fun.args = NULL;
    esp_intr_free(p_timer_obj[group_num][timer_num]->timer_isr_fun.timer_isr_handle);

    return ESP_OK;
}

esp_err_t timer_init(timer_group_t group_num, timer_idx_t timer_num, const timer_config_t *config)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(config != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_PARAM_ADDR_ERROR);
    ESP_RETURN_ON_FALSE(config->divider > 1 && config->divider < 65537, ESP_ERR_INVALID_ARG, TIMER_TAG,  DIVIDER_RANGE_ERROR);
    ESP_RETURN_ON_FALSE(config->intr_type < TIMER_INTR_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG, "only support Level Interrupt");
    if (p_timer_obj[group_num][timer_num] == NULL) {
        p_timer_obj[group_num][timer_num] = (timer_obj_t *) heap_caps_calloc(1, sizeof(timer_obj_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num], ESP_ERR_NO_MEM, TIMER_TAG, "no mem for timer object");
    }
    timer_hal_context_t *hal = &p_timer_obj[group_num][timer_num]->hal;

    periph_module_enable(timer_group_periph_signals.groups[group_num].module);

    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_init(hal, group_num, timer_num);
    timer_hal_set_counter_value(hal, 0);
    timer_ll_set_clock_source(p_timer_obj[group_num][timer_num]->hal.dev, timer_num, config->clk_src);
    timer_ll_set_clock_prescale(hal->dev, timer_num, config->divider);
    timer_ll_set_count_direction(p_timer_obj[group_num][timer_num]->hal.dev, timer_num, config->counter_dir);
    timer_ll_enable_intr(hal->dev, TIMER_LL_EVENT_ALARM(timer_num), false);
    timer_ll_clear_intr_status(hal->dev, TIMER_LL_EVENT_ALARM(timer_num));
    timer_ll_enable_alarm(hal->dev, timer_num, config->alarm_en);
    timer_ll_enable_auto_reload(hal->dev, timer_num, config->auto_reload);
    timer_ll_enable_counter(hal->dev, timer_num, config->counter_en);
    p_timer_obj[group_num][timer_num]->clk_src = config->clk_src;
    p_timer_obj[group_num][timer_num]->alarm_en = config->alarm_en;
    p_timer_obj[group_num][timer_num]->auto_reload_en = config->auto_reload;
    p_timer_obj[group_num][timer_num]->direction = config->counter_dir;
    p_timer_obj[group_num][timer_num]->counter_en = config->counter_en;
    p_timer_obj[group_num][timer_num]->divider = config->divider;
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);

    return ESP_OK;
}

esp_err_t timer_deinit(timer_group_t group_num, timer_idx_t timer_num)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    timer_hal_context_t *hal = &p_timer_obj[group_num][timer_num]->hal;

    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_ll_enable_counter(hal->dev, timer_num, false);
    timer_ll_enable_intr(hal->dev, TIMER_LL_EVENT_ALARM(timer_num), false);
    timer_ll_clear_intr_status(hal->dev, TIMER_LL_EVENT_ALARM(timer_num));
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);

    free(p_timer_obj[group_num][timer_num]);
    p_timer_obj[group_num][timer_num] = NULL;

    return ESP_OK;
}

esp_err_t timer_get_config(timer_group_t group_num, timer_idx_t timer_num, timer_config_t *config)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(config != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_PARAM_ADDR_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);

    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    config->alarm_en = p_timer_obj[group_num][timer_num]->alarm_en;
    config->auto_reload = p_timer_obj[group_num][timer_num]->auto_reload_en;
    config->counter_dir = p_timer_obj[group_num][timer_num]->direction;
    config->counter_en = p_timer_obj[group_num][timer_num]->counter_en;
    config->divider = p_timer_obj[group_num][timer_num]->divider;
    config->intr_type = TIMER_INTR_LEVEL;
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_group_intr_enable(timer_group_t group_num, timer_intr_t en_mask)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_ll_enable_intr(p_timer_obj[group_num][0]->hal.dev, en_mask, true);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_group_intr_disable(timer_group_t group_num, timer_intr_t disable_mask)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_ll_enable_intr(p_timer_obj[group_num][0]->hal.dev, disable_mask, false);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

/* This function is deprecated */
timer_intr_t IRAM_ATTR timer_group_intr_get_in_isr(timer_group_t group_num)
{
    return timer_ll_get_intr_status(TIMER_LL_GET_HW(group_num));
}

uint32_t IRAM_ATTR timer_group_get_intr_status_in_isr(timer_group_t group_num)
{
    uint32_t intr_status = 0;
    if (p_timer_obj[group_num][TIMER_0] != NULL) {
        intr_status = timer_ll_get_intr_status(TIMER_LL_GET_HW(group_num)) & TIMER_LL_EVENT_ALARM(0);
    }
#if SOC_TIMER_GROUP_TIMERS_PER_GROUP > 1
    else if (p_timer_obj[group_num][TIMER_1] != NULL) {
        intr_status = timer_ll_get_intr_status(TIMER_LL_GET_HW(group_num)) & TIMER_LL_EVENT_ALARM(1);
    }
#endif
    return intr_status;
}

void IRAM_ATTR timer_group_clr_intr_status_in_isr(timer_group_t group_num, timer_idx_t timer_num)
{
    timer_ll_clear_intr_status(p_timer_obj[group_num][timer_num]->hal.dev, TIMER_LL_EVENT_ALARM(timer_num));
}

/* This function is deprecated */
void IRAM_ATTR timer_group_intr_clr_in_isr(timer_group_t group_num, timer_idx_t timer_num)
{
    timer_group_clr_intr_status_in_isr(group_num, timer_num);
}

void IRAM_ATTR timer_group_enable_alarm_in_isr(timer_group_t group_num, timer_idx_t timer_num)
{
    timer_ll_enable_alarm(p_timer_obj[group_num][timer_num]->hal.dev, timer_num, true);
}

uint64_t IRAM_ATTR timer_group_get_counter_value_in_isr(timer_group_t group_num, timer_idx_t timer_num)
{
    uint64_t val = timer_ll_get_counter_value(p_timer_obj[group_num][timer_num]->hal.dev, timer_num);
    return val;
}

void IRAM_ATTR timer_group_set_alarm_value_in_isr(timer_group_t group_num, timer_idx_t timer_num, uint64_t alarm_val)
{
    timer_ll_set_alarm_value(p_timer_obj[group_num][timer_num]->hal.dev, timer_num, alarm_val);
    p_timer_obj[group_num][timer_num]->alarm_value = alarm_val;
}

void IRAM_ATTR timer_group_set_counter_enable_in_isr(timer_group_t group_num, timer_idx_t timer_num, timer_start_t counter_en)
{
    timer_ll_enable_counter(p_timer_obj[group_num][timer_num]->hal.dev, timer_num, counter_en);
    p_timer_obj[group_num][timer_num]->counter_en = counter_en;
}

/* This function is deprecated */
void IRAM_ATTR timer_group_clr_intr_sta_in_isr(timer_group_t group_num, timer_intr_t intr_mask)
{
    for (uint32_t timer_idx = 0; timer_idx < TIMER_MAX; timer_idx++) {
        if (intr_mask & BIT(timer_idx)) {
            timer_group_clr_intr_status_in_isr(group_num, timer_idx);
        }
    }
}

bool IRAM_ATTR timer_group_get_auto_reload_in_isr(timer_group_t group_num, timer_idx_t timer_num)
{
    return p_timer_obj[group_num][timer_num]->auto_reload_en;
}

esp_err_t IRAM_ATTR timer_spinlock_take(timer_group_t group_num)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t IRAM_ATTR timer_spinlock_give(timer_group_t group_num)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

/**
 * @brief This function will be called during start up, to check that this legacy timer group driver is not running along with the gptimer driver
 */
__attribute__((constructor))
static void check_legacy_timer_driver_conflict(void)
{
    extern int timer_group_driver_init_count;
    timer_group_driver_init_count++;
    if (timer_group_driver_init_count > 1) {
        ESP_EARLY_LOGE(TIMER_TAG, "CONFLICT! The legacy timer group driver can't work along with the gptimer driver");
        abort();
    }
    ESP_EARLY_LOGW(TIMER_TAG, "legacy timer group driver is deprecated, please migrate to use driver/gptimer.h");
}
