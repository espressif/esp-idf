#define CATCH_CONFIG_MAIN  // This tells the catch header to generate a main
#include "catch.hpp"
#include "mqtt_client.h"

extern "C" {
#include "Mockesp_event.h"
#include "Mockesp_log.h"
#include "Mockesp_system.h"
#include "Mockesp_mac.h"
#include "Mockesp_transport.h"
#include "Mockesp_transport_ssl.h"
#include "Mockesp_transport_tcp.h"
#include "Mockesp_transport_ws.h"
#include "Mockevent_groups.h"
#include "Mockhttp_parser.h"
#include "Mockqueue.h"
#include "Mocktask.h"

    /*
     * The following functions are not directly called but the generation of them
     * from cmock is broken, so we need to define them here.
     */
    BaseType_t xQueueTakeMutexRecursive(QueueHandle_t xMutex,
                                        TickType_t xTicksToWait)
    {
        return 0;
    }
    BaseType_t xQueueGiveMutexRecursive(QueueHandle_t xMutex)
    {
        return 0;
    }
}

struct ClientInitializedFixture {
    esp_mqtt_client_handle_t client;
    ClientInitializedFixture()
    {
        TEST_PROTECT();
        int mtx;
        int transport_list;
        int transport;
        int event_group;
        uint8_t mac[] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
        esp_log_write_Ignore();
        xQueueCreateMutex_ExpectAnyArgsAndReturn(
            reinterpret_cast<QueueHandle_t>(&mtx));
        xEventGroupCreate_IgnoreAndReturn(reinterpret_cast<EventGroupHandle_t>(&event_group));
        esp_log_timestamp_IgnoreAndReturn(0);
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
    struct http_parser_url ret_uri;
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
        xTaskCreatePinnedToCore_ExpectAnyArgsAndReturn(pdTRUE);
        auto res = esp_mqtt_client_start(client);
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
