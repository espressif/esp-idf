/* Touch Pad Interrupt Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "driver/touch_pad.h"
#include "soc/rtc_periph.h"
#include "soc/sens_periph.h"

static const char *TAG = "Touch pad";

static QueueHandle_t que_touch = NULL;
typedef struct touch_msg {
    touch_pad_intr_mask_t intr_mask;
    uint32_t pad_num;
    uint32_t pad_status;
    uint32_t pad_val;
} touch_event_t;

#define TOUCH_BUTTON_NUM    4
#define TOUCH_BUTTON_WATERPROOF_ENABLE 1
#define TOUCH_BUTTON_DENOISE_ENABLE    1

static const touch_pad_t button[TOUCH_BUTTON_NUM] = {
    TOUCH_PAD_NUM7,     // 'SELECT' button.
    TOUCH_PAD_NUM9,     // 'MENU' button.
    TOUCH_PAD_NUM11,    // 'BACK' button.
    TOUCH_PAD_NUM13,    // Guard ring for waterproof design.
    // if this pad be touched, other pads no response.
};

/*
 * Touch threshold. The threshold determines the sensitivity of the touch.
 * This threshold is derived by testing changes in readings from different touch channels.
 * If (raw_data - baseline) > baseline * threshold, the pad be actived.
 * If (raw_data - baseline) < baseline * threshold, the pad be inactived.
 */
static const float button_threshold[TOUCH_BUTTON_NUM] = {
    0.2, // 20%.
    0.2, // 20%.
    0.2, // 20%.
    0.1, // 10%.
};

/*
  Handle an interrupt triggered when a pad is touched.
  Recognize what pad has been touched and save it in a table.
 */
static void touchsensor_interrupt_cb(void *arg)
{
    int task_awoken = pdFALSE;
    touch_event_t evt;

    evt.intr_mask = touch_pad_read_intr_status_mask();
    evt.pad_status = touch_pad_get_status();
    evt.pad_num = touch_pad_get_current_meas_channel();

    if (evt.intr_mask & TOUCH_PAD_INTR_MASK_DONE) {
        touch_pad_filter_read_baseline(evt.pad_num, &evt.pad_val);
    }
    xQueueSendFromISR(que_touch, &evt, &task_awoken);
    if (task_awoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static void tp_example_set_thresholds(void)
{
    uint32_t touch_value;
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        //read baseline value
        touch_pad_filter_read_baseline(button[i], &touch_value);
        //set interrupt threshold.
        touch_pad_set_thresh(button[i], touch_value * button_threshold[i]);
        ESP_LOGI(TAG, "test init: touch pad [%d] base %d, thresh %d", \
                 button[i], touch_value, (uint32_t)(touch_value * button_threshold[i]));
    }
}

static void touchsensor_filter_set(touch_filter_mode_t mode)
{
    /* Filter function */
    touch_filter_config_t filter_info = {
        .mode = mode,           // Test jitter and filter 1/4.
        .debounce_cnt = 1,      // 1 time count.
        .hysteresis_thr = 3,    // 3%
        .noise_thr = 0,         // 50%
        .noise_neg_thr = 0,     // 50%
        .neg_noise_limit = 10,  // 10 time count.
        .jitter_step = 4,       // use for jitter mode.
    };
    touch_pad_filter_set_config(&filter_info);
    touch_pad_filter_enable();
    touch_pad_filter_reset_baseline(TOUCH_PAD_MAX);
    ESP_LOGI(TAG, "touch pad filter init");
}

static void tp_example_read_task(void *pvParameter)
{
    touch_event_t evt = {0};
    static uint8_t guard_mode_flag = 0;
    /* Wait touch sensor init done */
    vTaskDelay(100 / portTICK_RATE_MS);
    tp_example_set_thresholds();

    while (1) {
        int ret = xQueueReceive(que_touch, &evt, (portTickType)portMAX_DELAY);
        if (ret != pdTRUE) {
            continue;
        }
        if (evt.intr_mask & TOUCH_PAD_INTR_MASK_ACTIVE) {
            /* if guard pad be touched, other pads no response. */
            if (evt.pad_num == button[3]) {
                guard_mode_flag = 1;
                ESP_LOGW(TAG, "TouchSensor [%d] be actived, enter guard mode", evt.pad_num);
            } else {
                if (guard_mode_flag == 0) {
                    ESP_LOGI(TAG, "TouchSensor [%d] be actived, status mask 0x%x", evt.pad_num, evt.pad_status);
                } else {
                    ESP_LOGW(TAG, "In guard mode. No response");
                }
            }
        }
        if (evt.intr_mask & TOUCH_PAD_INTR_MASK_INACTIVE) {
            /* if guard pad be touched, other pads no response. */
            if (evt.pad_num == button[3]) {
                guard_mode_flag = 0;
                ESP_LOGW(TAG, "TouchSensor [%d] be actived, exit guard mode", evt.pad_num);
            } else {
                if (guard_mode_flag == 0) {
                    ESP_LOGI(TAG, "TouchSensor [%d] be inactived, status mask 0x%x", evt.pad_num, evt.pad_status);
                }
            }
        }
        if (evt.intr_mask & TOUCH_PAD_INTR_MASK_DONE) {
            ESP_LOGI(TAG, "TouchSensor [%d] measure done, raw data %d", evt.pad_num, evt.pad_val);
        }
    }
}

void app_main(void)
{
    if (que_touch == NULL) {
        que_touch = xQueueCreate(TOUCH_BUTTON_NUM, sizeof(touch_event_t));
    }
    // Initialize touch pad peripheral, it will start a timer to run a filter
    ESP_LOGI(TAG, "Initializing touch pad");
    /* Initialize touch pad peripheral. */
    touch_pad_init();
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        touch_pad_config(button[i]);
    }
#if 0
    /* If you want change the touch sensor default setting, please write here(after initialize). There are examples: */
    touch_pad_set_meas_time(TOUCH_PAD_SLEEP_CYCLE_DEFAULT, TOUCH_PAD_SLEEP_CYCLE_DEFAULT);
    touch_pad_set_voltage(TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD, TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD);
    touch_pad_set_inactive_connect(TOUCH_PAD_INACTIVE_CONNECT_DEFAULT);
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        touch_pad_set_cnt_mode(i, TOUCH_PAD_SLOPE_DEFAULT, TOUCH_PAD_TIE_OPT_DEFAULT);
    }
