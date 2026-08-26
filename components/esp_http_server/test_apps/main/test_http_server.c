/*
 * SPDX-FileCopyrightText: 2018-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <esp_system.h>
#include <esp_http_server.h>
#include <esp_heap_caps.h>
#include <net/if.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/* http_parser.h must precede esp_httpd_priv.h, which embeds a
 * struct http_parser_url by value */
#include <http_parser.h>
#include "../../src/esp_httpd_priv.h"

#include "unity.h"
#include "test_utils.h"
#include "mock_http_server_client.h"

int pre_start_mem, post_stop_mem, post_stop_min_mem;
bool basic_sanity = true;

#define TAG "test_http_server"

esp_err_t null_func(httpd_req_t *req)
{
    return ESP_OK;
}

httpd_uri_t handler_limit_uri (char* path)
{
    httpd_uri_t uri = {
        .uri      = path,
        .method   = HTTP_GET,
        .handler  = null_func,
        .user_ctx = NULL,
    };
    return uri;
};

#ifdef CONFIG_HTTPD_WS_SUPPORT
static httpd_handle_t start_test_ws_server(uint16_t server_port, uint16_t ctrl_port)
{
    httpd_handle_t hd = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = server_port;
    config.ctrl_port = ctrl_port;

    httpd_uri_t ws_uri = {
        .uri = "/ws",
        .method = HTTP_GET,
        .handler = null_func,
        .user_ctx = NULL,
        .is_websocket = true,
        .handle_ws_control_frames = false,
        .supported_subprotocol = NULL,
    };

    TEST_ASSERT_EQUAL(ESP_OK, httpd_start(&hd, &config));
    TEST_ASSERT_EQUAL(ESP_OK, httpd_register_uri_handler(hd, &ws_uri));
    return hd;
}

static httpd_uri_t handler_limit_ws_uri(char *path, const char *subprotocol)
{
    httpd_uri_t uri = {
        .uri                   = path,
        .method                = HTTP_GET,
        .handler               = null_func,
        .user_ctx              = NULL,
        .is_websocket          = true,
        .supported_subprotocol = subprotocol,
    };
    return uri;
}

static int ws_recv_fail_handler_calls;

typedef struct {
    const uint8_t *data;
    size_t len;
    size_t offset;
} ws_scripted_recv_ctx_t;

typedef struct {
    uint8_t data[32];
    size_t len;
} ws_send_capture_ctx_t;

static ws_scripted_recv_ctx_t ws_scripted_recv_ctx;
static ws_send_capture_ctx_t ws_send_capture_ctx;

static int ws_recv_fail_override(httpd_handle_t hd, int sockfd, char *buf, size_t buf_len, int flags)
{
    (void)hd;
    (void)sockfd;
    (void)buf;
    (void)buf_len;
    (void)flags;
    return HTTPD_SOCK_ERR_FAIL;
}

static int ws_scripted_recv_override(httpd_handle_t hd, int sockfd, char *buf, size_t buf_len, int flags)
{
    (void)hd;
    (void)sockfd;
    (void)flags;

    size_t remaining = ws_scripted_recv_ctx.len - ws_scripted_recv_ctx.offset;
    if (remaining == 0) {
        return HTTPD_SOCK_ERR_FAIL;
    }

    size_t to_copy = remaining < buf_len ? remaining : buf_len;
    memcpy(buf, ws_scripted_recv_ctx.data + ws_scripted_recv_ctx.offset, to_copy);
    ws_scripted_recv_ctx.offset += to_copy;
    return (int)to_copy;
}

static int ws_scripted_send_override(httpd_handle_t hd, int sockfd, const char *buf, size_t buf_len, int flags)
{
    (void)hd;
    (void)sockfd;
    (void)flags;

    if (ws_send_capture_ctx.len + buf_len > sizeof(ws_send_capture_ctx.data)) {
        return HTTPD_SOCK_ERR_FAIL;
    }

    memcpy(ws_send_capture_ctx.data + ws_send_capture_ctx.len, buf, buf_len);
    ws_send_capture_ctx.len += buf_len;
    return (int)buf_len;
}

static esp_err_t ws_counting_handler(httpd_req_t *req)
{
    (void)req;
    ws_recv_fail_handler_calls++;
    return ESP_OK;
}
#endif /* CONFIG_HTTPD_WS_SUPPORT */

static inline unsigned num_digits(unsigned x)
{
    unsigned digits = 1;
    while ((x = x/10) != 0) {
        digits++;
    }
    return digits;
}

#define HTTPD_TEST_MAX_URI_HANDLERS 8

void test_handler_limit(httpd_handle_t hd)
{
    int i;
    char x[HTTPD_TEST_MAX_URI_HANDLERS+1][num_digits(HTTPD_TEST_MAX_URI_HANDLERS)+1];
    httpd_uri_t uris[HTTPD_TEST_MAX_URI_HANDLERS+1];

    for (i = 0; i < HTTPD_TEST_MAX_URI_HANDLERS + 1; i++) {
        sprintf(x[i],"%d",i);
        uris[i] = handler_limit_uri(x[i]);
    }

    /* Register multiple instances of the same handler for MAX URI Handlers */
    for (i = 0; i < HTTPD_TEST_MAX_URI_HANDLERS; i++) {
        TEST_ASSERT(httpd_register_uri_handler(hd, &uris[i]) == ESP_OK);
    }

    /* Register the MAX URI + 1 Handlers should fail */
    TEST_ASSERT(httpd_register_uri_handler(hd, &uris[HTTPD_TEST_MAX_URI_HANDLERS]) != ESP_OK);

    /* Unregister the one of the Handler should pass */
    TEST_ASSERT(httpd_unregister_uri_handler(hd, uris[0].uri, uris[0].method) == ESP_OK);

    /* Unregister non added Handler should fail */
    TEST_ASSERT(httpd_unregister_uri_handler(hd, uris[0].uri, uris[0].method) != ESP_OK);

    /* Register the MAX URI Handler should pass */
    TEST_ASSERT(httpd_register_uri_handler(hd, &uris[0]) == ESP_OK);

    /* Reregister same instance of handler should fail */
    TEST_ASSERT(httpd_register_uri_handler(hd, &uris[0]) != ESP_OK);

    /* Register the MAX URI + 1 Handlers should fail */
    TEST_ASSERT(httpd_register_uri_handler(hd, &uris[HTTPD_TEST_MAX_URI_HANDLERS]) != ESP_OK);

    /* Unregister the same handler for MAX URI Handlers */
    for (i = 0; i < HTTPD_TEST_MAX_URI_HANDLERS; i++) {
        TEST_ASSERT(httpd_unregister_uri_handler(hd, uris[i].uri, uris[i].method) == ESP_OK);
    }
    basic_sanity = false;

#ifdef CONFIG_HTTPD_WS_SUPPORT
    /* --- WS subprotocol memory leak check ---
     * Each registration strdup's the supported_subprotocol string.
     * Verify that unregistration frees it (expected leak per handler:
     * strlen(subprotocol) + 1 bytes if the bug is present).
     */
    char ws_paths[HTTPD_TEST_MAX_URI_HANDLERS][8];
    httpd_uri_t ws_uris[HTTPD_TEST_MAX_URI_HANDLERS];
    const char *subprotocol = "chat";

    for (i = 0; i < HTTPD_TEST_MAX_URI_HANDLERS; i++) {
        sprintf(ws_paths[i], "/ws%d", i);
        ws_uris[i] = handler_limit_ws_uri(ws_paths[i], subprotocol);
    }

    size_t heap_before = heap_caps_get_free_size(MALLOC_CAP_8BIT);

    for (i = 0; i < HTTPD_TEST_MAX_URI_HANDLERS; i++) {
        TEST_ASSERT(httpd_register_uri_handler(hd, &ws_uris[i]) == ESP_OK);
    }
    for (i = 0; i < HTTPD_TEST_MAX_URI_HANDLERS; i++) {
        TEST_ASSERT(httpd_unregister_uri_handler(hd, ws_uris[i].uri, ws_uris[i].method) == ESP_OK);
    }

    size_t heap_after = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    int leaked = (int)heap_before - (int)heap_after;

    TEST_ASSERT_MESSAGE(leaked <= 0, "Heap leaked after WS handler unregister");
#endif /* CONFIG_HTTPD_WS_SUPPORT */
}

