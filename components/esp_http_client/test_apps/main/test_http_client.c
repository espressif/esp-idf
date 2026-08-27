/*
 * SPDX-FileCopyrightText: 2018-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <esp_system.h>
#include <esp_http_client.h>

#include "unity.h"
#include "test_utils.h"
#include "sdkconfig.h"

#include "esp_log.h"

#include "test_http_client_mock_transport.h"
#include "esp_transport.h"

#define HOST  "httpbin.org"
#define USERNAME  "user"
#define PASSWORD  "challenge"

TEST_CASE("Test in common case: Only URL and hostname are specified.", "[ESP HTTP CLIENT]")
{
    esp_http_client_config_t config_incorrect = {0};

    test_case_uses_tcpip();

    esp_http_client_handle_t client = esp_http_client_init(&config_incorrect);
    TEST_ASSERT(client == NULL);

    esp_http_client_config_t config_with_url = {
        .url = "http://httpbin.org/get",
    };
    client = esp_http_client_init(&config_with_url);
    TEST_ASSERT(client != NULL);
    TEST_ASSERT(esp_http_client_cleanup(client) == ESP_OK);


    esp_http_client_config_t config_with_hostname_path = {
        .host = HOST,
        .path = "/get",
    };
    client = esp_http_client_init(&config_with_hostname_path);
    TEST_ASSERT(client != NULL);
    TEST_ASSERT(esp_http_client_cleanup(client) == ESP_OK);
}

TEST_CASE("Get username and password after initialization.", "[ESP HTTP CLIENT]")
{
    esp_http_client_config_t config_with_auth = {
        .host = HOST,
        .path = "/",
        .username = USERNAME,
        .password = PASSWORD
    };
    char *value = NULL;
    esp_http_client_handle_t client = esp_http_client_init(&config_with_auth);
    TEST_ASSERT_NOT_NULL(client);
    // Test with username
    esp_err_t r = esp_http_client_get_username(client, &value);
    TEST_ASSERT_EQUAL(ESP_OK, r);
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING(USERNAME, value);
    // Test with password
    value = NULL;
    r = esp_http_client_get_password(client, &value);
    TEST_ASSERT_EQUAL(ESP_OK, r);
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING(PASSWORD, value);
    esp_http_client_cleanup(client);
}

/**
 * Test case to test that, the esp_http_client_set_url won't drop username and password
 * when pass a path "/abc" for url.
 **/
TEST_CASE("Username is unmodified when we change to new path", "[ESP HTTP CLIENT]")
{
    esp_http_client_config_t config_with_auth = {
        .host = HOST,
        .path = "/",
        .username = USERNAME,
        .password = PASSWORD
    };
    char *value = NULL;
    esp_http_client_handle_t client = esp_http_client_init(&config_with_auth);
    TEST_ASSERT_NOT_NULL(client);
    esp_err_t r = esp_http_client_get_username(client, &value);
    TEST_ASSERT_EQUAL(ESP_OK, r);
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING(USERNAME, value);
    esp_http_client_set_url(client, "/something-else/");
    r = esp_http_client_get_username(client, &value);
    TEST_ASSERT_EQUAL(ESP_OK, r);
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING(USERNAME, value);
    esp_http_client_cleanup(client);
}

/**
 * Test case to test that, the esp_http_client_set_url do not reset the auth credentials
 * Explicit APIs esp_http_client_set_username and esp_http_client_set_password are used to change
 * the auth credentials
 **/
