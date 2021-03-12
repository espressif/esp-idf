#include "unity.h"

#include "esp_transport.h"
#include "esp_transport_tcp.h"
#include "esp_transport_ssl.h"
#include "esp_transport_ws.h"
#include "test_utils.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "freertos/event_groups.h"

#define TCP_CONNECT_DONE (1)
#define TCP_LISTENER_DONE (2)
#define TCP_ACCEPTOR_DONE (4)
#define TCP_LISTENER_ACCEPTED (8)
#define TCP_LISTENER_READY (16)

struct tcp_connect_task_params {
    int timeout_ms;
    int port;
    EventGroupHandle_t tcp_connect_done;
    int ret;
    int listen_sock;
    int accepted_sock;
    int last_connect_sock;
    bool tcp_listener_failed;
    esp_transport_handle_t transport_under_test;
    bool accept_connection;
    bool consume_sock_backlog;
};

#define TEST_TRANSPORT_BIND_IFNAME() \
    struct ifreq ifr; \
    ifr.ifr_name[0] = 'l'; \
    ifr.ifr_name[1] = 'o'; \
    ifr.ifr_name[2] = '\0';

/**
 * @brief Recursively connects with a new socket to loopback interface until the last one blocks.
 * The last socket is closed upon test teardown, that initiates recursive cleanup (close) for all
 * active/connected sockets.
 */
