/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "driver/timer.h"
#include "driver/periph_ctrl.h"
#include "hal/timer_hal.h"
#include "soc/timer_periph.h"
#include "soc/rtc.h"

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
} timer_obj_t;

static timer_obj_t *p_timer_obj[TIMER_GROUP_MAX][TIMER_MAX] = {0};
static portMUX_TYPE timer_spinlock[TIMER_GROUP_MAX] = {portMUX_INITIALIZER_UNLOCKED, portMUX_INITIALIZER_UNLOCKED};

esp_err_t timer_get_counter_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t *timer_val)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_val != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_PARAM_ADDR_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_get_counter_value(&(p_timer_obj[group_num][timer_num]->hal), timer_val);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_get_counter_time_sec(timer_group_t group_num, timer_idx_t timer_num, double *time)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(time != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_PARAM_ADDR_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    uint64_t timer_val;
    esp_err_t err = timer_get_counter_value(group_num, timer_num, &timer_val);
    if (err == ESP_OK) {
        uint32_t div;
        timer_hal_get_divider(&(p_timer_obj[group_num][timer_num]->hal), &div);
        *time = (double)timer_val * div / rtc_clk_apb_freq_get();
#if SOC_TIMER_GROUP_SUPPORT_XTAL
        if (timer_hal_get_use_xtal(&(p_timer_obj[group_num][timer_num]->hal))) {
            *time = (double)timer_val * div / ((int)rtc_clk_xtal_freq_get() * 1000000);
        }
#endif
    }
    return err;
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
    timer_hal_set_counter_enable(&(p_timer_obj[group_num][timer_num]->hal), TIMER_START);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_pause(timer_group_t group_num, timer_idx_t timer_num)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_set_counter_enable(&(p_timer_obj[group_num][timer_num]->hal), TIMER_PAUSE);
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
    timer_hal_set_counter_increase(&(p_timer_obj[group_num][timer_num]->hal), counter_dir);
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
    timer_hal_set_auto_reload(&(p_timer_obj[group_num][timer_num]->hal), reload);
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
    timer_hal_set_divider(&(p_timer_obj[group_num][timer_num]->hal), divider);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_set_alarm_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t alarm_value)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_set_alarm_value(&(p_timer_obj[group_num][timer_num]->hal), alarm_value);
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
    timer_hal_get_alarm_value(&(p_timer_obj[group_num][timer_num]->hal), alarm_value);
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
    timer_hal_set_alarm_enable(&(p_timer_obj[group_num][timer_num]->hal), alarm_en);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

static void IRAM_ATTR timer_isr_default(void *arg)
{
    bool is_awoken = false;

    timer_obj_t *timer_obj = (timer_obj_t *)arg;
    if (timer_obj == NULL) {
        return;
    }
    if (timer_obj->timer_isr_fun.fn == NULL) {
        return;
    }

    TIMER_ENTER_CRITICAL(&timer_spinlock[timer_obj->timer_isr_fun.isr_timer_group]);
    {
        uint32_t intr_status = 0;
        timer_hal_get_intr_status(&(timer_obj->hal), &intr_status);
        if (intr_status & BIT(timer_obj->hal.idx)) {
            // Clear intrrupt status
            timer_hal_clear_intr_status(&(timer_obj->hal));
            uint64_t old_alarm_value = 0;
            timer_hal_get_alarm_value(&(timer_obj->hal), &old_alarm_value);
            // call user registered callback
            is_awoken = timer_obj->timer_isr_fun.fn(timer_obj->timer_isr_fun.args);
            // reenable alarm if required
            uint64_t new_alarm_value = 0;
            timer_hal_get_alarm_value(&(timer_obj->hal), &new_alarm_value);
            bool reenable_alarm = (new_alarm_value != old_alarm_value) || timer_hal_get_auto_reload(&timer_obj->hal);
            timer_hal_set_alarm_enable(&(timer_obj->hal), reenable_alarm);
        }
    }
    TIMER_EXIT_CRITICAL(&timer_spinlock[timer_obj->timer_isr_fun.isr_timer_group]);

    if (is_awoken) {
        portYIELD_FROM_ISR();
    }
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

esp_err_t timer_isr_register(timer_group_t group_num, timer_idx_t timer_num,
                             void (*fn)(void *), void *arg, int intr_alloc_flags, timer_isr_handle_t *handle)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(fn != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_PARAM_ADDR_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);

    uint32_t status_reg = 0;
    uint32_t mask = 0;
    timer_hal_get_status_reg_mask_bit(&(p_timer_obj[group_num][timer_num]->hal), &status_reg, &mask);
    return esp_intr_alloc_intrstatus(timer_group_periph_signals.groups[group_num].t0_irq_id + timer_num, intr_alloc_flags, status_reg, mask, fn, arg, handle);
}

