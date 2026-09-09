/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file test_http_client_basic.c
 * @brief P0 Critical Tests: Basic HTTP Client Functionality
 *
 * This file contains P0 (critical) tests covering:
 * - HTTP methods (GET, POST, PUT, DELETE, HEAD)
 * - HTTP status codes (2xx success, 4xx client errors, 5xx server errors)
 * - Basic request/response handling
 * - Both positive (success) and negative (error) scenarios
 */

#include <stdlib.h>
#include <string.h>
#include "esp_http_client.h"
#include "esp_log.h"
#include "unity.h"
#include "sdkconfig.h"
#include "test_http_client_mock_transport.h"

/*
 * Every case in this file drives the client through a mock transport injected
 * via esp_http_client_config_t::transport. Without custom transport support
 * the clients would fall back to a real transport aimed at test-server.local,
 * which does not exist, so the whole file compiles out.
 */
#if CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT

static const char *TAG = "test_basic";

/* ============================================
 * Test Response Templates
 * ============================================ */

// 200 OK - Successful GET response
static const char *response_200_ok =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: 32\r\n"
    "\r\n"
    "{\"message\":\"success\",\"code\":200}";

// 201 Created - Successful POST response
static const char *response_201_created =
    "HTTP/1.1 201 Created\r\n"
    "Content-Type: application/json\r\n"
    "Location: /resource/12345\r\n"
    "Content-Length: 31\r\n"
    "\r\n"
    "{\"id\":12345,\"status\":\"created\"}";

// 204 No Content - Successful DELETE response (no body)
static const char *response_204_no_content =
    "HTTP/1.1 204 No Content\r\n"
    "Content-Length: 0\r\n"
    "\r\n";

// 400 Bad Request - Client error
static const char *response_400_bad_request =
    "HTTP/1.1 400 Bad Request\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: 34\r\n"
    "\r\n"
    "{\"error\":\"Invalid request format\"}";

// 401 Unauthorized - Authentication required
static const char *response_401_unauthorized =
    "HTTP/1.1 401 Unauthorized\r\n"
    "WWW-Authenticate: Basic realm=\"Test\"\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: 35\r\n"
    "\r\n"
    "{\"error\":\"Authentication required\"}";

// 404 Not Found - Resource not found
static const char *response_404_not_found =
    "HTTP/1.1 404 Not Found\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: 30\r\n"
    "\r\n"
    "{\"error\":\"Resource not found\"}";

// 500 Internal Server Error
static const char *response_500_server_error =
    "HTTP/1.1 500 Internal Server Error\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: 33\r\n"
    "\r\n"
    "{\"error\":\"Internal server error\"}";

// 503 Service Unavailable
static const char *response_503_unavailable =
    "HTTP/1.1 503 Service Unavailable\r\n"
    "Retry-After: 60\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: 36\r\n"
    "\r\n"
    "{\"error\":\"Service temporarily down\"}";

/* ============================================
 * Helper Functions
 * ============================================ */

static esp_err_t basic_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADERS_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADERS_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER: %s: %s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA: %d bytes", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    default:
        break;
    }
    return ESP_OK;
}

/* ============================================
 * P0 Test: HTTP Methods - Positive Cases
 * ============================================ */

/**
 * Test: GET request succeeds with 200 OK
 *
 * Positive scenario: Normal GET request returns success
 */
TEST_CASE("GET request succeeds with 200 OK", "[esp_http_client][basic][p0][positive]")
{
    ESP_LOGI(TAG, "Testing GET request - positive case");

    // Setup mock transport with 200 OK response
    mock_http_transport_config_t mock_config = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mock_config.mode = MOCK_TRANSPORT_MODE_NORMAL;
    mock_config.response_data = response_200_ok;
    mock_config.response_len = strlen(response_200_ok);

    esp_transport_handle_t mock_transport = mock_http_transport_create(&mock_config);
    TEST_ASSERT_NOT_NULL(mock_transport);

    // Create HTTP client
    esp_http_client_config_t config = {
        .url = "http://test-server.local/api/data",
        .method = HTTP_METHOD_GET,
        .event_handler = basic_event_handler,
        .transport = mock_transport,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    // Perform request
    esp_err_t err = esp_http_client_perform(client);

    // Verify success
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(200, esp_http_client_get_status_code(client));
    TEST_ASSERT_EQUAL(32, esp_http_client_get_content_length(client));

    // Verify transport was used
    mock_http_transport_stats_t stats;
    mock_http_transport_get_stats(mock_transport, &stats);
    TEST_ASSERT_EQUAL(1, stats.connect_calls);
    TEST_ASSERT_GREATER_THAN(0, stats.write_calls);  // Sent request
    TEST_ASSERT_GREATER_THAN(0, stats.read_calls);   // Received response

    ESP_LOGI(TAG, "OK: GET request successful");

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock_transport);
}

