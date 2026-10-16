/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file test_esp_https_ota.c
 * @brief esp_https_ota_begin() must not download over plain HTTP unless
 *        CONFIG_ESP_HTTPS_OTA_ALLOW_HTTP is set, and must still allow it when it is.
 *
 * No case reaches the network. Each one installs http_client_init_cb, which
 * runs after esp_http_client_init() and before the first request. It records
 * the client's transport, so a case can tell where begin() stopped:
 * - not called: rejected before the client was created
 * - called, then ESP_ERR_INVALID_ARG: rejected by the transport check after it
 * - called and returns STOP_BEFORE_CONNECT: passed every check
 */

#include "esp_https_ota.h"
#include "unity.h"
#include "sdkconfig.h"

/* Returned by the callback to end begin() before it connects */
#define STOP_BEFORE_CONNECT ESP_ERR_NOT_FINISHED

/* Only its presence matters: it satisfies the server-verification check in begin */
static const char dummy_cert_pem[] = "-----BEGIN CERTIFICATE-----\n-----END CERTIFICATE-----\n";

static struct {
    const char *set_url;                  /* in: URL the callback switches to, or NULL */
    esp_err_t ret;                        /* in: callback return value */
    int calls;                            /* out: number of callback calls */
    esp_err_t set_url_err;                /* out: result of the URL switch */
    esp_http_client_transport_t seen;     /* out: transport when the callback ran */
} s_cb;

/*
 * No TEST_ASSERT in here: a failure would longjmp out of esp_https_ota_begin()
 * and leak its client. Errors are recorded and returned, so begin() cleans up.
 */
static esp_err_t init_cb(esp_http_client_handle_t client)
{
    s_cb.calls++;
    if (s_cb.set_url != NULL) {
        s_cb.set_url_err = esp_http_client_set_url(client, s_cb.set_url);
        if (s_cb.set_url_err != ESP_OK) {
            return s_cb.set_url_err;
        }
    }
    s_cb.seen = esp_http_client_get_transport_type(client);
    return s_cb.ret;
}

/* Run esp_https_ota_begin() with init_cb and abort any handle it returns */
static esp_err_t ota_begin(esp_http_client_config_t *http_cfg, const char *cb_set_url, esp_err_t cb_ret)
{
    s_cb.set_url = cb_set_url;
    s_cb.ret = cb_ret;
    s_cb.calls = 0;
    s_cb.set_url_err = ESP_OK;
    s_cb.seen = HTTP_TRANSPORT_UNKNOWN;

    http_cfg->cert_pem = dummy_cert_pem;
    esp_https_ota_config_t ota_cfg = {
        .http_config = http_cfg,
        .http_client_init_cb = init_cb,
    };
    esp_https_ota_handle_t handle = NULL;
    esp_err_t err = esp_https_ota_begin(&ota_cfg, &handle);
    if (handle != NULL) {
        esp_https_ota_abort(handle);
    }
    return err;
}

#if !CONFIG_ESP_HTTPS_OTA_ALLOW_HTTP
TEST_CASE("host/path config with unset transport_type is pinned to SSL", "[esp_https_ota]")
{
    esp_http_client_config_t http_cfg = {
        .host = "test-server.local",
        .path = "/fw.bin",
    };
    TEST_ASSERT_EQUAL(STOP_BEFORE_CONNECT, ota_begin(&http_cfg, NULL, STOP_BEFORE_CONNECT));
    TEST_ASSERT_EQUAL(1, s_cb.calls);
    TEST_ASSERT_EQUAL(HTTP_TRANSPORT_OVER_SSL, s_cb.seen);
}

TEST_CASE("http URL is rejected after init when HTTP is not allowed", "[esp_https_ota]")
{
    esp_http_client_config_t http_cfg = {
        .url = "http://test-server.local/fw.bin",
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, ota_begin(&http_cfg, NULL, ESP_OK));
    TEST_ASSERT_EQUAL(1, s_cb.calls);
    TEST_ASSERT_EQUAL(HTTP_TRANSPORT_OVER_TCP, s_cb.seen);
}

TEST_CASE("http URL with explicit OVER_SSL is rejected after init when HTTP is not allowed", "[esp_https_ota]")
{
    esp_http_client_config_t http_cfg = {
        .url = "http://test-server.local/fw.bin",
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, ota_begin(&http_cfg, NULL, ESP_OK));
    TEST_ASSERT_EQUAL(1, s_cb.calls);
    TEST_ASSERT_EQUAL(HTTP_TRANSPORT_OVER_TCP, s_cb.seen);
}

