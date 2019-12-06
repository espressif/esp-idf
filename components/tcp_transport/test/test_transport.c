#include "unity.h"

#include "esp_transport.h"
#include "esp_transport_tcp.h"
#include "esp_transport_ssl.h"
#include "esp_transport_ws.h"

TEST_CASE("tcp_transport: init and deinit transport list", "[tcp_transport][leaks=0]")
{
    esp_transport_list_handle_t transport_list = esp_transport_list_init();
    esp_transport_handle_t tcp = esp_transport_tcp_init();
    esp_transport_list_add(transport_list, tcp, "tcp");
    TEST_ASSERT_EQUAL(ESP_OK, esp_transport_list_destroy(transport_list));
}

TEST_CASE("tcp_transport: using ssl transport separately", "[tcp_transport][leaks=0]")
{
    esp_transport_handle_t h = esp_transport_ssl_init();
    TEST_ASSERT_EQUAL(ESP_OK, esp_transport_destroy(h));
}

TEST_CASE("tcp_transport: using ws transport separately", "[tcp_transport][leaks=0]")
{
    esp_transport_handle_t tcp = esp_transport_tcp_init();
    esp_transport_handle_t ws = esp_transport_ws_init(tcp);
    TEST_ASSERT_EQUAL(ESP_OK, esp_transport_destroy(ws));
    TEST_ASSERT_EQUAL(ESP_OK, esp_transport_destroy(tcp));
}
