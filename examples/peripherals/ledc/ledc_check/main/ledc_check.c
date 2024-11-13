/* LEDC (LED Controller) basic checks example

   This code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <math.h>
#include "driver/ledc.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "esp_clk_tree.h"

#define CHECK_SET       0 // 1
#if CHECK_SET != 0
#warning Add to the esp-idf//components//esp_driver_ledc//include//driver//ledc.h
uint32_t ledc_calculate_divisor(uint32_t src_clk_freq, int freq_hz, uint32_t precision);
#endif

#define CHECK_ALL       0 // 1 // Set CHECK_ALL to 1 for test all frequencies from 1Hz to 40MHz 
#define FDIV            1 // 2 // 5 // Test not all frequencies, but more frequencies
#define DELAY_ms        500 // 3000

// #undef ESP_ERROR_CHECK
// #define ESP_ERROR_CHECK(err) do {if (err != ESP_OK) {printf("freq=%lu, err=%d, LINE=%d\n", freq, err, __LINE__);} } while (0);

#define LEDC_OUTPUT_IO          (26) // Define the output GPIO
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_HIGH_SPEED_MODE // LEDC_LOW_SPEED_MODE //
#define LEDC_CHANNEL            LEDC_CHANNEL_0

#define LEDC_DUTY               ((1UL << timer.duty_resolution) / 2) // 50%
/* Warning:
 * For ESP32, ESP32S2, ESP32S3, ESP32C3, ESP32C2, ESP32C6, ESP32H2, ESP32P4 targets,
 * when LEDC_DUTY_RES selects the maximum duty resolution (i.e. value equal to SOC_LEDC_TIMER_BIT_WIDTH),
 * 100% duty cycle is not reachable (duty cannot be set to (2 ** SOC_LEDC_TIMER_BIT_WIDTH)).
 */

#define FREQ_PERCENT            (10)
#define DUTY_PERCENT            (10)

ledc_timer_config_t timer;
ledc_channel_config_t channel;

static void check(uint32_t freq)
{
    printf("freq=%lu \t", freq);

    // Set the LEDC peripheral configuration
    uint32_t src_clk_freq;
    ESP_ERROR_CHECK(esp_clk_tree_src_get_freq_hz(LEDC_USE_APB_CLK, ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &src_clk_freq));
    // Prepare and then apply the LEDC PWM timer configuration
    timer = (ledc_timer_config_t) {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = ledc_find_suitable_duty_resolution(src_clk_freq, freq),
        .freq_hz          = freq,
        .clk_cfg          = LEDC_USE_APB_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer));

    // Prepare and then apply the LEDC PWM channel configuration
    channel = (ledc_channel_config_t) {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = LEDC_DUTY,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&channel));

#if CHECK_SET
    uint32_t divider = ledc_calculate_divisor(src_clk_freq, timer.freq_hz, 0x1 << timer.duty_resolution);
    ESP_ERROR_CHECK(ledc_timer_set(timer.speed_mode, timer.timer_num, divider, timer.duty_resolution, timer.clk_cfg));

    // Set duty
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
#endif

    vTaskDelay((2000 / freq) / portTICK_PERIOD_MS);

    uint32_t get_freq = ledc_get_freq(LEDC_MODE, LEDC_CHANNEL);
    float freq_percent = 100.0 * fabs((float)timer.freq_hz - get_freq) / timer.freq_hz;
    uint32_t get_duty = ledc_get_duty(LEDC_MODE, LEDC_CHANNEL);
    float duty_percent = 100.0 * fabs((float)channel.duty - get_duty) / channel.duty;

    //if ((timer.freq_hz != get_freq) || (channel.duty != get_duty)) {
    if ((freq_percent >= FREQ_PERCENT) || (duty_percent >= DUTY_PERCENT)) {
        printf("%6.2f%% timer.freq_hz=%lu, get_freq=%lu, \t\t %6.2f%% channel.duty=%lu, get_duty=%lu \t\t timer.duty_resolution=%u\n",
               freq_percent, timer.freq_hz, get_freq,
               duty_percent, channel.duty, get_duty,
               timer.duty_resolution);
    } else {
        printf("\n");
    }
}

void app_main(void) 
{
    uint32_t freq;
    for (freq = 1; freq <= 40000000; freq += 1) {
#if CHECK_ALL != 1
        if (freq > 10000000) {
            freq += 10000000 / FDIV - 1;
        } else if (freq > 1000000) {
            freq += 1000000 / FDIV - 1;
        } else if (freq > 100000) {
            freq += 100000 / FDIV - 1;
        } else if (freq > 10000) {
            freq += 10000 / FDIV - 1;
        } else if (freq > 1000) {
            freq += 1000 / FDIV - 1;
        } else if (freq > 100) {
            freq += 100 / FDIV - 1;
        } else if (freq > 10) {
            freq += 10 / FDIV - 1;
        }
#endif

        check(freq);

        vTaskDelay(DELAY_ms / portTICK_PERIOD_MS);
    }
    printf("END.\n");
}