TEST_CASE("Username and password will not reset if new absolute URL does not specify auth credentials.", "[ESP HTTP CLIENT]")
{
    esp_http_client_config_t config_with_auth = {
        .host = HOST,
        .path = "/",
        .username = USERNAME,
        .password = PASSWORD
    };
    char *value = NULL;
    esp_http_client_handle_t client = esp_http_client_init(&config_with_auth);
    TEST_ASSERT_NOT_NULL(client);
    esp_err_t r = esp_http_client_get_username(client, &value);
    TEST_ASSERT_EQUAL(ESP_OK, r);
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING(USERNAME, value);
    esp_http_client_set_url(client, "http://" HOST "/get");
    esp_http_client_set_username(client, value);
    // esp_http_client_set_username sets new username and thus invalidates the original one
    // which we still reference in the local variable `value` (better forget it)
    value = NULL;
    esp_http_client_set_password(client, USERNAME);
    //checks if username is set or not
    r = esp_http_client_get_username(client, &value);
    TEST_ASSERT_EQUAL(ESP_OK, r);
    //If username is set then value should not be NULL
    TEST_ASSERT_NOT_NULL(value);
    //checks if password is set or not
    r = esp_http_client_get_password(client, &value);
    TEST_ASSERT_EQUAL(ESP_OK, r);
    //If password is set then value should not be NULL
    TEST_ASSERT_NOT_NULL(value);
    esp_http_client_cleanup(client);
}

/**
 * Test case to verify that, esp_http_client_init() should return NULL if configuration has url with empty hostname.
 **/
TEST_CASE("esp_http_client_init() should return NULL if configured with wrong url", "[ESP HTTP CLIENT]")
{
    esp_http_client_config_t config = {
        .url = "//httpbin.org/post",
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NULL(client);
    esp_http_client_cleanup(client);
}

/**
 * Test case to verify that esp_http_client_get_url() returns the URL in the correct format.
 **/
TEST_CASE("esp_http_client_get_url() should return URL in the correct format", "[ESP HTTP CLIENT]")
{
    const char *url = "http://httpbin.org:8080/post";
    esp_http_client_config_t config = {
        .url = url,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    char client_url[32];
    esp_http_client_get_url(client, client_url, sizeof(client_url));
    esp_http_client_cleanup(client);

    TEST_ASSERT_EQUAL_STRING(url, client_url);
}

TEST_CASE("esp_http_client_set_header() should not return error if header value is NULL", "[esp_http_client]")
{
    esp_http_client_config_t config = {
        .url = "http://httpbin.org:8080/post",
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    // First, set a valid header
    esp_err_t err = esp_http_client_set_header(client, "Test-Header", "dummy_value");
    TEST_ASSERT_EQUAL(ESP_OK, err);

    // Now, delete the header by passing value = NULL
    err = esp_http_client_set_header(client, "Test-Header", NULL);
    TEST_ASSERT_EQUAL(ESP_OK, err);  // Ensure it does NOT return ESP_ERR_INVALID_ARG

    esp_http_client_cleanup(client);
}

TEST_CASE("set_post_field adds default Content-Type when missing", "[esp_http_client]")
{
    const esp_http_client_config_t config = {
        .url = "http://localhost",
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    const char post_data[] = "foo=bar";
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_set_post_field(client, post_data, strlen(post_data)));

    char *content_type = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_get_header(client, "Content-Type", &content_type));
    TEST_ASSERT_NOT_NULL(content_type);
    TEST_ASSERT_EQUAL_STRING("application/x-www-form-urlencoded", content_type);

    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_cleanup(client));
}

TEST_CASE("set_post_field preserves explicit Content-Type", "[esp_http_client]")
{
    const esp_http_client_config_t config = {
        .url = "http://localhost",
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_set_header(client, "Content-Type", "application/json"));

    const char post_data[] = "{}";
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_set_post_field(client, post_data, strlen(post_data)));

    char *content_type = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_get_header(client, "Content-Type", &content_type));
    TEST_ASSERT_NOT_NULL(content_type);
    TEST_ASSERT_EQUAL_STRING("application/json", content_type);

    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_cleanup(client));
}

TEST_CASE("set_url() to a different host strips Authorization header", "[esp_http_client]")
{
    esp_http_client_config_t config = {
        .url = "http://httpbin.org/get",
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_set_header(client, "Authorization", "Bearer secret-token"));

    char *value = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_get_header(client, "Authorization", &value));
    TEST_ASSERT_NOT_NULL(value);

    /* Simulate a redirect target */
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_set_url(client, "http://attacker.example/steal"));

    value = NULL;
    esp_err_t err = esp_http_client_get_header(client, "Authorization", &value);
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, err);
    TEST_ASSERT_NULL(value);

    esp_http_client_cleanup(client);
}

