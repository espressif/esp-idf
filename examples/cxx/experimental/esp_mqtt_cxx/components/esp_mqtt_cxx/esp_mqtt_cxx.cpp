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

#include <string>
#include <algorithm>

#include "mqtt_client.h"
#include "esp_log.h"

#include "esp_mqtt.hpp"
#include "esp_mqtt_client_config.hpp"

namespace {

// Helper for static assert.
template<class T>
constexpr bool always_false = false;

template<class... Ts> struct overloaded : Ts... {
    using Ts::operator()...;
};
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

using namespace idf::mqtt;

/*
 *  This function is responsible for fill in the configurations for the broker related data
 *  of mqtt_client_config_t
 */
void config_broker(esp_mqtt_client_config_t &mqtt_client_cfg, BrokerConfiguration const &broker)
{
    std::visit(overloaded{
        [&mqtt_client_cfg](Host const & host)
        {
            mqtt_client_cfg.host = host.address.c_str();
            mqtt_client_cfg.path = host.path.c_str();
            mqtt_client_cfg.transport = host.transport;
        },
        [&mqtt_client_cfg](URI const & uri)
        {
            mqtt_client_cfg.uri = uri.address.c_str();
        },
        []([[maybe_unused ]]auto & unknown)
        {
            static_assert(always_false<decltype(unknown)>, "Missing type handler for variant handler");
        }
    },
    broker.address.address);

    std::visit(overloaded{
        []([[maybe_unused]]Insecure const & insecure) {},
        [&mqtt_client_cfg](GlobalCAStore const & use_global_store)
        {
            mqtt_client_cfg.use_global_ca_store = true;
        },
        [&mqtt_client_cfg](CryptographicInformation const & certificates)
        {
            std::visit(overloaded{
                [&mqtt_client_cfg](PEM const & pem)
                {
                    mqtt_client_cfg.cert_pem = pem.data;
                }, [&mqtt_client_cfg](DER const & der)
                {
                    mqtt_client_cfg.cert_pem = der.data;
                    mqtt_client_cfg.cert_len = der.len;
                }}, certificates);
        },
        []([[maybe_unused]] PSK const & psk) {},
        []([[maybe_unused]] auto & unknown)
        {
            static_assert(always_false<decltype(unknown)>, "Missing type handler for variant handler");
        }
    },
    broker.security);
    mqtt_client_cfg.port = broker.address.port;
}

/*
 *  This function is responsible for fill in the configurations for the client credentials related data
 *  of mqtt_client_config_t
 */
void config_client_credentials(esp_mqtt_client_config_t &mqtt_client_cfg, ClientCredentials const &credentials)
{
    mqtt_client_cfg.client_id = credentials.client_id.has_value() ?  credentials.client_id.value().c_str() : nullptr ;
    mqtt_client_cfg.username = credentials.username.has_value() ?  credentials.username.value().c_str() : nullptr ;
    std::visit(overloaded{
        [&mqtt_client_cfg](Password const & password)
        {
            mqtt_client_cfg.password = password.data.c_str();
        },
        [](ClientCertificate const & certificate) {},
        [](SecureElement const & enable_secure_element) {},
        []([[maybe_unused ]]auto & unknown)
        {
            static_assert(always_false<decltype(unknown)>, "Missing type handler for variant handler");
        }
    }, credentials.authentication);
}

esp_mqtt_client_config_t make_config(BrokerConfiguration const &broker, ClientCredentials const  &credentials, Configuration const &config)
{
    esp_mqtt_client_config_t mqtt_client_cfg{};
    config_broker(mqtt_client_cfg, broker);
    config_client_credentials(mqtt_client_cfg, credentials);
    return mqtt_client_cfg;
}
}

