/* C++ MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <cstdint>
#include <string>
#include "esp_mqtt_client_config.hpp"
#include "nvs_flash.h"
#include "protocol_examples_common.h"


#include "esp_log.h"
#include "esp_mqtt.hpp"

namespace {
constexpr auto *TAG = "MQTT_EXAMPLE";

extern const char mqtt_eclipse_org_pem_start[]   asm("_binary_mqtt_eclipseprojects_io_pem_start");
extern const char mqtt_eclipse_org_pem_end[]   asm("_binary_mqtt_eclipseprojects_io_pem_end");

class MyClient final : public idf::mqtt::Client {
public:
    using idf::mqtt::Client::Client;
private:
    void on_connected(esp_mqtt_event_handle_t const event) override
    {
        using idf::mqtt::QoS;
        subscribe(messages.get());
        subscribe(sent_load.get(), QoS::AtMostOnce);
    }
    void on_data(esp_mqtt_event_handle_t const event) override
    {
        if (messages.match(event->topic, event->topic_len)) {
            ESP_LOGI(TAG, "Received in the messages topic");
        }
    }
    idf::mqtt::Filter messages{std::string{"$SYS/broker/messages/received"}};
    idf::mqtt::Filter sent_load{std::string{"$SYS/broker/load/+/sent"}};
};
}

namespace mqtt = idf::mqtt;

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    mqtt::BrokerConfiguration broker{
        .address = {mqtt::URI{std::string{CONFIG_BROKER_URI}}},
        .security = mqtt::CryptographicInformation{mqtt::PEM{mqtt_eclipse_org_pem_start}}
    };
    idf::mqtt::ClientCredentials credentials{};
    idf::mqtt::Configuration config{};

    MyClient client{broker, credentials, config};
    while (true) {
        constexpr TickType_t xDelay = 500 / portTICK_PERIOD_MS;
        vTaskDelay( xDelay );
    }
}