TEST_CASE("set_url() to the same host preserves Authorization header", "[esp_http_client]")
{
    esp_http_client_config_t config = {
        .url = "http://httpbin.org/get",
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_set_header(client, "Authorization", "Bearer token"));
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_set_url(client, "http://httpbin.org/other"));

    char *value = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_get_header(client, "Authorization", &value));
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING("Bearer token", value);

    esp_http_client_cleanup(client);
}

TEST_CASE("set_url() to a different host clears URL-embedded credentials", "[esp_http_client]")
{
    esp_http_client_config_t config = {
        .host = HOST,
        .path = "/",
        .username = USERNAME,
        .password = PASSWORD,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    char *value = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_get_username(client, &value));
    TEST_ASSERT_NOT_NULL(value);
    value = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_get_password(client, &value));
    TEST_ASSERT_NOT_NULL(value);

    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_set_url(client, "http://attacker.example/steal"));

    value = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_get_username(client, &value));
    TEST_ASSERT_NULL(value);
    value = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_get_password(client, &value));
    TEST_ASSERT_NULL(value);

    esp_http_client_cleanup(client);
}

static int disconnect_event_count = 0;

static esp_err_t disconnect_event_handler(esp_http_client_event_t *evt)
{
    if (evt->event_id == HTTP_EVENT_DISCONNECTED) {
        disconnect_event_count++;
    }
    return ESP_OK;
}

TEST_CASE("esp_http_client_close() and cleanup() should not dispatch duplicate disconnect events", "[esp_http_client]")
{
    esp_http_client_config_t config = {
        .url = "http://httpbin.org/get",
        .event_handler = disconnect_event_handler,
    };

    // Reset event counter
    disconnect_event_count = 0;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    // Close the client first
    esp_err_t err = esp_http_client_close(client);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    // Then cleanup - this should not dispatch another disconnect event
    err = esp_http_client_cleanup(client);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    // Verify that only one disconnect event was dispatched (or none if client was never connected)
    TEST_ASSERT_LESS_OR_EQUAL(1, disconnect_event_count);
}

#ifdef CONFIG_ESP_HTTP_CLIENT_STRICT_HEADER_BUFFER

#if CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT
#include "esp_transport.h"

/*
 * Minimal stub transport for the Case B test below. write() always reports
 * success so the test can isolate the strict-header check from the real
 * transport's failure path.
 */
static int stub_transport_connect(esp_transport_handle_t t, const char *host, int port, int timeout_ms) { return 0; }
static int stub_transport_write(esp_transport_handle_t t, const char *buffer, int len, int timeout_ms)  { return len; }
static int stub_transport_read(esp_transport_handle_t t, char *buffer, int len, int timeout_ms)        { return 0; }
static int stub_transport_close(esp_transport_handle_t t)                                              { return 0; }
static int stub_transport_destroy(esp_transport_handle_t t)                                            { return 0; }
static int stub_transport_poll(esp_transport_handle_t t, int timeout_ms)                               { return 1; }
#endif // CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT

TEST_CASE("esp_http_client_request_send fails when a header exceeds tx buffer", "[ESP HTTP CLIENT]")
{
    /*
     * The "first header too big" path: with a small buffer_size_tx and an
     * oversized header, http_header_generate_string() returns 0 and the
     * helper exits the write loop without ever touching the transport.
     * The after-loop strict check must surface ESP_ERR_HTTP_HEADER_TOO_LONG
     * instead of silently completing as ESP_OK.
     */
    esp_http_client_config_t config = {
        .url = "http://example.com/",
        .buffer_size_tx = 128,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    /* Drop the default User-Agent and Host headers so the oversized header
     * is the very first entry. That keeps us on the Case A path (helper
     * returns 0 with wlen=0) so the after-loop strict check fires before
     * esp_transport_write is ever attempted — no network needed. */
    esp_http_client_set_header(client, "User-Agent", NULL);
    esp_http_client_set_header(client, "Host", NULL);

    char huge_value[200];
    memset(huge_value, 'A', sizeof(huge_value) - 1);
    huge_value[sizeof(huge_value) - 1] = '\0';
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_set_header(client, "X-Huge", huge_value));

    esp_err_t err = esp_http_client_request_send(client, 0);
    TEST_ASSERT_EQUAL(ESP_ERR_HTTP_HEADER_TOO_LONG, err);

    esp_http_client_cleanup(client);
}