/********************* Test Handler Limit End *******************/

httpd_handle_t test_httpd_start(uint16_t id)
{
    httpd_handle_t hd;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = HTTPD_TEST_MAX_URI_HANDLERS;
    config.server_port += id;
    config.ctrl_port += id;
    TEST_ASSERT(httpd_start(&hd, &config) == ESP_OK);
    return hd;
}

#define SERVER_INSTANCES 2

/* Currently this only tests for the number of tasks.
 * Heap leakage is not tested as LWIP allocates memory
 * which may not be freed immediately causing erroneous
 * evaluation. Another test to implement would be the
 * monitoring of open sockets, but LWIP presently provides
 * no such API for getting the number of open sockets.
 */
TEST_CASE("Leak Test", "[HTTP SERVER]")
{
    httpd_handle_t hd[SERVER_INSTANCES];
    unsigned task_count;
    bool res = true;

    test_case_uses_tcpip();

    task_count = uxTaskGetNumberOfTasks();
    ESP_LOGI(TAG, "Initial task count: %d\n", task_count);

    pre_start_mem = esp_get_free_heap_size();

    for (int i = 0; i < SERVER_INSTANCES; i++) {
        hd[i] = test_httpd_start(i);
        vTaskDelay(10);
        unsigned num_tasks = uxTaskGetNumberOfTasks();
        task_count++;
        if (num_tasks != task_count) {
            ESP_LOGE(TAG, "Incorrect task count (starting): %d expected %d\n",
                   num_tasks, task_count);
            res = false;
        }
    }

    for (int i = 0; i < SERVER_INSTANCES; i++) {
        if (httpd_stop(hd[i]) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to stop httpd task %d\n", i);
            res = false;
        }
        vTaskDelay(10);
        unsigned num_tasks = uxTaskGetNumberOfTasks();
        task_count--;
        if (num_tasks != task_count) {
            ESP_LOGE(TAG, "Incorrect task count (stopping): %d expected %d\n",
                   num_tasks, task_count);
            res = false;
        }
    }
    post_stop_mem = esp_get_free_heap_size();
    TEST_ASSERT(res == true);
}

TEST_CASE("Basic Functionality Tests", "[HTTP SERVER]")
{
    httpd_handle_t hd;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    test_case_uses_tcpip();

    TEST_ASSERT(httpd_start(&hd, &config) == ESP_OK);
    test_handler_limit(hd);
    TEST_ASSERT(httpd_stop(hd) == ESP_OK);
}

TEST_CASE("URI Wildcard Matcher Tests", "[HTTP SERVER]")
{
    struct uritest {
        const char *template;
        const char *uri;
        bool matches;
    };

    struct uritest uris[] = {
        {"/", "/", true},
        {"", "", true},
        {"/", "", false},
        {"/wrong", "/", false},
        {"/", "/wrong", false},
        {"/asdfghjkl/qwertrtyyuiuioo", "/asdfghjkl/qwertrtyyuiuioo", true},
        {"/path", "/path", true},
        {"/path", "/path/", false},
        {"/path/", "/path", false},

        {"?", "", false}, // this is not valid, but should not crash
        {"?", "sfsdf", false},

        {"/path/?", "/pa", false},
        {"/path/?", "/path", true},
        {"/path/?", "/path/", true},
        {"/path/?", "/path/alalal", false},

        {"/path/*", "/path", false},
        {"/path/*", "/", false},
        {"/path/*", "/path/", true},
        {"/path/*", "/path/blabla", true},

        {"*", "", true},
        {"*", "/", true},
        {"*", "/aaa", true},

        {"/path/?*", "/pat", false},
        {"/path/?*", "/pathb", false},
        {"/path/?*", "/pathxx", false},
        {"/path/?*", "/pathblabla", false},
        {"/path/?*", "/path", true},
        {"/path/?*", "/path/", true},
        {"/path/?*", "/path/blabla", true},

        {"/path/*?", "/pat", false},
        {"/path/*?", "/pathb", false},
        {"/path/*?", "/pathxx", false},
        {"/path/*?", "/path", true},
        {"/path/*?", "/path/", true},
        {"/path/*?", "/path/blabla", true},

        {"/path/*/xxx", "/path/", false},
        {"/path/*/xxx", "/path/*/xxx", true},
        {}
    };

    struct uritest *ut = &uris[0];

    while(ut->template != 0) {
        bool match = httpd_uri_match_wildcard(ut->template, ut->uri, strlen(ut->uri));
        TEST_ASSERT(match == ut->matches);
        ut++;
    }
}

TEST_CASE("Max Allowed Sockets Test", "[HTTP SERVER]")
{
    test_case_uses_tcpip();

    httpd_handle_t hd;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* Starting server with default config options should pass */
    TEST_ASSERT(httpd_start(&hd, &config) == ESP_OK);
    TEST_ASSERT(httpd_stop(hd) == ESP_OK);

    /* Default value of max_open_sockets is already set as per
     * maximum limit imposed by LWIP. Increasing this beyond the
     * maximum allowed value, without increasing LWIP limit,
     * should fail */
    config.max_open_sockets += 1;
    TEST_ASSERT(httpd_start(&hd, &config) != ESP_OK);
}

