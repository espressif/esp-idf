// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_intr.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "driver/timer.h"
#include "driver/periph_ctrl.h"

static const char* TIMER_TAG = "timer_group";
#define TIMER_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(TIMER_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }

#define TIMER_GROUP_NUM_ERROR   "TIMER GROUP NUM ERROR"
#define TIMER_NUM_ERROR         "HW TIMER NUM ERROR"
#define TIMER_PARAM_ADDR_ERROR  "HW TIMER PARAM ADDR ERROR"
#define TIMER_COUNT_DIR_ERROR   "HW TIMER COUNTER DIR ERROR"
#define TIMER_AUTORELOAD_ERROR  "HW TIMER AUTORELOAD ERROR"
#define TIMER_SCALE_ERROR       "HW TIMER SCALE ERROR"
#define TIMER_ALARM_ERROR       "HW TIMER ALARM ERROR"
#define DIVIDER_RANGE_ERROR     "HW TIMER divider outside of [2, 65536] range error"
static timg_dev_t *TG[2] = {&TIMERG0, &TIMERG1};
static portMUX_TYPE timer_spinlock[TIMER_GROUP_MAX] = {portMUX_INITIALIZER_UNLOCKED, portMUX_INITIALIZER_UNLOCKED};

#define TIMER_ENTER_CRITICAL(mux)      portENTER_CRITICAL(mux);
#define TIMER_EXIT_CRITICAL(mux)       portEXIT_CRITICAL(mux);

esp_err_t timer_get_counter_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t* timer_val)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_val != NULL, TIMER_PARAM_ADDR_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&timer_spinlock[group_num]);
    TG[group_num]->hw_timer[timer_num].update = 1;
    *timer_val = ((uint64_t) TG[group_num]->hw_timer[timer_num].cnt_high << 32)
        | (TG[group_num]->hw_timer[timer_num].cnt_low);
    portEXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_get_counter_time_sec(timer_group_t group_num, timer_idx_t timer_num, double* time)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(time != NULL, TIMER_PARAM_ADDR_ERROR, ESP_ERR_INVALID_ARG);

    uint64_t timer_val;
    esp_err_t err = timer_get_counter_value(group_num, timer_num, &timer_val);
    if (err == ESP_OK) {
        uint16_t div = TG[group_num]->hw_timer[timer_num].config.divider;
        *time = (double)timer_val * div / TIMER_BASE_CLK;
    }
    return err;
}

