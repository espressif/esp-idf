/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gptimer.h"
#include "driver/gpio.h"
#include "driver/gpio_etm.h"
#include "esp_log.h"

static const char *TAG = "example";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your board spec ////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define HC_SR04_TRIG_GPIO  0
#define HC_SR04_ECHO_GPIO  1

#define EXAMPLE_GPTIMER_RESOLUTION_HZ  1000000 // 1MHz, 1 tick = 1us

/**
 * @brief User defined context, to be passed to GPIO ISR callback function.
 */
typedef struct {
    gptimer_handle_t gptimer;
    TaskHandle_t task_to_notify;
    gpio_num_t echo_gpio;
} gpio_callback_user_data_t;

/**
 * @brief GPIO ISR callback function, called when there's any edge detected on the GPIO.
 */
static void hc_sr04_echo_callback(void *user_data)
{
    static uint64_t cap_val_end_of_sample = 0;
    static uint64_t cap_val_begin_of_sample = 0;
    gpio_callback_user_data_t *callback_user_data = (gpio_callback_user_data_t *)user_data;
    gpio_num_t echo_gpio = callback_user_data->echo_gpio;
    TaskHandle_t task_to_notify = callback_user_data->task_to_notify;
    gptimer_handle_t gptimer = callback_user_data->gptimer;

    if (gpio_get_level(echo_gpio) == 1) {
        // store the timestamp when pos edge is detected
        gptimer_get_captured_count(gptimer, &cap_val_begin_of_sample);
        cap_val_end_of_sample = cap_val_begin_of_sample;
    } else {
        gptimer_get_captured_count(gptimer, &cap_val_end_of_sample);
        uint32_t tof_ticks = (uint32_t)(cap_val_end_of_sample - cap_val_begin_of_sample);

        // notify the task to calculate the distance
        xTaskNotifyFromISR(task_to_notify, tof_ticks, eSetValueWithOverwrite, NULL);
    }
}

/**
 * @brief generate single pulse on Trig pin to start a new sample
 */
static void gen_trig_output(void)
{
    gpio_set_level(HC_SR04_TRIG_GPIO, 1); // set high
    esp_rom_delay_us(10);
    gpio_set_level(HC_SR04_TRIG_GPIO, 0); // set low
}

void app_main(void)
{
    ESP_LOGI(TAG, "Configure trig gpio");
    gpio_config_t trig_io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << HC_SR04_TRIG_GPIO,
    };
    ESP_ERROR_CHECK(gpio_config(&trig_io_conf));
    // drive low by default
    ESP_ERROR_CHECK(gpio_set_level(HC_SR04_TRIG_GPIO, 0));

    ESP_LOGI(TAG, "Configure echo gpio");
    gpio_config_t echo_io_conf = {
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_ANYEDGE, // capture signal on both edge
        .pull_up_en = true, // pull up internally
        .pin_bit_mask = 1ULL << HC_SR04_ECHO_GPIO,
    };
    ESP_ERROR_CHECK(gpio_config(&echo_io_conf));

    ESP_LOGI(TAG, "Create etm event handle for echo gpio");
    esp_etm_event_handle_t gpio_event = NULL;
    gpio_etm_event_config_t gpio_event_config = {
        .edge = GPIO_ETM_EVENT_EDGE_ANY, // capture signal on both edge
    };
    ESP_ERROR_CHECK(gpio_new_etm_event(&gpio_event_config, &gpio_event));
    ESP_ERROR_CHECK(gpio_etm_event_bind_gpio(gpio_event, HC_SR04_ECHO_GPIO));

    ESP_LOGI(TAG, "Create gptimer handle");
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = EXAMPLE_GPTIMER_RESOLUTION_HZ,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    ESP_LOGI(TAG, "Get gptimer etm task handle (capture)");
    esp_etm_task_handle_t gptimer_capture_task = NULL;
    gptimer_etm_task_config_t gptimer_etm_task_conf = {
        .task_type = GPTIMER_ETM_TASK_CAPTURE,
    };
    ESP_ERROR_CHECK(gptimer_new_etm_task(gptimer, &gptimer_etm_task_conf, &gptimer_capture_task));

    ESP_LOGI(TAG, "Create ETM channel then connect gpio event and gptimer task");
    esp_etm_channel_handle_t etm_chan = NULL;
    esp_etm_channel_config_t etm_chan_config = {};
    ESP_ERROR_CHECK(esp_etm_new_channel(&etm_chan_config, &etm_chan));
    // GPIO any edge ==> ETM channel ==> GPTimer capture task
    ESP_ERROR_CHECK(esp_etm_channel_connect(etm_chan, gpio_event, gptimer_capture_task));

    ESP_LOGI(TAG, "Install GPIO edge interrupt");
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    gpio_callback_user_data_t callback_user_data = {
        .gptimer = gptimer,
        .echo_gpio = HC_SR04_ECHO_GPIO,
        .task_to_notify = xTaskGetCurrentTaskHandle(),
    };
    ESP_ERROR_CHECK(gpio_isr_handler_add(HC_SR04_ECHO_GPIO, hc_sr04_echo_callback, &callback_user_data));

    ESP_LOGI(TAG, "Enable etm channel and gptimer");
    ESP_ERROR_CHECK(esp_etm_channel_enable(etm_chan));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    // Print the ETM channel usage
    ESP_ERROR_CHECK(esp_etm_dump(stdout));

    ESP_LOGI(TAG, "Start gptimer");
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    uint32_t tof_ticks;
    while (1) {
        // trigger the sensor to start a new sample
        gen_trig_output();
        // wait for echo done signal
        if (xTaskNotifyWait(0x00, ULONG_MAX, &tof_ticks, pdMS_TO_TICKS(1000)) == pdTRUE) {
            if (tof_ticks > 35000) {
                // out of range
                continue;
            }
            // convert the pulse width into measure distance
            float distance = (float) tof_ticks / 58.0f;
            ESP_LOGI(TAG, "Measured distance: %.2fcm", distance);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