TEST_CASE("Interface Binding Test", "[HTTP SERVER]")
{
    test_case_uses_tcpip();

    httpd_handle_t hd;
    struct ifreq ifr;

    /* Test 1: Server starts successfully with NULL if_name (default behavior - INADDR_ANY) */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 8080;
    TEST_ASSERT(config.if_name == NULL);
    TEST_ASSERT(httpd_start(&hd, &config) == ESP_OK);
    TEST_ASSERT(httpd_stop(hd) == ESP_OK);

    /* Test 2: Server starts successfully with loopback interface binding */
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, "lo0");  // Loopback interface
    config.server_port = 8081;
    config.if_name = &ifr;

    /* On embedded systems, loopback may not exist, so we test both success and expected failure */
    esp_err_t ret = httpd_start(&hd, &config);
    if (ret == ESP_OK) {
        /* If loopback exists and binding succeeds, verify we can stop cleanly */
        TEST_ASSERT(httpd_stop(hd) == ESP_OK);
    } else {
        /* If loopback doesn't exist or binding fails, that's also acceptable */
        /* The important part is that the server doesn't crash */
        ESP_LOGI(TAG, "Loopback binding failed as expected on this platform\n");
    }

    /* Test 3: Server handles empty interface name (should bind to all interfaces) */
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_name[0] = '\0';  // Empty interface name
    config.server_port = 8082;
    config.if_name = &ifr;

    /* Empty interface name should be ignored and server should start normally */
    TEST_ASSERT(httpd_start(&hd, &config) == ESP_OK);
    TEST_ASSERT(httpd_stop(hd) == ESP_OK);

    /* Test 4: Server handles invalid interface name gracefully */
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, "nonex");  // Invalid interface
    config.server_port = 8083;
    config.if_name = &ifr;

    /* Starting with invalid interface should fail gracefully */
    ret = httpd_start(&hd, &config);
    TEST_ASSERT(ret != ESP_OK);
    if (ret != ESP_OK) {
        /* Expected failure - invalid interface */
        ESP_LOGI(TAG, "Invalid interface binding failed as expected\n");
    } else {
        /* On some platforms, the check might not happen until actual use */
        httpd_stop(hd);
    }

    /* Test 5: Verify backward compatibility - multiple servers without interface binding */
    httpd_handle_t hd1, hd2;
    httpd_config_t config1 = HTTPD_DEFAULT_CONFIG();
    httpd_config_t config2 = HTTPD_DEFAULT_CONFIG();
    config1.server_port = 8084;
    config1.ctrl_port = ESP_HTTPD_DEF_CTRL_PORT + 1;
    config2.server_port = 8085;
    config2.ctrl_port = ESP_HTTPD_DEF_CTRL_PORT + 2;

    TEST_ASSERT(httpd_start(&hd1, &config1) == ESP_OK);
    TEST_ASSERT(httpd_start(&hd2, &config2) == ESP_OK);
    TEST_ASSERT(httpd_stop(hd1) == ESP_OK);
    TEST_ASSERT(httpd_stop(hd2) == ESP_OK);
}

TEST_CASE("httpd_resp_set_hdr rejects CRLF in header field and value", "[HTTP SERVER][security]")
{
    httpd_req_t fake_req = {0};

    /* \r\n in value */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      httpd_resp_set_hdr(&fake_req, "X-Field", "val\r\nX-Injected: pwned"));
    /* bare \n in value */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      httpd_resp_set_hdr(&fake_req, "X-Field", "val\nX-Injected: pwned"));
    /* \r\n in field name */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      httpd_resp_set_hdr(&fake_req, "X-Field\r\nX-Injected: pwned", "val"));
}

TEST_CASE("httpd_resp_set_status rejects CRLF in status string", "[HTTP SERVER][security]")
{
    httpd_req_t fake_req = {0};

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      httpd_resp_set_status(&fake_req, "200 OK\r\nX-Injected: pwned"));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      httpd_resp_set_status(&fake_req, "200 OK\nX-Injected: pwned"));
}

TEST_CASE("httpd_resp_set_type rejects CRLF in content type", "[HTTP SERVER][security]")
{
    httpd_req_t fake_req = {0};

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      httpd_resp_set_type(&fake_req, "text/html\r\nX-Injected: pwned"));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      httpd_resp_set_type(&fake_req, "text/html\nX-Injected: pwned"));
}

/* ---- httpd_queue_work backpressure ---- */

static SemaphoreHandle_t s_qw_gate;
static volatile int s_qw_work_runs;

static void qw_blocking_work(void *arg)
{
    /* Hold the httpd thread inside this work fn so the ctrl-socket mbox
     * stops draining. Auto-release after 2 s as a safety net in case the
     * test asserts mid-way and never reaches the explicit give. */
    xSemaphoreTake((SemaphoreHandle_t)arg, pdMS_TO_TICKS(2000));
}

static void qw_counting_work(void *arg)
{
    (void)arg;
    s_qw_work_runs++;
}

TEST_CASE("httpd_queue_work fast-fails on ctrl mbox saturation", "[HTTP SERVER]")
{
    test_case_uses_tcpip();

    httpd_handle_t hd = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    TEST_ASSERT_EQUAL(ESP_OK, httpd_start(&hd, &config));

    s_qw_gate = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(s_qw_gate);
    s_qw_work_runs = 0;

    /* Park the httpd thread in a blocked work item so the mbox can fill. */
    TEST_ASSERT_EQUAL(ESP_OK, httpd_queue_work(hd, qw_blocking_work, s_qw_gate));
    vTaskDelay(pdMS_TO_TICKS(100));

    /* Spam queue_work past the mbox cap; first ones succeed, rest must
     * return ESP_FAIL synchronously (default non-blocking behavior). */
    int ok_count = 0;
    int fail_count = 0;
    for (int i = 0; i < CONFIG_LWIP_UDP_RECVMBOX_SIZE * 2 + 4; i++) {
        esp_err_t err = httpd_queue_work(hd, qw_counting_work, NULL);
        if (err == ESP_OK) {
            ok_count++;
        } else {
            TEST_ASSERT_EQUAL(ESP_FAIL, err);
            fail_count++;
        }
    }
    TEST_ASSERT_GREATER_THAN(0, ok_count);
    TEST_ASSERT_LESS_OR_EQUAL(CONFIG_LWIP_UDP_RECVMBOX_SIZE, ok_count);
    TEST_ASSERT_GREATER_THAN(0, fail_count);

    /* Release the parked work; every accepted item must now actually run. */
    xSemaphoreGive(s_qw_gate);
    vTaskDelay(pdMS_TO_TICKS(300));
    TEST_ASSERT_EQUAL(ok_count, s_qw_work_runs);

    vSemaphoreDelete(s_qw_gate);
    s_qw_gate = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, httpd_stop(hd));
}

#ifdef CONFIG_HTTPD_WS_SUPPORT
/* ------------------------------------------------------------------------- *
 * White-box fixtures for the dedicated control-frame handler.
 *
 * These tests drive httpd_req_new() directly against a fake sock_db, feeding a
 * crafted (zero-masked) WebSocket frame through a recv_fn override and capturing
 * the server's reply through a send_fn override. No TCP/IP is involved, so they
 * run identically on hardware and under QEMU.
 * ------------------------------------------------------------------------- */
