/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "esp_check.h"
#include "driver/ledc.h"
#include "esp_private/etm_interface.h"
#include "esp_heap_caps.h"
#include "hal/ledc_ll.h"

#define TAG "ledc_etm"

typedef struct {
    union {
        esp_etm_event_t evt_base;
        esp_etm_task_t task_base;
    };
    ledc_mode_t speed_mode;
    volatile uint32_t *en_reg_addr;
    uint32_t en_bit;
} ledc_etm_event_task_t;

static portMUX_TYPE s_ledc_etm_spinlock = portMUX_INITIALIZER_UNLOCKED;

static esp_err_t ledc_del_etm_event(esp_etm_event_t *event)
{
    ledc_etm_event_task_t *etm_event = __containerof(event, ledc_etm_event_task_t, evt_base);

    if (etm_event->en_reg_addr != 0) {
        portENTER_CRITICAL(&s_ledc_etm_spinlock);
        ledc_ll_etm_enable_evt_task(LEDC_LL_GET_HW(), etm_event->speed_mode, etm_event->en_reg_addr, etm_event->en_bit, false);
        portEXIT_CRITICAL(&s_ledc_etm_spinlock);
    }

    free(etm_event);
    return ESP_OK;
}

static esp_err_t ledc_del_etm_task(esp_etm_task_t *task)
{
    ledc_etm_event_task_t *etm_task = __containerof(task, ledc_etm_event_task_t, task_base);

    if (etm_task->en_reg_addr != 0) {
        portENTER_CRITICAL(&s_ledc_etm_spinlock);
        ledc_ll_etm_enable_evt_task(LEDC_LL_GET_HW(), etm_task->speed_mode, etm_task->en_reg_addr, etm_task->en_bit, false);
        portEXIT_CRITICAL(&s_ledc_etm_spinlock);
    }

    free(etm_task);
    return ESP_OK;
}

