/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 Tests for the adc2 device driver
*/
#include "esp_system.h"
#include "unity.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "test_utils.h"
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C3, ESP32S3)

static const char* TAG = "test_adc2";

#define DEFAULT_SSID "TEST_SSID"
#define DEFAULT_PWD  "TEST_PASS"

#if CONFIG_IDF_TARGET_ESP32
#define ADC2_CHAN1              ADC2_CHANNEL_9
#define ADC_WIDTH               ADC_WIDTH_BIT_12
#define ADC_HIGH                4095
#define ADC_ERROR_THRES         20
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC2_CHAN1              ADC2_CHANNEL_7
#define ADC_WIDTH               ADC_WIDTH_BIT_13
#define ADC_HIGH                8191
#define ADC_ERROR_THRES         100
#elif CONFIG_IDF_TARGET_ESP32C3
#define ADC2_CHAN1              ADC2_CHANNEL_0
#define ADC_WIDTH               ADC_WIDTH_BIT_12
#define ADC_HIGH                4095
#define ADC_ERROR_THRES         100
#elif CONFIG_IDF_TARGET_ESP32S3
#define ADC2_CHAN1              ADC2_CHANNEL_0
#define ADC_WIDTH               ADC_WIDTH_BIT_12
#define ADC_HIGH                4095
#define ADC_ERROR_THRES         100
#endif

#define ADC_LOW                 0
#define TEST_NUM                8

#define MINUS_UNTIL_ZERO(a, b) ( ((a) > (b)) ? ((a)-(b)): 0)
#define TIME_REMAIN(start, now, timeout) ((start) >= (now) ? MINUS_UNTIL_ZERO((timeout), (now)-(start)) : -1)


static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    printf("ev_handle_called.\n");
    switch(event_id) {
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
    //do not actually connect in test case
            //;
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
            TEST_ESP_OK(esp_wifi_connect());
            break;
        default:
            break;
    }
    return ;
}

static void ip_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    ip_event_got_ip_t *event;
    printf("ev_handle_called.\n");
    switch(event_id) {
        case IP_EVENT_STA_GOT_IP:
            event = (ip_event_got_ip_t*)event_data;
            ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
            ESP_LOGI(TAG, "got ip:" IPSTR "\n", IP2STR(&event->ip_info.ip));
            break;
        default:
            break;
    }

    return ;
}

static int event_init(void)
{
    TEST_ESP_OK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler, NULL));
    return ESP_OK;
}

static int event_deinit(void)
{
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler));
    return ESP_OK;
}

TEST_CASE("adc2 work with wifi","[adc]")
{
    test_case_uses_tcpip();

    //---------------------------------WiFi init-----------------------------------//
    printf("nvs init\n");
    esp_err_t r = nvs_flash_init();
    if (r == ESP_ERR_NVS_NO_FREE_PAGES || r == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        printf("no free pages or nvs version mismatch, erase..\n");
        TEST_ESP_OK(nvs_flash_erase());
        r = nvs_flash_init();
    }
    TEST_ESP_OK( r);
    esp_netif_init();
    event_init();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = DEFAULT_SSID,
            .password = DEFAULT_PWD
        },
    };
    TEST_ESP_OK(esp_wifi_set_mode(WIFI_MODE_STA));
    TEST_ESP_OK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    //---------------------------------ADC init-----------------------------------//
    int read_raw;
    int target_value;
    gpio_num_t test_adc_io;
    bool test_list[TEST_NUM] ={1, 1, 0, 0, 1, 0, 1, 0};

    adc2_pad_get_io_num(ADC2_CHAN1, &test_adc_io);
    TEST_ESP_OK(adc2_config_channel_atten(ADC2_CHAN1, ADC_ATTEN_0db));
    printf("test_adc_io is %d\n", test_adc_io);

    //---------------------------------GPIO init-----------------------------------//
    gpio_config_t gpio_cfg = {
        .pin_bit_mask = BIT64(test_adc_io),
        .mode = GPIO_MODE_OUTPUT,
        //for powersave reasons, the GPIO should not be floating, select pullup
        .pull_up_en = true,
        .pull_down_en = false,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&gpio_cfg);

    for (int i = 0; i < TEST_NUM; i++) {
        TEST_ESP_OK(gpio_set_level(test_adc_io, test_list[i]));
        target_value = test_list[i] ? ADC_HIGH : ADC_LOW;

        /* ADC2 single read before WIFI start */
        TEST_ESP_OK(adc2_get_raw(ADC2_CHAN1, ADC_WIDTH, &read_raw));
        printf("Before WiFi starts, ADC read: %d (target_value: %d)\n", read_raw, target_value);
        TEST_ASSERT_INT_WITHIN(ADC_ERROR_THRES, target_value, read_raw);

        /* ADC2 single read when WIFI is on */
        TEST_ESP_OK(esp_wifi_start());
    #if CONFIG_IDF_TARGET_ESP32
        TEST_ASSERT_EQUAL(adc2_get_raw(ADC2_CHAN1, ADC_WIDTH, &read_raw), ESP_ERR_TIMEOUT);
    #elif SOC_ADC_ARBITER_SUPPORTED
        esp_err_t ret;
        int32_t start = xTaskGetTickCount();
        int32_t now;
        int32_t remain_wait_ms = 0;
        int32_t timeout = pdMS_TO_TICKS(10);

        do {
            now = xTaskGetTickCount();
            remain_wait_ms = pdTICKS_TO_MS(TIME_REMAIN(start, now, timeout));

            ret = adc2_get_raw(ADC2_CHAN1, ADC_WIDTH, &read_raw);
            if (ret == ESP_OK) {
                printf("When WiFi is ON, ADC read: %d (target_value: %d)\n", read_raw, target_value);
                TEST_ASSERT_INT_WITHIN(ADC_ERROR_THRES, target_value, read_raw);
                break;
            } else if (ret == ESP_ERR_INVALID_STATE) {
                continue;
            } else {
                TEST_ESP_OK(ret);
            }
        } while (remain_wait_ms);
    #endif

        /* ADC2 single read after WIFI is off */
        TEST_ESP_OK(esp_wifi_stop());
        TEST_ESP_OK(adc2_get_raw(ADC2_CHAN1, ADC_WIDTH, &read_raw));
        printf("After WiFi is OFF, ADC read: %d (target_value: %d)\n", read_raw, target_value);
        TEST_ASSERT_INT_WITHIN(ADC_ERROR_THRES, target_value, read_raw);
    }

    TEST_ESP_OK(esp_wifi_deinit());
    event_deinit();
    nvs_flash_deinit();

    TEST_IGNORE_MESSAGE("this test case is ignored due to the critical memory leak of esp_netif and event_loop.");
}

