/* MCPWM capture example: HC-SR04

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
 * This example will show you how to use capture function to read HC-SR04 sonar sensor.
 *
 * HC_SR04_SAMPLE_PERIOD_MS should be at least 50ms
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_check.h"
#include "soc/rtc.h"
#include "driver/mcpwm.h"

const static char *TAG = "hc-sr04";

#define HC_SR04_SAMPLE_PERIOD_MS    100
_Static_assert(HC_SR04_SAMPLE_PERIOD_MS > 50, "Sample period too short!");
#define HC_SR04_PIN_ECHO    GPIO_NUM_18
#define HC_SR04_PIN_TRIG    GPIO_NUM_19

#define TRIGGER_THREAD_STACK_SIZE 512
#define TRIGGER_THREAD_PRIORITY 5

typedef struct {
    uint32_t capture_signal;
    mcpwm_capture_signal_t sel_cap_signal;
} capture;

static uint32_t cap_val_begin_of_sample = 0;
static uint32_t cap_val_end_of_sample = 0;

static xQueueHandle cap_queue;

/**
 * @brief generate single pulse on Trig pin to activate a new sample
 */
static void gen_trig_output(void *arg) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (true) {
        vTaskDelayUntil(&xLastWakeTime, HC_SR04_SAMPLE_PERIOD_MS / portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(gpio_set_level(HC_SR04_PIN_TRIG, 1)); // set high
        esp_rom_delay_us(10);
        ESP_ERROR_CHECK(gpio_set_level(HC_SR04_PIN_TRIG, 0)); // set low
    }
}

/**
 * @brief this is an ISR callback, we take action according to the captured edge
 */
static bool sr04_echo_isr_handler(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_sig, const cap_event_data_t *edata,
                                  void *arg) {
    //calculate the interval in the ISR,
    //so that the interval will be always correct even when cap_queue is not handled in time and overflow.
    BaseType_t high_task_wakeup = pdFALSE;
    if (edata->cap_edge == MCPWM_POS_EDGE) {
        // store the timestamp when pos edge is detected
        cap_val_begin_of_sample = edata->cap_value;
        cap_val_end_of_sample = cap_val_begin_of_sample;
    } else {
        cap_val_end_of_sample = edata->cap_value;
        // following formula refers to: https://www.elecrow.com/download/HC_SR04%20Datasheet.pdf
        uint32_t pulse_count = cap_val_end_of_sample - cap_val_begin_of_sample;
        // send measurement back though queue
        xQueueSendFromISR(cap_queue, &pulse_count, &high_task_wakeup);
    }
    return high_task_wakeup == pdTRUE;
}

void app_main(void) {
    ESP_LOGI(TAG, "HC-SR04 example based on capture function from MCPWM");

    // the queue where we read data
    cap_queue = xQueueCreate(1, sizeof(uint32_t));
    if (cap_queue == NULL) {
        ESP_LOGE(TAG, "failed to alloc cap_queue");
        return;
    }

    /* configure Echo pin */
    // set CAP_0 on GPIO
    ESP_ERROR_CHECK(mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM_CAP_0, HC_SR04_PIN_ECHO));
    // enable pull down CAP0, to reduce noise
    ESP_ERROR_CHECK(gpio_pulldown_en(HC_SR04_PIN_ECHO));
    // enable both edge capture on CAP0
    mcpwm_capture_config_t conf = {
        .cap_edge = MCPWM_BOTH_EDGE,
        .cap_prescale = 1,
        .capture_cb = sr04_echo_isr_handler,
        .user_data = NULL
    };
    ESP_ERROR_CHECK(mcpwm_capture_enable_channel(MCPWM_UNIT_0, MCPWM_SELECT_CAP0, &conf));
    ESP_LOGI(TAG, "Echo pin configured");

    /* configure Trig pin */
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pin_bit_mask = BIT64(HC_SR04_PIN_TRIG),
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_ERROR_CHECK(gpio_set_level(HC_SR04_PIN_TRIG, 0)); // drive low by default
    ESP_LOGI(TAG, "Trig pin configured");

    // start generating trig signal
    xTaskCreate(gen_trig_output, "gen_trig_output", TRIGGER_THREAD_STACK_SIZE, NULL, TRIGGER_THREAD_PRIORITY, NULL);
    ESP_LOGI(TAG, "trig task started");
    // forever loop
    while (true) {
        uint32_t pulse_count;
        // block and wait for new measurement
        xQueueReceive(cap_queue, &pulse_count, portMAX_DELAY);
        uint32_t pulse_width_us = pulse_count * (1000000.0 / rtc_clk_apb_freq_get());
        // following formula is based on: https://www.elecrow.com/download/HC_SR04%20Datasheet.pdf
        if (pulse_width_us > 35000) {
            // out of range
            continue;
        }
        float distance = (float) pulse_width_us / 58;
        ESP_LOGI(TAG, "Pulse width: %uus, Measured distance: %.2fcm", pulse_width_us, distance);
    }
}