/**
 * Test: POST request succeeds with 201 Created
 *
 * Positive scenario: POST with body returns success
 */
TEST_CASE("POST request succeeds with 201 Created", "[esp_http_client][basic][p0][positive]")
{
    ESP_LOGI(TAG, "Testing POST request - positive case");

    // Setup mock transport with 201 Created response
    mock_http_transport_config_t mock_config = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mock_config.mode = MOCK_TRANSPORT_MODE_NORMAL;
    mock_config.response_data = response_201_created;
    mock_config.response_len = strlen(response_201_created);

    esp_transport_handle_t mock_transport = mock_http_transport_create(&mock_config);
    TEST_ASSERT_NOT_NULL(mock_transport);

    // Create HTTP client
    esp_http_client_config_t config = {
        .url = "http://test-server.local/api/resource",
        .method = HTTP_METHOD_POST,
        .event_handler = basic_event_handler,
        .transport = mock_transport,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    // Set POST data
    const char *post_data = "{\"name\":\"test\",\"value\":123}";
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    // Perform request
    esp_err_t err = esp_http_client_perform(client);

    // Verify success
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(201, esp_http_client_get_status_code(client));

    // Verify POST data was sent
    mock_http_transport_stats_t stats;
    mock_http_transport_get_stats(mock_transport, &stats);
    TEST_ASSERT_GREATER_THAN(strlen(post_data), stats.total_bytes_written);

    ESP_LOGI(TAG, "OK: POST request successful");

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock_transport);
}

/**
 * Test: PUT request succeeds with 200 OK
 *
 * Positive scenario: PUT request updates resource
 */
TEST_CASE("PUT request succeeds with 200 OK", "[esp_http_client][basic][p0][positive]")
{
    ESP_LOGI(TAG, "Testing PUT request - positive case");

    mock_http_transport_config_t mock_config = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mock_config.mode = MOCK_TRANSPORT_MODE_NORMAL;
    mock_config.response_data = response_200_ok;
    mock_config.response_len = strlen(response_200_ok);

    esp_transport_handle_t mock_transport = mock_http_transport_create(&mock_config);
    TEST_ASSERT_NOT_NULL(mock_transport);

    esp_http_client_config_t config = {
        .url = "http://test-server.local/api/resource/123",
        .method = HTTP_METHOD_PUT,
        .event_handler = basic_event_handler,
        .transport = mock_transport,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    const char *put_data = "{\"status\":\"updated\"}";
    esp_http_client_set_post_field(client, put_data, strlen(put_data));

    esp_err_t err = esp_http_client_perform(client);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(200, esp_http_client_get_status_code(client));

    ESP_LOGI(TAG, "OK: PUT request successful");

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock_transport);
}

/**
 * Test: DELETE request succeeds with 204 No Content
 *
 * Positive scenario: DELETE removes resource, no body returned
 */
TEST_CASE("DELETE request succeeds with 204 No Content", "[esp_http_client][basic][p0][positive]")
{
    ESP_LOGI(TAG, "Testing DELETE request - positive case");

    mock_http_transport_config_t mock_config = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mock_config.mode = MOCK_TRANSPORT_MODE_NORMAL;
    mock_config.response_data = response_204_no_content;
    mock_config.response_len = strlen(response_204_no_content);

    esp_transport_handle_t mock_transport = mock_http_transport_create(&mock_config);
    TEST_ASSERT_NOT_NULL(mock_transport);

    esp_http_client_config_t config = {
        .url = "http://test-server.local/api/resource/123",
        .method = HTTP_METHOD_DELETE,
        .event_handler = basic_event_handler,
        .transport = mock_transport,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    esp_err_t err = esp_http_client_perform(client);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(204, esp_http_client_get_status_code(client));
    TEST_ASSERT_EQUAL(0, esp_http_client_get_content_length(client));  // No content

    ESP_LOGI(TAG, "OK: DELETE request successful");

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock_transport);
}

