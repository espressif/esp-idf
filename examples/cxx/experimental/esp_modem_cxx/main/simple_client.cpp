/* PPPoS Client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "cxx_include/esp_modem_dte.hpp"
#include "esp_modem_config.h"
#include "cxx_include/esp_modem_api.hpp"
#include "esp_event_cxx.hpp"
#include "simple_mqtt_client.hpp"


#define BROKER_URL "mqtt://mqtt.eclipseprojects.io"

using namespace esp_modem;
using namespace idf::event;

constexpr auto TAG = "cmux_example";


extern "C" void app_main(void)
{
    /* Init and register system/core components */
    auto loop = std::make_shared<ESPEventLoop>();
    ESP_ERROR_CHECK(esp_netif_init());
    /* Configure the DTE */
    esp_modem_dte_config_t dte_config = ESP_MODEM_DTE_DEFAULT_CONFIG();

    /* Configure the DCE */
    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG(CONFIG_EXAMPLE_MODEM_PPP_APN);

    /* Configure the PPP netif */
    esp_netif_config_t netif_ppp_config = ESP_NETIF_DEFAULT_PPP();

    /* Create the DTE, PPP and DCE objects */
    auto uart_dte = create_uart_dte(&dte_config);
    assert(uart_dte);

    esp_netif_t *esp_netif = esp_netif_new(&netif_ppp_config);
    assert(esp_netif);

    #if CONFIG_EXAMPLE_MODEM_DEVICE_BG96 == 1
    std::unique_ptr<DCE> dce = create_BG96_dce(&dce_config, uart_dte, esp_netif);
    #elif CONFIG_EXAMPLE_MODEM_DEVICE_SIM800 == 1
    std::unique_ptr<DCE> dce = create_SIM800_dce(&dce_config, uart_dte, esp_netif);
    #elif CONFIG_EXAMPLE_MODEM_DEVICE_SIM7600 == 1
    std::unique_ptr<DCE> dce = create_SIM7600_dce(&dce_config, uart_dte, esp_netif);
    #else
    #error "Unsupported device"
    #endif
    assert(dce);

    /* Setup basic operation mode for the DCE (pin if used, CMUX mode) */
#if CONFIG_EXAMPLE_NEED_SIM_PIN == 1
    bool pin_ok = true;
    if (dce->read_pin(pin_ok) == command_result::OK && !pin_ok) {
        if (dce->set_pin(CONFIG_EXAMPLE_SIM_PIN) == command_result::OK) {
            vTaskDelay(pdMS_TO_TICKS(1000)); // Need to wait for some time after unlocking the SIM
        } else {
            ESP_LOGE(TAG, "Failed to set PIN... exiting");
            return;
        }
    }
#endif

    if (dce->set_mode(esp_modem::modem_mode::CMUX_MODE) && dce->set_mode(esp_modem::modem_mode::DATA_MODE)) {
        ESP_LOGI(TAG, "Modem has correctly entered multiplexed command/data mode");
    } else {
        ESP_LOGE(TAG, "Failed to configure desired mode... exiting");
        return;
    }

    /* Read some data from the modem */
    std::string str;
    while (dce->get_operator_name(str) != esp_modem::command_result::OK) {
        // Getting operator name could fail... retry after 500 ms
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    ESP_LOGI(TAG, "Operator name: %s", str.c_str());

    /* Try to connect to the network and publish an mqtt topic */
    ESPEventHandlerSync event_handler(loop);
    event_handler.listen_to(ESPEvent(IP_EVENT, ESPEventID(ESP_EVENT_ANY_ID)));
    auto result = event_handler.wait_event_for(std::chrono::milliseconds(60000));
    if (result.timeout) {
        ESP_LOGE(TAG, "Cannot get IP within specified timeout... exiting");
        return;
    } else if (result.event.id == ESPEventID(IP_EVENT_PPP_GOT_IP)) {
        auto *event = (ip_event_got_ip_t *)result.ev_data;
        ESP_LOGI(TAG, "Got IP address");
        ESP_LOGI(TAG, "IP          : " IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI(TAG, "Netmask     : " IPSTR, IP2STR(&event->ip_info.netmask));
        ESP_LOGI(TAG, "Gateway     : " IPSTR, IP2STR(&event->ip_info.gw));

        /* When connected to network, subscribe and publish some MQTT data */
        MqttClient mqtt(BROKER_URL);
        event_handler.listen_to(MqttClient::get_event(MqttClient::Event::CONNECT));
        event_handler.listen_to(MqttClient::get_event(MqttClient::Event::DATA));

        auto reg = loop->register_event(MqttClient::get_event(MqttClient::Event::DATA),
                                        [&mqtt](const ESPEvent &event, void *data) {
                                            ESP_LOGI(TAG, " TOPIC: %s", mqtt.get_topic(data).c_str());
                                            ESP_LOGI(TAG, " DATA: %s", mqtt.get_data(data).c_str());
                                        });
        mqtt.connect();
        while (true) {
            result = event_handler.wait_event_for(std::chrono::milliseconds(60000));
            if (result.event == MqttClient::get_event(MqttClient::Event::CONNECT)) {
                mqtt.subscribe("/topic/esp-modem");
                mqtt.publish("/topic/esp-modem", "Hello modem");
                continue;
            } else if (result.event == MqttClient::get_event(MqttClient::Event::DATA)) {
                ESP_LOGI(TAG, "Data received");
                break; /* Continue with CMUX example after getting data from MQTT */
            } else {
                break;
            }
        }

    } else if (result.event.id == ESPEventID(IP_EVENT_PPP_LOST_IP)) {
        ESP_LOGE(TAG, "PPP client has lost connection... exiting");
        return;
    }

    /* Again reading some data from the modem */
    if (dce->get_imsi(str) == esp_modem::command_result::OK) {
        ESP_LOGI(TAG, "Modem IMSI number: %s", str.c_str());
    }
}
