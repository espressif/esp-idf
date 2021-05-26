/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 Tests for the adc device driver
*/
#include "esp_system.h"
#include "driver/adc.h"
#include "driver/rtc_io.h"
#include "driver/gpio.h"
#include "unity.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "test_utils.h"
#include "soc/adc_periph.h"

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S3,ESP32C3)
#include "driver/dac.h"

static const char *TAG = "test_adc";

#ifdef CONFIG_IDF_TARGET_ESP32
#define ADC1_TEST_WIDTH         ADC_WIDTH_BIT_12
#define ADC2_TEST_WIDTH         ADC_WIDTH_BIT_12
#elif defined CONFIG_IDF_TARGET_ESP32S2
#define ADC1_TEST_WIDTH         ADC_WIDTH_BIT_13   //ESP32S2 only support 13 bit width
#define ADC2_TEST_WIDTH         ADC_WIDTH_BIT_13   //ESP32S2 only support 13 bit width
#endif

#define ADC1_TEST_ATTEN         ADC_ATTEN_DB_11
#define ADC2_TEST_ATTEN         ADC_ATTEN_DB_11

#if CONFIG_IDF_TARGET_ESP32
#define ADC1_TEST_CHANNEL_NUM   8
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC1_TEST_CHANNEL_NUM   10
#endif
#define ADC2_TEST_CHANNEL_NUM   6

static const int adc1_ch[ADC1_TEST_CHANNEL_NUM] = {
    ADC1_CHANNEL_0,
    ADC1_CHANNEL_1,
    ADC1_CHANNEL_2,
    ADC1_CHANNEL_3,
    ADC1_CHANNEL_4,
    ADC1_CHANNEL_5,
    ADC1_CHANNEL_6,
    ADC1_CHANNEL_7,
#if CONFIG_IDF_TARGET_ESP32S2
    ADC1_CHANNEL_8,
    ADC1_CHANNEL_9,
#endif
};

static const int adc2_ch[ADC2_TEST_CHANNEL_NUM] = {
    ADC2_CHANNEL_0,
    ADC2_CHANNEL_1,
    ADC2_CHANNEL_2,
    ADC2_CHANNEL_3,
    ADC2_CHANNEL_4,
    ADC2_CHANNEL_5,
};

#define ADC_GET_IO_NUM(periph, channel) (adc_channel_io_map[periph][channel])

void adc_fake_tie_middle(adc_unit_t adc_unit, adc_channel_t channel)
{
    gpio_num_t gpio_num = 0;
    if (adc_unit & ADC_UNIT_1) {
        gpio_num = ADC_GET_IO_NUM(0, channel);
        TEST_ESP_OK(rtc_gpio_init(gpio_num));
        TEST_ESP_OK(rtc_gpio_pullup_en(gpio_num));
        TEST_ESP_OK(rtc_gpio_pulldown_en(gpio_num));
        TEST_ESP_OK(gpio_set_pull_mode(gpio_num, GPIO_PULLUP_PULLDOWN));
        TEST_ESP_OK(rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED));
    }
    if (adc_unit & ADC_UNIT_2) {
        gpio_num = ADC_GET_IO_NUM(1, channel);
        TEST_ESP_OK(rtc_gpio_init(gpio_num));
        TEST_ESP_OK(rtc_gpio_pullup_en(gpio_num));
        TEST_ESP_OK(rtc_gpio_pulldown_en(gpio_num));
        TEST_ESP_OK(gpio_set_pull_mode(gpio_num, GPIO_PULLUP_PULLDOWN));
        TEST_ESP_OK(rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED));
    }
    vTaskDelay(10 / portTICK_RATE_MS);
}