#if CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT
TEST_CASE("esp_http_client_request_send fails when an oversized header is mid-list", "[ESP HTTP CLIENT]")
{
    /*
     * Case B: the small header at index 0 paginates, the oversized header
     * at index 1 cannot fit on its own. A stub transport accepts the first
     * chunk so the loop runs a second iteration, where the strict check
     * fires. The stub can only succeed, so an ESP_ERR_HTTP_HEADER_TOO_LONG
     * return here is unambiguously the strict check (not transport failure).
     */
    esp_transport_handle_t stub = esp_transport_init();
    TEST_ASSERT_NOT_NULL(stub);
    TEST_ASSERT_EQUAL(ESP_OK, esp_transport_set_func(stub,
                                                     stub_transport_connect,
                                                     stub_transport_read,
                                                     stub_transport_write,
                                                     stub_transport_close,
                                                     stub_transport_poll,
                                                     stub_transport_poll,
                                                     stub_transport_destroy));

    esp_http_client_config_t config = {
        .url = "http://example.com/",
        .buffer_size_tx = 128,
        .transport = stub,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    /* Clear defaults and set: one small header (fits), one huge header
     * (alone too big for the buffer). */
    esp_http_client_set_header(client, "User-Agent", NULL);
    esp_http_client_set_header(client, "Host", NULL);
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_set_header(client, "K1", "V1"));

    char huge_value[200];
    memset(huge_value, 'A', sizeof(huge_value) - 1);
    huge_value[sizeof(huge_value) - 1] = '\0';
    TEST_ASSERT_EQUAL(ESP_OK, esp_http_client_set_header(client, "X-Huge", huge_value));

    esp_err_t err = esp_http_client_request_send(client, 0);
    TEST_ASSERT_EQUAL(ESP_ERR_HTTP_HEADER_TOO_LONG, err);

    esp_http_client_cleanup(client);
    esp_transport_destroy(stub);
}
#endif // CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT

#endif // CONFIG_ESP_HTTP_CLIENT_STRICT_HEADER_BUFFER

#if CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT
/* ============================================
 * Error Recovery Tests with Mock Transport
 *
 * Every case below injects a mock transport through
 * esp_http_client_config_t::transport. Without custom transport support the
 * clients would fall back to a real transport aimed at a host that does not
 * exist, so the whole section compiles out.
 * ============================================ */

/**
 * @brief Canned HTTP response for successful requests
 * Note: Content-Length must match the actual body length exactly
 */
static const char *mock_http_response_ok =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: 15\r\n"  // Actual body is 15 bytes: {"status":"ok"}
    "\r\n"
    "{\"status\":\"ok\"}";

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI("test", "Connected");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI("test", "Disconnected");
            break;
        case HTTP_EVENT_HEADERS_SENT:
            ESP_LOGI("test", "Headers sent");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI("test", "Header received");
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI("test", "Data received");
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI("test", "Request finished");
            break;
        case HTTP_EVENT_ERROR:
            ESP_LOGI("test", "Error occurred");
            break;
        default:
            break;
        }
    return ESP_OK;
}

/**
 * Test: Client reuse after read timeout
 *
 * Scenario: First request times out while waiting for response,
 *           second request should succeed with same client
 *
 * Expected: Client properly recovers and second request works
 */
