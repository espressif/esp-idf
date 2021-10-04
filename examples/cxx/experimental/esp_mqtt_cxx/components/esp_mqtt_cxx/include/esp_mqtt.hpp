// Copyright 2021 Espressif Systems (Shanghai) CO LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//         http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include <string_view>
#ifndef __cpp_exceptions
#error MQTT class can only be used when __cpp_exceptions is enabled. Enable CONFIG_COMPILER_CXX_EXCEPTIONS in Kconfig
#endif

#include <optional>
#include <variant>
#include <utility>
#include <memory>
#include <string>
#include "esp_exception.hpp"
#include "esp_mqtt_client_config.hpp"
#include "mqtt_client.h"

namespace idf::mqtt {

constexpr auto *TAG = "mqtt_client_cpp";

struct MQTTException : ESPException {
    using ESPException::ESPException;
};

/**
 * @brief QoS for publish and subscribe
 *
 * Sets the QoS as:
 * AtMostOnce  : Best effort delivery of messages. Message loss can occur.
 * AtLeastOnce : Guaranteed delivery of messages. Duplicates can occur.
 * ExactlyOnce : Guaranteed delivery of messages exactly once.
 *
 */
enum class QoS { AtMostOnce = 0, AtLeastOnce = 1, ExactlyOnce = 2 };

/**
 * @brief Sets if a message must be retained.
 *
 * Retained messages are delivered to future subscribers that match the topic name.
 *
 */
enum class Retain : bool { NotRetained = false, Retained = true };


/**
 * @brief Message class template to publish.
 *
 */
template <typename T> struct Message {
    T data; /*!< Data for publish. Should be a contiguous type*/
    QoS qos = QoS::AtLeastOnce; /*!< QoS for the message*/
    Retain retain = Retain::NotRetained; /*!< Retention mark for the message.*/
};

/**
 * @brief Message type that holds std::string for data
 *
 */
using StringMessage = Message<std::string>;

[[nodiscard]] bool filter_is_valid(std::string::const_iterator first, std::string::const_iterator last);

/**
 * @brief Filter for mqtt topic subscription.
 * @throws std::domain_error if the filter is invalid.
 *
 * Topic filter.
 *
 */
class Filter {
public:

    explicit Filter(std::string user_filter);


    /**
     * @brief Get the filter string used.
     *
     */
    const std::string &get();

    /**
     * @brief Checks the filter string against a topic name.
     *
     * @param first Iterator to the beginning of the sequence.
     * @param last  Iterator to the end of the sequence.
     *
     * @return true if the topic name match the filter
     */
    [[nodiscard]] bool match(std::string::const_iterator first,
                             std::string::const_iterator last) const noexcept;

    /**
     * @brief Checks the filter string against a topic name.
     *
     * @param topic topic name
     *
     * @return true if the topic name match the filter
     */
    [[nodiscard]] bool match(const std::string &topic) const noexcept;

    /**
     * @brief Checks the filter string against a topic name.
     *
     * @param first Char array with topic name.
     * @param last  Size of given topic name.
     *
     * @return true if the topic name match the filter
     */
    [[nodiscard]] bool match(const char *const begin, int size) const noexcept;


private:

    /**
     * @brief Advance the topic to the next level.
     *
     * An mqtt topic ends with a /. This function is used to iterate in topic levels.
     *
     * @return Iterator to the start of the topic.
     */
    [[nodiscard]] std::string::const_iterator advance(std::string::const_iterator begin, std::string::const_iterator end) const;
    std::string filter;
};

/**
 * @brief Message identifier to track delivery.
 *
 */
enum class MessageID : int {};

/**
 * @brief Base class for MQTT client
 *
 * Should be inherited to provide event handlers.
 */
class Client {
public:

    Client(const BrokerConfiguration &broker,const  ClientCredentials &credentials,const  Configuration &config);

    Client(const esp_mqtt_client_config_t &config);

    /**
     * @brief Subscribe to topic
     *
     * @param filter
     * @param qos QoS subscription, defaulted as QoS::AtLeastOnce
     *
     * @return Optional MessageID. In case of failure std::nullopt is returned.
     */
    std::optional<MessageID> subscribe(const std::string &filter, QoS qos = QoS::AtLeastOnce);

    /**
     * @brief publish message to topic
     *
     * @tparam Container Type for data container. Must be a contiguous memory.
     * @param topic Topic name
     * @param message Message struct containing data, qos and retain configuration.
     *
     * @return Optional MessageID. In case of failure std::nullopt is returned.
     */
    template <class Container> std::optional<MessageID> publish(const std::string &topic, const Message<Container>& message)
    {
        return publish(topic, std::begin(message.data), std::end(message.data), message.qos, message.retain);
    }

    /**
     * @brief publish message to topic
     *
     * @tparam InputIt Input data iterator type.
     * @param topic Topic name
     * @param first, last Iterator pair of data to publish
     * @param message Message struct containing data, qos and retain configuration.
     *
     * @return Optional MessageID. In case of failure std::nullopt is returned.
     */
    template <class InputIt>
    std::optional<MessageID> publish(const std::string &topic, InputIt first, InputIt last, QoS qos = QoS::AtLeastOnce, Retain retain = Retain::NotRetained)
    {
        auto size = std::distance(first, last);
        auto res =  esp_mqtt_client_publish(handler.get(), topic.c_str(), &(*first), size, static_cast<int>(qos),
                                            static_cast<int>(retain));
        if (res < 0) {
            return std::nullopt;
        }
        return MessageID{res};
    }

    virtual ~Client() = default;

protected:
    struct MqttClientDeleter {
        void operator()(esp_mqtt_client *client_handler)
        {
            esp_mqtt_client_destroy(client_handler);
        }
    };

    using ClientHandler = std::unique_ptr<esp_mqtt_client, MqttClientDeleter>;
    ClientHandler handler;

private:
    static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id,
                                   void *event_data) noexcept;
    void init(const esp_mqtt_client_config_t &config);
    virtual void on_error(const esp_mqtt_event_handle_t event);
    virtual void on_disconnected(const esp_mqtt_event_handle_t event);
    virtual void on_subscribed(const esp_mqtt_event_handle_t event);
    virtual void on_unsubscribed(const esp_mqtt_event_handle_t event);
    virtual void on_published(const esp_mqtt_event_handle_t event);
    virtual void on_before_connect(const esp_mqtt_event_handle_t event);
    virtual void on_connected(const esp_mqtt_event_handle_t event) = 0;
    virtual void on_data(const esp_mqtt_event_handle_t event) = 0;
};
} // namespace idf::mqtt