/**
 * Test: HEAD request succeeds with headers only
 *
 * Positive scenario: HEAD request returns headers but no body
 */
TEST_CASE("HEAD request succeeds with headers only", "[esp_http_client][basic][p0][positive]")
{
    ESP_LOGI(TAG, "Testing HEAD request - positive case");

    // HEAD response has headers but no body
    const char *response_head =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 1024\r\n"  // Says content length but sends no body
        "\r\n";

    mock_http_transport_config_t mock_config = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mock_config.mode = MOCK_TRANSPORT_MODE_NORMAL;
    mock_config.response_data = response_head;
    mock_config.response_len = strlen(response_head);

    esp_transport_handle_t mock_transport = mock_http_transport_create(&mock_config);
    TEST_ASSERT_NOT_NULL(mock_transport);

    esp_http_client_config_t config = {
        .url = "http://test-server.local/api/resource",
        .method = HTTP_METHOD_HEAD,
        .event_handler = basic_event_handler,
        .transport = mock_transport,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    esp_err_t err = esp_http_client_perform(client);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(200, esp_http_client_get_status_code(client));
    // Note: Content-Length header says 1024, but no body should be received for HEAD

    ESP_LOGI(TAG, "OK: HEAD request successful");

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock_transport);
}

/* ============================================
 * P0 Test: HTTP Status Codes - 4xx Client Errors
 * ============================================ */

/**
 * Test: Client handles 400 Bad Request gracefully
 *
 * Negative scenario: Server rejects malformed request
 * Expected: Client reports error but doesn't crash, can be reused
 */
TEST_CASE("Client handles 400 Bad Request error", "[esp_http_client][basic][p0][negative]")
{
    ESP_LOGI(TAG, "Testing 400 Bad Request - negative case");

    mock_http_transport_config_t mock_config = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mock_config.mode = MOCK_TRANSPORT_MODE_NORMAL;
    mock_config.response_data = response_400_bad_request;
    mock_config.response_len = strlen(response_400_bad_request);

    esp_transport_handle_t mock_transport = mock_http_transport_create(&mock_config);
    TEST_ASSERT_NOT_NULL(mock_transport);

    esp_http_client_config_t config = {
        .url = "http://test-server.local/api/bad",
        .event_handler = basic_event_handler,
        .transport = mock_transport,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    // Request completes but with error status
    esp_err_t err = esp_http_client_perform(client);
    TEST_ASSERT_EQUAL(ESP_OK, err);  // Transport succeeds, but status is 400
    TEST_ASSERT_EQUAL(400, esp_http_client_get_status_code(client));

    // Verify client can be reused after 4xx error
    mock_config.response_data = response_200_ok;
    mock_config.response_len = strlen(response_200_ok);
    mock_http_transport_set_config(mock_transport, &mock_config);

    err = esp_http_client_perform(client);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(200, esp_http_client_get_status_code(client));

    ESP_LOGI(TAG, "OK: 400 error handled gracefully, client reusable");

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock_transport);
}

