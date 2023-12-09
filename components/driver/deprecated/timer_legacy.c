/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_clk_tree.h"
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

#if SOC_PERIPH_CLK_CTRL_SHARED
#define GPTIMER_CLOCK_SRC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define GPTIMER_CLOCK_SRC_ATOMIC()
#endif

typedef struct {
    timer_isr_t fn;  /*!< isr function */
    void *args;      /*!< isr function args */
    timer_isr_handle_t timer_isr_handle;  /*!< interrupt handle */
    timer_group_t isr_timer_group;        /*!< timer group of interrupt triggered */
} timer_isr_func_t;

typedef struct {
    timer_hal_context_t hal;
    timer_isr_func_t timer_isr_fun;
    timer_src_clk_t clk_src;
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
    *timer_val = timer_hal_capture_and_get_counter_value(&p_timer_obj[group_num][timer_num]->hal);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_get_counter_time_sec(timer_group_t group_num, timer_idx_t timer_num, double *time)
{
    ESP_RETURN_ON_FALSE(group_num < TIMER_GROUP_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(timer_num < TIMER_MAX, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NUM_ERROR);
    ESP_RETURN_ON_FALSE(time != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_PARAM_ADDR_ERROR);
    ESP_RETURN_ON_FALSE(p_timer_obj[group_num][timer_num] != NULL, ESP_ERR_INVALID_ARG, TIMER_TAG,  TIMER_NEVER_INIT_ERROR);
    uint64_t timer_val = timer_hal_capture_and_get_counter_value(&p_timer_obj[group_num][timer_num]->hal);
    uint32_t div = p_timer_obj[group_num][timer_num]->divider;
    // get clock source frequency
    uint32_t counter_src_hz = 0;
    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)p_timer_obj[group_num][timer_num]->clk_src,
                        ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &counter_src_hz),
                        TIMER_TAG, "get clock source frequency failed");
    *time = (double)timer_val * div / counter_src_hz;
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
    esp_err_t ret = ESP_OK;

    timer_disable_intr(group_num, timer_num);
    p_timer_obj[group_num][timer_num]->timer_isr_fun.fn = isr_handler;
    p_timer_obj[group_num][timer_num]->timer_isr_fun.args = args;
    p_timer_obj[group_num][timer_num]->timer_isr_fun.isr_timer_group = group_num;
    ret = timer_isr_register(group_num, timer_num, timer_isr_default, (void *)p_timer_obj[group_num][timer_num],
                             intr_alloc_flags, &(p_timer_obj[group_num][timer_num]->timer_isr_fun.timer_isr_handle));
    ESP_RETURN_ON_ERROR(ret, TIMER_TAG, "register interrupt service failed");
    timer_enable_intr(group_num, timer_num);

    return ret;
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

    PERIPH_RCC_ACQUIRE_ATOMIC(timer_group_periph_signals.groups[group_num].module, ref_count) {
        if (ref_count == 0) {
            timer_ll_enable_bus_clock(group_num, true);
            timer_ll_reset_register(group_num);
        }
    }

    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_init(hal, group_num, timer_num);
    timer_hal_set_counter_value(hal, 0);

    timer_src_clk_t clk_src = TIMER_SRC_CLK_DEFAULT;
    if (config->clk_src) {
        clk_src = config->clk_src;
    }
    GPTIMER_CLOCK_SRC_ATOMIC() {
        // although `clk_src` is of `timer_src_clk_t` type, but it's binary compatible with `gptimer_clock_source_t`,
        // as the underlying enum entries come from the same `soc_module_clk_t`
        timer_ll_set_clock_source(p_timer_obj[group_num][timer_num]->hal.dev, timer_num, (gptimer_clock_source_t)clk_src);
        timer_ll_enable_clock(hal->dev, timer_num, true);
    }
    timer_ll_set_clock_prescale(hal->dev, timer_num, config->divider);
    timer_ll_set_count_direction(p_timer_obj[group_num][timer_num]->hal.dev, timer_num, config->counter_dir);
    timer_ll_enable_intr(hal->dev, TIMER_LL_EVENT_ALARM(timer_num), false);
    timer_ll_clear_intr_status(hal->dev, TIMER_LL_EVENT_ALARM(timer_num));
    timer_ll_enable_alarm(hal->dev, timer_num, config->alarm_en);
    timer_ll_enable_auto_reload(hal->dev, timer_num, config->auto_reload);
    timer_ll_enable_counter(hal->dev, timer_num, config->counter_en);
    p_timer_obj[group_num][timer_num]->clk_src = clk_src;
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

    // disable the source clock
    GPTIMER_CLOCK_SRC_ATOMIC() {
        timer_ll_enable_clock(hal->dev, hal->timer_id, false);
    }
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_ll_enable_intr(hal->dev, TIMER_LL_EVENT_ALARM(timer_num), false);
    timer_ll_clear_intr_status(hal->dev, TIMER_LL_EVENT_ALARM(timer_num));
    timer_hal_deinit(hal);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);

    PERIPH_RCC_RELEASE_ATOMIC(timer_group_periph_signals.groups[group_num].module, ref_count) {
        if (ref_count == 0) {
            timer_ll_enable_bus_clock(group_num, false);
        }
    }

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

void IRAM_ATTR timer_group_enable_alarm_in_isr(timer_group_t group_num, timer_idx_t timer_num)
{
    timer_ll_enable_alarm(p_timer_obj[group_num][timer_num]->hal.dev, timer_num, true);
}

uint64_t IRAM_ATTR timer_group_get_counter_value_in_isr(timer_group_t group_num, timer_idx_t timer_num)
{
    timer_ll_trigger_soft_capture(p_timer_obj[group_num][timer_num]->hal.dev, timer_num);
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

bool IRAM_ATTR timer_group_get_auto_reload_in_isr(timer_group_t group_num, timer_idx_t timer_num)
{
    return p_timer_obj[group_num][timer_num]->auto_reload_en;
}

/**
 * @brief This function will be called during start up, to check that this legacy timer group driver is not running along with the gptimer driver
 */
__attribute__((constructor))
static void check_legacy_timer_driver_conflict(void)
{
    // This function was declared as weak here. gptimer driver has one implementation.
    // So if gptimer driver is not linked in, then `gptimer_new_timer()` should be NULL at runtime.
    extern __attribute__((weak)) esp_err_t gptimer_new_timer(const void *config, void **ret_timer);
    if ((void *)gptimer_new_timer != NULL) {
        ESP_EARLY_LOGE(TIMER_TAG, "CONFLICT! driver_ng is not allowed to be used with the legacy driver");
        abort();
    }
    ESP_EARLY_LOGW(TIMER_TAG, "legacy driver is deprecated, please migrate to `driver/gptimer.h`");
}
