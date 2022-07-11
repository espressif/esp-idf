// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "driver/timer.h"
#include "driver/periph_ctrl.h"
#include "hal/timer_hal.h"
#include "soc/rtc.h"

static const char *TIMER_TAG = "timer_group";

#define TIMER_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(TIMER_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }

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
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_val != NULL, TIMER_PARAM_ADDR_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num][timer_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_get_counter_value(&(p_timer_obj[group_num][timer_num]->hal), timer_val);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_get_counter_time_sec(timer_group_t group_num, timer_idx_t timer_num, double *time)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(time != NULL, TIMER_PARAM_ADDR_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num][timer_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);
    uint64_t timer_val;
    esp_err_t err = timer_get_counter_value(group_num, timer_num, &timer_val);
    if (err == ESP_OK) {
        uint32_t div;
        timer_hal_get_divider(&(p_timer_obj[group_num][timer_num]->hal), &div);
        *time = (double)timer_val * div / rtc_clk_apb_freq_get();
#ifdef TIMER_GROUP_SUPPORTS_XTAL_CLOCK
        if (timer_hal_get_use_xtal(&(p_timer_obj[group_num][timer_num]->hal))) {
            *time = (double)timer_val * div / ((int)rtc_clk_xtal_freq_get() * 1000000);
        }
#endif
    }
    return err;
}

esp_err_t timer_set_counter_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t load_val)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num][timer_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_set_counter_value(&(p_timer_obj[group_num][timer_num]->hal), load_val);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_start(timer_group_t group_num, timer_idx_t timer_num)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num][timer_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_set_counter_enable(&(p_timer_obj[group_num][timer_num]->hal), TIMER_START);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_pause(timer_group_t group_num, timer_idx_t timer_num)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num][timer_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_set_counter_enable(&(p_timer_obj[group_num][timer_num]->hal), TIMER_PAUSE);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_set_counter_mode(timer_group_t group_num, timer_idx_t timer_num, timer_count_dir_t counter_dir)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(counter_dir < TIMER_COUNT_MAX, TIMER_COUNT_DIR_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num][timer_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_set_counter_increase(&(p_timer_obj[group_num][timer_num]->hal), counter_dir);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_set_auto_reload(timer_group_t group_num, timer_idx_t timer_num, timer_autoreload_t reload)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(reload < TIMER_AUTORELOAD_MAX, TIMER_AUTORELOAD_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num][timer_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_set_auto_reload(&(p_timer_obj[group_num][timer_num]->hal), reload);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_set_divider(timer_group_t group_num, timer_idx_t timer_num, uint32_t divider)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(divider > 1 && divider < 65537, DIVIDER_RANGE_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num][timer_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_set_divider(&(p_timer_obj[group_num][timer_num]->hal), (uint16_t) divider);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_set_alarm_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t alarm_value)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num][timer_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_set_alarm_value(&(p_timer_obj[group_num][timer_num]->hal), alarm_value);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_get_alarm_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t *alarm_value)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(alarm_value != NULL, TIMER_PARAM_ADDR_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num][timer_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_get_alarm_value(&(p_timer_obj[group_num][timer_num]->hal), alarm_value);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_set_alarm(timer_group_t group_num, timer_idx_t timer_num, timer_alarm_t alarm_en)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(alarm_en < TIMER_ALARM_MAX, TIMER_ALARM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num][timer_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);
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
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num][timer_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);

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
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num][timer_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);

    timer_disable_intr(group_num, timer_num);
    p_timer_obj[group_num][timer_num]->timer_isr_fun.fn = NULL;
    p_timer_obj[group_num][timer_num]->timer_isr_fun.args = NULL;
    esp_intr_free(p_timer_obj[group_num][timer_num]->timer_isr_fun.timer_isr_handle);

    return ESP_OK;
}

esp_err_t timer_isr_register(timer_group_t group_num, timer_idx_t timer_num,
                             void (*fn)(void *), void *arg, int intr_alloc_flags, timer_isr_handle_t *handle)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(fn != NULL, TIMER_PARAM_ADDR_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num][timer_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);

    int intr_source = 0;
    uint32_t status_reg = 0;
    uint32_t mask = 0;
    switch (group_num) {
    case TIMER_GROUP_0:
    default:
        if ((intr_alloc_flags & ESP_INTR_FLAG_EDGE) == 0) {
            intr_source = ETS_TG0_T0_LEVEL_INTR_SOURCE + timer_num;
        } else {
            intr_source = ETS_TG0_T0_EDGE_INTR_SOURCE + timer_num;
        }
        timer_hal_get_status_reg_mask_bit(&(p_timer_obj[TIMER_GROUP_0][timer_num]->hal), &status_reg, &mask);
        break;
    case TIMER_GROUP_1:
        if ((intr_alloc_flags & ESP_INTR_FLAG_EDGE) == 0) {
            intr_source = ETS_TG1_T0_LEVEL_INTR_SOURCE + timer_num;
        } else {
            intr_source = ETS_TG1_T0_EDGE_INTR_SOURCE + timer_num;
        }
        timer_hal_get_status_reg_mask_bit(&(p_timer_obj[TIMER_GROUP_1][timer_num]->hal), &status_reg, &mask);
        break;
    }
    return esp_intr_alloc_intrstatus(intr_source, intr_alloc_flags, status_reg, mask, fn, arg, handle);
}

esp_err_t timer_init(timer_group_t group_num, timer_idx_t timer_num, const timer_config_t *config)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(config != NULL, TIMER_PARAM_ADDR_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(config->divider > 1 && config->divider < 65537, DIVIDER_RANGE_ERROR, ESP_ERR_INVALID_ARG);

    if (group_num == TIMER_GROUP_0) {
        periph_module_enable(PERIPH_TIMG0_MODULE);
    } else if (group_num == TIMER_GROUP_1) {
        periph_module_enable(PERIPH_TIMG1_MODULE);
    }

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
    if (config->intr_type == TIMER_INTR_LEVEL) {
        timer_hal_set_level_int_enable(&(p_timer_obj[group_num][timer_num]->hal), true);
    }
    // currently edge interrupt is not supported
    // if (config->intr_type == TIMER_INTR_EDGE) {
    //     timer_hal_set_edge_int_enable(&(p_timer_obj[group_num][timer_num]->hal), true);
    // }
    timer_hal_set_counter_enable(&(p_timer_obj[group_num][timer_num]->hal), config->counter_en);
#ifdef TIMER_GROUP_SUPPORTS_XTAL_CLOCK
    timer_hal_set_use_xtal(&(p_timer_obj[group_num][timer_num]->hal), config->clk_src);
#endif
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);

    return ESP_OK;
}

esp_err_t timer_deinit(timer_group_t group_num, timer_idx_t timer_num)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num][timer_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);

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
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(config != NULL, TIMER_PARAM_ADDR_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num][timer_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);

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
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);
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
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);
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
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num][timer_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    timer_hal_intr_enable(&(p_timer_obj[group_num][timer_num]->hal));
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_disable_intr(timer_group_t group_num, timer_idx_t timer_num)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(p_timer_obj[group_num][timer_num] != NULL, TIMER_NEVER_INIT_ERROR, ESP_ERR_INVALID_ARG);
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
    } else if (p_timer_obj[group_num][TIMER_1] != NULL) {
        timer_hal_get_intr_status(&(p_timer_obj[group_num][TIMER_1]->hal), &intr_status);
    }
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
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t IRAM_ATTR timer_spinlock_give(timer_group_t group_num)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}
