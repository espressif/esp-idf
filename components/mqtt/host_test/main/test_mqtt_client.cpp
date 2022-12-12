#define CATCH_CONFIG_MAIN  // This tells the catch header to generate a main
#include "catch.hpp"

extern "C" {
#include "Mockesp_event.h"
#include "Mockesp_mac.h"
#include "Mockesp_transport.h"
#include "Mockesp_transport_ssl.h"
#include "Mockesp_transport_tcp.h"
#include "Mockesp_transport_ws.h"
#include "Mockevent_groups.h"
#include "Mockhttp_parser.h"
#include "Mockqueue.h"
#include "Mocktask.h"
#include "Mockesp_timer.h"

    /*
     * The following functions are not directly called but the generation of them
     * from cmock is broken, so we need to define them here.
     */
    esp_err_t esp_tls_get_and_clear_last_error(esp_tls_error_handle_t h, int *esp_tls_code, int *esp_tls_flags)
    {
        return ESP_OK;
    }
}

#include "mqtt_client.h"

struct ClientInitializedFixture {
    esp_mqtt_client_handle_t client;
    ClientInitializedFixture()
    {
        [[maybe_unused]] auto protect = TEST_PROTECT();
        int mtx;
        int transport_list;
        int transport;
        int event_group;
        uint8_t mac[] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
        esp_timer_get_time_IgnoreAndReturn(0);
        xQueueTakeMutexRecursive_IgnoreAndReturn(true);
        xQueueGiveMutexRecursive_IgnoreAndReturn(true);
        xQueueCreateMutex_ExpectAnyArgsAndReturn(
            reinterpret_cast<QueueHandle_t>(&mtx));
        xEventGroupCreate_IgnoreAndReturn(reinterpret_cast<EventGroupHandle_t>(&event_group));
        esp_transport_list_init_IgnoreAndReturn(reinterpret_cast<esp_transport_list_handle_t>(&transport_list));
        esp_transport_tcp_init_IgnoreAndReturn(reinterpret_cast<esp_transport_handle_t>(&transport));
        esp_transport_ssl_init_IgnoreAndReturn(reinterpret_cast<esp_transport_handle_t>(&transport));
        esp_transport_ws_init_IgnoreAndReturn(reinterpret_cast<esp_transport_handle_t>(&transport));
        esp_transport_ws_set_subprotocol_IgnoreAndReturn(ESP_OK);
        esp_transport_list_add_IgnoreAndReturn(ESP_OK);
        esp_transport_set_default_port_IgnoreAndReturn(ESP_OK);
        http_parser_parse_url_IgnoreAndReturn(0);
        http_parser_url_init_ExpectAnyArgs();
        esp_event_loop_create_IgnoreAndReturn(ESP_OK);
        esp_read_mac_IgnoreAndReturn(ESP_OK);
        esp_read_mac_ReturnThruPtr_mac(mac);
        esp_transport_list_destroy_IgnoreAndReturn(ESP_OK);
        vEventGroupDelete_Ignore();
        vQueueDelete_Ignore();

        esp_mqtt_client_config_t config{};
        client = esp_mqtt_client_init(&config);
    }
    ~ClientInitializedFixture()
    {
        esp_mqtt_client_destroy(client);
    }
};
TEST_CASE_METHOD(ClientInitializedFixture, "Client set uri")
{
        struct http_parser_url ret_uri = {
            .field_set = 1,
            .port = 0,
            .field_data = { { 0, 1} }
        };
    SECTION("User set a correct URI") {
        http_parser_parse_url_StopIgnore();
        http_parser_parse_url_ExpectAnyArgsAndReturn(0);
        http_parser_parse_url_ReturnThruPtr_u(&ret_uri);
        auto res = esp_mqtt_client_set_uri(client, " ");
        REQUIRE(res == ESP_OK);
    }
    SECTION("Incorrect URI from user") {
        http_parser_parse_url_StopIgnore();
        http_parser_parse_url_ExpectAnyArgsAndReturn(1);
        http_parser_parse_url_ReturnThruPtr_u(&ret_uri);
        auto res = esp_mqtt_client_set_uri(client, " ");
        REQUIRE(res == ESP_FAIL);
    }
}
TEST_CASE_METHOD(ClientInitializedFixture, "Client Start")
{
    SECTION("Successful start") {
        esp_mqtt_client_config_t config{};
        config.broker.address.uri = "mqtt://1.1.1.1";
        struct http_parser_url ret_uri = {
            .field_set = 1 | (1<<1),
            .port = 0,
            .field_data = { { 0, 4 } /*mqtt*/, { 7, 1 } } // at least *scheme* and *host*
        };
        http_parser_parse_url_StopIgnore();
        http_parser_parse_url_ExpectAnyArgsAndReturn(0);
        http_parser_parse_url_ReturnThruPtr_u(&ret_uri);
        xTaskCreatePinnedToCore_ExpectAnyArgsAndReturn(pdTRUE);
        auto res = esp_mqtt_set_config(client, &config);
        REQUIRE(res == ESP_OK);
        res = esp_mqtt_client_start(client);
        REQUIRE(res == ESP_OK);
    }
    SECTION("Failed on initialization") {
        xTaskCreatePinnedToCore_ExpectAnyArgsAndReturn(pdFALSE);
        auto res = esp_mqtt_client_start(nullptr);
        REQUIRE(res == ESP_ERR_INVALID_ARG);
    }
    SECTION("Client already started") {}
    SECTION("Failed to start task") {
        xTaskCreatePinnedToCore_ExpectAnyArgsAndReturn(pdFALSE);
        auto res = esp_mqtt_client_start(client);
        REQUIRE(res == ESP_FAIL);
    }
}
