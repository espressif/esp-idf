/* PPPoS Client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#ifndef _SIMPLE_MQTT_CLIENT_H_
#define _SIMPLE_MQTT_CLIENT_H_

#include <string>
#include <memory>
#include "esp_event_cxx.hpp"

struct MqttClientHandle;

/**
 * @brief Simple MQTT client wrapper
 */
class MqttClient {
public:
    enum class Event {
        CONNECT,
        DATA,
    };

    explicit MqttClient(const std::string & uri);
    ~MqttClient();

    /**
     * @brief Start the mqtt-client
     */
    void connect();

    /**
     * @brief Publish to topic
     * @param topic Topic to publish
     * @param data Data to publish
     * @param qos QoS (0 by default)
     * @return message id
     */
    int publish(const std::string & topic, const std::string & data, int qos = 0);

    /**
     * @brief Subscribe to a topic
     * @param topic Topic to subscribe
     * @param qos QoS (0 by default)
     * @return message id
     */
    int subscribe(const std::string & topic, int qos = 0);

    /**
     * @brief Get topic from event data
     * @return String topic
     */
    std::string get_topic(void *);

    /**
     * @brief Get published data from event
     * @return String representation of the data
     */
    std::string get_data(void *);

    /**
     * @brief Convert internal MQTT event to standard ESPEvent
     * @param ev internal mqtt event
     * @return corresponding ESPEvent
     */
    static idf::event::ESPEvent get_event(Event ev);

private:
    std::unique_ptr<MqttClientHandle> h;
};


#endif //_SIMPLE_MQTT_CLIENT_H_