esp_err_t ledc_timer_new_etm_event(ledc_mode_t speed_mode, ledc_timer_t timer_sel, const ledc_timer_etm_event_config_t *config, esp_etm_event_handle_t *out_event)
{
    ESP_RETURN_ON_FALSE(speed_mode < LEDC_SPEED_MODE_MAX && timer_sel < LEDC_TIMER_MAX && config && config->event_type < LEDC_TIMER_ETM_EVENT_MAX && out_event, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    ledc_etm_event_task_t *event = heap_caps_calloc(1, sizeof(ledc_etm_event_task_t), MALLOC_CAP_DEFAULT);
    ESP_RETURN_ON_FALSE(event, ESP_ERR_NO_MEM, TAG, "no memory for ETM event");

    uint32_t evt_id = LEDC_LL_ETM_TIMER_EVENT_ID(speed_mode, timer_sel, config->event_type);
    volatile uint32_t *en_reg = LEDC_LL_ETM_TIMER_EVENT_EN_REG(speed_mode, config->event_type);
    uint32_t en_bit = LEDC_LL_ETM_TIMER_EVENT_EN_BIT(speed_mode, timer_sel, config->event_type);

    portENTER_CRITICAL(&s_ledc_etm_spinlock);
    ledc_ll_etm_enable_evt_task(LEDC_LL_GET_HW(), speed_mode, en_reg, en_bit, true);
    portEXIT_CRITICAL(&s_ledc_etm_spinlock);
    event->speed_mode = speed_mode;
    event->en_reg_addr = en_reg;
    event->en_bit = en_bit;
    event->evt_base.event_id = evt_id;
    event->evt_base.trig_periph = ETM_TRIG_PERIPH_LEDC;
    event->evt_base.del = ledc_del_etm_event;

    *out_event = &event->evt_base;
    return ESP_OK;
}

esp_err_t ledc_channel_new_etm_event(ledc_mode_t speed_mode, ledc_channel_t channel, const ledc_channel_etm_event_config_t *config, esp_etm_event_handle_t *out_event)
{
    ESP_RETURN_ON_FALSE(speed_mode < LEDC_SPEED_MODE_MAX && channel < LEDC_CHANNEL_MAX && config && config->event_type < LEDC_CHANNEL_ETM_EVENT_MAX && out_event, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    ledc_etm_event_task_t *event = heap_caps_calloc(1, sizeof(ledc_etm_event_task_t), MALLOC_CAP_DEFAULT);
    ESP_RETURN_ON_FALSE(event, ESP_ERR_NO_MEM, TAG, "no memory for ETM event");

    uint32_t evt_id = LEDC_LL_ETM_CHANNEL_EVENT_ID(speed_mode, channel, config->event_type);
    volatile uint32_t *en_reg = LEDC_LL_ETM_CHANNEL_EVENT_EN_REG(speed_mode, config->event_type);
    uint32_t en_bit = LEDC_LL_ETM_CHANNEL_EVENT_EN_BIT(speed_mode, channel, config->event_type);

    portENTER_CRITICAL(&s_ledc_etm_spinlock);
    ledc_ll_etm_enable_evt_task(LEDC_LL_GET_HW(), speed_mode, en_reg, en_bit, true);
    portEXIT_CRITICAL(&s_ledc_etm_spinlock);
    event->speed_mode = speed_mode;
    event->en_reg_addr = en_reg;
    event->en_bit = en_bit;
    event->evt_base.event_id = evt_id;
    event->evt_base.trig_periph = ETM_TRIG_PERIPH_LEDC;
    event->evt_base.del = ledc_del_etm_event;

    *out_event = &event->evt_base;
    return ESP_OK;
}

esp_err_t ledc_timer_new_etm_task(ledc_mode_t speed_mode, ledc_timer_t timer_sel, const ledc_timer_etm_task_config_t *config, esp_etm_task_handle_t *out_task)
{
    ESP_RETURN_ON_FALSE(speed_mode < LEDC_SPEED_MODE_MAX && timer_sel < LEDC_TIMER_MAX && config && config->task_type < LEDC_TIMER_ETM_TASK_MAX && out_task, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    ledc_etm_event_task_t *task = heap_caps_calloc(1, sizeof(ledc_etm_event_task_t), MALLOC_CAP_DEFAULT);
    ESP_RETURN_ON_FALSE(task, ESP_ERR_NO_MEM, TAG, "no memory for ETM task");

    uint32_t task_id = LEDC_LL_ETM_TIMER_TASK_ID(speed_mode, timer_sel, config->task_type);
    volatile uint32_t *en_reg = LEDC_LL_ETM_TIMER_TASK_EN_REG(speed_mode, config->task_type);
    uint32_t en_bit = LEDC_LL_ETM_TIMER_TASK_EN_BIT(speed_mode, timer_sel, config->task_type);

    portENTER_CRITICAL(&s_ledc_etm_spinlock);
    ledc_ll_etm_enable_evt_task(LEDC_LL_GET_HW(), speed_mode, en_reg, en_bit, true);
    portEXIT_CRITICAL(&s_ledc_etm_spinlock);
    task->speed_mode = speed_mode;
    task->en_reg_addr = en_reg;
    task->en_bit = en_bit;
    task->task_base.task_id = task_id;
    task->task_base.trig_periph = ETM_TRIG_PERIPH_LEDC;
    task->task_base.del = ledc_del_etm_task;

    *out_task = &task->task_base;
    return ESP_OK;
}

esp_err_t ledc_channel_new_etm_task(ledc_mode_t speed_mode, ledc_channel_t channel, const ledc_channel_etm_task_config_t *config, esp_etm_task_handle_t *out_task)
{
    ESP_RETURN_ON_FALSE(speed_mode < LEDC_SPEED_MODE_MAX && channel < LEDC_CHANNEL_MAX && config && config->task_type < LEDC_CHANNEL_ETM_TASK_MAX && out_task, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    ledc_etm_event_task_t *task = heap_caps_calloc(1, sizeof(ledc_etm_event_task_t), MALLOC_CAP_DEFAULT);
    ESP_RETURN_ON_FALSE(task, ESP_ERR_NO_MEM, TAG, "no memory for ETM task");

    uint32_t task_id = LEDC_LL_ETM_CHANNEL_TASK_ID(speed_mode, channel, config->task_type);
    volatile uint32_t *en_reg = LEDC_LL_ETM_CHANNEL_TASK_EN_REG(speed_mode, config->task_type);
    uint32_t en_bit = LEDC_LL_ETM_CHANNEL_TASK_EN_BIT(speed_mode, channel, config->task_type);

    portENTER_CRITICAL(&s_ledc_etm_spinlock);
    ledc_ll_etm_enable_evt_task(LEDC_LL_GET_HW(), speed_mode, en_reg, en_bit, true);
    portEXIT_CRITICAL(&s_ledc_etm_spinlock);
    task->speed_mode = speed_mode;
    task->en_reg_addr = en_reg;
    task->en_bit = en_bit;
    task->task_base.task_id = task_id;
    task->task_base.trig_periph = ETM_TRIG_PERIPH_LEDC;
    task->task_base.del = ledc_del_etm_task;

    *out_task = &task->task_base;
    return ESP_OK;
}
