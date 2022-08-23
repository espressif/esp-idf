/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "soc/adc_periph.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "test_common_adc.h"
#include "test_utils.h"

#if (SOC_ADC_PERIPH_NUM > 1)

static const char* TAG = "test_adc2";

#define DEFAULT_SSID "TEST_SSID"
#define DEFAULT_PWD  "TEST_PASS"

#if CONFIG_IDF_TARGET_ESP32
#define TEST_ADC2_CHAN0         ADC_CHANNEL_9
#elif CONFIG_IDF_TARGET_ESP32S2
#define TEST_ADC2_CHAN0         ADC_CHANNEL_0
#elif CONFIG_IDF_TARGET_ESP32C3
#define TEST_ADC2_CHAN0         ADC_CHANNEL_0
#elif CONFIG_IDF_TARGET_ESP32S3
#define TEST_ADC2_CHAN0         ADC_CHANNEL_0
#endif

#define ADC_ERROR_THRES         100
#define TEST_NUM                8

#define MINUS_UNTIL_ZERO(a, b) ( ((a) > (b)) ? ((a)-(b)): 0)
#define TIME_REMAIN(start, now, timeout) ((now) >= (start) ? MINUS_UNTIL_ZERO((timeout), (now)-(start)) : -1)


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

    /* Restrict the number of buffers to allocate to account for limited memory when running large number of tests */
    cfg.static_rx_buf_num = 2;
    cfg.static_tx_buf_num = 2;

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
    int test_adc_io;
    bool test_list[TEST_NUM] = {1, 1, 0, 0, 1, 0, 1, 0};

    TEST_ESP_OK(adc_oneshot_channel_to_io(ADC_UNIT_2, TEST_ADC2_CHAN0, &test_adc_io));
    printf("test_adc_io is %d\n", test_adc_io);

    //-------------ADC2 Init---------------//
    adc_oneshot_unit_handle_t adc2_handle;
    adc_oneshot_unit_init_cfg_t init_config2 = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    TEST_ESP_OK(adc_oneshot_new_unit(&init_config2, &adc2_handle));

    //-------------ADC2 TEST Channel 0 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    TEST_ESP_OK(adc_oneshot_config_channel(adc2_handle, TEST_ADC2_CHAN0, &config));

    for (int i = 0; i < TEST_NUM; i++) {
        /* Tune test ADC channel level */
        test_adc_set_io_level(ADC_UNIT_2, TEST_ADC2_CHAN0, test_list[i]);
        target_value = test_list[i] ? ADC_TEST_HIGH_VAL : ADC_TEST_LOW_VAL;


        /* ADC2 single read before WIFI start */
        TEST_ESP_OK(adc_oneshot_read(adc2_handle, TEST_ADC2_CHAN0, &read_raw));
        printf("Before WiFi starts, ADC read: %d (target_value: %d)\n", read_raw, target_value);
        TEST_ASSERT_INT_WITHIN(ADC_ERROR_THRES, target_value, read_raw);


        /* ADC2 single read when WIFI is on */
        TEST_ESP_OK(esp_wifi_start());
#if CONFIG_IDF_TARGET_ESP32
        TEST_ASSERT_EQUAL(ESP_ERR_TIMEOUT, adc_oneshot_read(adc2_handle, TEST_ADC2_CHAN0, &read_raw));
#else
        esp_err_t ret = ESP_FAIL;
        int32_t start = xTaskGetTickCount();
        int32_t now;
        int32_t remain_wait_ms = 0;
        int32_t timeout = pdMS_TO_TICKS(10);

        do {
            now = xTaskGetTickCount();
            remain_wait_ms = pdTICKS_TO_MS(TIME_REMAIN(start, now, timeout));
            ret = adc_oneshot_read(adc2_handle, TEST_ADC2_CHAN0, &read_raw);
            if (ret == ESP_OK) {
                printf("When WiFi is ON, ADC read: %d (target_value: %d)\n", read_raw, target_value);
                TEST_ASSERT_INT_WITHIN(ADC_ERROR_THRES, target_value, read_raw);
                break;
            } else if (ret == ESP_ERR_TIMEOUT) {
                continue;
            } else {
                TEST_ESP_OK(ret);
            }
        } while (remain_wait_ms);
#endif


        /* ADC2 single read after WIFI is off */
        TEST_ESP_OK(esp_wifi_stop());
        TEST_ESP_OK(adc_oneshot_read(adc2_handle, TEST_ADC2_CHAN0, &read_raw));
        printf("After WiFi is OFF, ADC read: %d (target_value: %d)\n\n", read_raw, target_value);
        TEST_ASSERT_INT_WITHIN(ADC_ERROR_THRES, target_value, read_raw);
    }

    TEST_ESP_OK(esp_wifi_deinit());
    event_deinit();
    nvs_flash_deinit();
    TEST_ESP_OK(adc_oneshot_del_unit(adc2_handle));

    TEST_IGNORE_MESSAGE("this test case is ignored due to the critical memory leak of esp_netif and event_loop.");
}

#endif  //#if (SOC_ADC_PERIPH_NUM > 1)