#endif
#if TOUCH_BUTTON_DENOISE_ENABLE
    /* Denoise setting at TouchSensor 0. */
    touch_pad_denoise_t denoise = {
        /* The bits to be cancelled are determined according to the noise level. */
        .grade = TOUCH_PAD_DENOISE_BIT4,
        .cap_level = TOUCH_PAD_DENOISE_CAP_L7,
    };
    touch_pad_denoise_set_config(&denoise);
    touch_pad_denoise_enable();
    ESP_LOGI(TAG, "Denoise function init");
#endif

#if TOUCH_BUTTON_WATERPROOF_ENABLE
    /* Waterproof function */
    touch_pad_waterproof_t waterproof = {
        .guard_ring_pad = button[3],   // If no ring pad, set 0;
        /* It depends on the number of the parasitic capacitance of the shield pad. */
        .shield_driver = TOUCH_PAD_SHIELD_DRV_L0,   //40pf
    };
    touch_pad_waterproof_set_config(&waterproof);
    touch_pad_waterproof_enable();
    ESP_LOGI(TAG, "touch pad waterproof init");
#endif

    /* Filter setting */
    touchsensor_filter_set(TOUCH_PAD_FILTER_IIR_8);
    /* Register touch interrupt ISR, enable intr type. */
    touch_pad_isr_register(touchsensor_interrupt_cb, NULL, TOUCH_PAD_INTR_MASK_ALL);
    touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE);
    // touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_DONE); // Use for debug

    /* Enable touch sensor clock. Work mode is "timer trigger". */
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    touch_pad_fsm_start();

    // Start a task to show what pads have been touched
    xTaskCreate(&tp_example_read_task, "touch_pad_read_task", 2048, NULL, 5, NULL);
}
