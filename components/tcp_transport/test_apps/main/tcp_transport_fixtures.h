/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#ifndef _TCP_TRANSPORT_FIXTURES_H_
#define _TCP_TRANSPORT_FIXTURES_H_

/**
 * @brief Structures and types for passing socket options
 */
enum expected_sock_option_types {
    SOCK_OPT_TYPE_BOOL,
    SOCK_OPT_TYPE_INT,
};
struct expected_sock_option {
    int level;
    int optname;
    int optval;
    enum expected_sock_option_types opttype;
};

/**
 * @brief Helper test functions for timeout connection tests
 *
 * This case simulates connection timeout running tcp connect asynchronously with other socket connection
 * consuming entire socket listener backlog.
 * Important: Both tasks must run on the same core, with listener's prio higher to make sure that
 * 1) first the localhost_listener() creates and connects all sockets until the last one blocks
 * 2) before the tcp_connect_task() attempts to connect and thus fails with connection timeout
 */
void tcp_transport_test_connection_timeout(esp_transport_handle_t transport_under_test);


/**
 * @brief Helper test function to check socket options configured separately by transports
 *
 * This sets up the connection test to start two tasks, but unlike tcp_transport_test_connection_timeout,
 * here we just let the connection to happen or at least open on TCP layer so we get the internal socket
 * descriptor. While the connection is in progress or connected, we can check the socket options configured
 * by the tcp_transport API.
 */
void tcp_transport_test_socket_options(esp_transport_handle_t transport_under_test, bool async,
                                       const struct expected_sock_option *expected_opts, size_t sock_options_len);

#endif //_TCP_TRANSPORT_FIXTURES_H_
