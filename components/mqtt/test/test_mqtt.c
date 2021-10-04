#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "unity.h"
#include "test_utils.h"
#include "mqtt_client.h"
#include "nvs_flash.h"
#include "esp_ota_ops.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"

static void test_leak_setup(const char * file, long line)
{
    uint8_t mac[6];
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    printf("%s:%ld: time=%ld.%lds, mac:" MACSTR "\n", file, line, te.tv_sec, te.tv_usec, MAC2STR(mac));
    unity_reset_leak_checks();
}

TEST_CASE("mqtt init with invalid url", "[mqtt][leaks=0]")
{
    test_leak_setup(__FILE__, __LINE__);
    const esp_mqtt_client_config_t mqtt_cfg = {
            .uri = "INVALID",
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    TEST_ASSERT_EQUAL(NULL, client );
}

TEST_CASE("mqtt init and deinit", "[mqtt][leaks=0]")
{
    test_leak_setup(__FILE__, __LINE__);
    const esp_mqtt_client_config_t mqtt_cfg = {
            // no connection takes place, but the uri has to be valid for init() to succeed
            .uri = "mqtts://localhost:8883",
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    TEST_ASSERT_NOT_EQUAL(NULL, client );
    esp_mqtt_client_destroy(client);
}

static const char* this_bin_addr(void)
{
    spi_flash_mmap_handle_t out_handle;
    const void *binary_address;
    const esp_partition_t* partition = esp_ota_get_running_partition();
    esp_partition_mmap(partition, 0, partition->size, SPI_FLASH_MMAP_DATA, &binary_address, &out_handle);
    return binary_address;
}

TEST_CASE("mqtt enqueue and destroy outbox", "[mqtt][leaks=0]")
{
    const char * bin_addr = this_bin_addr();
    test_leak_setup(__FILE__, __LINE__);
    const int messages = 20;
    const int size = 2000;
    const esp_mqtt_client_config_t mqtt_cfg = {
            // no connection takes place, but the uri has to be valid for init() to succeed
            .uri = "mqtts://localhost:8883",
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    TEST_ASSERT_NOT_EQUAL(NULL, client );
    int bytes_before = esp_get_free_heap_size();
    for (int i=0; i<messages; ++i) {
        esp_mqtt_client_publish(client, "test", bin_addr, size, 1, 0);
    }
    int bytes_after = esp_get_free_heap_size();
    // check that outbox allocated all messages on heap
    TEST_ASSERT_GREATER_OR_EQUAL(messages*size, bytes_before - bytes_after);

    esp_mqtt_client_destroy(client);
}

#if SOC_EMAC_SUPPORTED

void eth_test_fixture_connect(void);
void eth_test_fixture_deinit(void);

static const int CONNECT_BIT = BIT0;
static const int DISCONNECT_BIT = BIT1;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    EventGroupHandle_t *event_group = handler_args;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            xEventGroupSetBits(*event_group, CONNECT_BIT);
            break;
        case MQTT_EVENT_DISCONNECTED:
            xEventGroupSetBits(*event_group, DISCONNECT_BIT);
            break;
        default:
            break;
    }
}

TEST_CASE("connect disconnect", "[mqtt][test_env=UT_T2_Ethernet]")
{
    test_leak_setup(__FILE__, __LINE__);
    test_case_uses_tcpip();
    eth_test_fixture_connect();
    const int TEST_CONNECT_TIMEOUT = 10000;
    const esp_mqtt_client_config_t mqtt_cfg = {
            // no connection takes place, but the uri has to be valid for init() to succeed
            .uri = CONFIG_MQTT_TEST_BROKER_URI,
    };
    EventGroupHandle_t event_group = xEventGroupCreate();
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    TEST_ASSERT_NOT_EQUAL(NULL, client );
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, &event_group);
    TEST_ASSERT_EQUAL(ESP_OK, esp_mqtt_client_start(client));
    TEST_ASSERT_TRUE(xEventGroupWaitBits(event_group, CONNECT_BIT, pdTRUE, pdTRUE, pdMS_TO_TICKS(TEST_CONNECT_TIMEOUT)) & CONNECT_BIT);
    esp_mqtt_client_disconnect(client);
    TEST_ASSERT_TRUE(xEventGroupWaitBits(event_group, DISCONNECT_BIT, pdTRUE, pdTRUE, pdMS_TO_TICKS(TEST_CONNECT_TIMEOUT)) & DISCONNECT_BIT);
    esp_mqtt_client_destroy(client);
    eth_test_fixture_deinit();
}
#endif // SOC_EMAC_SUPPORTED