void adc_fake_tie_high(adc_unit_t adc_unit, adc_channel_t channel)
{
    gpio_num_t gpio_num = 0;
    if (adc_unit & ADC_UNIT_1) {
        gpio_num = ADC_GET_IO_NUM(0, channel);
        TEST_ESP_OK(rtc_gpio_init(gpio_num));
        TEST_ESP_OK(rtc_gpio_pullup_en(gpio_num));
        TEST_ESP_OK(rtc_gpio_pulldown_dis(gpio_num));
        TEST_ESP_OK(gpio_set_pull_mode(gpio_num, GPIO_PULLUP_ONLY));
        TEST_ESP_OK(rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_OUTPUT_ONLY));
        TEST_ESP_OK(rtc_gpio_set_level(gpio_num, 1));
    }
    if (adc_unit & ADC_UNIT_2) {
        gpio_num = ADC_GET_IO_NUM(1, channel);
        TEST_ESP_OK(rtc_gpio_init(gpio_num));
        TEST_ESP_OK(rtc_gpio_pullup_en(gpio_num));
        TEST_ESP_OK(rtc_gpio_pulldown_dis(gpio_num));
        TEST_ESP_OK(gpio_set_pull_mode(gpio_num, GPIO_PULLUP_ONLY));
        TEST_ESP_OK(rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_OUTPUT_ONLY));
        TEST_ESP_OK(rtc_gpio_set_level(gpio_num, 1));
    }
    vTaskDelay(10 / portTICK_RATE_MS);
}

void adc_fake_tie_low(adc_unit_t adc_unit, adc_channel_t channel)
{
    gpio_num_t gpio_num = 0;
    if (adc_unit & ADC_UNIT_1) {
        gpio_num = ADC_GET_IO_NUM(0, channel);
        TEST_ESP_OK(rtc_gpio_init(gpio_num));
        TEST_ESP_OK(rtc_gpio_pullup_dis(gpio_num));
        TEST_ESP_OK(rtc_gpio_pulldown_en(gpio_num));
        TEST_ESP_OK(gpio_set_pull_mode(gpio_num, GPIO_PULLDOWN_ONLY));
        TEST_ESP_OK(rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_OUTPUT_ONLY));
        TEST_ESP_OK(rtc_gpio_set_level(gpio_num, 0));
    }
    if (adc_unit & ADC_UNIT_2) {
        gpio_num = ADC_GET_IO_NUM(1, channel);
        TEST_ESP_OK(rtc_gpio_init(gpio_num));
        TEST_ESP_OK(rtc_gpio_pullup_dis(gpio_num));
        TEST_ESP_OK(rtc_gpio_pulldown_en(gpio_num));
        TEST_ESP_OK(gpio_set_pull_mode(gpio_num, GPIO_PULLDOWN_ONLY));
        TEST_ESP_OK(rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_OUTPUT_ONLY));
        TEST_ESP_OK(rtc_gpio_set_level(gpio_num, 0));
    }
    vTaskDelay(10 / portTICK_RATE_MS);
}

void adc_io_normal(adc_unit_t adc_unit, adc_channel_t channel)
{
    gpio_num_t gpio_num = 0;
    if (adc_unit & ADC_UNIT_1) {
        gpio_num = ADC_GET_IO_NUM(0, channel);
        TEST_ESP_OK(rtc_gpio_init(gpio_num));
        TEST_ESP_OK(rtc_gpio_pullup_dis(gpio_num));
        TEST_ESP_OK(rtc_gpio_pulldown_dis(gpio_num));
        TEST_ESP_OK(gpio_set_pull_mode(gpio_num, GPIO_FLOATING));
        TEST_ESP_OK(rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED));
    }
    if (adc_unit & ADC_UNIT_2) {
        gpio_num = ADC_GET_IO_NUM(1, channel);
        TEST_ESP_OK(rtc_gpio_init(gpio_num));
        TEST_ESP_OK(rtc_gpio_pullup_dis(gpio_num));
        TEST_ESP_OK(rtc_gpio_pulldown_dis(gpio_num));
        TEST_ESP_OK(gpio_set_pull_mode(gpio_num, GPIO_FLOATING));
        TEST_ESP_OK(rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED));
    }
    vTaskDelay(10 / portTICK_RATE_MS);
}