TEST_CASE("http URL set by http_client_init_cb is rejected when HTTP is not allowed", "[esp_https_ota]")
{
    esp_http_client_config_t http_cfg = {
        .url = "https://test-server.local/fw.bin",
    };
    esp_err_t err = ota_begin(&http_cfg, "http://test-server.local/fw.bin", ESP_OK);
    TEST_ASSERT_EQUAL(ESP_OK, s_cb.set_url_err);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);
    TEST_ASSERT_EQUAL(1, s_cb.calls);
    TEST_ASSERT_EQUAL(HTTP_TRANSPORT_OVER_TCP, s_cb.seen);
}

TEST_CASE("explicit TCP transport is rejected before init when HTTP is not allowed", "[esp_https_ota]")
{
    esp_http_client_config_t http_cfg = {
        .url = "http://test-server.local/fw.bin",
        .transport_type = HTTP_TRANSPORT_OVER_TCP,
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, ota_begin(&http_cfg, NULL, ESP_OK));
    TEST_ASSERT_EQUAL(0, s_cb.calls);
}

TEST_CASE("https URL with explicit TCP transport is rejected before init when HTTP is not allowed", "[esp_https_ota]")
{
    /* The URL scheme does not matter: OVER_TCP itself asks for plain HTTP */
    esp_http_client_config_t http_cfg = {
        .url = "https://test-server.local/fw.bin",
        .transport_type = HTTP_TRANSPORT_OVER_TCP,
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, ota_begin(&http_cfg, NULL, ESP_OK));
    TEST_ASSERT_EQUAL(0, s_cb.calls);
}

TEST_CASE("out-of-range transport_type is rejected before init when HTTP is not allowed", "[esp_https_ota]")
{
    /* esp_http_client_init() would treat it as plain HTTP */
    esp_http_client_config_t http_cfg = {
        .host = "test-server.local",
        .path = "/fw.bin",
        .transport_type = (esp_http_client_transport_t)0x7F,
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, ota_begin(&http_cfg, NULL, ESP_OK));
    TEST_ASSERT_EQUAL(0, s_cb.calls);
}

#else /* CONFIG_ESP_HTTPS_OTA_ALLOW_HTTP */

/* Each plain HTTP config passes every check and reaches the callback on TCP */
static void assert_http_allowed(esp_http_client_config_t *http_cfg, const char *cb_set_url)
{
    esp_err_t err = ota_begin(http_cfg, cb_set_url, STOP_BEFORE_CONNECT);
    TEST_ASSERT_EQUAL(ESP_OK, s_cb.set_url_err);
    TEST_ASSERT_EQUAL(STOP_BEFORE_CONNECT, err);
    TEST_ASSERT_EQUAL(1, s_cb.calls);
    TEST_ASSERT_EQUAL(HTTP_TRANSPORT_OVER_TCP, s_cb.seen);
}

TEST_CASE("http URL is allowed when HTTP is allowed", "[esp_https_ota]")
{
    esp_http_client_config_t http_cfg = {
        .url = "http://test-server.local/fw.bin",
    };
    assert_http_allowed(&http_cfg, NULL);
}

TEST_CASE("explicit TCP transport is allowed when HTTP is allowed", "[esp_https_ota]")
{
    esp_http_client_config_t http_cfg = {
        .url = "http://test-server.local/fw.bin",
        .transport_type = HTTP_TRANSPORT_OVER_TCP,
    };
    assert_http_allowed(&http_cfg, NULL);
}

TEST_CASE("host/path config with unset transport_type is not pinned when HTTP is allowed", "[esp_https_ota]")
{
    esp_http_client_config_t http_cfg = {
        .host = "test-server.local",
        .path = "/fw.bin",
    };
    assert_http_allowed(&http_cfg, NULL);
}

TEST_CASE("http URL set by http_client_init_cb is allowed when HTTP is allowed", "[esp_https_ota]")
{
    esp_http_client_config_t http_cfg = {
        .url = "https://test-server.local/fw.bin",
    };
    assert_http_allowed(&http_cfg, "http://test-server.local/fw.bin");
}
#endif /* !CONFIG_ESP_HTTPS_OTA_ALLOW_HTTP */

void app_main(void)
{
    unity_run_menu();
}