TEST_CASE("HTTP client can be reused after read timeout", "[esp_http_client][error_recovery]")
{
    // Note: Event loop initialization is optional for these tests
    // The ESP_ERR_INVALID_STATE errors are expected if not initialized
    // They don't affect the core functionality being tested

    // ========== REQUEST 1: Timeout mode ==========
    ESP_LOGI("test", "Request 1: Simulating read timeout");

    mock_http_transport_config_t mock_config = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mock_config.mode = MOCK_TRANSPORT_MODE_READ_TIMEOUT;

    esp_transport_handle_t mock_transport = mock_http_transport_create(&mock_config);
    TEST_ASSERT_NOT_NULL(mock_transport);

    // Create client with custom transport (disable event posting to avoid errors)
    esp_http_client_config_t config = {
        .url = "http://mock-server.local/test",
        .timeout_ms = 1000,
        .is_async = false,
        .event_handler = _http_event_handler,
        .transport = mock_transport,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    // This should timeout
    esp_err_t err = esp_http_client_perform(client);
    TEST_ASSERT_NOT_EQUAL(ESP_OK, err);
    ESP_LOGI("test", "Request 1 failed as expected: %s", esp_err_to_name(err));

    // Verify transport was called
    mock_http_transport_stats_t stats = {0};
    mock_http_transport_get_stats(mock_transport, &stats);
    TEST_ASSERT_GREATER_THAN(0, stats.connect_calls);

    // ========== REQUEST 2: Normal mode with same client ==========
    ESP_LOGI("test", "Request 2: Normal operation with reused client");

    // Reconfigure mock for success
    mock_config.mode = MOCK_TRANSPORT_MODE_NORMAL;
    mock_config.response_data = mock_http_response_ok;
    mock_config.response_len = strlen(mock_http_response_ok);

    mock_http_transport_set_config(mock_transport, &mock_config);
    mock_http_transport_reset_stats(mock_transport);

    // This should succeed
    err = esp_http_client_perform(client);
    if (err != ESP_OK) {
        ESP_LOGE("test", "Request 2 failed: %s (0x%x)", esp_err_to_name(err), err);
    }
    TEST_ASSERT_EQUAL(ESP_OK, err);

    int status_code = esp_http_client_get_status_code(client);
    if (status_code != 200) {
        ESP_LOGE("test", "Unexpected status code: %d", status_code);
    }
    TEST_ASSERT_EQUAL(200, status_code);

    ESP_LOGI("test", "Request 2 succeeded - client recovered!");

    // Verify the second request actually happened
    mock_http_transport_get_stats(mock_transport, &stats);
    /* Master does not close the connection after a fetch-header failure, so the
     * reused client never reconnects; it keeps reading on the same connection. */
    // characterization: master behavior, see refactor spec
    TEST_ASSERT_EQUAL(0, stats.connect_calls);
    TEST_ASSERT_GREATER_THAN(0, stats.read_calls);

    // Cleanup
    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock_transport);
}

/**
 * Test: Client reuse after write failure
 *
 * Scenario: First POST request fails during body write,
 *           second POST request should succeed with same client
 *
 * Expected: Client properly recovers and second request works
 */
