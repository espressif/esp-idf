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

void app_main(void)
{
    unity_run_menu();
}