static void connect_once(struct tcp_connect_task_params *params)
{
    struct sockaddr_in dest_addr_ip4 = {    .sin_addr.s_addr = htonl(INADDR_LOOPBACK),
                                            .sin_family = AF_INET,
                                            .sin_port = htons(params->port)    };
    int connect_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (connect_sock < 0) {
        params->tcp_listener_failed = true;
        return;
    }
    params->last_connect_sock = connect_sock;
    int err = connect(connect_sock,  (struct sockaddr *)&dest_addr_ip4, sizeof(dest_addr_ip4));
    if (err != 0) {
        // The last connection is expected to fail here, since the both sockets get closed on test cleanup
        return;
    }
    connect_once(params);
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
    struct tcp_connect_task_params *params = pvParameters;
    struct sockaddr_in dest_addr_ip4 = {    .sin_addr.s_addr = htonl(INADDR_ANY),
                                            .sin_family = AF_INET,
                                            .sin_port = htons(params->port)    };
    // Create listener socket and bind it to ANY address
    params->listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    int opt = 1;
    setsockopt(params->listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (params->listen_sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket");
        params->tcp_listener_failed = true;
        goto failed;
    }
    int err = bind(params->listen_sock, (struct sockaddr *)&dest_addr_ip4, sizeof(dest_addr_ip4));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        params->tcp_listener_failed = true;
        goto failed;
    }

    // Listen with backlog set to a low number
    err = listen(params->listen_sock, 4);
    if (err != 0) {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        params->tcp_listener_failed = true;
        goto failed;
    }

    // Listener is ready at this point
    xEventGroupSetBits(params->tcp_connect_done, TCP_LISTENER_READY);

    if (params->consume_sock_backlog) {
        // Ideally we would set backlog to 0, but since this is an implementation specific recommendation parameter,
        // we recursively create sockets and try to connect to this listener in order to consume the backlog. After
        // the backlog is consumed, the last connection blocks (waiting for accept), but at that point we are sure
        // that any other connection would also block
        connect_once(params);
    } else if (params->accept_connection) {
        struct sockaddr_storage source_addr;
        socklen_t addr_len = sizeof(source_addr);
        params->accepted_sock = accept(params->listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (params->accepted_sock < 0) {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            goto failed;
        }
        xEventGroupSetBits(params->tcp_connect_done, TCP_LISTENER_ACCEPTED); // Mark the socket as accepted
        // ...and wait for the "acceptor" tests to finish
        xEventGroupWaitBits(params->tcp_connect_done, TCP_ACCEPTOR_DONE, true, true, params->timeout_ms * 10);
    }

failed:
    xEventGroupSetBits(params->tcp_connect_done, TCP_LISTENER_DONE);
    vTaskSuspend(NULL);
}

static void tcp_connect_task(void *pvParameters)
{
    struct tcp_connect_task_params *params = pvParameters;

    params->ret = esp_transport_connect(params->transport_under_test, "localhost", params->port, params->timeout_ms);
    if (params->accept_connection) {
        // If we test the accepted connection, need to wait until the test completes
        xEventGroupWaitBits(params->tcp_connect_done, TCP_ACCEPTOR_DONE, true, true, params->timeout_ms * 10);
    }
    xEventGroupSetBits(params->tcp_connect_done, TCP_CONNECT_DONE);
    vTaskSuspend(NULL);
}


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

static void transport_connection_timeout_test(esp_transport_handle_t transport_under_test)
{
    // This case simulates connection timeout running tcp connect asynchronously with other socket connection
    // consuming entire socket listener backlog.
    // Important: Both tasks must run on the same core, with listener's prio higher to make sure that
    // 1) first the localhost_listener() creates and connects all sockets until the last one blocks
    // 2) before the tcp_connect_task() attempts to connect and thus fails with connection timeout

    struct tcp_connect_task_params params = {   .tcp_connect_done = xEventGroupCreate(),
                                                .timeout_ms = 200,
                                                .port = 80,
                                                .consume_sock_backlog = true,
                                                .transport_under_test = transport_under_test };
    TickType_t max_wait = pdMS_TO_TICKS(params.timeout_ms * 10);
    TaskHandle_t localhost_listener_task_handle = NULL;
    TaskHandle_t tcp_connect_task_handle = NULL;

    test_case_uses_tcpip();

    // Create listener and connect it with as many sockets until the last one blocks
    xTaskCreatePinnedToCore(localhost_listener, "localhost_listener", 4096, (void*)&params, 5, &localhost_listener_task_handle, 0);

    // Perform tcp-connect in a separate task to check asynchronously for the timeout
    xTaskCreatePinnedToCore(tcp_connect_task, "tcp_connect_task", 4096, (void*)&params, 4, &tcp_connect_task_handle, 0);

    // Roughly measure tick-time spent while trying to connect
    TickType_t start = xTaskGetTickCount();
    EventBits_t bits = xEventGroupWaitBits(params.tcp_connect_done, TCP_CONNECT_DONE, true, true, max_wait);
    TickType_t end = xTaskGetTickCount();

    TEST_ASSERT_EQUAL(TCP_CONNECT_DONE, TCP_CONNECT_DONE & bits);          // Connection has finished
    TEST_ASSERT_EQUAL(-1, params.ret);   // Connection failed with -1

    // Test connection attempt took expected timeout value
    TEST_ASSERT_INT_WITHIN(pdMS_TO_TICKS(params.timeout_ms/5), pdMS_TO_TICKS(params.timeout_ms), end-start);

    // Closing both parties of the last "blocking" connection to unwind localhost_listener() and let other connected sockets closed
    close(params.listen_sock);
    close(params.last_connect_sock);

    // Cleanup
    xEventGroupWaitBits(params.tcp_connect_done, TCP_LISTENER_DONE, true, true, max_wait);
    TEST_ASSERT_EQUAL(false, params.tcp_listener_failed);
    vEventGroupDelete(params.tcp_connect_done);
    test_utils_task_delete(localhost_listener_task_handle);
    test_utils_task_delete(tcp_connect_task_handle);
}

TEST_CASE("tcp_transport: connect timeout", "[tcp_transport]")
{
    // Init the transport under test
    esp_transport_list_handle_t transport_list = esp_transport_list_init();
    esp_transport_handle_t tcp = esp_transport_tcp_init();
    esp_transport_list_add(transport_list, tcp, "tcp");

    transport_connection_timeout_test(tcp);
    esp_transport_close(tcp);
    esp_transport_list_destroy(transport_list);
}

TEST_CASE("ssl_transport: connect timeout", "[tcp_transport]")
{
    // Init the transport under test
    esp_transport_list_handle_t transport_list = esp_transport_list_init();
    esp_transport_handle_t tcp = esp_transport_tcp_init();
    esp_transport_list_add(transport_list, tcp, "tcp");
    esp_transport_handle_t ssl = esp_transport_ssl_init();
    esp_transport_list_add(transport_list, ssl, "ssl");

    transport_connection_timeout_test(ssl);
    esp_transport_close(tcp);
    esp_transport_close(ssl);
    esp_transport_list_destroy(transport_list);
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

// This is a private API of the tcp transport, but needed for socket operation tests
int esp_transport_get_socket(esp_transport_handle_t t);

// Structures and types for passing socket options
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

static void socket_operation_test(esp_transport_handle_t transport_under_test,
                                  const struct expected_sock_option expected_opts[], size_t sock_options_len)
{
    struct tcp_connect_task_params params = {   .tcp_connect_done = xEventGroupCreate(),
            .timeout_ms = 200,
            .port = 80,
            .accept_connection = true,
            .transport_under_test = transport_under_test };
    TickType_t max_wait = pdMS_TO_TICKS(params.timeout_ms * 10);
    TaskHandle_t localhost_listener_task_handle = NULL;
    TaskHandle_t tcp_connect_task_handle = NULL;

    test_case_uses_tcpip();

    // Create a listener and wait for it to be ready
    xTaskCreatePinnedToCore(localhost_listener, "localhost_listener", 4096, (void*)&params, 5, &localhost_listener_task_handle, 0);
    xEventGroupWaitBits(params.tcp_connect_done, TCP_LISTENER_READY, true, true, max_wait);
    // Perform tcp-connect in a separate task
    xTaskCreatePinnedToCore(tcp_connect_task, "tcp_connect_task", 4096, (void*)&params, 6, &tcp_connect_task_handle, 0);

    // Wait till the connection gets accepted to get the client's socket
    xEventGroupWaitBits(params.tcp_connect_done, TCP_LISTENER_ACCEPTED, true, true, max_wait);
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

    close(sock); // close the tcp_transport's socket so we don't have to wait for connection timeout
    xEventGroupSetBits(params.tcp_connect_done, TCP_ACCEPTOR_DONE);
    xEventGroupWaitBits(params.tcp_connect_done, TCP_CONNECT_DONE, true, true, max_wait);
    // Closing the listener and acceptor sockets
    close(params.listen_sock);
    close(params.accepted_sock);

    // Cleanup
    TEST_ASSERT_EQUAL(false, params.tcp_listener_failed);
    vEventGroupDelete(params.tcp_connect_done);
    test_utils_task_delete(localhost_listener_task_handle);
    test_utils_task_delete(tcp_connect_task_handle);
}

static void tcp_transport_keepalive_test(esp_transport_handle_t transport_under_test, esp_transport_keep_alive_t *config)
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

    socket_operation_test(transport_under_test, expected_opts, sizeof(expected_opts)/sizeof(struct expected_sock_option));
}

TEST_CASE("tcp_transport: Keep alive test", "[tcp_transport]")
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
            .keep_alive_count = 3 };
    esp_transport_tcp_set_keep_alive(tcp, &keep_alive_cfg);

    // Bind device interface to loopback
    TEST_TRANSPORT_BIND_IFNAME();
    esp_transport_tcp_set_interface_name(tcp, &ifr);

    tcp_transport_keepalive_test(tcp, &keep_alive_cfg);

    // Cleanup
    esp_transport_close(tcp);
    esp_transport_list_destroy(transport_list);
}

