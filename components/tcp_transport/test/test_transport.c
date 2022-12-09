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

struct tcp_connect_task_params {
    int timeout_ms;
    int port;
    EventGroupHandle_t tcp_connect_done;
    int ret;
    int listen_sock;
    int last_connect_sock;
    bool tcp_listener_failed;
};

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
 * @brief creates a listener (without and acceptor) and connect to as many times as possible
 * to prepare an endpoint which would make the client block but not complete TCP handshake
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

    // Ideally we would set backlog to 0, but since this is an implementation specific recommendation parameter,
    // we recursively create sockets and try to connect to this listener in order to consume the backlog. After
    // the backlog is consumed, the last connection blocks (waiting for accept), but at that point we are sure
    // that any other connection would also block
    connect_once(params);

failed:
    xEventGroupSetBits(params->tcp_connect_done, TCP_LISTENER_DONE);
    vTaskSuspend(NULL);
}

static void tcp_connect_task(void *pvParameters)
{
    struct tcp_connect_task_params *params = pvParameters;

    esp_transport_list_handle_t transport_list = esp_transport_list_init();
    esp_transport_handle_t tcp = esp_transport_tcp_init();
    esp_transport_list_add(transport_list, tcp, "tcp");

    params->ret = esp_transport_connect(tcp, "localhost", params->port, params->timeout_ms);
    xEventGroupSetBits(params->tcp_connect_done, TCP_CONNECT_DONE);
    esp_transport_close(tcp);
    esp_transport_list_destroy(transport_list);
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

TEST_CASE("tcp_transport: connect timeout", "[tcp_transport]")
{
    // This case simulates connection timeout running tcp connect asynchronously with other socket connection
    // consuming entire socket listener backlog.
    // Important: Both tasks must run on the same core, with listener's prio higher to make sure that
    // 1) first the localhost_listener() creates and connects all sockets until the last one blocks
    // 2) before the tcp_connect_task() attempts to connect and thus fails with connection timeout

    struct tcp_connect_task_params params = {   .tcp_connect_done = xEventGroupCreate(),
                                                .timeout_ms = 200,
                                                .port = 80 };
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

    TEST_ASSERT_EQUAL(TCP_CONNECT_DONE, bits);          // Connection has finished
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