TEST_CASE("ADC1 rtc read", "[adc1]")
{
    int adc1_val[ADC1_TEST_CHANNEL_NUM] = {0};

    /* adc1 Configure */
    adc1_config_width(ADC1_TEST_WIDTH);
    ESP_LOGI(TAG, "ADC1 [CH - GPIO]:");
    for (int i = 0; i < ADC1_TEST_CHANNEL_NUM; i++) {
        TEST_ESP_OK( adc1_config_channel_atten(adc1_ch[i], ADC1_TEST_ATTEN) );
        ESP_LOGI(TAG, "[CH%d - IO%d]:", adc1_ch[i], ADC_GET_IO_NUM(0, adc1_ch[i]));
    }
    printf("ADC tie normal read: ");
    vTaskDelay(10 / portTICK_RATE_MS);

    /* adc Read */
    printf("ADC1: ");
    for (int i = 0; i < ADC1_TEST_CHANNEL_NUM; i++) {
        adc1_val[i] = adc1_get_raw((adc1_channel_t)adc1_ch[i]);
        printf("CH%d-%d ", adc1_ch[i], adc1_val[i]);
    }
    printf("\n");

    /* tie high */
    for (int i = 0; i < ADC1_TEST_CHANNEL_NUM; i++) {
        adc_fake_tie_high(ADC_UNIT_1, adc1_ch[i]);
    }
    printf("ADC tie high read: ");
    vTaskDelay(50 / portTICK_RATE_MS);
    /* adc Read */
    printf("ADC1: ");
    for (int i = 0; i < ADC1_TEST_CHANNEL_NUM; i++) {
        adc1_val[i] = adc1_get_raw((adc1_channel_t)adc1_ch[i]);
        printf("CH%d-%d ", adc1_ch[i], adc1_val[i]);
#ifdef CONFIG_IDF_TARGET_ESP32S2
        TEST_ASSERT_EQUAL( adc1_val[i], 0x1fff );
#endif
    }
    printf("\n");

    /* tie low */
    for (int i = 0; i < ADC1_TEST_CHANNEL_NUM; i++) {
        adc_fake_tie_low(ADC_UNIT_1, adc1_ch[i]);
    }
    printf("ADC tie low  read: ");
    vTaskDelay(50 / portTICK_RATE_MS);
    /* adc Read */
    printf("ADC1: ");
    for (int i = 0; i < ADC1_TEST_CHANNEL_NUM; i++) {
        adc1_val[i] = adc1_get_raw((adc1_channel_t)adc1_ch[i]);
        printf("CH%d-%d ", adc1_ch[i], adc1_val[i]);
#ifdef CONFIG_IDF_TARGET_ESP32S2
        TEST_ASSERT_INT_WITHIN( 100, 0, adc1_val[i] );
#endif
    }
    printf("\n");

    /* tie midedle */
    for (int i = 0; i < ADC1_TEST_CHANNEL_NUM; i++) {
        adc_fake_tie_middle(ADC_UNIT_1, adc1_ch[i]);
    }
    printf("ADC tie mid  read: ");
    vTaskDelay(50 / portTICK_RATE_MS);
    /* adc Read */
    printf("ADC1: ");
    for (int i = 0; i < ADC1_TEST_CHANNEL_NUM; i++) {
        adc1_val[i] = adc1_get_raw((adc1_channel_t)adc1_ch[i]);
        printf("CH%d-%d ", adc1_ch[i], adc1_val[i]);
#ifdef CONFIG_IDF_TARGET_ESP32S2
        TEST_ASSERT_NOT_EQUAL( adc1_val[i], 0x1fff );
        TEST_ASSERT_NOT_EQUAL( adc1_val[i], 0 );
#endif
    }
    printf("\n");

    for (int i = 0; i < ADC1_TEST_CHANNEL_NUM; i++) {
        adc_io_normal(ADC_UNIT_1, adc1_ch[i]);
    }
}