namespace idf::mqtt {

Client::Client(BrokerConfiguration const &broker, ClientCredentials const  &credentials, Configuration const &config): Client(make_config(broker, credentials, config))  {}

Client::Client(esp_mqtt_client_config_t const &config) :  handler(esp_mqtt_client_init(&config))
{
    if (handler == nullptr) {
        throw MQTTException(ESP_FAIL);
    };
    CHECK_THROW_SPECIFIC(esp_mqtt_client_register_event(handler.get(), MQTT_EVENT_ANY, mqtt_event_handler, this), mqtt::MQTTException);
    CHECK_THROW_SPECIFIC(esp_mqtt_client_start(handler.get()), mqtt::MQTTException);
}


void Client::mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) noexcept
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    auto *event = static_cast<esp_mqtt_event_t *>(event_data);
    auto &client = *static_cast<Client *>(handler_args);
    switch (event->event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        client.on_connected(event);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        client.on_disconnected(event);
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        client.on_subscribed(event);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        client.on_unsubscribed(event);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        client.on_published(event);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        client.on_data(event);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        client.on_error(event);
        break;
    case MQTT_EVENT_BEFORE_CONNECT:
        ESP_LOGI(TAG, "MQTT_EVENT_BEFORE_CONNECT");
        client.on_before_connect(event);
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void Client::on_error(esp_mqtt_event_handle_t const event)
{
    auto log_error_if_nonzero = [](const char *message, int error_code) {
        if (error_code != 0) {
            ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
        }
    };
    if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
        log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
        log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
        log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
        ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
    }
}
void Client::on_disconnected(esp_mqtt_event_handle_t const event)
{
}
void Client::on_subscribed(esp_mqtt_event_handle_t const event)
{
    printf("Subscribed to %.*s\r\n", event->topic_len, event->topic);
}
void Client::on_unsubscribed(esp_mqtt_event_handle_t const event)
{
}
void Client::on_published(esp_mqtt_event_handle_t const event)
{
}
void Client::on_before_connect(esp_mqtt_event_handle_t const event)
{
}
void Client::on_connected(esp_mqtt_event_handle_t const event)
{
}
void Client::on_data(esp_mqtt_event_handle_t const event)
{
}

std::optional<MessageID> Client::subscribe(std::string const &topic, QoS qos)
{
    auto res = esp_mqtt_client_subscribe(handler.get(), topic.c_str(),
                                         static_cast<int>(qos));
    if (res < 0) {
        return std::nullopt;
    }
    return MessageID{res};
}

bool is_valid(std::string::const_iterator first, std::string::const_iterator last)
{
    if (first == last) {
        return false;
    }
    auto number = std::find(first, last, '#');
    if (number != last) {
        if (std::next(number) != last) {
            return false;
        }
        if (*std::prev(number) != '/' && number != first) {
            return false;
        }
    }

    auto plus = std::find(first, last, '+');
    if (plus != last) {
        if (*(std::prev(plus)) != '/' && plus != first) {
            return false;
        }
        if (std::next(plus) != last && *(std::next(plus)) != '/') {
            return false;
        }
    }
    return true;
}

Filter::Filter(std::string user_filter) : filter(std::move(user_filter))
{
    if (!is_valid(filter.begin(), filter.end())) {
        throw std::domain_error("Forbidden Filter string");
    }
}

[[nodiscard]] bool Filter::match(std::string::const_iterator topic_begin, std::string::const_iterator topic_end) const noexcept
{
    auto filter_begin = filter.begin();
    auto filter_end = filter.end();
    for (auto mismatch = std::mismatch(filter_begin, filter_end, topic_begin);
            mismatch.first != filter.end() and mismatch.second != topic_end;
            mismatch = std::mismatch(filter_begin, filter_end, topic_begin)) {
        if (*mismatch.first != '#' and * mismatch.first != '+') {
            return false;
        }
        if (*mismatch.first == '#') {
            return true;
        }
        if (*mismatch.first == '+') {
            filter_begin = advance(mismatch.first, filter_end);
            topic_begin = advance(mismatch.second, topic_end);
            if (filter_begin == filter_end and topic_begin != topic_end) {
                return false;
            }
        }
    }
    return true;
}
const std::string &Filter::get()
{
    return filter;
}

[[nodiscard]] bool Filter::match(char const *const first, int size) const noexcept
{
    auto it = static_cast<std::string::const_iterator>(first);
    return match(it, it + size);
}
std::string::const_iterator Filter::advance(std::string::const_iterator first, std::string::const_iterator last) const
{
    constexpr auto separator = '/';
    return std::find(first, last, separator);
}

}
