/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "iot_button.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"

#define USE_ESP_TIMER   CONFIG_BUTTON_USE_ESP_TIMER
#if USE_ESP_TIMER
#define STOP_TIMER(tmr)   esp_timer_stop(tmr)
#define DELETE_TIMER(tmr) esp_timer_delete(tmr)
#else
#define STOP_TIMER(tmr)   xTimerStop(tmr, portMAX_DELAY)
#define DELETE_TIMER(tmr) xTimerDelete(tmr, portMAX_DELAY);
#endif

#define IOT_CHECK(tag, a, ret)  if(!(a)) {                                             \
        ESP_LOGE(tag,"%s:%d (%s)", __FILE__, __LINE__, __FUNCTION__);      \
        return (ret);                                                                   \
        }
#define ERR_ASSERT(tag, param)  IOT_CHECK(tag, (param) == ESP_OK, ESP_FAIL)
#define POINT_ASSERT(tag, param, ret)    IOT_CHECK(tag, (param) != NULL, (ret))

typedef enum {
    BUTTON_STATE_IDLE = 0,
    BUTTON_STATE_PUSH,
    BUTTON_STATE_PRESSED,
} button_status_t;

typedef struct button_dev button_dev_t;
typedef struct btn_cb button_cb_t;

struct btn_cb{
    TickType_t interval;
    button_cb cb;
    void* arg;
    #if !USE_ESP_TIMER
    TimerHandle_t tmr;
    #else
    esp_timer_handle_t tmr;
    #endif
    button_dev_t *pbtn;
    button_cb_t *next_cb;
};

struct button_dev{
    uint8_t io_num;
    uint8_t active_level;
    uint32_t serial_thres_sec;
    button_status_t state;
    button_cb_t tap_short_cb;
    button_cb_t tap_psh_cb;
    button_cb_t tap_rls_cb;
    button_cb_t press_serial_cb;
    button_cb_t* cb_head;
};

#define BUTTON_GLITCH_FILTER_TIME_MS   CONFIG_BUTTON_IO_GLITCH_FILTER_TIME_MS
static const char* TAG = "button";

// static void button_press_cb(TimerHandle_t tmr)
static void button_press_cb(void* tmr)
{
    #if !USE_ESP_TIMER
    button_cb_t* btn_cb = (button_cb_t*) pvTimerGetTimerID(tmr);
    #else
    button_cb_t* btn_cb = (button_cb_t*)(tmr);
    #endif

    button_dev_t* btn = btn_cb->pbtn;
    // low, then restart
    if (btn->active_level == gpio_get_level(btn->io_num)) {
        btn->state = BUTTON_STATE_PRESSED;
        if (btn_cb->cb) {
            btn_cb->cb(btn_cb->arg);
        }
    }
}

// static void button_tap_psh_cb(TimerHandle_t tmr)
static void button_tap_psh_cb(void* tmr)
{
    #if !USE_ESP_TIMER
    button_cb_t* btn_cb = (button_cb_t*) pvTimerGetTimerID(tmr);
    #else
    button_cb_t* btn_cb = (button_cb_t*)(tmr);
    #endif

    button_dev_t* btn = btn_cb->pbtn;
    STOP_TIMER(btn->tap_rls_cb.tmr);

    int lv = gpio_get_level(btn->io_num);

    if (btn->active_level == lv) {
        // high, then key is up
        btn->state = BUTTON_STATE_PUSH;
        if (btn->press_serial_cb.tmr) {
            #if !USE_ESP_TIMER
            xTimerChangePeriod(btn->press_serial_cb.tmr, btn->serial_thres_sec*1000 / portTICK_PERIOD_MS, portMAX_DELAY);
            xTimerReset(btn->press_serial_cb.tmr, portMAX_DELAY);
            #else
            esp_timer_stop(btn->press_serial_cb.tmr);
            esp_timer_start_once(btn->press_serial_cb.tmr, btn->serial_thres_sec * 1000 * 1000);
            #endif

        }
        if (btn->tap_psh_cb.cb) {
            btn->tap_psh_cb.cb(btn->tap_psh_cb.arg);
        }
    } else {
        // 50ms, check if this is a real key up
        if (btn->tap_rls_cb.tmr) {
            STOP_TIMER(btn->tap_rls_cb.tmr);
            #if !USE_ESP_TIMER
            xTimerReset(btn->tap_rls_cb.tmr, portMAX_DELAY);
            #else
            esp_timer_start_once(btn->tap_rls_cb.tmr, btn->tap_rls_cb.interval * portTICK_PERIOD_MS * 1000);
            #endif
        }
    }
}