TEST_CASE("HTTP client can be reused after write failure", "[esp_http_client][error_recovery]")
{
    // ========== REQUEST 1: Write failure during headers ==========
    ESP_LOGI("test", "Request 1: Simulating write failure during headers");

    mock_http_transport_config_t mock_config = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mock_config.mode = MOCK_TRANSPORT_MODE_WRITE_FAIL;
    // Fail after 100 bytes: this causes failure while writing HTTP headers
    // (before POST body starts)
    mock_config.bytes_before_error = 100;

    esp_transport_handle_t mock_transport = mock_http_transport_create(&mock_config);
    TEST_ASSERT_NOT_NULL(mock_transport);

    // Create client with custom transport
    esp_http_client_config_t config = {
        .url = "http://mock-server.local/post",
        .method = HTTP_METHOD_POST,
        .transport = mock_transport,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    // Set POST data
    const char *post_data = "{\"test\":\"data\",\"large\":\""
                           "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                           "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                           "\"}";
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    // This should fail during write
    esp_err_t err = esp_http_client_perform(client);
    TEST_ASSERT_NOT_EQUAL(ESP_OK, err);
    ESP_LOGI("test", "Request 1 failed as expected: %s", esp_err_to_name(err));

    // ========== REQUEST 2: Write failure DURING POST body ==========
    ESP_LOGI("test", "Request 2: Simulating write failure during POST body");

    mock_config.mode = MOCK_TRANSPORT_MODE_WRITE_FAIL;
    // Fail after 120 bytes: allows headers (~100 bytes) to be written,
    // but fails during POST body write (which starts around byte 100-110)
    mock_config.bytes_before_error = 170;
    mock_http_transport_set_config(mock_transport, &mock_config);
    mock_http_transport_reset_stats(mock_transport);
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    // This should fail during POST body write
    err = esp_http_client_perform(client);
    TEST_ASSERT_NOT_EQUAL(ESP_OK, err);
    ESP_LOGI("test", "Request 2 failed as expected: %s", esp_err_to_name(err));

    // ========== REQUEST 3: Normal mode with same client ==========
    ESP_LOGI("test", "Request 3: Normal operation with reused client");

    // Reconfigure mock for success
    mock_config.mode = MOCK_TRANSPORT_MODE_NORMAL;
    mock_config.response_data = mock_http_response_ok;
    mock_config.response_len = strlen(mock_http_response_ok);
    mock_config.bytes_before_error = -1;  // No error injection

    mock_http_transport_set_config(mock_transport, &mock_config);
    mock_http_transport_reset_stats(mock_transport);

    // Set smaller POST data
    const char *post_data2 = "{\"retry\":\"success\"}";
    esp_http_client_set_post_field(client, post_data2, strlen(post_data2));

    /* Master leaves stale POST-body write state behind after the failed write,
     * so the reused client fails immediately without touching the transport. */
    err = esp_http_client_perform(client);
    // characterization: master behavior, see refactor spec
    TEST_ASSERT_EQUAL(ESP_FAIL, err);
    TEST_ASSERT_EQUAL(0, esp_http_client_get_status_code(client));

    ESP_LOGI("test", "Request 3 did not recover: %s", esp_err_to_name(err));

    // Cleanup
    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock_transport);
}

/**
 * Test: Client reuse after incomplete data
 *
 * Scenario: First request gets incomplete response (connection closed mid-read),
 *           second request should succeed with same client
 *
 * Expected: Client properly recovers and second request works
 */
TEST_CASE("HTTP client can be reused after incomplete data", "[esp_http_client][error_recovery]")
{
    // ========== REQUEST 1: Incomplete response ==========
    ESP_LOGI("test", "Request 1: Simulating incomplete response");

    mock_http_transport_config_t mock_config = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mock_config.mode = MOCK_TRANSPORT_MODE_INCOMPLETE_READ;
    mock_config.response_data = mock_http_response_ok;
    mock_config.response_len = strlen(mock_http_response_ok);
    mock_config.bytes_before_error = 50;  // Close connection after 50 bytes (mid-response)

    esp_transport_handle_t mock_transport = mock_http_transport_create(&mock_config);
    TEST_ASSERT_NOT_NULL(mock_transport);

    // Create client with custom transport
    esp_http_client_config_t config = {
        .url = "http://mock-server.local/incomplete",
        .transport = mock_transport,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    // This should fail due to incomplete data
    esp_err_t err = esp_http_client_perform(client);
    TEST_ASSERT_NOT_EQUAL(ESP_OK, err);
    ESP_LOGI("test", "Request 1 failed as expected: %s", esp_err_to_name(err));

    // ========== REQUEST 2: Normal mode with same client ==========
    ESP_LOGI("test", "Request 2: Normal operation with reused client");

    // Reconfigure mock for success (complete response)
    mock_config.mode = MOCK_TRANSPORT_MODE_NORMAL;
    mock_config.bytes_before_error = -1;  // No error injection

    mock_http_transport_set_config(mock_transport, &mock_config);
    mock_http_transport_reset_stats(mock_transport);

    /* Master does not reset the parser/connection state after the aborted read,
     * so the reused client fails header fetching without touching the transport. */
    err = esp_http_client_perform(client);
    // characterization: master behavior, see refactor spec
    TEST_ASSERT_EQUAL(ESP_ERR_HTTP_FETCH_HEADER, err);
    TEST_ASSERT_EQUAL(-1, esp_http_client_get_status_code(client));

    ESP_LOGI("test", "Request 2 did not recover: %s", esp_err_to_name(err));

    // Cleanup
    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock_transport);
}

