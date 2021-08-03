/* MCPWM sync example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
 * This example will show you how to use capture function to read HC-SR04 sonar sensor.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_rom_gpio.h"
#include "soc/mcpwm_periph.h"
#include "hal/gpio_hal.h"
#include "esp_log.h"
#include "esp_check.h"
#include "soc/rtc.h"
#include "driver/mcpwm.h"

const static char *TAG = "sync_example";

#define TARGET_MCPWM_UNIT MCPWM_UNIT_0
#define TIMER0_OUTPUT_GPIO GPIO_NUM_16
#define TIMER1_OUTPUT_GPIO GPIO_NUM_17
#define TIMER2_OUTPUT_GPIO GPIO_NUM_18
#define SIMU_GPIO_SYNC_SOURCE_GPIO GPIO_NUM_21
#define SIMU_GPIO_SYNC_SIMULATE_GPIO GPIO_NUM_19

void app_main(void) {
    ESP_LOGI(TAG, "MCPWM sync example");

    // init MCPWM: 10% duty cycle on all three timers in MCPWM unit 0 (currently not synchronous)
    mcpwm_config_t pwm_config = {
        .frequency = 1000,
        .cmpr_a = 10,
        .cmpr_b = 0,
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0,
    };
    ESP_ERROR_CHECK(mcpwm_init(TARGET_MCPWM_UNIT, MCPWM_TIMER_0, &pwm_config));
    ESP_ERROR_CHECK(mcpwm_init(TARGET_MCPWM_UNIT, MCPWM_TIMER_1, &pwm_config));
    ESP_ERROR_CHECK(mcpwm_init(TARGET_MCPWM_UNIT, MCPWM_TIMER_2, &pwm_config));

    // bind output to GPIO
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM0A, TIMER0_OUTPUT_GPIO));
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM1A, TIMER1_OUTPUT_GPIO));
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM2A, TIMER2_OUTPUT_GPIO));
    ESP_LOGI(TAG, "PWM started, not synchronized");

    vTaskDelay(pdMS_TO_TICKS(1000));
    // temporarily disable GPIO output, by binding to GenBs which have 0 output
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM0B, TIMER0_OUTPUT_GPIO));
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM1B, TIMER1_OUTPUT_GPIO));
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM2B, TIMER2_OUTPUT_GPIO));
    vTaskDelay(pdMS_TO_TICKS(2000));

    ESP_LOGI(TAG, "Sync timers with GPIO approach");
    // first configure sync source
    mcpwm_sync_config_t sync_conf = {
        .sync_sig = MCPWM_SELECT_GPIO_SYNC0,
        .timer_val = 0,
        .count_direction = MCPWM_TIMER_DIRECTION_UP,
    };
    ESP_ERROR_CHECK(mcpwm_sync_configure(TARGET_MCPWM_UNIT, MCPWM_TIMER_0, &sync_conf));
    ESP_ERROR_CHECK(mcpwm_sync_configure(TARGET_MCPWM_UNIT, MCPWM_TIMER_1, &sync_conf));
    ESP_ERROR_CHECK(mcpwm_sync_configure(TARGET_MCPWM_UNIT, MCPWM_TIMER_2, &sync_conf));
    // then configure GPIO
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM_SYNC_0, SIMU_GPIO_SYNC_SOURCE_GPIO));
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = BIT64(SIMU_GPIO_SYNC_SIMULATE_GPIO);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_ERROR_CHECK(gpio_set_level(SIMU_GPIO_SYNC_SIMULATE_GPIO, 0));
    ESP_ERROR_CHECK(gpio_set_level(SIMU_GPIO_SYNC_SIMULATE_GPIO, 1));
    // wait for at least one TEP
    vTaskDelay(pdMS_TO_TICKS(10));
    // re-enable GPIO output, to see the result
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM0A, TIMER0_OUTPUT_GPIO));
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM1A, TIMER1_OUTPUT_GPIO));
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM2A, TIMER2_OUTPUT_GPIO));
    ESP_LOGI(TAG, "Output should already be synchronized");

    vTaskDelay(pdMS_TO_TICKS(1000));

    // stop and restart timers to mess them
    ESP_ERROR_CHECK(mcpwm_stop(TARGET_MCPWM_UNIT, MCPWM_TIMER_2));
    ESP_ERROR_CHECK(mcpwm_stop(TARGET_MCPWM_UNIT, MCPWM_TIMER_1));
    ESP_ERROR_CHECK(mcpwm_stop(TARGET_MCPWM_UNIT, MCPWM_TIMER_0));
    vTaskDelay(pdMS_TO_TICKS(2000));
    ESP_ERROR_CHECK(mcpwm_start(TARGET_MCPWM_UNIT, MCPWM_TIMER_0));
    ESP_ERROR_CHECK(mcpwm_start(TARGET_MCPWM_UNIT, MCPWM_TIMER_1));
    ESP_ERROR_CHECK(mcpwm_start(TARGET_MCPWM_UNIT, MCPWM_TIMER_2));
    ESP_LOGI(TAG, "force synchronous lost");

    vTaskDelay(pdMS_TO_TICKS(1000));
    // temporarily disable GPIO output, by binding to GenBs which have 0 output
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM0B, TIMER0_OUTPUT_GPIO));
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM1B, TIMER1_OUTPUT_GPIO));
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM2B, TIMER2_OUTPUT_GPIO));
    vTaskDelay(pdMS_TO_TICKS(2000));

#ifdef SOC_MCPWM_SWSYNC_CAN_PROPAGATE
    // use the trick that only available on esp32s3
    mcpwm_set_timer_sync_output(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_SWSYNC_SOURCE_SYNCIN);
    sync_conf.sync_sig = MCPWM_SELECT_TIMER0_SYNC;
    mcpwm_sync_configure(MCPWM_UNIT_0, MCPWM_TIMER_0, &sync_conf);
    mcpwm_sync_configure(MCPWM_UNIT_0, MCPWM_TIMER_1, &sync_conf);
    mcpwm_sync_configure(MCPWM_UNIT_0, MCPWM_TIMER_2, &sync_conf);
    // then send soft sync event to timer0
    mcpwm_timer_trigger_soft_sync(MCPWM_UNIT_0, MCPWM_TIMER_0);
    // re-enable GPIO output
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM0A, TIMER0_OUTPUT_GPIO));
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM1A, TIMER1_OUTPUT_GPIO));
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM2A, TIMER2_OUTPUT_GPIO));
    ESP_LOGI(TAG, "Output should already be synchronized on esp32s3");

    vTaskDelay(pdMS_TO_TICKS(1000));
#endif

    // temporarily disable GPIO output, by binding to GenBs which have 0 output
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM0B, TIMER0_OUTPUT_GPIO));
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM1B, TIMER1_OUTPUT_GPIO));
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM2B, TIMER2_OUTPUT_GPIO));
    vTaskDelay(pdMS_TO_TICKS(2000));
    // create phase between each timer.
    // for this case all timers has 10% of period phase between each other
    sync_conf.sync_sig = MCPWM_SELECT_GPIO_SYNC0;
    sync_conf.timer_val = 0;  // no phase applied
    mcpwm_sync_configure(MCPWM_UNIT_0, MCPWM_TIMER_0, &sync_conf);
    sync_conf.timer_val = 900;  // fill the counter with 90.0% of period will cause next pulse being delayed 10% period
    mcpwm_sync_configure(MCPWM_UNIT_0, MCPWM_TIMER_1, &sync_conf);
    sync_conf.timer_val = 800;  // fill the counter with 80.0% of period will cause next pulse being delayed 20% period
    mcpwm_sync_configure(MCPWM_UNIT_0, MCPWM_TIMER_2, &sync_conf);
    // trigger positive edge
    ESP_ERROR_CHECK(gpio_set_level(SIMU_GPIO_SYNC_SIMULATE_GPIO, 0));
    ESP_ERROR_CHECK(gpio_set_level(SIMU_GPIO_SYNC_SIMULATE_GPIO, 1));
    // wait for at least one TEP
    vTaskDelay(pdMS_TO_TICKS(10));
    // re-enable GPIO output, to see the result
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM0A, TIMER0_OUTPUT_GPIO));
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM1A, TIMER1_OUTPUT_GPIO));
    ESP_ERROR_CHECK(mcpwm_gpio_init(TARGET_MCPWM_UNIT, MCPWM2A, TIMER2_OUTPUT_GPIO));
    ESP_LOGI(TAG, "Each output pulse should be placed with 10 percents of period");

    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_ERROR_CHECK(mcpwm_stop(TARGET_MCPWM_UNIT, MCPWM_TIMER_2));
    ESP_ERROR_CHECK(mcpwm_stop(TARGET_MCPWM_UNIT, MCPWM_TIMER_1));
    ESP_ERROR_CHECK(mcpwm_stop(TARGET_MCPWM_UNIT, MCPWM_TIMER_0));
}