esp_err_t timer_set_counter_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t load_val)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    TG[group_num]->hw_timer[timer_num].load_high = (uint32_t) (load_val >> 32);
    TG[group_num]->hw_timer[timer_num].load_low = (uint32_t) load_val;
    TG[group_num]->hw_timer[timer_num].reload = 1;
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_start(timer_group_t group_num, timer_idx_t timer_num)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    TG[group_num]->hw_timer[timer_num].config.enable = 1;
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_pause(timer_group_t group_num, timer_idx_t timer_num)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    TG[group_num]->hw_timer[timer_num].config.enable = 0;
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_set_counter_mode(timer_group_t group_num, timer_idx_t timer_num, timer_count_dir_t counter_dir)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(counter_dir < TIMER_COUNT_MAX, TIMER_COUNT_DIR_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    TG[group_num]->hw_timer[timer_num].config.increase = counter_dir;
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_set_auto_reload(timer_group_t group_num, timer_idx_t timer_num, timer_autoreload_t reload)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(reload < TIMER_AUTORELOAD_MAX, TIMER_AUTORELOAD_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    TG[group_num]->hw_timer[timer_num].config.autoreload = reload;
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_set_divider(timer_group_t group_num, timer_idx_t timer_num, uint32_t divider)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(divider > 1 && divider < 65537, DIVIDER_RANGE_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    int timer_en = TG[group_num]->hw_timer[timer_num].config.enable;
    TG[group_num]->hw_timer[timer_num].config.enable = 0;
    TG[group_num]->hw_timer[timer_num].config.divider = (uint16_t) divider;
    TG[group_num]->hw_timer[timer_num].config.enable = timer_en;
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_set_alarm_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t alarm_value)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    TG[group_num]->hw_timer[timer_num].alarm_high = (uint32_t) (alarm_value >> 32);
    TG[group_num]->hw_timer[timer_num].alarm_low = (uint32_t) alarm_value;
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_get_alarm_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t* alarm_value)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(alarm_value != NULL, TIMER_PARAM_ADDR_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&timer_spinlock[group_num]);
    *alarm_value = ((uint64_t) TG[group_num]->hw_timer[timer_num].alarm_high << 32)
                | (TG[group_num]->hw_timer[timer_num].alarm_low);
    portEXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_set_alarm(timer_group_t group_num, timer_idx_t timer_num, timer_alarm_t alarm_en)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(alarm_en < TIMER_ALARM_MAX, TIMER_ALARM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    TG[group_num]->hw_timer[timer_num].config.alarm_en = alarm_en;
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_isr_register(timer_group_t group_num, timer_idx_t timer_num, 
    void (*fn)(void*), void * arg, int intr_alloc_flags, timer_isr_handle_t *handle)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(fn != NULL, TIMER_PARAM_ADDR_ERROR, ESP_ERR_INVALID_ARG);

    int intr_source = 0;
    uint32_t status_reg = 0;
    int mask = 0;
    switch(group_num) {
        case TIMER_GROUP_0:
        default:
            if((intr_alloc_flags & ESP_INTR_FLAG_EDGE) == 0) {
                intr_source = ETS_TG0_T0_LEVEL_INTR_SOURCE + timer_num;
            } else {
                intr_source = ETS_TG0_T0_EDGE_INTR_SOURCE + timer_num;
            }
            status_reg = TIMG_INT_ST_TIMERS_REG(0);
            mask = 1<<timer_num;
            break;
        case TIMER_GROUP_1:
            if((intr_alloc_flags & ESP_INTR_FLAG_EDGE) == 0) {
                intr_source = ETS_TG1_T0_LEVEL_INTR_SOURCE + timer_num;
            } else {
                intr_source = ETS_TG1_T0_EDGE_INTR_SOURCE + timer_num;
            }
            status_reg = TIMG_INT_ST_TIMERS_REG(1);
            mask = 1<<timer_num;
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

    if(group_num == 0) {
        periph_module_enable(PERIPH_TIMG0_MODULE);
    } else if(group_num == 1) {
        periph_module_enable(PERIPH_TIMG1_MODULE);
    }
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    TG[group_num]->hw_timer[timer_num].config.autoreload = config->auto_reload;
    TG[group_num]->hw_timer[timer_num].config.divider = (uint16_t) config->divider;
    TG[group_num]->hw_timer[timer_num].config.enable = config->counter_en;
    TG[group_num]->hw_timer[timer_num].config.increase = config->counter_dir;
    TG[group_num]->hw_timer[timer_num].config.alarm_en = config->alarm_en;
    TG[group_num]->hw_timer[timer_num].config.level_int_en = (config->intr_type == TIMER_INTR_LEVEL ? 1 : 0);
    TG[group_num]->hw_timer[timer_num].config.edge_int_en = (config->intr_type == TIMER_INTR_LEVEL ? 0 : 1);
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_get_config(timer_group_t group_num, timer_idx_t timer_num, timer_config_t *config)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(config != NULL, TIMER_PARAM_ADDR_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_ENTER_CRITICAL(&timer_spinlock[group_num]);
    config->alarm_en = TG[group_num]->hw_timer[timer_num].config.alarm_en;
    config->auto_reload = TG[group_num]->hw_timer[timer_num].config.autoreload;
    config->counter_dir = TG[group_num]->hw_timer[timer_num].config.increase;
    config->divider =  (TG[group_num]->hw_timer[timer_num].config.divider == 0 ?
        65536 : TG[group_num]->hw_timer[timer_num].config.divider);
    config->counter_en = TG[group_num]->hw_timer[timer_num].config.enable;
    if(TG[group_num]->hw_timer[timer_num].config.level_int_en) {
        config->intr_type = TIMER_INTR_LEVEL;
    }
    TIMER_EXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_group_intr_enable(timer_group_t group_num, uint32_t en_mask)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&timer_spinlock[group_num]);
    TG[group_num]->int_ena.val |= en_mask;
    portEXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_group_intr_disable(timer_group_t group_num, uint32_t disable_mask)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&timer_spinlock[group_num]);
    TG[group_num]->int_ena.val &= (~disable_mask);
    portEXIT_CRITICAL(&timer_spinlock[group_num]);
    return ESP_OK;
}

esp_err_t timer_enable_intr(timer_group_t group_num, timer_idx_t timer_num)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    return timer_group_intr_enable(group_num, BIT(timer_num));
}

esp_err_t timer_disable_intr(timer_group_t group_num, timer_idx_t timer_num)
{
    TIMER_CHECK(group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG);
    TIMER_CHECK(timer_num < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    return timer_group_intr_disable(group_num, BIT(timer_num));
}


