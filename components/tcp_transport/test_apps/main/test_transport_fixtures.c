/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity.h"

#include "esp_transport.h"
#include "esp_transport_tcp.h"
#include "test_utils.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "freertos/event_groups.h"
#include "tcp_transport_fixtures.h"
#include "unity_test_utils.h"

#define LISTENER_TASK_PRIORITY (UNITY_FREERTOS_PRIORITY+1)
#define CONNECT_TASK_PRIORITY  (LISTENER_TASK_PRIORITY+1)
#define CONNECT_TASK_PRIORITY_LOWER (LISTENER_TASK_PRIORITY-2)

// This is a private API of the tcp transport, but needed for socket operation tests
int esp_transport_get_socket(esp_transport_handle_t t);

/**
 * @brief Event flags for synchronization between the listener task, the connection task and the test task
 */
enum {
    TCP_CONNECT_DONE        = 1 << 0,    /*!< Indicates that the connection task has finished, so the transport_connect() exited */
    TCP_LISTENER_DONE       = 1 << 1,    /*!< Indicates that the listener task has finished either with success for failure */
    TCP_TEST_DONE           = 1 << 2,    /*!< Indicates that the test case finished, test tear-down() called */
    TCP_LISTENER_READY      = 1 << 3,    /*!< Indicates that the listener task is ready to accept connections */
    TCP_LISTENER_ACCEPTED   = 1 << 4,    /*!< Indicates that the listener task has accepted a connection (from transport_connect()) */
};

/**
 * @brief Connection test configuration parameters
 */
struct tcp_connect_test_config {
    esp_transport_handle_t transport_under_test;
    bool accept_connection;
    bool consume_sock_backlog;
    bool connect_async;
    int timeout_ms;
    int port;
    bool connect_task_prio_lower;
};

/**
 * @brief Test setup structure containing all the info needed for the connection tests
 */
struct tcp_connect_test_storage {
    struct tcp_connect_test_config config;
    TickType_t max_wait;
    EventGroupHandle_t tcp_connect_done;
    int connect_return_value;
    int listen_sock;
    int accepted_sock;
    int last_connect_sock;
    bool tcp_listener_failed;
    TaskHandle_t listener_task;
    TaskHandle_t tcp_connect_task;
};

typedef struct tcp_connect_test_storage *tcp_connect_test_t;

/**
 * @brief Recursively connects with a new socket to loopback interface until the last one blocks.
 * The last socket is closed upon test teardown, that initiates recursive cleanup (close) for all
 * active/connected sockets.
 */
static void connect_once(struct tcp_connect_test_storage *storage)
{
    struct sockaddr_in dest_addr_ip4 = {    .sin_addr.s_addr = htonl(INADDR_LOOPBACK),
            .sin_family = AF_INET,
            .sin_port = htons(storage->config.port)    };
    int connect_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (connect_sock < 0) {
        storage->tcp_listener_failed = true;
        return;
    }
    storage->last_connect_sock = connect_sock;
    int err = connect(connect_sock,  (struct sockaddr *)&dest_addr_ip4, sizeof(dest_addr_ip4));
    if (err != 0) {
        // The last connection is expected to fail here, since the both sockets get closed on test cleanup
        return;
    }
    connect_once(storage);
    close(connect_sock);
}

/**
 * @brief creates a listener (and an acceptor if configured)
 *
 * if consume_sock_backlog set: connect as many times as possible to prepare an endpoint which
 * would make the client block but not complete TCP handshake
 *
 * if accept_connection set: waiting normally for connection creating an acceptor to mimic tcp-transport endpoint
 */
