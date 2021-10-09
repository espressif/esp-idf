/* PPPoS Client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <memory>
#include "mqtt_client.h"
#include "esp_event_cxx.hpp"
#include "simple_mqtt_client.hpp"

using namespace idf::event;

/**
 * Reference to the MQTT event base
 */
ESP_EVENT_DECLARE_BASE(MQTT_EVENTS);

/**
 * Thin wrapper around C mqtt_client
 */
struct MqttClientHandle
{
    explicit MqttClientHandle(const std::string & uri)
    {
        esp_mqtt_client_config_t config = { };
        config.uri = uri.c_str();
        client = esp_mqtt_client_init(&config);
        esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, this);
    }

    ~MqttClientHandle()
    {
        esp_mqtt_client_destroy(client);
    }

    static void mqtt_event_handler(void *args, esp_event_base_t base, int32_t id, void *data)
    {
        // forwards the internal event to the global ESPEvent
        esp_event_post(base, id, data, sizeof(esp_mqtt_event_t), portMAX_DELAY);
    }

    esp_mqtt_client_handle_t client;
};

/**
 * @brief Definitions of MqttClient methods
 */
MqttClient::MqttClient(const std::string & uri):
    h(std::unique_ptr<MqttClientHandle>(new MqttClientHandle(uri)))
    {}

void MqttClient::connect()
{
    esp_mqtt_client_start(h->client);
}

idf::event::ESPEvent MqttClient::get_event(MqttClient::Event ev)
{
    switch (ev) {
        case Event::CONNECT: {
            return { MQTT_EVENTS, ESPEventID(MQTT_EVENT_CONNECTED) };
        }
        case Event::DATA:
            return { MQTT_EVENTS, ESPEventID(MQTT_EVENT_DATA) };
    }
    return { };
}

int MqttClient::publish(const std::string &topic, const std::string &data, int qos)
{
    return esp_mqtt_client_publish(h->client, topic.c_str(), data.c_str(), 0, qos, 0);
}

int MqttClient::subscribe(const std::string &topic, int qos)
{
    return esp_mqtt_client_subscribe(h->client, topic.c_str(), qos);
}

std::string MqttClient::get_topic(void * event_data)
{
    auto event = (esp_mqtt_event_handle_t)event_data;
    if (event == nullptr || event->client != h->client)
        return {};

    return std::string(event->topic, event->topic_len);
}

std::string MqttClient::get_data(void * event_data)
{
    auto event = (esp_mqtt_event_handle_t)event_data;
    if (event == nullptr || event->client != h->client)
        return {};

    return std::string(event->data, event->data_len);
}

MqttClient::~MqttClient() = default;
