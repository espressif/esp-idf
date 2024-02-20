/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
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

#if SOC_WIFI_SUPPORTED && SOC_ADC_SUPPORTED

static const char* TAG = "test_adc_wifi";

#define DEFAULT_SSID "TEST_SSID"
#define DEFAULT_PWD  "TEST_PASS"

//ADC Channels
#if CONFIG_IDF_TARGET_ESP32
#define ADC1_WIFI_TEST_CHAN0            ADC_CHANNEL_4
#define ADC2_WIFI_TEST_CHAN0            ADC_CHANNEL_9
#else
#define ADC1_WIFI_TEST_CHAN0            ADC_CHANNEL_0
#define ADC2_WIFI_TEST_CHAN0            ADC_CHANNEL_0
#endif

#define ADC_ERROR_THRES         200
#define TEST_NUM                8

#define MINUS_UNTIL_ZERO(a, b) ( ((a) > (b)) ? ((a)-(b)): 0)
#define TIME_REMAIN(start, now, timeout) ((now) >= (start) ? MINUS_UNTIL_ZERO((timeout), (now)-(start)) : -1)

static int read_raw;
static int target_value;
static int test_adc_io;
static bool test_list[TEST_NUM] = {1, 1, 0, 0, 1, 0, 1, 0};

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    printf("ev_handle_called.\n");
    switch (event_id) {
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
    switch (event_id) {
    case IP_EVENT_STA_GOT_IP:
        event = (ip_event_got_ip_t*)event_data;
        ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
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

static void s_test_adc_work_when_wifi_on(adc_oneshot_unit_handle_t adc_handle, adc_channel_t channel)
{
    esp_err_t ret = ESP_FAIL;
    int32_t start = xTaskGetTickCount();
    int32_t now;
    int32_t remain_wait_ms = 0;
    int32_t timeout = pdMS_TO_TICKS(10);
    do {
        now = xTaskGetTickCount();
        remain_wait_ms = pdTICKS_TO_MS(TIME_REMAIN(start, now, timeout));
        ret = adc_oneshot_read(adc_handle, channel, &read_raw);
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
}

__attribute__((unused)) static void adc_work_with_wifi(adc_unit_t unit_id, adc_channel_t channel)
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
    TEST_ESP_OK(r);
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

    TEST_ESP_OK(adc_oneshot_channel_to_io(unit_id, channel, &test_adc_io));
    printf("test_adc_io is %d\n", test_adc_io);

    //-------------ADC Init---------------//
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = unit_id,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    TEST_ESP_OK(adc_oneshot_new_unit(&init_config, &adc_handle));

    //-------------ADC TEST Channel Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    TEST_ESP_OK(adc_oneshot_config_channel(adc_handle, channel, &config));

    for (int i = 0; i < TEST_NUM; i++) {
        /* Tune test ADC channel level */
        test_adc_set_io_level(unit_id, channel, test_list[i]);
        target_value = test_list[i] ? ADC_TEST_HIGH_VAL : ADC_TEST_LOW_VAL;

        /* ADC single read before WIFI start */
        TEST_ESP_OK(adc_oneshot_read(adc_handle, channel, &read_raw));
        printf("Before WiFi starts, ADC read: %d (target_value: %d)\n", read_raw, target_value);
        TEST_ASSERT_INT_WITHIN(ADC_ERROR_THRES, target_value, read_raw);

        /* ADC single read when WIFI is on */
        TEST_ESP_OK(esp_wifi_start());
#if CONFIG_IDF_TARGET_ESP32
        if (unit_id == 1) {
            // On ESP32 ADC2, PWDET/PKDET controller is for Wi-Fi internal use only.
            // If Wi-Fi module is using the SAR ADC2, users can not measure the analog signal from the pins using SAR ADC2.
            // After SAR ADC2 is released by Wi-Fi, users can use SAR ADC2 normally.
            TEST_ASSERT_EQUAL(ESP_ERR_TIMEOUT, adc_oneshot_read(adc_handle, channel, &read_raw));
        } else {
            s_test_adc_work_when_wifi_on(adc_handle, channel);
        }
#else
        s_test_adc_work_when_wifi_on(adc_handle, channel);
#endif
        /* ADC single read after WIFI is off */
        TEST_ESP_OK(esp_wifi_stop());
        TEST_ESP_OK(adc_oneshot_read(adc_handle, channel, &read_raw));
        printf("After WiFi is OFF, ADC read: %d (target_value: %d)\n\n", read_raw, target_value);
        TEST_ASSERT_INT_WITHIN(ADC_ERROR_THRES, target_value, read_raw);
    }

    TEST_ESP_OK(adc_oneshot_del_unit(adc_handle));
    ESP_LOGI(TAG, "ADC%d with WiFi test is success", unit_id + 1);

    TEST_ESP_OK(esp_wifi_deinit());
    event_deinit();
    nvs_flash_deinit();

    TEST_IGNORE_MESSAGE("this test case is ignored due to the critical memory leak of esp_netif and event_loop.");
}

#if CONFIG_IDF_TARGET_ESP32C6
// On ESP32C6, ADC need to call two modem clocks: modem_syscon_ll_enable_fe_80m_clock and modem_syscon_ll_enable_fe_apb_clock.
// Without calling these two clocks, PWDET mode will not take into effect, so ADC readings will be wrong.
TEST_CASE("ADC1 work with WiFi", "[adc]")
{
    adc_work_with_wifi(ADC_UNIT_1, ADC1_WIFI_TEST_CHAN0);
}
#endif  // CONFIG_IDF_TARGET_ESP32C6

#if (SOC_ADC_PERIPH_NUM >= 2) && !CONFIG_IDF_TARGET_ESP32C3
// On ESP32C3, ADC2 is no longer supported, due to its HW limitation.
TEST_CASE("ADC2 work with WiFi", "[adc]")
{
    adc_work_with_wifi(ADC_UNIT_2, ADC2_WIFI_TEST_CHAN0);
}
#endif // (SOC_ADC_PERIPH_NUM >= 2) && !CONFIG_IDF_TARGET_ESP32C3

#endif //SOC_WIFI_SUPPORTED && SOC_ADC_SUPPORTED