static void button_tap_rls_cb(void* tmr)
{
    #if !USE_ESP_TIMER
    button_cb_t* btn_cb = (button_cb_t*) pvTimerGetTimerID(tmr);
    #else
    button_cb_t* btn_cb = (button_cb_t*)(tmr);
    #endif
    button_dev_t* btn = btn_cb->pbtn;
    STOP_TIMER(btn->tap_rls_cb.tmr);

    if (btn->active_level == gpio_get_level(btn->io_num)) {

    } else {
        // high, then key is up
        button_cb_t *pcb = btn->cb_head;
        while (pcb != NULL) {
            if (pcb->tmr != NULL) {
                STOP_TIMER(pcb->tmr);
            }
            pcb = pcb->next_cb;
        }
        if (btn->press_serial_cb.tmr && btn->press_serial_cb.tmr != NULL) {
            STOP_TIMER(btn->press_serial_cb.tmr);
        }
        if (btn->tap_short_cb.cb && btn->state == BUTTON_STATE_PUSH) {
            btn->tap_short_cb.cb(btn->tap_short_cb.arg);
        }
        if(btn->tap_rls_cb.cb && btn->state != BUTTON_STATE_IDLE) {
            btn->tap_rls_cb.cb(btn->tap_rls_cb.arg);
        }
        btn->state = BUTTON_STATE_IDLE;
    }
}

static void button_press_serial_cb(void* tmr)
{
    #if !USE_ESP_TIMER
    button_dev_t* btn = (button_dev_t*) pvTimerGetTimerID(tmr);
    #else
    button_dev_t* btn = (button_dev_t*)(tmr);
    #endif

    if (btn->press_serial_cb.cb) {
        btn->press_serial_cb.cb(btn->press_serial_cb.arg);
    }
    #if !USE_ESP_TIMER
    xTimerChangePeriod(btn->press_serial_cb.tmr, btn->press_serial_cb.interval, portMAX_DELAY);
    xTimerReset(btn->press_serial_cb.tmr, portMAX_DELAY);
    #else
    esp_timer_stop(btn->press_serial_cb.tmr);
    esp_timer_start_once(btn->press_serial_cb.tmr, btn->press_serial_cb.interval * portTICK_PERIOD_MS * 1000);
    #endif
}