TEST_CASE("ADC2 rtc read", "[adc2]")
{
    int adc2_val[ADC2_TEST_CHANNEL_NUM] = {0};

    /* adc2 Configure */
    ESP_LOGI(TAG, "ADC2 [CH - GPIO]:");
    for (int i = 0; i < ADC2_TEST_CHANNEL_NUM; i++) {
        TEST_ESP_OK( adc2_config_channel_atten(adc2_ch[i], ADC2_TEST_ATTEN) );
        ESP_LOGI(TAG, "[CH%d - IO%d]:", adc2_ch[i], ADC_GET_IO_NUM(1, adc2_ch[i]));
    }
    printf("ADC float read: ");
    vTaskDelay(10 / portTICK_RATE_MS);

    /* adc Read */
    printf("ADC2: ");
    for (int i = 0; i < ADC2_TEST_CHANNEL_NUM; i++) {
        TEST_ESP_OK( adc2_get_raw((adc2_channel_t)adc2_ch[i], ADC2_TEST_WIDTH, &adc2_val[i]) );
        printf("CH%d-%d ", adc2_ch[i], adc2_val[i]);
    }
    printf("\n");

    /* tie high */
    for (int i = 0; i < ADC2_TEST_CHANNEL_NUM; i++) {
        adc_fake_tie_high(ADC_UNIT_2, adc2_ch[i]);
    }
    printf("ADC tie high read: ");
    vTaskDelay(10 / portTICK_RATE_MS);
    /* adc Read */
    printf("ADC2: ");
    for (int i = 0; i < ADC2_TEST_CHANNEL_NUM; i++) {
        TEST_ESP_OK( adc2_get_raw((adc2_channel_t)adc2_ch[i], ADC2_TEST_WIDTH, &adc2_val[i]) );
        printf("CH%d-%d ", adc2_ch[i], adc2_val[i]);
#ifdef CONFIG_IDF_TARGET_ESP32S2
        TEST_ASSERT_EQUAL( adc2_val[i], 0x1fff );
#endif
    }
    printf("\n");

    /* tie low */
    for (int i = 0; i < ADC2_TEST_CHANNEL_NUM; i++) {
        adc_fake_tie_low(ADC_UNIT_2, adc2_ch[i]);
    }
    printf("ADC tie low read: ");
    vTaskDelay(10 / portTICK_RATE_MS);
    /* adc Read */
    printf("ADC2: ");
    for (int i = 0; i < ADC2_TEST_CHANNEL_NUM; i++) {
        TEST_ESP_OK( adc2_get_raw((adc2_channel_t)adc2_ch[i], ADC2_TEST_WIDTH, &adc2_val[i]) );
        printf("CH%d-%d ", adc2_ch[i], adc2_val[i]);
#ifdef CONFIG_IDF_TARGET_ESP32S2
        TEST_ASSERT_INT_WITHIN( 100, 0, adc2_val[i] );
#endif
    }
    printf("\n");

    /* tie midedle */
    for (int i = 0; i < ADC2_TEST_CHANNEL_NUM; i++) {
        adc_fake_tie_middle(ADC_UNIT_2, adc2_ch[i]);
    }
    printf("ADC tie middle read: ");
    vTaskDelay(10 / portTICK_RATE_MS);
    /* adc Read */
    printf("ADC2: ");
    for (int i = 0; i < ADC2_TEST_CHANNEL_NUM; i++) {
        TEST_ESP_OK( adc2_get_raw((adc2_channel_t)adc2_ch[i], ADC2_TEST_WIDTH, &adc2_val[i]) );
        printf("CH%d-%d ", adc2_ch[i], adc2_val[i]);
#ifdef CONFIG_IDF_TARGET_ESP32S2
        TEST_ASSERT_NOT_EQUAL( 0, adc2_val[i]  );
        TEST_ASSERT_NOT_EQUAL( 0x1fff, adc2_val[i]  );
#endif
    }
    printf("\n");

    for (int i = 0; i < ADC1_TEST_CHANNEL_NUM; i++) {
        adc_io_normal(ADC_UNIT_1, adc1_ch[i]);
    }
}

