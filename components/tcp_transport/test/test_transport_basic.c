#include "unity.h"

#include "esp_transport.h"
#include "esp_transport_tcp.h"
#include "esp_transport_ssl.h"
#include "esp_transport_ws.h"
#include "esp_log.h"


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

TEST_CASE("transport: init and deinit multiple transport items", "[tcp_transport][leaks=0]")
{
    esp_transport_list_handle_t transport_list = esp_transport_list_init();
    esp_transport_handle_t tcp = esp_transport_tcp_init();
    esp_transport_list_add(transport_list, tcp, "tcp");
    esp_transport_handle_t ssl = esp_transport_ssl_init();
    esp_transport_list_add(transport_list, ssl, "ssl");
    esp_transport_handle_t ws = esp_transport_ws_init(tcp);
    esp_transport_list_add(transport_list, ws, "ws");
    esp_transport_handle_t wss = esp_transport_ws_init(ssl);
    esp_transport_list_add(transport_list, wss, "wss");
    TEST_ASSERT_EQUAL(ESP_OK, esp_transport_list_destroy(transport_list));
}