static void button_gpio_isr_handler(void* arg)
{
    button_dev_t* btn = (button_dev_t*) arg;
    BaseType_t HPTaskAwoken = pdFALSE;
    int level = gpio_get_level(btn->io_num);
    if (level == btn->active_level) {
        if (btn->tap_psh_cb.tmr) {
            #if !USE_ESP_TIMER
            xTimerStopFromISR(btn->tap_psh_cb.tmr, &HPTaskAwoken);
            xTimerResetFromISR(btn->tap_psh_cb.tmr, &HPTaskAwoken);
            #else
            esp_timer_stop(btn->tap_psh_cb.tmr);
            esp_timer_start_once(btn->tap_psh_cb.tmr, btn->tap_psh_cb.interval * portTICK_PERIOD_MS * 1000);
            #endif
        }

        button_cb_t *pcb = btn->cb_head;
        while (pcb != NULL) {
            if (pcb->tmr != NULL) {
                #if !USE_ESP_TIMER
                xTimerStopFromISR(pcb->tmr, &HPTaskAwoken);
                xTimerResetFromISR(pcb->tmr, &HPTaskAwoken);
                #else
                esp_timer_stop(pcb->tmr);
                esp_timer_start_once(pcb->tmr, pcb->interval * portTICK_PERIOD_MS * 1000);
                #endif
            }
            pcb = pcb->next_cb;
        }
    } else {
        // 50ms, check if this is a real key up
        if (btn->tap_rls_cb.tmr) {
            #if !USE_ESP_TIMER
            xTimerStopFromISR(btn->tap_rls_cb.tmr, &HPTaskAwoken);
            xTimerResetFromISR(btn->tap_rls_cb.tmr, &HPTaskAwoken);
            #else
            esp_timer_stop(btn->tap_rls_cb.tmr);
            esp_timer_start_once(btn->tap_rls_cb.tmr, btn->tap_rls_cb.interval * portTICK_PERIOD_MS * 1000);
            #endif
        }
    }
    if(HPTaskAwoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

#if !USE_ESP_TIMER
static void button_free_tmr(TimerHandle_t* tmr)
#else
static void button_free_tmr(esp_timer_handle_t *tmr)
#endif
{
    if (tmr && *tmr) {
        STOP_TIMER(*tmr);
        DELETE_TIMER(*tmr);
        *tmr = NULL;
    }
}

esp_err_t iot_button_delete(button_handle_t btn_handle)
{
    POINT_ASSERT(TAG, btn_handle, ESP_ERR_INVALID_ARG);
    button_dev_t* btn = (button_dev_t*) btn_handle;
    gpio_set_intr_type(btn->io_num, GPIO_INTR_DISABLE);
    gpio_isr_handler_remove(btn->io_num);

    button_free_tmr(&btn->tap_rls_cb.tmr);
    button_free_tmr(&btn->tap_psh_cb.tmr);
    button_free_tmr(&btn->tap_short_cb.tmr);
    button_free_tmr(&btn->press_serial_cb.tmr);

    button_cb_t *pcb = btn->cb_head;
    while (pcb != NULL) {
        button_cb_t *cb_next = pcb->next_cb;
        button_free_tmr(&pcb->tmr);
        free(pcb);
        pcb = cb_next;
    }
    free(btn);
    return ESP_OK;
}

button_handle_t iot_button_create(gpio_num_t gpio_num, button_active_t active_level)
{
    #if USE_ESP_TIMER
    esp_rom_printf("use esp timer !!!\n");
    esp_timer_init();
    #endif

    IOT_CHECK(TAG, gpio_num < GPIO_NUM_MAX, NULL);
    button_dev_t* btn = (button_dev_t*) calloc(1, sizeof(button_dev_t));
    POINT_ASSERT(TAG, btn, NULL);
    btn->active_level = active_level;
    btn->io_num = gpio_num;
    btn->state = BUTTON_STATE_IDLE;
    btn->tap_rls_cb.arg = NULL;
    btn->tap_rls_cb.cb = NULL;
    btn->tap_rls_cb.interval = BUTTON_GLITCH_FILTER_TIME_MS / portTICK_PERIOD_MS;
    btn->tap_rls_cb.pbtn = btn;
    #if !USE_ESP_TIMER
    btn->tap_rls_cb.tmr = xTimerCreate("btn_rls_tmr", btn->tap_rls_cb.interval, pdFALSE,
            &btn->tap_rls_cb, button_tap_rls_cb);
    #else
    esp_timer_create_args_t tmr_param_rls;
    tmr_param_rls.arg = &btn->tap_rls_cb;
    tmr_param_rls.callback = button_tap_rls_cb;
    tmr_param_rls.dispatch_method = ESP_TIMER_TASK;
    tmr_param_rls.name = "btn_rls_tmr";
    esp_timer_create(&tmr_param_rls, &btn->tap_rls_cb.tmr);
    #endif

    btn->tap_psh_cb.arg = NULL;
    btn->tap_psh_cb.cb = NULL;
    btn->tap_psh_cb.interval = BUTTON_GLITCH_FILTER_TIME_MS / portTICK_PERIOD_MS;
    btn->tap_psh_cb.pbtn = btn;
    #if !USE_ESP_TIMER
    btn->tap_psh_cb.tmr = xTimerCreate("btn_psh_tmr", btn->tap_psh_cb.interval, pdFALSE,
            &btn->tap_psh_cb, button_tap_psh_cb);
    #else
    esp_timer_create_args_t tmr_param_psh;
    tmr_param_psh.arg = &btn->tap_psh_cb;
    tmr_param_psh.callback = button_tap_psh_cb;
    tmr_param_psh.dispatch_method = ESP_TIMER_TASK;
    tmr_param_psh.name = "btn_psh_tmr";
    esp_timer_create(&tmr_param_psh, &btn->tap_psh_cb.tmr);
    #endif
    gpio_install_isr_service(0);
    gpio_config_t gpio_conf;
    gpio_conf.intr_type = GPIO_INTR_ANYEDGE;
    gpio_conf.mode = GPIO_MODE_INPUT;
    gpio_conf.pin_bit_mask = (1ULL << gpio_num);
    gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&gpio_conf);
    gpio_isr_handler_add(gpio_num, button_gpio_isr_handler, btn);
    return (button_handle_t) btn;
}

esp_err_t iot_button_rm_cb(button_handle_t btn_handle, button_cb_type_t type)
{
    button_dev_t* btn = (button_dev_t*) btn_handle;
    button_cb_t* btn_cb = NULL;
    if (type == BUTTON_CB_PUSH) {
        btn_cb = &btn->tap_psh_cb;
    } else if (type == BUTTON_CB_RELEASE) {
        btn_cb = &btn->tap_rls_cb;
    } else if (type == BUTTON_CB_TAP) {
        btn_cb = &btn->tap_short_cb;
    } else if (type == BUTTON_CB_SERIAL) {
        btn_cb = &btn->press_serial_cb;
    }
    btn_cb->cb = NULL;
    btn_cb->arg = NULL;
    btn_cb->pbtn = btn;
    button_free_tmr(&btn_cb->tmr);
    return ESP_OK;
}

esp_err_t iot_button_set_serial_cb(button_handle_t btn_handle, uint32_t start_after_sec, TickType_t interval_tick, button_cb cb, void* arg)
{
    button_dev_t* btn = (button_dev_t*) btn_handle;
    btn->serial_thres_sec = start_after_sec;

    if (btn->press_serial_cb.tmr == NULL) {
        #if !USE_ESP_TIMER
        btn->press_serial_cb.tmr = xTimerCreate("btn_serial_tmr", btn->serial_thres_sec*1000 / portTICK_PERIOD_MS,
                            pdFALSE, btn, button_press_serial_cb);
        #else
        esp_timer_create_args_t tmr_param_ser;
        tmr_param_ser.arg = btn;
        tmr_param_ser.callback = button_press_serial_cb;
        tmr_param_ser.dispatch_method = ESP_TIMER_TASK;
        tmr_param_ser.name = "btn_serial_tmr";
        esp_timer_create(&tmr_param_ser, &btn->press_serial_cb.tmr);
        #endif
    }
    btn->press_serial_cb.arg = arg;
    btn->press_serial_cb.cb = cb;
    btn->press_serial_cb.interval = interval_tick;
    btn->press_serial_cb.pbtn = btn;
    #if !USE_ESP_TIMER
    xTimerChangePeriod(btn->press_serial_cb.tmr, btn->serial_thres_sec*1000 / portTICK_PERIOD_MS, portMAX_DELAY);
    #endif
    return ESP_OK;
}

esp_err_t iot_button_set_evt_cb(button_handle_t btn_handle, button_cb_type_t type, button_cb cb, void* arg)
{
    POINT_ASSERT(TAG, btn_handle, ESP_ERR_INVALID_ARG);
    button_dev_t* btn = (button_dev_t*) btn_handle;
    if (type == BUTTON_CB_PUSH) {
        btn->tap_psh_cb.arg = arg;
        btn->tap_psh_cb.cb = cb;
        btn->tap_psh_cb.interval = BUTTON_GLITCH_FILTER_TIME_MS / portTICK_PERIOD_MS;
        btn->tap_psh_cb.pbtn = btn;
        #if !USE_ESP_TIMER
        xTimerChangePeriod(btn->tap_psh_cb.tmr, btn->tap_psh_cb.interval, portMAX_DELAY);
        #endif
    } else if (type == BUTTON_CB_RELEASE) {
        btn->tap_rls_cb.arg = arg;
        btn->tap_rls_cb.cb = cb;
        btn->tap_rls_cb.interval = BUTTON_GLITCH_FILTER_TIME_MS / portTICK_PERIOD_MS;
        btn->tap_rls_cb.pbtn = btn;
        #if !USE_ESP_TIMER
        xTimerChangePeriod(btn->tap_rls_cb.tmr, btn->tap_psh_cb.interval, portMAX_DELAY);
        #endif
    } else if (type == BUTTON_CB_TAP) {
        btn->tap_short_cb.arg = arg;
        btn->tap_short_cb.cb = cb;
        btn->tap_short_cb.interval = BUTTON_GLITCH_FILTER_TIME_MS / portTICK_PERIOD_MS;
        btn->tap_short_cb.pbtn = btn;
    } else if (type == BUTTON_CB_SERIAL) {
        iot_button_set_serial_cb(btn_handle, 1, 1000 / portTICK_PERIOD_MS, cb, arg);
    }
    return ESP_OK;
}

esp_err_t iot_button_add_custom_cb(button_handle_t btn_handle, uint32_t press_sec, button_cb cb, void* arg)
{
    POINT_ASSERT(TAG, btn_handle, ESP_ERR_INVALID_ARG);
    IOT_CHECK(TAG, press_sec != 0, ESP_ERR_INVALID_ARG);
    button_dev_t* btn = (button_dev_t*) btn_handle;
    button_cb_t* cb_new = (button_cb_t*) calloc(1, sizeof(button_cb_t));
    POINT_ASSERT(TAG, cb_new, ESP_FAIL);
    cb_new->arg = arg;
    cb_new->cb = cb;
    cb_new->interval = press_sec * 1000 / portTICK_PERIOD_MS;
    cb_new->pbtn = btn;
    #if !USE_ESP_TIMER
    cb_new->tmr = xTimerCreate("btn_press_tmr", cb_new->interval, pdFALSE, cb_new, button_press_cb);
    #else
    esp_timer_create_args_t tmr_param_cus;
    tmr_param_cus.arg = cb_new;
    tmr_param_cus.callback = button_press_cb;
    tmr_param_cus.dispatch_method = ESP_TIMER_TASK;
    tmr_param_cus.name = "btn_press_custom_tmr";
    esp_timer_create(&tmr_param_cus, &cb_new->tmr);
    #endif
    cb_new->next_cb = btn->cb_head;
    btn->cb_head = cb_new;
    return ESP_OK;
}