#include "touch_scope.h"
/**
 * 0: ADC1 channels raw data debug.
 * 1: ADC2 channels raw data debug.
 */
#define SCOPE_DEBUG_TYPE            1
#define SCOPE_DEBUG_CHANNEL_MAX    (10)
#define SCOPE_DEBUG_ENABLE         (0)
#define SCOPE_UART_BUADRATE        (256000)
#define SCOPE_DEBUG_FREQ_MS        (50)

/**
 * Manual test: Capture ADC-DMA data and display it on the serial oscilloscope. Used to observe the stability of the data.
 * Use step:
 *      1. Call this function in `esp-idf/tools/unit-test-app/main/app_main.c`.
 *      2. Use `ESP-Tuning Tool`(download from `www.espressif.com`) to capture.
 *      3. The readings of multiple channels will be displayed on the tool.
 */
void test_adc_slope_debug(void)
{
    float scope_temp[SCOPE_DEBUG_CHANNEL_MAX] = {0};  // max scope channel is 10.
    test_tp_scope_debug_init(0, -1, -1, SCOPE_UART_BUADRATE);

#if SCOPE_DEBUG_TYPE == 0
    /* adc1 Configure */
    adc1_config_width(ADC1_TEST_WIDTH);
    ESP_LOGI(TAG, "ADC1 [CH - GPIO] atten %d:", ADC1_TEST_ATTEN);
    for (int i = 0; i < ADC1_TEST_CHANNEL_NUM; i++) {
        TEST_ESP_OK( adc1_config_channel_atten(adc1_ch[i], ADC1_TEST_ATTEN) );
        ESP_LOGI(TAG, "[CH%d - IO%d]", adc1_ch[i], ADC_GET_IO_NUM(0, adc1_ch[i]));
    }
    /* tie midedle */
    for (int i = 0; i < ADC1_TEST_CHANNEL_NUM; i++) {
        adc_fake_tie_middle(ADC_UNIT_1, adc1_ch[i]);
    }
    vTaskDelay(10 / portTICK_RATE_MS);

    while (1) {
        /* adc Read */
        for (int i = 0; i < ADC1_TEST_CHANNEL_NUM; i++) {
            scope_temp[i] = adc1_get_raw((adc1_channel_t)adc1_ch[i]);
        }
        test_tp_print_to_scope(scope_temp, ADC1_TEST_CHANNEL_NUM);
        vTaskDelay(SCOPE_DEBUG_FREQ_MS / portTICK_RATE_MS);
    }
#elif SCOPE_DEBUG_TYPE == 1
    int adc2_val[ADC2_TEST_CHANNEL_NUM] = {0};

    /* adc2 Configure */
    ESP_LOGI(TAG, "ADC2 [CH - GPIO] atten %d:", ADC2_TEST_ATTEN);
    for (int i = 0; i < ADC2_TEST_CHANNEL_NUM; i++) {
        TEST_ESP_OK( adc2_config_channel_atten(adc2_ch[i], ADC2_TEST_ATTEN) );
        ESP_LOGI(TAG, "[CH%d - IO%d]:", adc2_ch[i], ADC_GET_IO_NUM(1, adc2_ch[i]));
    }
    /* tie midedle */
    for (int i = 0; i < ADC2_TEST_CHANNEL_NUM; i++) {
        adc_fake_tie_middle(ADC_UNIT_2, adc2_ch[i]);
    }
    vTaskDelay(10 / portTICK_RATE_MS);

    while (1) {
        /* adc Read */
        printf("ADC2: ");
        for (int i = 0; i < ADC2_TEST_CHANNEL_NUM; i++) {
            adc2_get_raw((adc2_channel_t)adc2_ch[i], ADC2_TEST_WIDTH, &adc2_val[i]);
            scope_temp[i] = adc2_val[i];
        }

        test_tp_print_to_scope(scope_temp, ADC2_TEST_CHANNEL_NUM);
        vTaskDelay(SCOPE_DEBUG_FREQ_MS / portTICK_RATE_MS);
    }
#endif
}

#endif