TEST_CASE("ssl_transport: Keep alive test", "[tcp_transport]")
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
            .keep_alive_count = 4 };
    esp_transport_ssl_set_keep_alive(ssl, &keep_alive_cfg);

    // Bind device interface to loopback
    TEST_TRANSPORT_BIND_IFNAME();
    esp_transport_ssl_set_interface_name(ssl, &ifr);

    tcp_transport_keepalive_test(ssl, &keep_alive_cfg);

    // Cleanup
    esp_transport_close(ssl);
    esp_transport_list_destroy(transport_list);
}

TEST_CASE("ws_transport: Keep alive test", "[tcp_transport]")
{
    // Init the transport under test
    esp_transport_list_handle_t transport_list = esp_transport_list_init();
    esp_transport_handle_t ssl = esp_transport_ssl_init();
    esp_transport_list_add(transport_list, ssl, "ssl");
    esp_tls_init_global_ca_store();
    esp_transport_ssl_enable_global_ca_store(ssl);
    esp_transport_handle_t ws = esp_transport_ws_init(ssl);
    esp_transport_list_add(transport_list, ws, "wss");

    // Perform the test
    esp_transport_keep_alive_t  keep_alive_cfg = {
            .keep_alive_interval = 1,
            .keep_alive_idle = 2,
            .keep_alive_enable = true,
            .keep_alive_count = 3 };
    esp_transport_tcp_set_keep_alive(ssl, &keep_alive_cfg);

    // Bind device interface to loopback
    TEST_TRANSPORT_BIND_IFNAME();
    esp_transport_ssl_set_interface_name(ssl, &ifr);

    tcp_transport_keepalive_test(ws, &keep_alive_cfg);

    // Cleanup
    esp_transport_close(ssl);
    esp_transport_list_destroy(transport_list);
}

// Note: This functionality is tested and kept only for compatibility reasons with IDF <= 4.x
//       It is strongly encouraged to use transport within lists only
TEST_CASE("ssl_transport: Check that parameters (keepalive) are set independently on the list", "[tcp_transport]")
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
            .keep_alive_count = 3 };
    esp_transport_ssl_set_keep_alive(ssl, &keep_alive_cfg);

    // Bind device interface to loopback
    TEST_TRANSPORT_BIND_IFNAME();
    esp_transport_ssl_set_interface_name(ssl, &ifr);

    tcp_transport_keepalive_test(ssl, &keep_alive_cfg);

    // Cleanup
    esp_transport_close(ssl);
    esp_transport_destroy(ssl);
}