static void localhost_listener(void *pvParameters)
{
    const char* TAG = "tcp_transport_test";
    struct tcp_connect_test_storage *storage = pvParameters;
    struct sockaddr_in dest_addr_ip4 = {    .sin_addr.s_addr = htonl(INADDR_ANY),
            .sin_family = AF_INET,
            .sin_port = htons(storage->config.port)    };
    // Create listener socket and bind it to ANY address
    storage->listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    int opt = 1;
    setsockopt(storage->listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (storage->listen_sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket");
        storage->tcp_listener_failed = true;
        goto failed;
    }
    int err = bind(storage->listen_sock, (struct sockaddr *)&dest_addr_ip4, sizeof(dest_addr_ip4));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        storage->tcp_listener_failed = true;
        goto failed;
    }

    // Listen with backlog set to a low number
    err = listen(storage->listen_sock, 4);
    if (err != 0) {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        storage->tcp_listener_failed = true;
        goto failed;
    }

    // Listener is ready at this point
    xEventGroupSetBits(storage->tcp_connect_done, TCP_LISTENER_READY);

    if (storage->config.consume_sock_backlog) {
        // Ideally we would set backlog to 0, but since this is an implementation specific recommendation parameter,
        // we recursively create sockets and try to connect to this listener in order to consume the backlog. After
        // the backlog is consumed, the last connection blocks (waiting for accept), but at that point we are sure
        // that any other connection would also block
        connect_once(storage);
    } else if (storage->config.accept_connection) {
        struct sockaddr_storage source_addr;
        socklen_t addr_len = sizeof(source_addr);
        storage->accepted_sock = accept(storage->listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (storage->accepted_sock < 0) {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            goto failed;
        }
        xEventGroupSetBits(storage->tcp_connect_done, TCP_LISTENER_ACCEPTED); // Mark the socket as accepted
        // ...and wait for the "acceptor" tests to finish
        xEventGroupWaitBits(storage->tcp_connect_done, TCP_TEST_DONE, true, true, storage->config.timeout_ms * 10);
    }

failed:
    xEventGroupSetBits(storage->tcp_connect_done, TCP_LISTENER_DONE);
    vTaskSuspend(NULL);
}

/**
 * @brief This task simply tries to connect to localhost (server provided by listner's task) using tcp_transport
 */
static void tcp_connect_task(void *pvParameters)
{
    struct tcp_connect_test_storage *storage = pvParameters;

    int (*connect_fn)(esp_transport_handle_t, const char *, int, int) =
            storage->config.connect_async ? esp_transport_connect_async : esp_transport_connect;

    storage->connect_return_value = connect_fn(storage->config.transport_under_test, "localhost", storage->config.port, storage->config.timeout_ms);

    if (storage->config.accept_connection) {
        // If we test the accepted connection, need to wait until the test completes
        xEventGroupWaitBits(storage->tcp_connect_done, TCP_TEST_DONE, true, true, storage->config.timeout_ms * 10);
    }
    xEventGroupSetBits(storage->tcp_connect_done, TCP_CONNECT_DONE);
    vTaskSuspend(NULL);
}

static inline void close_if_valid(int *s)
{
    if (*s >= 0) {
        close(*s);
        *s = -1;
    }
}

/**
 * @brief Connect test setup function
 *
 * Creates the Test storage, configures it accordingly and starts two tasks
 * * localhost_listener -- to provide a simple server endpoint for the transport layers to connect to
 * * tcp_connect_task -- to perform the connection
 */
static tcp_connect_test_t connect_test_setup(struct tcp_connect_test_config *config)
{
    tcp_connect_test_t t = calloc(1, sizeof(struct tcp_connect_test_storage));
    if (!t) {
        return NULL;
    }
    memcpy(&t->config, config, sizeof(struct tcp_connect_test_config));
    t->tcp_connect_done = xEventGroupCreate();
    if (!t->tcp_connect_done) {
        return NULL;
    }
    t->max_wait = pdMS_TO_TICKS(config->timeout_ms * 10);

    t->listen_sock = t->last_connect_sock = t->accepted_sock = -1; // mark all sockets invalid

    test_case_uses_tcpip();

    // Create listener task
    xTaskCreatePinnedToCore(localhost_listener, "localhost_listener", 4096, t, LISTENER_TASK_PRIORITY, &t->listener_task, 0);
    xEventGroupWaitBits(t->tcp_connect_done, TCP_LISTENER_READY, true, true, t->max_wait);

    // Perform tcp-connect in a separate task to check asynchronously for the timeout or to connect (depends on the test config)
    xTaskCreatePinnedToCore(tcp_connect_task, "tcp_connect_task", 4096, t,
                            config->connect_task_prio_lower? CONNECT_TASK_PRIORITY_LOWER : CONNECT_TASK_PRIORITY, &t->tcp_connect_task, 0);

    return t;
}

/**
 * @brief Destroys and cleans out the test environment
 */
static void connect_test_teardown(tcp_connect_test_t t)
{
    // Mark the test done and wait for the listener to check if finished with no issues
    xEventGroupSetBits(t->tcp_connect_done, TCP_TEST_DONE);
    xEventGroupWaitBits(t->tcp_connect_done, TCP_LISTENER_DONE, true, true, t->max_wait);
    TEST_ASSERT_EQUAL(false, t->tcp_listener_failed);

    // Closing both parties of the last "blocking" connection to unwind localhost_listener() and let other connected sockets closed
    close_if_valid(&t->listen_sock);
    close_if_valid(&t->last_connect_sock);
    close_if_valid(&t->accepted_sock);

    // Cleanup
    vTaskSuspend(t->tcp_connect_task);
    vTaskSuspend(t->listener_task);
    vEventGroupDelete(t->tcp_connect_done);
    unity_utils_task_delete(t->tcp_connect_task);
    unity_utils_task_delete(t->listener_task);
    free(t);
}

/**
 * @brief Utility function for testing timeouts for different transports
 */
void tcp_transport_test_connection_timeout(esp_transport_handle_t transport_under_test)
{

    struct tcp_connect_test_config params = {
            .timeout_ms = 200,
            .port = 80,
            .consume_sock_backlog = true,
            .connect_async = false,
            .transport_under_test = transport_under_test,
            .connect_task_prio_lower = true
    };

    tcp_connect_test_t test = connect_test_setup(&params);
    TEST_ASSERT_NOT_NULL(test);

    // Roughly measure tick-time spent while trying to connect
#if !CONFIG_FREERTOS_SMP // IDF-5225 - timeout is several times shorter than expected, probably not measured correctly
    TickType_t start = xTaskGetTickCount();
#endif
    EventBits_t bits = xEventGroupWaitBits(test->tcp_connect_done, TCP_CONNECT_DONE, true, true, test->max_wait);
#if !CONFIG_FREERTOS_SMP // IDF-5225 - timeout is several times shorter than expected, probably not measured correctly
    TickType_t end = xTaskGetTickCount();
#endif

    TEST_ASSERT_EQUAL(TCP_CONNECT_DONE, TCP_CONNECT_DONE & bits);       // Connection has finished

#if !CONFIG_FREERTOS_SMP // IDF-5225 - timeout is several times shorter than expected, probably not measured correctly
    TEST_ASSERT_EQUAL(-1, test->connect_return_value);          // Connection failed with -1
    // Test connection attempt took expected timeout value
    TEST_ASSERT_INT_WITHIN(pdMS_TO_TICKS(params.timeout_ms/5), pdMS_TO_TICKS(params.timeout_ms), end-start);
#endif

    // Close the last bound connection, to recursively unwind the consumed backlog
    close_if_valid(&test->last_connect_sock);

    connect_test_teardown(test);
}

/**
 * @brief Utility function for testing timeouts for different transports, options and both sync and async connection
 */
void tcp_transport_test_socket_options(esp_transport_handle_t transport_under_test, bool async,
                                       const struct expected_sock_option *expected_opts, size_t sock_options_len)
{
    struct tcp_connect_test_config params = {
            .timeout_ms = 200,
            .port = 80,
            .accept_connection = true,
            .consume_sock_backlog = false,
            .transport_under_test = transport_under_test,
            .connect_async = async,
            .connect_task_prio_lower = false
    };

    tcp_connect_test_t test = connect_test_setup(&params);
    TEST_ASSERT_NOT_NULL(test);

    // Wait till the connection gets accepted to get the client's socket
    xEventGroupWaitBits(test->tcp_connect_done, TCP_LISTENER_ACCEPTED, true, true, test->max_wait);
    int sock = esp_transport_get_socket(params.transport_under_test);
    for (int i=0; i<sock_options_len; ++i) {
        int value = -1;
        socklen_t optlen = (socklen_t)sizeof(value);
        TEST_ASSERT_EQUAL(getsockopt(sock, expected_opts[i].level, expected_opts[i].optname,
                                     (void*)&value, &optlen), 0);
        if (expected_opts[i].opttype == SOCK_OPT_TYPE_BOOL) {
            TEST_ASSERT_EQUAL((bool)value, (bool) expected_opts[i].optval);
        } else if (expected_opts[i].opttype == SOCK_OPT_TYPE_INT) {
            TEST_ASSERT_EQUAL(value, expected_opts[i].optval);
        } else {
            TEST_FAIL_MESSAGE("Unsupported socket option type");
        }
    }

    // close the tcp_transport's socket so we don't have to wait for connection timeout
    close(sock);

    connect_test_teardown(test);
}