/**
 * Test: Client handles 401 Unauthorized
 *
 * Negative scenario: Authentication required, no credentials configured.
 *
 * characterization: master behavior, see refactor spec
 * This test app's sdkconfig.ci.default sets
 * CONFIG_ESP_HTTP_CLIENT_ENABLE_BASIC_AUTH=y, so esp_http_client_add_auth()
 * (esp_http_client.c ~L2132-2198) can recognize the "Basic" scheme in this
 * response's WWW-Authenticate header. That function sets
 * client->process_again = 1 purely from successfully parsing that header -
 * it never checks whether any credentials are actually configured. The
 * credential check happens later and separately, in
 * esp_http_client_prepare() (~L800-803): it only gates whether an
 * Authorization header gets *attached* to the retried request, not whether
 * a retry is *attempted*. So this client - no auth_type, no
 * username/password, no URL-embedded credentials - still retries on a 401:
 * it resends a byte-identical, credential-less request. This is the same
 * shared redirect_counter / max_authorization_retries mechanism the FSM
 * refactor's auth-retry handling and counter-split fix are meant to
 * address. To pin that credential-less-retry behavior deterministically
 * (rather than depend on how many canned responses happen to be queued),
 * this test caps the retry at 1 and queues a second identical 401, so the
 * client hits esp_http_client_add_auth()'s own
 * "redirect_counter >= max_authorization_retries" guard (~L2140-2143)
 * on the second 401 and terminates with a real, mock-independent outcome.
 */
TEST_CASE("Client handles 401 Unauthorized error", "[esp_http_client][basic][p0][negative]")
{
    ESP_LOGI(TAG, "Testing 401 Unauthorized - negative case");

    mock_http_transport_config_t mock_config = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mock_config.mode = MOCK_TRANSPORT_MODE_NORMAL;
    mock_config.response_data = response_401_unauthorized;
    mock_config.response_len = strlen(response_401_unauthorized);

    esp_transport_handle_t mock_transport = mock_http_transport_create(&mock_config);
    TEST_ASSERT_NOT_NULL(mock_transport);
    /* Second 401 so the credential-less retry lands on the deterministic
     * max_authorization_retries cap below, instead of exhausting the
     * mock's queue and timing out. */
    mock_http_transport_queue_response(mock_transport, response_401_unauthorized, 0);

    esp_http_client_config_t config = {
        .url = "http://test-server.local/api/protected",
        .event_handler = basic_event_handler,
        .max_authorization_retries = 1,
        .transport = mock_transport,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    esp_err_t err = esp_http_client_perform(client);
    /* characterization: master behavior, see refactor spec
     * The first 401 is answered by a credential-less retry (see the file
     * comment above). The second 401 then trips
     * esp_http_client_add_auth()'s "redirect_counter(1) >=
     * max_authorization_retries(1)" guard, which logs "reached
     * max_authorization_retries" and returns ESP_FAIL directly;
     * esp_http_client_perform() propagates that ESP_FAIL to the caller
     * without any further retry. */
    TEST_ASSERT_EQUAL(ESP_FAIL, err);
    /* characterization: master behavior, see refactor spec
     * status_code is a direct field read of the last response actually
     * parsed (the second 401) - untouched by the retry-cap error path. */
    TEST_ASSERT_EQUAL(401, esp_http_client_get_status_code(client));

    /* characterization: master behavior, see refactor spec
     * Confirms the retry actually happened (2 writes: the original
     * request and the one credential-less retry) rather than the client
     * simply giving up on the first 401. As in test_http_client_auth.c,
     * this assumes one mock_write() call per request's header block,
     * which held for every GET-with-no-body case observed in this suite;
     * a refactor that splits header writes across multiple
     * esp_transport_write() calls would need to update this count
     * without necessarily changing behavior. */
    mock_http_transport_stats_t stats;
    TEST_ASSERT_EQUAL(ESP_OK, mock_http_transport_get_stats(mock_transport, &stats));
    TEST_ASSERT_EQUAL(2, stats.write_calls);

    /* characterization: master behavior, see refactor spec
     * The queued second 401 triggers a queue-advance on the retry's write
     * (previous response fully read, a queued response is still pending -
     * see test_http_client_mock_transport.h), which resets the capture
     * buffer just before the retry is written. So this last-request capture
     * holds only the credential-less retry, not the original request.
     * Confirms no Authorization header is attached: no auth_type,
     * username/password, or URL-embedded credentials were ever configured
     * on this client, so esp_http_client_prepare()'s credential check
     * (~L800-803) has nothing to attach even though add_auth() unconditionally
     * schedules the retry. */
    char req[2048];
    TEST_ASSERT_EQUAL(ESP_OK, mock_http_transport_get_last_request(mock_transport, req, sizeof(req), NULL));
    /* Non-vacuity check: confirms the capture actually holds the retried
     * request line (not empty, not the original request left over from a
     * missed reset) before trusting the Authorization-absence assert below. */
    TEST_ASSERT_NOT_NULL(strstr(req, "GET /api/protected"));
    TEST_ASSERT_NULL(strstr(req, "Authorization"));

    ESP_LOGI(TAG, "OK: 401 error handled, credential-less retry capped and reported");

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock_transport);
}

