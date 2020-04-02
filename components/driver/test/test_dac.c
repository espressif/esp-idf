/*
 Tests for the dac device driver
*/
#include "esp_system.h"
#include "driver/adc.h"
#include "driver/dac.h"
#include "unity.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "test_utils.h"
#include "driver/i2s.h"

static const char *TAG = "test_dac";

#ifdef CONFIG_IDF_TARGET_ESP32
#define ADC_TEST_WIDTH         ADC_WIDTH_BIT_12
#elif defined CONFIG_IDF_TARGET_ESP32S2
#define ADC_TEST_WIDTH         ADC_WIDTH_BIT_13   //ESP32S2 only support 13 bit width
#endif
#define ADC_TEST_ATTEN         ADC_ATTEN_DB_11

#if CONFIG_IDF_TARGET_ESP32
#define ADC_TEST_CHANNEL_NUM   ADC2_CHANNEL_8   // GPIO25
#define DAC_TEST_CHANNEL_NUM   DAC_CHANNEL_1    // GPIO25
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_TEST_CHANNEL_NUM   ADC2_CHANNEL_6   // GPIO17
#define DAC_TEST_CHANNEL_NUM   DAC_CHANNEL_1    // GPIO17
#endif

#define DAC_OUT_MAX       (200)
#define DAC_OUT_TIMES     (10)
#define DAC_OUT_STEP      (DAC_OUT_MAX / DAC_OUT_TIMES)

#define DAC_TEST_TIMES    (100)

TEST_CASE("DAC output (RTC) check by adc", "[dac]")
{
    gpio_num_t adc_gpio_num, dac_gpio_num;

    TEST_ESP_OK( adc2_pad_get_io_num( ADC_TEST_CHANNEL_NUM, &adc_gpio_num ) );
    TEST_ESP_OK( dac_pad_get_io_num( DAC_TEST_CHANNEL_NUM, &dac_gpio_num ) );

    printf("Please connect ADC2 CH%d-GPIO%d <--> DAC CH%d-GPIO%d.\n", ADC_TEST_CHANNEL_NUM, adc_gpio_num,
           DAC_TEST_CHANNEL_NUM + 1, dac_gpio_num );

    TEST_ESP_OK( dac_output_enable( DAC_TEST_CHANNEL_NUM ) );

    //be sure to do the init before using adc2.
    printf("adc2_init...\n");
    TEST_ESP_OK( adc2_config_channel_atten( ADC_TEST_CHANNEL_NUM, ADC_TEST_ATTEN ) );

    vTaskDelay(2 * portTICK_RATE_MS);

    printf("start conversion.\n");
    int output_data = 0;
    int read_raw = 0, read_old = 0;
    for (int i = 0; i < DAC_OUT_TIMES; i++) {
        TEST_ESP_OK( dac_output_voltage( DAC_TEST_CHANNEL_NUM, output_data ) );
        output_data += DAC_OUT_STEP;
        vTaskDelay(2 * portTICK_RATE_MS);
        TEST_ESP_OK( adc2_get_raw( ADC_TEST_CHANNEL_NUM, ADC_TEST_WIDTH, &read_raw) );
        ESP_LOGI(TAG, "DAC%d - ADC%d", output_data, read_raw);
        if (read_old != 0) {
            TEST_ASSERT_GREATER_THAN(read_old, read_raw);
        }
        read_old = read_raw;
    }
    TEST_ESP_OK( dac_output_disable( DAC_TEST_CHANNEL_NUM ) );
}

TEST_CASE("DAC cw generator output (RTC) check by adc", "[dac]")
{
    gpio_num_t adc_gpio_num, dac_gpio_num;

    TEST_ESP_OK( adc2_pad_get_io_num( ADC_TEST_CHANNEL_NUM, &adc_gpio_num ) );
    TEST_ESP_OK( dac_pad_get_io_num( DAC_TEST_CHANNEL_NUM, &dac_gpio_num ) );

    printf("Please connect ADC2 CH%d-GPIO%d <--> DAC CH%d-GPIO%d.\n", ADC_TEST_CHANNEL_NUM, adc_gpio_num,
           DAC_TEST_CHANNEL_NUM + 1, dac_gpio_num );

    dac_cw_config_t cw = {
        .en_ch = DAC_TEST_CHANNEL_NUM,
        .scale = DAC_CW_SCALE_2,
        .phase = DAC_CW_PHASE_0,
        .freq = 1000,
#if CONFIG_IDF_TARGET_ESP32
        .offset = 64,
#elif CONFIG_IDF_TARGET_ESP32S2
        .offset = 16,
#endif
    };
    TEST_ESP_OK( dac_cw_generator_config(&cw) );
    TEST_ESP_OK( dac_cw_generator_enable() );
    TEST_ESP_OK( dac_output_enable( DAC_TEST_CHANNEL_NUM ) );

    //be sure to do the init before using adc2.
    printf("adc2_init...\n");
    TEST_ESP_OK( adc2_config_channel_atten( ADC_TEST_CHANNEL_NUM, ADC_TEST_ATTEN ) );

    vTaskDelay(2 * portTICK_RATE_MS);

    printf("start conversion.\n");
    int read_raw[3] = {0};
    for (int i = 0; i < DAC_TEST_TIMES; i++) {
        vTaskDelay(10 * portTICK_RATE_MS);
        TEST_ESP_OK( adc2_get_raw( ADC_TEST_CHANNEL_NUM, ADC_TEST_WIDTH, &read_raw[0]) );
        ESP_LOGI(TAG, "ADC: %d", read_raw[0]);
        /* Should open after dac cali. */
        // if (read_raw[0] == read_raw[1]) {
        //     TEST_ASSERT_NOT_EQUAL(read_raw[1], read_raw[2]);
        // }
        read_raw[2] = read_raw[1];
        read_raw[1] = read_raw[0];
    }

    TEST_ESP_OK( dac_cw_generator_disable() );
    TEST_ESP_OK( dac_output_disable( DAC_TEST_CHANNEL_NUM ) );
}
