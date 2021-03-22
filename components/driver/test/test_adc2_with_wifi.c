/*
 Tests for the adc2 device driver
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
#include "driver/gpio.h"

static const char* TAG = "test_adc2";

#ifdef CONFIG_IDF_TARGET_ESP32
    #define ADC_TEST_WIDTH         ADC_WIDTH_BIT_12
    #define ADC_TEST_RESOLUTION    (4096)
    #define ADC_TEST_DAC_RANGE     (256)
    #define ADC_TEST_CH1           ADC2_CHANNEL_8
    #define ADC_TEST_CH2           ADC2_CHANNEL_9
    #define ADC_TEST_ERROR         (600)
    #define ADC1_CHANNEL_4_IO      (32)
    #define SAMPLE_RATE            (36000)
    #define SAMPLE_BITS            (16)
#elif defined CONFIG_IDF_TARGET_ESP32S2
    #define ADC_TEST_WIDTH         ADC_WIDTH_BIT_13   //ESP32S2 only support 13 bit width
    #define ADC_TEST_RESOLUTION    (8192)
    #define ADC_TEST_DAC_RANGE     (210)
    #define ADC_TEST_CH1           ADC2_CHANNEL_6
    #define ADC_TEST_CH2           ADC2_CHANNEL_7
    #define ADC_TEST_ERROR         (1500)
#endif
#define DEFAULT_SSID "TEST_SSID"
#define DEFAULT_PWD "TEST_PASS"

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
    int     read_raw;
    int     target_value;

    test_case_uses_tcpip();

    //adc and dac init
    TEST_ESP_OK( dac_output_enable( DAC_CHANNEL_1 ));
    TEST_ESP_OK( dac_output_enable( DAC_CHANNEL_2 ));
    TEST_ESP_OK( dac_output_voltage( DAC_CHANNEL_1, 30 ));
    TEST_ESP_OK( dac_output_voltage( DAC_CHANNEL_2, 60 ));
    TEST_ESP_OK( adc2_config_channel_atten( ADC_TEST_CH1, ADC_ATTEN_0db ));
    TEST_ESP_OK( adc2_config_channel_atten( ADC_TEST_CH2, ADC_ATTEN_0db ));
    //init wifi
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

    //test read value
    TEST_ESP_OK( adc2_get_raw( ADC_TEST_CH1, ADC_TEST_WIDTH, &read_raw ));
    target_value = 30*ADC_TEST_RESOLUTION*3/ADC_TEST_DAC_RANGE; //3 = 3.3/1.1
    printf("dac set: %d, adc read: %d (target_value: %d)\n", 30, read_raw, target_value );
    TEST_ASSERT_INT_WITHIN( ADC_TEST_ERROR, target_value, read_raw );
    TEST_ESP_OK( adc2_get_raw( ADC_TEST_CH2, ADC_TEST_WIDTH, &read_raw ));
    target_value = 60*ADC_TEST_RESOLUTION*3/ADC_TEST_DAC_RANGE;
    printf("dac set: %d, adc read: %d (target_value: %d)\n", 60, read_raw, target_value );
    TEST_ASSERT_INT_WITHIN( ADC_TEST_ERROR, target_value, read_raw );

    //now start wifi
    printf("wifi start...\n");
    TEST_ESP_OK(esp_wifi_start());
    //test reading during wifi on
#ifdef CONFIG_IDF_TARGET_ESP32
    TEST_ASSERT_EQUAL( adc2_get_raw( ADC_TEST_CH1, ADC_TEST_WIDTH, &read_raw ), ESP_ERR_TIMEOUT );
    TEST_ASSERT_EQUAL( adc2_get_raw( ADC_TEST_CH2, ADC_TEST_WIDTH, &read_raw ), ESP_ERR_TIMEOUT );
#elif defined CONFIG_IDF_TARGET_ESP32S2
    TEST_ASSERT_EQUAL( adc2_get_raw( ADC_TEST_CH1, ADC_TEST_WIDTH, &read_raw ), ESP_OK );
    TEST_ASSERT_EQUAL( adc2_get_raw( ADC_TEST_CH2, ADC_TEST_WIDTH, &read_raw ), ESP_OK );
#endif
    //wifi stop again
    printf("wifi stop...\n");
    TEST_ESP_OK( esp_wifi_stop() );
    TEST_ESP_OK(esp_wifi_deinit());
    event_deinit();
    nvs_flash_deinit();

    //test read value
    TEST_ESP_OK( adc2_get_raw( ADC_TEST_CH1, ADC_TEST_WIDTH, &read_raw ));
    target_value = 30*ADC_TEST_RESOLUTION*3/ADC_TEST_DAC_RANGE; //3 = 3.3/1.1
    printf("dac set: %d, adc read: %d (target_value: %d)\n", 30, read_raw, target_value );
    TEST_ASSERT_INT_WITHIN( ADC_TEST_ERROR, target_value, read_raw );
    TEST_ESP_OK( adc2_get_raw( ADC_TEST_CH2, ADC_TEST_WIDTH, &read_raw ));
    target_value = 60*ADC_TEST_RESOLUTION*3/ADC_TEST_DAC_RANGE;
    printf("dac set: %d, adc read: %d (target_value: %d)\n", 60, read_raw, target_value );
    TEST_ASSERT_INT_WITHIN( ADC_TEST_ERROR, target_value, read_raw );

    printf("test passed...\n");

    TEST_IGNORE_MESSAGE("this test case is ignored due to the critical memory leak of esp_netif and event_loop.");
}

#ifdef CONFIG_IDF_TARGET_ESP32
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
    TEST_ESP_OK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
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