/**
 * Test: Client handles 404 Not Found
 *
 * Negative scenario: Requested resource doesn't exist
 */
TEST_CASE("Client handles 404 Not Found error", "[esp_http_client][basic][p0][negative]")
{
    ESP_LOGI(TAG, "Testing 404 Not Found - negative case");

    mock_http_transport_config_t mock_config = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mock_config.mode = MOCK_TRANSPORT_MODE_NORMAL;
    mock_config.response_data = response_404_not_found;
    mock_config.response_len = strlen(response_404_not_found);

    esp_transport_handle_t mock_transport = mock_http_transport_create(&mock_config);
    TEST_ASSERT_NOT_NULL(mock_transport);

    esp_http_client_config_t config = {
        .url = "http://test-server.local/api/nonexistent",
        .event_handler = basic_event_handler,
        .transport = mock_transport,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    esp_err_t err = esp_http_client_perform(client);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(404, esp_http_client_get_status_code(client));

    ESP_LOGI(TAG, "OK: 404 error handled gracefully");

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock_transport);
}

/* ============================================
 * P0 Test: HTTP Status Codes - 5xx Server Errors
 * ============================================ */

/**
 * Test: Client handles 500 Internal Server Error
 *
 * Negative scenario: Server encounters internal error
 * Expected: Client reports error, remains usable for retry
 */
TEST_CASE("Client handles 500 Internal Server Error", "[esp_http_client][basic][p0][negative]")
{
    ESP_LOGI(TAG, "Testing 500 Server Error - negative case");

    mock_http_transport_config_t mock_config = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mock_config.mode = MOCK_TRANSPORT_MODE_NORMAL;
    mock_config.response_data = response_500_server_error;
    mock_config.response_len = strlen(response_500_server_error);

    esp_transport_handle_t mock_transport = mock_http_transport_create(&mock_config);
    TEST_ASSERT_NOT_NULL(mock_transport);

    esp_http_client_config_t config = {
        .url = "http://test-server.local/api/failing",
        .event_handler = basic_event_handler,
        .transport = mock_transport,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    // First request gets 500 error
    esp_err_t err = esp_http_client_perform(client);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(500, esp_http_client_get_status_code(client));

    ESP_LOGI(TAG, "First request got 500 error");

    // Simulate retry after server recovers
    mock_config.response_data = response_200_ok;
    mock_config.response_len = strlen(response_200_ok);
    mock_http_transport_set_config(mock_transport, &mock_config);

    err = esp_http_client_perform(client);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(200, esp_http_client_get_status_code(client));

    ESP_LOGI(TAG, "OK: 500 error handled, retry successful");

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock_transport);
}

/**
 * Test: Client handles 503 Service Unavailable
 *
 * Negative scenario: Service temporarily down
 */
TEST_CASE("Client handles 503 Service Unavailable", "[esp_http_client][basic][p0][negative]")
{
    ESP_LOGI(TAG, "Testing 503 Service Unavailable - negative case");

    mock_http_transport_config_t mock_config = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mock_config.mode = MOCK_TRANSPORT_MODE_NORMAL;
    mock_config.response_data = response_503_unavailable;
    mock_config.response_len = strlen(response_503_unavailable);

    esp_transport_handle_t mock_transport = mock_http_transport_create(&mock_config);
    TEST_ASSERT_NOT_NULL(mock_transport);

    esp_http_client_config_t config = {
        .url = "http://test-server.local/api/overloaded",
        .event_handler = basic_event_handler,
        .transport = mock_transport,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    esp_err_t err = esp_http_client_perform(client);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(503, esp_http_client_get_status_code(client));

    // Verify Retry-After header could be read
    // (In real scenarios, client would wait before retrying)

    ESP_LOGI(TAG, "OK: 503 error handled, service unavailable");

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock_transport);
}