esp_err_t timer_init(timer_group_t group_num, timer_idx_t timer_num, const timer_config_t *config)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(config != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_PARAM_ADDR_ERROR);
    ESP_RETURN_ON_FALSE(config->divider > 1 && config->divider < 65537, ESP_ERR_INVALID_ARG, TIMER_TAG,  DIVIDER_RANGE_ERROR);

    periph_module_enable(timer_group_periph_signals.groups[group_num].module);

    if (p_timer_obj[group_num][timer_num] == NULL) {
        p_timer_obj[group_num][timer_num] = (timer_obj_t *) heap_caps_calloc(1, sizeof(timer_obj_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (p_timer_obj[group_num][timer_num] == NULL) {
            ESP_LOGE(TIMER_TAG, "TIMER driver malloc error");
            return ESP_FAIL;
        }
    }

    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_init(&(p_timer_obj[group_num][timer_num]->hal), group_num, timer_num);
    timer_hal_reset_periph(&(p_timer_obj[group_num][timer_num]->hal));
    timer_hal_clear_intr_status(&(p_timer_obj[group_num][timer_num]->hal));
    timer_hal_set_auto_reload(&(p_timer_obj[group_num][timer_num]->hal), config->auto_reload);
    timer_hal_set_divider(&(p_timer_obj[group_num][timer_num]->hal), config->divider);
    timer_hal_set_counter_increase(&(p_timer_obj[group_num][timer_num]->hal), config->counter_dir);
    timer_hal_set_alarm_enable(&(p_timer_obj[group_num][timer_num]->hal), config->alarm_en);
    timer_hal_set_level_int_enable(&(p_timer_obj[group_num][timer_num]->hal), config->intr_type == TIMER_INTR_LEVEL);
    if (config->intr_type != TIMER_INTR_LEVEL) {
        ESP_LOGW(TIMER_TAG, "only support Level Interrupt, switch to Level Interrupt instead");
    }
    timer_hal_set_counter_enable(&(p_timer_obj[group_num][timer_num]->hal), config->counter_en);
#if SOC_TIMER_GROUP_SUPPORT_XTAL
    timer_hal_set_use_xtal(&(p_timer_obj[group_num][timer_num]->hal), config->clk_src);
#endif
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);

    return ESP_OK;
}