#endif  //#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C3, ESP32S3)


#ifdef CONFIG_IDF_TARGET_ESP32

#define ADC1_CHANNEL_4_IO      (32)
#define SAMPLE_RATE            (36000)
#define SAMPLE_BITS            (16)

static void i2s_adc_init(void)
{
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN,
        .sample_rate =  SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .intr_alloc_flags = 0,
        .dma_buf_count = 2,
        .dma_buf_len = 1024,
        .use_apll = 0,
    };
    // install and start I2S driver
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    // init ADC pad
    i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_4);
    // enable adc sampling, ADC_WIDTH_BIT_12, ADC_ATTEN_DB_11 hard-coded in adc_i2s_mode_init
    i2s_adc_enable(I2S_NUM_0);
}

static void i2s_adc_test(void)
{
    uint16_t *i2sReadBuffer = (uint16_t *)calloc(1024, sizeof(uint16_t));
    size_t bytesRead;
    for (int loop = 0; loop < 10; loop++) {
        for (int level = 0; level <= 1; level++) {
            if (level == 0) {
                gpio_set_pull_mode(ADC1_CHANNEL_4_IO, GPIO_PULLDOWN_ONLY);
            } else {
                gpio_set_pull_mode(ADC1_CHANNEL_4_IO, GPIO_PULLUP_ONLY);
            }
            vTaskDelay(200 / portTICK_RATE_MS);
            // read data from adc, will block until buffer is full
            i2s_read(I2S_NUM_0, (void *)i2sReadBuffer, 1024 * sizeof(uint16_t), &bytesRead, portMAX_DELAY);

            // calc average
            int64_t adcSumValue = 0;
            for (size_t i = 0; i < 1024; i++) {
                adcSumValue += i2sReadBuffer[i] & 0xfff;
            }
            int adcAvgValue = adcSumValue / 1024;
            printf("adc average val: %d\n", adcAvgValue);

            if (level == 0) {
                TEST_ASSERT_LESS_THAN(100, adcAvgValue);
            } else {
                TEST_ASSERT_GREATER_THAN(4000, adcAvgValue);
            }
        }
    }
    free(i2sReadBuffer);
}

static void i2s_adc_release(void)
{
    i2s_adc_disable(I2S_NUM_0);
    i2s_driver_uninstall(I2S_NUM_0);
}

TEST_CASE("adc1 and i2s work with wifi","[adc][ignore]")
{

    i2s_adc_init();
    //init wifi
    printf("nvs init\n");
    esp_err_t r = nvs_flash_init();
    if (r == ESP_ERR_NVS_NO_FREE_PAGES || r == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        printf("no free pages or nvs version mismatch, erase..\n");
        TEST_ESP_OK(nvs_flash_erase());
        r = nvs_flash_init();
    }
    TEST_ESP_OK(r);
    esp_netif_init();
    event_init();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = DEFAULT_SSID,
            .password = DEFAULT_PWD
        },
    };
    TEST_ESP_OK(esp_wifi_set_mode(WIFI_MODE_STA));
    TEST_ESP_OK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    i2s_adc_test();
    //now start wifi
    printf("wifi start...\n");
    TEST_ESP_OK(esp_wifi_start());
    //test reading during wifi on
    i2s_adc_test();
    //wifi stop again
    printf("wifi stop...\n");

    TEST_ESP_OK( esp_wifi_stop() );

    TEST_ESP_OK(esp_wifi_deinit());

    event_deinit();

    nvs_flash_deinit();
    i2s_adc_test();
    i2s_adc_release();
    printf("test passed...\n");
    TEST_IGNORE_MESSAGE("this test case is ignored due to the critical memory leak of esp_netif and event_loop.");
}
#endif
