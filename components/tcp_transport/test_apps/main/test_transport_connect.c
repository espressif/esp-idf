/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity_fixture.h"
#include "memory_checks.h"
#include "esp_transport.h"
#include "esp_transport_tcp.h"
#include "esp_transport_ssl.h"
#include "esp_transport_ws.h"
#include "esp_log.h"
#include "lwip/sockets.h"
#include "tcp_transport_fixtures.h"
#include "test_utils.h"

TEST_GROUP(transport_connect);

TEST_SETUP(transport_connect)
{
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(transport_connect)
{
    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

#define TEST_TRANSPORT_BIND_IFNAME() \
    struct ifreq ifr; \
    ifr.ifr_name[0] = 'l'; \
    ifr.ifr_name[1] = 'o'; \
    ifr.ifr_name[2] = '0'; \
    ifr.ifr_name[3] = '\0';


static void tcp_transport_keepalive_test(esp_transport_handle_t transport_under_test, bool async, esp_transport_keep_alive_t *config)
{
    static struct expected_sock_option expected_opts[4] = {
        { .level = SOL_SOCKET, .optname = SO_KEEPALIVE, .optval = 1, .opttype = SOCK_OPT_TYPE_BOOL },
        { .level = IPPROTO_TCP },
        { .level = IPPROTO_TCP },
        { .level = IPPROTO_TCP }
    };

    expected_opts[1].optname = TCP_KEEPIDLE;
    expected_opts[1].optval = config->keep_alive_idle;
    expected_opts[2].optname = TCP_KEEPINTVL;
    expected_opts[2].optval = config->keep_alive_interval;
    expected_opts[3].optname = TCP_KEEPCNT;
    expected_opts[3].optval = config->keep_alive_count;

    tcp_transport_test_socket_options(transport_under_test, async, expected_opts,
                                      sizeof(expected_opts) / sizeof(struct expected_sock_option));
}

TEST(transport_connect, tcp_connect_timeout)
{
    // Init the transport under test
    esp_transport_list_handle_t transport_list = esp_transport_list_init();
    esp_transport_handle_t tcp = esp_transport_tcp_init();
    esp_transport_list_add(transport_list, tcp, "tcp");

    tcp_transport_test_connection_timeout(tcp);
    esp_transport_close(tcp);
    esp_transport_list_destroy(transport_list);
}

TEST(transport_connect, ssl_connect_timeout)
{
    // Init the transport under test
    esp_transport_list_handle_t transport_list = esp_transport_list_init();
    esp_transport_handle_t tcp = esp_transport_tcp_init();
    esp_transport_list_add(transport_list, tcp, "tcp");
    esp_transport_handle_t ssl = esp_transport_ssl_init();
    esp_transport_list_add(transport_list, ssl, "ssl");

    tcp_transport_test_connection_timeout(ssl);
    esp_transport_close(tcp);
    esp_transport_close(ssl);
    esp_transport_list_destroy(transport_list);
}

TEST(transport_connect, tcp_keep_alive)
{
    // Init the transport under test
    esp_transport_list_handle_t transport_list = esp_transport_list_init();
    esp_transport_handle_t tcp = esp_transport_tcp_init();
    esp_transport_list_add(transport_list, tcp, "tcp");

    // Perform the test
    esp_transport_keep_alive_t  keep_alive_cfg = {
        .keep_alive_interval = 5,
        .keep_alive_idle = 4,
        .keep_alive_enable = true,
        .keep_alive_count = 3
    };
    esp_transport_tcp_set_keep_alive(tcp, &keep_alive_cfg);

    // Bind device interface to loopback
    TEST_TRANSPORT_BIND_IFNAME();
    esp_transport_tcp_set_interface_name(tcp, &ifr);

    // Run the test for both sync and async_connect
    tcp_transport_keepalive_test(tcp, true, &keep_alive_cfg);
    tcp_transport_keepalive_test(tcp, false, &keep_alive_cfg);

    // Cleanup
    esp_transport_close(tcp);
    esp_transport_list_destroy(transport_list);
}

TEST(transport_connect, ssl_keep_alive)
{
    // Init the transport under test
    esp_transport_list_handle_t transport_list = esp_transport_list_init();
    esp_transport_handle_t ssl = esp_transport_ssl_init();
    esp_transport_list_add(transport_list, ssl, "ssl");
    esp_tls_init_global_ca_store();
    esp_transport_ssl_enable_global_ca_store(ssl);

    // Perform the test
    esp_transport_keep_alive_t  keep_alive_cfg = {
        .keep_alive_interval = 2,
        .keep_alive_idle = 3,
        .keep_alive_enable = true,
        .keep_alive_count = 4
    };
    esp_transport_ssl_set_keep_alive(ssl, &keep_alive_cfg);

    // Bind device interface to loopback
    TEST_TRANSPORT_BIND_IFNAME();
    esp_transport_ssl_set_interface_name(ssl, &ifr);

    // Run the test for async_connect only
    // - TLS connection would connect on socket level only, returning tls-handshake in progress
    tcp_transport_keepalive_test(ssl, true, &keep_alive_cfg);

    // Cleanup
    esp_transport_close(ssl);
    esp_transport_list_destroy(transport_list);
}

TEST(transport_connect, ws_keep_alive)
{
    // Init the transport under test
    esp_transport_list_handle_t transport_list = esp_transport_list_init();
    esp_transport_handle_t tcp = esp_transport_tcp_init();
    esp_transport_list_add(transport_list, tcp, "tcp");
    esp_transport_handle_t ws = esp_transport_ws_init(tcp);
    esp_transport_list_add(transport_list, ws, "ws");

    // Perform the test
    esp_transport_keep_alive_t  keep_alive_cfg = {
        .keep_alive_interval = 11,
        .keep_alive_idle = 22,
        .keep_alive_enable = true,
        .keep_alive_count = 33
    };
    esp_transport_tcp_set_keep_alive(tcp, &keep_alive_cfg);

    // Bind device interface to loopback
    TEST_TRANSPORT_BIND_IFNAME();
    esp_transport_ssl_set_interface_name(tcp, &ifr);

    // Run the test for sync_connect only (ws doesn't support async)
    tcp_transport_keepalive_test(ws, false, &keep_alive_cfg);

    // Cleanup
    esp_transport_close(tcp);
    esp_transport_list_destroy(transport_list);
}

// Note: This functionality is tested and kept only for compatibility reasons with IDF <= 4.x
//       It is strongly encouraged to use transport within lists only
TEST(transport_connect, ssl_set_parameter_independently)
{
    // Init the transport under test
    esp_transport_handle_t ssl = esp_transport_ssl_init();
    esp_tls_init_global_ca_store();
    esp_transport_ssl_enable_global_ca_store(ssl);

    // Perform the test
    esp_transport_keep_alive_t  keep_alive_cfg = {
        .keep_alive_interval = 2,
        .keep_alive_idle = 4,
        .keep_alive_enable = true,
        .keep_alive_count = 3
    };
    esp_transport_ssl_set_keep_alive(ssl, &keep_alive_cfg);

    // Bind device interface to loopback
    TEST_TRANSPORT_BIND_IFNAME();
    esp_transport_ssl_set_interface_name(ssl, &ifr);

    tcp_transport_keepalive_test(ssl, true, &keep_alive_cfg);

    // Cleanup
    esp_transport_close(ssl);
    esp_transport_destroy(ssl);
}

TEST_GROUP_RUNNER(transport_connect)
{
    RUN_TEST_CASE(transport_connect, tcp_connect_timeout);
    RUN_TEST_CASE(transport_connect, ssl_connect_timeout);
    RUN_TEST_CASE(transport_connect, tcp_keep_alive);
    RUN_TEST_CASE(transport_connect, ssl_keep_alive);
    RUN_TEST_CASE(transport_connect, ws_keep_alive);
    RUN_TEST_CASE(transport_connect, ssl_set_parameter_independently);
}