static int s_ws_data_handler_calls;
static int s_ws_control_handler_calls;
static httpd_ws_type_t s_ws_control_seen_type;
static size_t s_ws_control_seen_len;
static esp_err_t s_ws_control_ret;

static const uint8_t *s_ws_recv_data;
static size_t s_ws_recv_len;
static size_t s_ws_recv_off;

static uint8_t s_ws_sent[128];
static size_t s_ws_sent_len;

/* recv_fn override: serve the crafted frame byte stream, one chunk per call. */
static int ws_feed_recv(httpd_handle_t hd, int sockfd, char *buf, size_t buf_len, int flags)
{
    (void)hd; (void)sockfd; (void)flags;
    size_t remaining = s_ws_recv_len - s_ws_recv_off;
    if (remaining == 0) {
        return HTTPD_SOCK_ERR_FAIL;
    }
    size_t n = (buf_len < remaining) ? buf_len : remaining;
    memcpy(buf, s_ws_recv_data + s_ws_recv_off, n);
    s_ws_recv_off += n;
    return (int)n;
}

/* send_fn override: capture whatever the server sends back (the protocol reply). */
static int ws_capture_send(httpd_handle_t hd, int sockfd, const char *buf, size_t buf_len, int flags)
{
    (void)hd; (void)sockfd; (void)flags;
    for (size_t i = 0; i < buf_len && s_ws_sent_len < sizeof(s_ws_sent); i++) {
        s_ws_sent[s_ws_sent_len++] = (uint8_t)buf[i];
    }
    return (int)buf_len;
}

static esp_err_t ws_data_handler_spy(httpd_req_t *req)
{
    (void)req;
    s_ws_data_handler_calls++;
    return ESP_OK;
}

static esp_err_t ws_control_handler_spy(httpd_req_t *req, const httpd_ws_frame_t *frame)
{
    (void)req;
    s_ws_control_handler_calls++;
    s_ws_control_seen_type = frame->type;
    s_ws_control_seen_len = frame->len;
    return s_ws_control_ret;
}

/* Wire a fake session/server and reset all fixtures around a single frame. */
static void ws_unit_ctx_init(struct httpd_data *hd, struct sock_db *session,
                             const uint8_t *frame, size_t frame_len)
{
    s_ws_data_handler_calls = 0;
    s_ws_control_handler_calls = 0;
    s_ws_control_seen_type = HTTPD_WS_TYPE_CONTINUE;
    s_ws_control_seen_len = 0;
    s_ws_control_ret = ESP_OK;
    s_ws_recv_data = frame;
    s_ws_recv_len = frame_len;
    s_ws_recv_off = 0;
    s_ws_sent_len = 0;
    memset(s_ws_sent, 0, sizeof(s_ws_sent));

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_open_sockets = 1;   /* keep any session enumeration in bounds */
    hd->config = config;
    hd->hd_sd = session;           /* non-NULL so httpd_sess_get() finds the reply target */
    hd->hd_req_aux.resp_hdrs = calloc(config.max_resp_headers, sizeof(*hd->hd_req_aux.resp_hdrs));
    TEST_ASSERT_NOT_NULL(hd->hd_req_aux.resp_hdrs);

    session->fd = 123;
    session->handle = (httpd_handle_t) hd;
    session->recv_fn = ws_feed_recv;
    session->send_fn = ws_capture_send;
    session->ws_handshake_done = true;
    session->ws_handler = ws_data_handler_spy;
    session->ws_close = false;
}

TEST_CASE("WS recv failure marks close without dispatching handler", "[HTTP SERVER][websocket]")
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    struct httpd_data hd = {0};
    struct sock_db session = {0};

    hd.config = config;
    hd.hd_req_aux.resp_hdrs = calloc(config.max_resp_headers, sizeof(*hd.hd_req_aux.resp_hdrs));
    TEST_ASSERT_NOT_NULL(hd.hd_req_aux.resp_hdrs);

    session.fd = 123;
    session.handle = (httpd_handle_t) &hd;
    session.recv_fn = ws_recv_fail_override;
    session.ws_handshake_done = true;
    session.ws_handler = ws_counting_handler;
    session.ws_control_frames = false;
    session.ws_close = false;

    ws_recv_fail_handler_calls = 0;

    esp_err_t ret = httpd_req_new(&hd, &session);

    TEST_ASSERT_EQUAL(ESP_OK, ret);
    TEST_ASSERT_EQUAL(0, ws_recv_fail_handler_calls);
    TEST_ASSERT_TRUE(session.ws_close);
    TEST_ASSERT_EQUAL(HTTPD_WS_TYPE_CLOSE, hd.hd_req_aux.ws_type);

    free(hd.hd_req_aux.resp_hdrs);
}

/* Regression test: enabling CONFIG_HTTPD_WS_SUPPORT must not reject HTTP/1.0
 * traffic on non-WS endpoints. The HTTP/1.1 requirement only applies once
 * an "Upgrade: websocket" header confirms the request is a WS handshake. */
TEST_CASE("Non-WS HTTP/1.0 request is not rejected by WS version check", "[HTTP SERVER][websocket]")
{
    test_case_uses_tcpip();
    httpd_handle_t hd = start_test_ws_server(8097, ESP_HTTPD_DEF_CTRL_PORT + 16);
    mock_server_request_t req = {
        .data = "GET /non-ws HTTP/1.0\r\n"
                "Host: localhost\r\n\r\n",
    };
    mock_server_response_t *resp = mock_server_send_request(8097, &req);
    TEST_ASSERT_NOT_NULL(resp);
    /* No handler is registered for /non-ws, so the server returns 404.
     * The crucial assertion is that it is NOT 400 from the WS version check —
     * i.e., HTTP/1.0 was accepted at the parser level. */
    mock_server_assert_status(resp, 404);
    mock_server_response_free(resp);
    TEST_ASSERT_EQUAL(ESP_OK, httpd_stop(hd));
}

TEST_CASE("WS handshake missing Sec-WebSocket-Version returns 400", "[HTTP SERVER][websocket]")
{
    test_case_uses_tcpip();
    httpd_handle_t hd = start_test_ws_server(8094, ESP_HTTPD_DEF_CTRL_PORT + 13);
    mock_server_request_t req = {
        .data = "GET /ws HTTP/1.1\r\n"
                "Host: localhost\r\n"
                "Upgrade: websocket\r\n"
                "Connection: Upgrade\r\n"
                "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n\r\n",
    };
    mock_server_response_t *resp = mock_server_send_request(8094, &req);
    TEST_ASSERT_NOT_NULL(resp);
    mock_server_assert_status(resp, 400);
    mock_server_response_free(resp);
    TEST_ASSERT_EQUAL(ESP_OK, httpd_stop(hd));
}

