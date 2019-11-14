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

static const char* TAG = "test_adc2";

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
    TEST_ESP_OK( adc2_config_channel_atten( ADC2_CHANNEL_8, ADC_ATTEN_0db ));
    TEST_ESP_OK( adc2_config_channel_atten( ADC2_CHANNEL_9, ADC_ATTEN_0db ));

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
    TEST_ESP_OK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    
    //test read value
    TEST_ESP_OK( adc2_get_raw( ADC2_CHANNEL_8, ADC_WIDTH_12Bit, &read_raw ));
    target_value = 30*4096*3/256; //3 = 3.3/1.1
    printf("dac set: %d, adc read: %d (target_value: %d)\n", 30, read_raw, target_value );
    TEST_ASSERT_INT_WITHIN( 600, target_value, read_raw );
    TEST_ESP_OK( adc2_get_raw( ADC2_CHANNEL_9, ADC_WIDTH_12Bit, &read_raw ));
    target_value = 60*4096*3/256;
    printf("dac set: %d, adc read: %d (target_value: %d)\n", 60, read_raw, target_value );
    TEST_ASSERT_INT_WITHIN( 600, target_value, read_raw );

    //now start wifi
    printf("wifi start...\n");
    TEST_ESP_OK(esp_wifi_start());

    //test reading during wifi on
    TEST_ASSERT_EQUAL( adc2_get_raw( ADC2_CHANNEL_8, ADC_WIDTH_12Bit, &read_raw ), ESP_ERR_TIMEOUT );
    TEST_ASSERT_EQUAL( adc2_get_raw( ADC2_CHANNEL_9, ADC_WIDTH_12Bit, &read_raw ), ESP_ERR_TIMEOUT );

    //wifi stop again
    printf("wifi stop...\n");
    TEST_ESP_OK( esp_wifi_stop() );
    TEST_ESP_OK(esp_wifi_deinit());
    event_deinit();
    nvs_flash_deinit();

    //test read value
    TEST_ESP_OK( adc2_get_raw( ADC2_CHANNEL_8, ADC_WIDTH_12Bit, &read_raw ));
    target_value = 30*4096*3/256; //3 = 3.3/1.1
    printf("dac set: %d, adc read: %d (target_value: %d)\n", 30, read_raw, target_value );
    TEST_ASSERT_INT_WITHIN( 600, target_value, read_raw );
    TEST_ESP_OK( adc2_get_raw( ADC2_CHANNEL_9, ADC_WIDTH_12Bit, &read_raw ));
    target_value = 60*4096*3/256;
    printf("dac set: %d, adc read: %d (target_value: %d)\n", 60, read_raw, target_value );
    TEST_ASSERT_INT_WITHIN( 600, target_value, read_raw );

    printf("test passed...\n");

    TEST_IGNORE_MESSAGE("this test case is ignored due to the critical memory leak of esp_netif and event_loop.");
}