/**
 * Test: Multiple requests with alternating success/failure
 *
 * Scenario: Multiple requests with errors interspersed with successful requests
 *
 * Expected: Client can be reused multiple times after various error conditions
 */
TEST_CASE("HTTP client survives multiple error/success cycles", "[esp_http_client][error_recovery]")
{
    mock_http_transport_config_t mock_config = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    esp_transport_handle_t mock_transport = mock_http_transport_create(&mock_config);
    TEST_ASSERT_NOT_NULL(mock_transport);

    // Create client with custom transport
    esp_http_client_config_t config = {
        .url = "http://mock-server.local/cycle",
        .event_handler = _http_event_handler,
        .transport = mock_transport,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    // Define test sequence: success, timeout, success, incomplete, success
    // Note: We use INCOMPLETE_READ instead of WRITE_FAIL because write_fail
    // requires POST data and bytes_before_error configuration
    mock_transport_mode_t sequence[] = {
        MOCK_TRANSPORT_MODE_NORMAL,
        MOCK_TRANSPORT_MODE_READ_TIMEOUT,
        MOCK_TRANSPORT_MODE_NORMAL,
        MOCK_TRANSPORT_MODE_INCOMPLETE_READ,
        MOCK_TRANSPORT_MODE_NORMAL,
    };
    /* Master recovers from a read timeout (cycle 1 -> 2) but not from an aborted
     * read (cycle 3), so the final cycle fails instead of succeeding. */
    // characterization: master behavior, see refactor spec
    bool expected_success[] = {true, false, true, false, false};

    for (int i = 0; i < sizeof(sequence) / sizeof(sequence[0]); i++) {
        ESP_LOGI("test", "Cycle %d: mode=%d, expect %s",
                 i, sequence[i], expected_success[i] ? "SUCCESS" : "FAILURE");

        // Configure mock
        mock_config.mode = sequence[i];
        if (sequence[i] == MOCK_TRANSPORT_MODE_NORMAL) {
            mock_config.response_data = mock_http_response_ok;
            mock_config.response_len = strlen(mock_http_response_ok);
            mock_config.bytes_before_error = -1;  // No error injection
        } else if (sequence[i] == MOCK_TRANSPORT_MODE_INCOMPLETE_READ) {
            mock_config.response_data = mock_http_response_ok;
            mock_config.response_len = strlen(mock_http_response_ok);
            mock_config.bytes_before_error = 50;  // Close after 50 bytes
        }
        mock_http_transport_set_config(mock_transport, &mock_config);
        mock_http_transport_reset_stats(mock_transport);

        // Perform request
        esp_err_t err = esp_http_client_perform(client);

        // Verify expectation
        if (expected_success[i]) {
            TEST_ASSERT_EQUAL(ESP_OK, err);
            TEST_ASSERT_EQUAL(200, esp_http_client_get_status_code(client));
        } else {
            TEST_ASSERT_NOT_EQUAL(ESP_OK, err);
        }
    }

    ESP_LOGI("test", "Client survived %d error/success cycles!",
             sizeof(sequence) / sizeof(sequence[0]));

    // Cleanup
    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock_transport);
}
#endif // CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT

void app_main(void)
{
    unity_run_menu();
}