TEST_CASE("WS handshake unsupported version returns 426 with Sec-WebSocket-Version header", "[HTTP SERVER][websocket]")
{
    test_case_uses_tcpip();
    httpd_handle_t hd = start_test_ws_server(8093, ESP_HTTPD_DEF_CTRL_PORT + 12);
    mock_server_request_t req = {
        .data = "GET /ws HTTP/1.1\r\n"
                "Host: localhost\r\n"
                "Upgrade: websocket\r\n"
                "Connection: Upgrade\r\n"
                "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
                "Sec-WebSocket-Version: 12\r\n\r\n",
    };
    mock_server_response_t *resp = mock_server_send_request(8093, &req);
    TEST_ASSERT_NOT_NULL(resp);
    mock_server_assert_status(resp, 426);
    TEST_ASSERT_NOT_NULL(strstr(resp->data, "Sec-WebSocket-Version: 13"));
    mock_server_response_free(resp);
    TEST_ASSERT_EQUAL(ESP_OK, httpd_stop(hd));
}

#if CONFIG_HTTPD_WS_STRICTER_RFC6455
TEST_CASE("WS HTTP/1.0 upgrade request returns 400", "[HTTP SERVER][websocket]")
{
    test_case_uses_tcpip();
    httpd_handle_t hd = start_test_ws_server(8091, ESP_HTTPD_DEF_CTRL_PORT + 10);
    mock_server_request_t req = {
        .data = "GET /ws HTTP/1.0\r\n"
                "Host: localhost\r\n"
                "Upgrade: websocket\r\n"
                "Connection: Upgrade\r\n"
                "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
                "Sec-WebSocket-Version: 13\r\n\r\n",
    };
    mock_server_response_t *resp = mock_server_send_request(8091, &req);
    TEST_ASSERT_NOT_NULL(resp);

    mock_server_assert_status(resp, 400);
    mock_server_response_free(resp);
    TEST_ASSERT_EQUAL(ESP_OK, httpd_stop(hd));
}

TEST_CASE("WS handshake missing Host returns 400", "[HTTP SERVER][websocket]")
{
    test_case_uses_tcpip();
    httpd_handle_t hd = start_test_ws_server(8090, ESP_HTTPD_DEF_CTRL_PORT + 9);
    mock_server_request_t req = {
        .data = "GET /ws HTTP/1.1\r\n"
                "Upgrade: websocket\r\n"
                "Connection: Upgrade\r\n"
                "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
                "Sec-WebSocket-Version: 13\r\n\r\n",
    };
    mock_server_response_t *resp = mock_server_send_request(8090, &req);
    TEST_ASSERT_NOT_NULL(resp);
    mock_server_assert_status(resp, 400);
    mock_server_response_free(resp);
    TEST_ASSERT_EQUAL(ESP_OK, httpd_stop(hd));
}

TEST_CASE("WS handshake invalid Sec-WebSocket-Key returns 400", "[HTTP SERVER][websocket]")
{
    test_case_uses_tcpip();
    httpd_handle_t hd = start_test_ws_server(8092, ESP_HTTPD_DEF_CTRL_PORT + 11);
    mock_server_request_t req = {
        .data = "GET /ws HTTP/1.1\r\n"
                "Host: localhost\r\n"
                "Upgrade: websocket\r\n"
                "Connection: Upgrade\r\n"
                "Sec-WebSocket-Key: AQID\r\n"
                "Sec-WebSocket-Version: 13\r\n\r\n",
    };
    mock_server_response_t *resp = mock_server_send_request(8092, &req);
    TEST_ASSERT_NOT_NULL(resp);
    mock_server_assert_status(resp, 400);
    mock_server_response_free(resp);
    TEST_ASSERT_EQUAL(ESP_OK, httpd_stop(hd));
}

/* Common fake-session wiring for the strict recv-path tests: a single-socket
 * server whose scripted recv feeds `frame` and whose send is captured. */
static void ws_setup_recv_fixture(struct httpd_data *hd, httpd_req_t *req,
                                  struct httpd_req_aux *aux, struct sock_db *session,
                                  const uint8_t *frame, size_t frame_len)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    ws_scripted_recv_ctx = (ws_scripted_recv_ctx_t){ .data = frame, .len = frame_len };
    memset(&ws_send_capture_ctx, 0, sizeof(ws_send_capture_ctx));

    hd->config = config;
    hd->config.max_open_sockets = 1;
    hd->hd_sd = session;
    req->handle = hd;
    req->aux = aux;
    aux->sd = session;
    session->fd = 123;
    session->handle = (httpd_handle_t)hd;
    session->recv_fn = ws_scripted_recv_override;
    session->send_fn = ws_scripted_send_override;
    session->ws_handshake_done = true;
}

/* Asserts the session was marked closing and a 1002 (protocol-error) CLOSE
 * frame was emitted on the wire. */
static void ws_assert_close_1002_sent(const struct sock_db *session)
{
    static const uint8_t expected_reply[] = { 0x88, 0x02, 0x03, 0xEA };

    TEST_ASSERT_TRUE(session->ws_close);
    TEST_ASSERT_EQUAL(sizeof(expected_reply), ws_send_capture_ctx.len);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_reply, ws_send_capture_ctx.data, sizeof(expected_reply));
}

TEST_CASE("WS recv RSV bit set sends CLOSE 1002 and marks close", "[HTTP SERVER][websocket]")
{
    static const uint8_t ws_frame[] = { 0xC1 }; /* RSV1=1, FIN=1, opcode TEXT */
    struct httpd_data hd = {0};
    httpd_req_t req = {0};
    struct httpd_req_aux aux = {0};
    struct sock_db session = {0};

    ws_setup_recv_fixture(&hd, &req, &aux, &session, ws_frame, sizeof(ws_frame));

    TEST_ASSERT_EQUAL(ESP_FAIL, httpd_ws_get_frame_type(&req));
    ws_assert_close_1002_sent(&session);
}

TEST_CASE("WS recv reserved non-control opcode sends CLOSE 1002", "[HTTP SERVER][websocket]")
{
    static const uint8_t ws_frame[] = { 0x83 }; /* FIN=1, opcode=0x3 (reserved) */
    struct httpd_data hd = {0};
    httpd_req_t req = {0};
    struct httpd_req_aux aux = {0};
    struct sock_db session = {0};

    ws_setup_recv_fixture(&hd, &req, &aux, &session, ws_frame, sizeof(ws_frame));

    TEST_ASSERT_EQUAL(ESP_FAIL, httpd_ws_get_frame_type(&req));
    TEST_ASSERT_EQUAL(HTTPD_WS_TYPE_CLOSE, aux.ws_type);
    ws_assert_close_1002_sent(&session);
}

TEST_CASE("WS recv reserved control opcode sends CLOSE 1002", "[HTTP SERVER][websocket]")
{
    static const uint8_t ws_frame[] = { 0x8B }; /* FIN=1, opcode=0xB (reserved) */
    struct httpd_data hd = {0};
    httpd_req_t req = {0};
    struct httpd_req_aux aux = {0};
    struct sock_db session = {0};

    ws_setup_recv_fixture(&hd, &req, &aux, &session, ws_frame, sizeof(ws_frame));

    TEST_ASSERT_EQUAL(ESP_FAIL, httpd_ws_get_frame_type(&req));
    TEST_ASSERT_EQUAL(HTTPD_WS_TYPE_CLOSE, aux.ws_type);
    ws_assert_close_1002_sent(&session);
}

TEST_CASE("WS recv fragmented control frame sends CLOSE 1002", "[HTTP SERVER][websocket]")
{
    static const uint8_t ws_frame[] = { 0x09 }; /* FIN=0, opcode=PING */
    struct httpd_data hd = {0};
    httpd_req_t req = {0};
    struct httpd_req_aux aux = {0};
    struct sock_db session = {0};

    ws_setup_recv_fixture(&hd, &req, &aux, &session, ws_frame, sizeof(ws_frame));

    TEST_ASSERT_EQUAL(ESP_FAIL, httpd_ws_get_frame_type(&req));
    TEST_ASSERT_EQUAL(HTTPD_WS_TYPE_CLOSE, aux.ws_type);
    ws_assert_close_1002_sent(&session);
}

TEST_CASE("WS recv unmasked frame sends CLOSE 1002 and marks close", "[HTTP SERVER][websocket]")
{
    /* Length byte 0x02: MASK=0, payload len=2. recv_frame reads this as its
     * first byte (the opcode byte is taken from aux), so it exercises the
     * unmasked-client-frame rejection rather than the mask-key path. */
    static const uint8_t ws_frame[] = { 0x02 };
    struct httpd_data hd = {0};
    httpd_req_t req = {0};
    struct httpd_req_aux aux = {0};
    struct sock_db session = {0};
    httpd_ws_frame_t frame = {0};

    ws_setup_recv_fixture(&hd, &req, &aux, &session, ws_frame, sizeof(ws_frame));
    aux.ws_type = HTTPD_WS_TYPE_BINARY;
    aux.ws_final = true;

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, httpd_ws_recv_frame(&req, &frame, 0));
    ws_assert_close_1002_sent(&session);
}

TEST_CASE("WS recv control frame with payload > 125 sends CLOSE 1002", "[HTTP SERVER][websocket]")
{
    /* PING (0x89), MASK=1 (0x80), length=126 (0x7E) — invalid extended length for control */
    static const uint8_t ws_frame[] = { 0x89, 0xFE };
    struct httpd_data hd = {0};
    httpd_req_t req = {0};
    struct httpd_req_aux aux = {0};
    struct sock_db session = {0};

    ws_setup_recv_fixture(&hd, &req, &aux, &session, ws_frame, sizeof(ws_frame));
    session.ws_control_frames = false;

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, httpd_ws_get_frame_type(&req));
    TEST_ASSERT_EQUAL(HTTPD_WS_TYPE_CLOSE, aux.ws_type);
    ws_assert_close_1002_sent(&session);
}

TEST_CASE("WS recv rejects non-minimal 16-bit payload length encoding", "[HTTP SERVER][websocket]")
{
    /* 0xFE = MASK=1 len=126; 0x00 0x7D = 125, which must use 7-bit form */
    static const uint8_t ws_frame[] = { 0xFE, 0x00, 0x7D };
    struct httpd_data hd = {0};
    httpd_req_t req = {0};
    struct httpd_req_aux aux = {0};
    struct sock_db session = {0};
    httpd_ws_frame_t frame = {0};

    ws_setup_recv_fixture(&hd, &req, &aux, &session, ws_frame, sizeof(ws_frame));
    aux.ws_type = HTTPD_WS_TYPE_TEXT;
    aux.ws_final = true;

    TEST_ASSERT_EQUAL(ESP_FAIL, httpd_ws_recv_frame(&req, &frame, 0));
    TEST_ASSERT_EQUAL(HTTPD_WS_TYPE_CLOSE, aux.ws_type);
    ws_assert_close_1002_sent(&session);
}

TEST_CASE("WS recv rejects non-minimal 64-bit payload length encoding", "[HTTP SERVER][websocket]")
{
    /* 0xFF = MASK=1 len=127; value 65535 must use 16-bit form */
    static const uint8_t ws_frame[] = { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF };
    struct httpd_data hd = {0};
    httpd_req_t req = {0};
    struct httpd_req_aux aux = {0};
    struct sock_db session = {0};
    httpd_ws_frame_t frame = {0};

    ws_setup_recv_fixture(&hd, &req, &aux, &session, ws_frame, sizeof(ws_frame));
    aux.ws_type = HTTPD_WS_TYPE_TEXT;
    aux.ws_final = true;

    TEST_ASSERT_EQUAL(ESP_FAIL, httpd_ws_recv_frame(&req, &frame, 0));
    TEST_ASSERT_EQUAL(HTTPD_WS_TYPE_CLOSE, aux.ws_type);
    ws_assert_close_1002_sent(&session);
}

TEST_CASE("WS send refuses non-CLOSE frame once session is closing", "[HTTP SERVER][websocket]")
{
    /* RFC 6455 §5.5.1: after a CLOSE is sent/received, only a CLOSE may follow. */
    static const uint8_t payload[] = { 0x41, 0x42 };
    static const uint8_t expected_close[] = { 0x88, 0x00 }; /* CLOSE, zero-length */

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    struct httpd_data hd = {0};
    struct sock_db session = {0};
    httpd_ws_frame_t data_frame = {
        .type = HTTPD_WS_TYPE_TEXT,
        .payload = (uint8_t *)payload,
        .len = sizeof(payload),
    };
    httpd_ws_frame_t close_frame = {
        .type = HTTPD_WS_TYPE_CLOSE,
        .payload = NULL,
        .len = 0,
    };

    memset(&ws_send_capture_ctx, 0, sizeof(ws_send_capture_ctx));

    hd.config = config;
    hd.config.max_open_sockets = 1;
    hd.hd_sd = &session;
    session.fd = 123;
    session.handle = (httpd_handle_t)&hd;
    session.send_fn = ws_scripted_send_override;
    session.ws_close = true; /* a CLOSE has already been sent/received */

    /* A data frame must be refused and nothing may go on the wire. */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, httpd_ws_send_frame_async(&hd, session.fd, &data_frame));
    TEST_ASSERT_EQUAL(0, ws_send_capture_ctx.len);

    /* The CLOSE frame itself is still permitted. */
    TEST_ASSERT_EQUAL(ESP_OK, httpd_ws_send_frame_async(&hd, session.fd, &close_frame));
    TEST_ASSERT_EQUAL(sizeof(expected_close), ws_send_capture_ctx.len);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_close, ws_send_capture_ctx.data, sizeof(expected_close));
}
#endif /* CONFIG_HTTPD_WS_STRICTER_RFC6455 */

TEST_CASE("WS send uses 16-bit length encoding for exactly 65535-byte payload", "[HTTP SERVER][websocket]")
{
    static const uint8_t expected_header[] = { 0x82, 0x7E, 0xFF, 0xFF };

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    struct httpd_data hd = {0};
    struct sock_db session = {0};
    httpd_ws_frame_t frame = {
        .type = HTTPD_WS_TYPE_BINARY,
        .payload = NULL,
        .len = UINT16_MAX,
    };

    memset(&ws_send_capture_ctx, 0, sizeof(ws_send_capture_ctx));

    hd.config = config;
    hd.config.max_open_sockets = 1;
    hd.hd_sd = &session;
    session.fd = 123;
    session.handle = (httpd_handle_t)&hd;
    session.send_fn = ws_scripted_send_override;

    TEST_ASSERT_EQUAL(ESP_OK, httpd_ws_send_frame_async(&hd, session.fd, &frame));
    TEST_ASSERT_EQUAL(sizeof(expected_header), ws_send_capture_ctx.len);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_header, ws_send_capture_ctx.data, sizeof(expected_header));
}

TEST_CASE("WS control handler receives PING and server replies PONG", "[HTTP SERVER][websocket]")
{
    /* Masked (zero-key) PING carrying a 2-byte payload "Hi". */
    static const uint8_t ping_frame[] = { 0x89, 0x82, 0x00, 0x00, 0x00, 0x00, 'H', 'i' };
    struct httpd_data hd = {0};
    struct sock_db session = {0};
    ws_unit_ctx_init(&hd, &session, ping_frame, sizeof(ping_frame));
    session.ws_control_frames = true;
    session.ws_control_handler = ws_control_handler_spy;

    esp_err_t ret = httpd_req_new(&hd, &session);

    TEST_ASSERT_EQUAL(ESP_OK, ret);
    TEST_ASSERT_EQUAL(1, s_ws_control_handler_calls);
    TEST_ASSERT_EQUAL(HTTPD_WS_TYPE_PING, s_ws_control_seen_type);
    TEST_ASSERT_EQUAL(2, s_ws_control_seen_len);
    TEST_ASSERT_EQUAL(0, s_ws_data_handler_calls);   /* control frame must not reach data handler */
    TEST_ASSERT_GREATER_THAN(0, s_ws_sent_len);
    TEST_ASSERT_EQUAL_HEX8(0x8A, s_ws_sent[0]);      /* FIN | PONG */
    TEST_ASSERT_FALSE(session.ws_close);

    free(hd.hd_req_aux.resp_hdrs);
}

TEST_CASE("WS control handler receives CLOSE and server replies CLOSE", "[HTTP SERVER][websocket]")
{
    static const uint8_t close_frame[] = { 0x88, 0x80, 0x00, 0x00, 0x00, 0x00 };
    struct httpd_data hd = {0};
    struct sock_db session = {0};
    ws_unit_ctx_init(&hd, &session, close_frame, sizeof(close_frame));
    session.ws_control_frames = true;
    session.ws_control_handler = ws_control_handler_spy;

    esp_err_t ret = httpd_req_new(&hd, &session);

    TEST_ASSERT_EQUAL(ESP_OK, ret);
    TEST_ASSERT_EQUAL(1, s_ws_control_handler_calls);
    TEST_ASSERT_EQUAL(HTTPD_WS_TYPE_CLOSE, s_ws_control_seen_type);
    TEST_ASSERT_EQUAL(0, s_ws_data_handler_calls);
    TEST_ASSERT_GREATER_THAN(0, s_ws_sent_len);
    TEST_ASSERT_EQUAL_HEX8(0x88, s_ws_sent[0]);      /* FIN | CLOSE */
    TEST_ASSERT_TRUE(session.ws_close);              /* server marked the session for close */

    free(hd.hd_req_aux.resp_hdrs);
}

TEST_CASE("WS control handler receives PONG with no reply", "[HTTP SERVER][websocket]")
{
    static const uint8_t pong_frame[] = { 0x8A, 0x80, 0x00, 0x00, 0x00, 0x00 };
    struct httpd_data hd = {0};
    struct sock_db session = {0};
    ws_unit_ctx_init(&hd, &session, pong_frame, sizeof(pong_frame));
    session.ws_control_frames = true;
    session.ws_control_handler = ws_control_handler_spy;

    esp_err_t ret = httpd_req_new(&hd, &session);

    TEST_ASSERT_EQUAL(ESP_OK, ret);
    TEST_ASSERT_EQUAL(1, s_ws_control_handler_calls);
    TEST_ASSERT_EQUAL(HTTPD_WS_TYPE_PONG, s_ws_control_seen_type);
    TEST_ASSERT_EQUAL(0, s_ws_data_handler_calls);
    TEST_ASSERT_EQUAL(0, s_ws_sent_len);             /* a PONG is never answered */
    TEST_ASSERT_FALSE(session.ws_close);

    free(hd.hd_req_aux.resp_hdrs);
}

TEST_CASE("WS without control handler auto-replies PING (backward compatible)", "[HTTP SERVER][websocket]")
{
    /* Mode 1: no control handler, flag off -> server must still auto-reply PONG and
     * must not dispatch the PING to the data handler (unchanged legacy behavior). */
    static const uint8_t ping_frame[] = { 0x89, 0x80, 0x00, 0x00, 0x00, 0x00 };
    struct httpd_data hd = {0};
    struct sock_db session = {0};
    ws_unit_ctx_init(&hd, &session, ping_frame, sizeof(ping_frame));
    session.ws_control_frames = false;
    session.ws_control_handler = NULL;

    esp_err_t ret = httpd_req_new(&hd, &session);

    TEST_ASSERT_EQUAL(ESP_OK, ret);
    TEST_ASSERT_EQUAL(0, s_ws_control_handler_calls);
    TEST_ASSERT_EQUAL(0, s_ws_data_handler_calls);
    TEST_ASSERT_GREATER_THAN(0, s_ws_sent_len);
    TEST_ASSERT_EQUAL_HEX8(0x8A, s_ws_sent[0]);      /* auto PONG */

    free(hd.hd_req_aux.resp_hdrs);
}

TEST_CASE("WS control handler error still replies then closes socket", "[HTTP SERVER][websocket]")
{
    /* A PING is used so ws_close stays false and cleanup does not touch the fake
     * control socket. The handler fails, but the server must still send the PONG,
     * and httpd_req_new() must propagate the error so the caller closes the socket. */
    static const uint8_t ping_frame[] = { 0x89, 0x80, 0x00, 0x00, 0x00, 0x00 };
    struct httpd_data hd = {0};
    struct sock_db session = {0};
    ws_unit_ctx_init(&hd, &session, ping_frame, sizeof(ping_frame));
    session.ws_control_frames = true;
    session.ws_control_handler = ws_control_handler_spy;
    s_ws_control_ret = ESP_FAIL;

    esp_err_t ret = httpd_req_new(&hd, &session);

    TEST_ASSERT_EQUAL(ESP_FAIL, ret);                /* error propagated to caller */
    TEST_ASSERT_EQUAL(1, s_ws_control_handler_calls);
    TEST_ASSERT_GREATER_THAN(0, s_ws_sent_len);
    TEST_ASSERT_EQUAL_HEX8(0x8A, s_ws_sent[0]);      /* reply sent despite handler error */

    free(hd.hd_req_aux.resp_hdrs);
}

#endif /* CONFIG_HTTPD_WS_SUPPORT */

/********* URL query / header pointer-accessor tests *********
 * These exercise httpd_req_get_url_query_str_ptr() and
 * httpd_req_get_hdr_value_str_ptr() against hand-built requests that mirror
 * the parser's output: the query is parsed with http_parser_parse_url() (the
 * exact mechanism the server uses), and the header scratch buffer reproduces
 * the parser layout ("Field: value" with the CRLF terminators replaced by
 * null bytes). The new pointer APIs are cross-checked against the existing
 * copy/length variants so any divergence is caught.
 *
 * Note: these assume CONFIG_HTTPD_VALIDATE_REQ is disabled (the default), so
 * httpd_valid_req() accepts the stack request used here. */

/* Parse a query-carrying URI into a stack request, like verify_url() does */
static void build_query_req(httpd_req_t *req, struct httpd_req_aux *aux, const char *uri)
{
    memset(req, 0, sizeof(*req));
    memset(aux, 0, sizeof(*aux));
    req->aux = aux;
    strlcpy((char *)req->uri, uri, sizeof(req->uri));
    http_parser_url_init(&aux->url_parse_res);
    TEST_ASSERT_EQUAL(0, http_parser_parse_url(req->uri, strlen(req->uri), 0,
                                               &aux->url_parse_res));
}

TEST_CASE("httpd_req_get_url_query_str_ptr returns query without copy", "[HTTP SERVER]")
{
    httpd_req_t req;
    struct httpd_req_aux aux;
    const char *expected = "foo=bar&baz=qux";
    build_query_req(&req, &aux, "/path?foo=bar&baz=qux");

    const char *q = NULL;
    size_t qlen = 0;
    TEST_ASSERT_EQUAL(ESP_OK, httpd_req_get_url_query_str_ptr(&req, &q, &qlen));
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL(strlen(expected), qlen);
    TEST_ASSERT_EQUAL(0, memcmp(q, expected, qlen));

    /* Must point into the request URI buffer, i.e. no copy was made */
    TEST_ASSERT_TRUE(q >= req.uri && q < req.uri + sizeof(req.uri));

    /* Cross-check against the length and copy variants */
    TEST_ASSERT_EQUAL(qlen, httpd_req_get_url_query_len(&req));
    char buf[64];
    TEST_ASSERT_EQUAL(ESP_OK, httpd_req_get_url_query_str(&req, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL(0, strncmp(buf, q, qlen));
}

TEST_CASE("httpd_req_get_url_query_str_ptr handles empty and missing query", "[HTTP SERVER]")
{
    httpd_req_t req = {0};
    struct httpd_req_aux aux = {0};
    req.aux = &aux;
    const char *q = NULL;
    size_t qlen = 0;

    /* Empty URI -> ESP_FAIL */
    TEST_ASSERT_EQUAL(ESP_FAIL, httpd_req_get_url_query_str_ptr(&req, &q, &qlen));

    /* URI without a query -> ESP_ERR_NOT_FOUND */
    build_query_req(&req, &aux, "/path/only");
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, httpd_req_get_url_query_str_ptr(&req, &q, &qlen));
}

TEST_CASE("httpd_req_get_url_query_str_ptr validates NULL args", "[HTTP SERVER][security]")
{
    httpd_req_t req;
    struct httpd_req_aux aux;
    build_query_req(&req, &aux, "/p?x=1");

    const char *q = NULL;
    size_t qlen = 0;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, httpd_req_get_url_query_str_ptr(NULL, &q, &qlen));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, httpd_req_get_url_query_str_ptr(&req, NULL, &qlen));
    /* Regression: a query is present, so a NULL buf_len used to be written to */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, httpd_req_get_url_query_str_ptr(&req, &q, NULL));
}

TEST_CASE("httpd_req_get_hdr_value_str_ptr returns value without copy", "[HTTP SERVER]")
{
    httpd_req_t req = {0};
    struct httpd_req_aux aux = {0};
    /* Parser layout: "Field: value" entries, CRLF terminators replaced by nulls */
    static char scratch[] = "Host: example.com\0\0X-Custom: hello world";
    aux.scratch = scratch;
    aux.req_hdrs_count = 2;
    req.aux = &aux;

    const char *val = NULL;
    size_t vlen = 0;
    TEST_ASSERT_EQUAL(ESP_OK, httpd_req_get_hdr_value_str_ptr(&req, "X-Custom", &val, &vlen));
    TEST_ASSERT_EQUAL(strlen("hello world"), vlen);
    TEST_ASSERT_EQUAL(0, strcmp(val, "hello world"));

    /* Must point into the scratch buffer, i.e. no copy was made */
    TEST_ASSERT_TRUE(val >= scratch && val < scratch + sizeof(scratch));

    /* Field match is case-insensitive, like the copy variant */
    TEST_ASSERT_EQUAL(ESP_OK, httpd_req_get_hdr_value_str_ptr(&req, "host", &val, &vlen));
    TEST_ASSERT_EQUAL(0, strcmp(val, "example.com"));

    /* Cross-check the new pointer API against the length and copy variants */
    TEST_ASSERT_EQUAL(strlen("hello world"), httpd_req_get_hdr_value_len(&req, "X-Custom"));
    char buf[32];
    TEST_ASSERT_EQUAL(ESP_OK, httpd_req_get_hdr_value_str(&req, "X-Custom", buf, sizeof(buf)));
    TEST_ASSERT_EQUAL(0, strcmp(buf, "hello world"));

    /* Missing field -> ESP_ERR_NOT_FOUND */
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND,
                      httpd_req_get_hdr_value_str_ptr(&req, "Nonexistent", &val, &vlen));
}

TEST_CASE("httpd_req_get_hdr_value_str_ptr validates NULL args", "[HTTP SERVER][security]")
{
    httpd_req_t req = {0};
    struct httpd_req_aux aux = {0};
    static char scratch[] = "X-Custom: hello world";
    aux.scratch = scratch;
    aux.req_hdrs_count = 1;
    req.aux = &aux;

    const char *val = NULL;
    size_t vlen = 0;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      httpd_req_get_hdr_value_str_ptr(NULL, "X-Custom", &val, &vlen));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      httpd_req_get_hdr_value_str_ptr(&req, NULL, &val, &vlen));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      httpd_req_get_hdr_value_str_ptr(&req, "X-Custom", NULL, &vlen));
    /* Regression-style guard: a value is present, so a NULL val_len must be
     * rejected rather than written to */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      httpd_req_get_hdr_value_str_ptr(&req, "X-Custom", &val, NULL));
}

void app_main(void)
{
    unity_run_menu();
}