esp_err_t timer_deinit(timer_group_t group_num, timer_idx_t timer_num)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);

    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_set_counter_enable(&(p_timer_obj[group_num][timer_num]->hal), TIMER_PAUSE);
    timer_hal_intr_disable(&(p_timer_obj[group_num][timer_num]->hal));
    timer_hal_clear_intr_status(&(p_timer_obj[group_num][timer_num]->hal));
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);

    heap_caps_free(p_timer_obj[group_num][timer_num]);
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
    config->alarm_en = timer_hal_get_alarm_enable(&(p_timer_obj[group_num][timer_num]->hal));
    config->auto_reload = timer_hal_get_auto_reload(&(p_timer_obj[group_num][timer_num]->hal));
    config->counter_dir = timer_hal_get_counter_increase(&(p_timer_obj[group_num][timer_num]->hal));
    config->counter_en = timer_hal_get_counter_enable(&(p_timer_obj[group_num][timer_num]->hal));

    uint32_t div;
    timer_hal_get_divider(&(p_timer_obj[group_num][timer_num]->hal), &div);
    config->divider = div;

    if (timer_hal_get_level_int_enable(&(p_timer_obj[group_num][timer_num]->hal))) {
        config->intr_type = TIMER_INTR_LEVEL;
    } else {
        config->intr_type = TIMER_INTR_MAX;
    }
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_group_intr_enable(timer_group_t group_num, timer_intr_t en_mask)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    for (int i = 0; i < TIMER_MAX; i++) {
        if (en_mask & BIT(i)) {
            timer_hal_intr_enable(&(p_timer_obj[group_num][i]->hal));
        }
    }
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_group_intr_disable(timer_group_t group_num, timer_intr_t disable_mask)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    for (int i = 0; i < TIMER_MAX; i++) {
        if (disable_mask & BIT(i)) {
            timer_hal_intr_disable(&(p_timer_obj[group_num][i]->hal));
        }
    }
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_enable_intr(timer_group_t group_num, timer_idx_t timer_num)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_intr_enable(&(p_timer_obj[group_num][timer_num]->hal));
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_disable_intr(timer_group_t group_num, timer_idx_t timer_num)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_intr_disable(&(p_timer_obj[group_num][timer_num]->hal));
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

/* This function is deprecated */
timer_intr_t IRAM_ATTR timer_group_intr_get_in_isr(timer_group_t group_num)
{
    uint32_t intr_raw_status = 0;
    timer_hal_get_intr_raw_status(group_num, &intr_raw_status);
    return intr_raw_status;
}

uint32_t IRAM_ATTR timer_group_get_intr_status_in_isr(timer_group_t group_num)
{
    uint32_t intr_status = 0;
    if (p_timer_obj[group_num][TIMER_0] != NULL) {
        timer_hal_get_intr_status(&(p_timer_obj[group_num][TIMER_0]->hal), &intr_status);
    }
#if SOC_TIMER_GROUP_TIMERS_PER_GROUP > 1
    else if (p_timer_obj[group_num][TIMER_1] != NULL) {
        timer_hal_get_intr_status(&(p_timer_obj[group_num][TIMER_1]->hal), &intr_status);
    }
#endif
    return intr_status;
}

/* This function is deprecated */
void IRAM_ATTR timer_group_intr_clr_in_isr(timer_group_t group_num, timer_idx_t timer_num)
{
    timer_group_clr_intr_status_in_isr(group_num, timer_num);
}

void IRAM_ATTR timer_group_clr_intr_status_in_isr(timer_group_t group_num, timer_idx_t timer_num)
{
    timer_hal_clear_intr_status(&(p_timer_obj[group_num][timer_num]->hal));
}

void IRAM_ATTR timer_group_enable_alarm_in_isr(timer_group_t group_num, timer_idx_t timer_num)
{
    timer_hal_set_alarm_enable(&(p_timer_obj[group_num][timer_num]->hal), true);
}

uint64_t IRAM_ATTR timer_group_get_counter_value_in_isr(timer_group_t group_num, timer_idx_t timer_num)
{
    uint64_t val;
    timer_hal_get_counter_value(&(p_timer_obj[group_num][timer_num]->hal), &val);
    return val;
}

void IRAM_ATTR timer_group_set_alarm_value_in_isr(timer_group_t group_num, timer_idx_t timer_num, uint64_t alarm_val)
{
    timer_hal_set_alarm_value(&(p_timer_obj[group_num][timer_num]->hal), alarm_val);
}

void IRAM_ATTR timer_group_set_counter_enable_in_isr(timer_group_t group_num, timer_idx_t timer_num, timer_start_t counter_en)
{
    timer_hal_set_counter_enable(&(p_timer_obj[group_num][timer_num]->hal), counter_en);
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
    return timer_hal_get_auto_reload(&(p_timer_obj[group_num][timer_num]->hal));
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