/* ============================================
 * P0 Test: Connection Failures - Negative Cases
 * ============================================ */

/**
 * Test: Client handles connection failure gracefully
 *
 * Negative scenario: Cannot connect to server
 * Expected: Error reported, client remains in valid state
 */
TEST_CASE("Client handles connection failure", "[esp_http_client][basic][p0][negative]")
{
    ESP_LOGI(TAG, "Testing connection failure - negative case");

    mock_http_transport_config_t mock_config = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mock_config.mode = MOCK_TRANSPORT_MODE_CONNECT_FAIL;

    esp_transport_handle_t mock_transport = mock_http_transport_create(&mock_config);
    TEST_ASSERT_NOT_NULL(mock_transport);

    esp_http_client_config_t config = {
        .url = "http://unreachable-server.local/api/test",
        .event_handler = basic_event_handler,
        .transport = mock_transport,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    // Connection should fail
    esp_err_t err = esp_http_client_perform(client);
    TEST_ASSERT_NOT_EQUAL(ESP_OK, err);

    ESP_LOGI(TAG, "Connection failed as expected: %s", esp_err_to_name(err));

    // Verify client can retry after connection failure
    mock_config.mode = MOCK_TRANSPORT_MODE_NORMAL;
    mock_config.response_data = response_200_ok;
    mock_config.response_len = strlen(response_200_ok);
    mock_http_transport_set_config(mock_transport, &mock_config);

    err = esp_http_client_perform(client);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(200, esp_http_client_get_status_code(client));

    ESP_LOGI(TAG, "OK: Connection failure handled, retry successful");

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock_transport);
}

/**
 * Test: POST with empty body succeeds
 *
 * Edge case: POST request with no data
 */
TEST_CASE("POST with empty body succeeds", "[esp_http_client][basic][p0][positive]")
{
    ESP_LOGI(TAG, "Testing POST with empty body - edge case");

    mock_http_transport_config_t mock_config = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mock_config.mode = MOCK_TRANSPORT_MODE_NORMAL;
    mock_config.response_data = response_201_created;
    mock_config.response_len = strlen(response_201_created);

    esp_transport_handle_t mock_transport = mock_http_transport_create(&mock_config);
    TEST_ASSERT_NOT_NULL(mock_transport);

    esp_http_client_config_t config = {
        .url = "http://test-server.local/api/resource",
        .method = HTTP_METHOD_POST,
        .event_handler = basic_event_handler,
        .transport = mock_transport,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    // POST with empty body (Content-Length: 0)
    esp_http_client_set_post_field(client, "", 0);

    esp_err_t err = esp_http_client_perform(client);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(201, esp_http_client_get_status_code(client));

    ESP_LOGI(TAG, "OK: POST with empty body successful");

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock_transport);
}

/**
 * Test: Response with empty body (Content-Length: 0)
 *
 * Edge case: Server returns headers but no body
 */
TEST_CASE("Client handles response with empty body", "[esp_http_client][basic][p0][positive]")
{
    ESP_LOGI(TAG, "Testing response with empty body - edge case");

    const char *response_empty_body =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 0\r\n"
        "\r\n";

    mock_http_transport_config_t mock_config = MOCK_HTTP_TRANSPORT_DEFAULT_CONFIG();
    mock_config.mode = MOCK_TRANSPORT_MODE_NORMAL;
    mock_config.response_data = response_empty_body;
    mock_config.response_len = strlen(response_empty_body);

    esp_transport_handle_t mock_transport = mock_http_transport_create(&mock_config);
    TEST_ASSERT_NOT_NULL(mock_transport);

    esp_http_client_config_t config = {
        .url = "http://test-server.local/api/empty",
        .event_handler = basic_event_handler,
        .transport = mock_transport,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);

    esp_err_t err = esp_http_client_perform(client);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(200, esp_http_client_get_status_code(client));
    TEST_ASSERT_EQUAL(0, esp_http_client_get_content_length(client));

    ESP_LOGI(TAG, "OK: Empty response body handled correctly");

    esp_http_client_cleanup(client);
    mock_http_transport_destroy(mock_transport);
}

#endif // CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT
