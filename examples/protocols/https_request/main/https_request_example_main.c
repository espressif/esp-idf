/*
 * HTTPS GET Example using plain Mbed TLS sockets
 *
 * Contacts the howsmyssl.com API via TLS v1.2 and reads a JSON
 * response.
 *
 * Adapted from the ssl_client1 example in Mbed TLS.
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2015-2023 Espressif Systems (Shanghai) CO LTD
 */

#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "protocol_examples_common.h"
#include "esp_sntp.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "esp_tls.h"
#include "sdkconfig.h"
#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE && CONFIG_EXAMPLE_USING_ESP_TLS_MBEDTLS
#include "esp_crt_bundle.h"
#endif
#include "time_sync.h"

/* Constants that aren't configurable in menuconfig */
#define WEB_SERVER "www.howsmyssl.com"
#define WEB_PORT "443"
#define WEB_URL "https://www.howsmyssl.com/a/check"

#define SERVER_URL_MAX_SZ 256

static const char *TAG = "example";

/* Timer interval once every day (24 Hours) */
#define TIME_PERIOD (86400000000ULL)

static const char HOWSMYSSL_REQUEST[] = "GET " WEB_URL " HTTP/1.1\r\n"
                             "Host: "WEB_SERVER"\r\n"
                             "User-Agent: esp-idf/1.0 esp32\r\n"
                             "\r\n";

#ifdef CONFIG_EXAMPLE_CLIENT_SESSION_TICKETS
static const char LOCAL_SRV_REQUEST[] = "GET " CONFIG_EXAMPLE_LOCAL_SERVER_URL " HTTP/1.1\r\n"
                             "Host: "WEB_SERVER"\r\n"
                             "User-Agent: esp-idf/1.0 esp32\r\n"
                             "\r\n";
#endif

/* Root cert for howsmyssl.com, taken from server_root_cert.pem

   The PEM file was extracted from the output of this command:
   openssl s_client -showcerts -connect www.howsmyssl.com:443 </dev/null

   The CA root cert is the last cert given in the chain of certs.

   To embed it in the app binary, the PEM file is named
   in the component.mk COMPONENT_EMBED_TXTFILES variable.
*/
extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
extern const uint8_t server_root_cert_pem_end[]   asm("_binary_server_root_cert_pem_end");

extern const uint8_t local_server_cert_pem_start[] asm("_binary_local_server_cert_pem_start");
extern const uint8_t local_server_cert_pem_end[]   asm("_binary_local_server_cert_pem_end");
#if CONFIG_EXAMPLE_USING_ESP_TLS_MBEDTLS
static const int server_supported_ciphersuites[] = {MBEDTLS_TLS_RSA_WITH_AES_256_GCM_SHA384, MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256, 0};
static const int server_unsupported_ciphersuites[] = {MBEDTLS_TLS_ECDHE_RSA_WITH_ARIA_128_CBC_SHA256, 0};
#endif
#ifdef CONFIG_EXAMPLE_CLIENT_SESSION_TICKETS
static esp_tls_client_session_t *tls_client_session = NULL;
static bool save_client_session = false;
#endif

static void https_get_request(esp_tls_cfg_t cfg, const char *WEB_SERVER_URL, const char *REQUEST)
{
    char buf[512];
    int ret, len;

    esp_tls_t *tls = esp_tls_init();
    if (!tls) {
        ESP_LOGE(TAG, "Failed to allocate esp_tls handle!");
        goto exit;
    }

    if (esp_tls_conn_http_new_sync(WEB_SERVER_URL, &cfg, tls) == 1) {
        ESP_LOGI(TAG, "Connection established...");
    } else {
        ESP_LOGE(TAG, "Connection failed...");
        int esp_tls_code = 0, esp_tls_flags = 0;
        esp_tls_error_handle_t tls_e = NULL;
        esp_tls_get_error_handle(tls, &tls_e);
        /* Try to get TLS stack level error and certificate failure flags, if any */
        ret = esp_tls_get_and_clear_last_error(tls_e, &esp_tls_code, &esp_tls_flags);
        if (ret == ESP_OK) {
            ESP_LOGE(TAG, "TLS error = -0x%x, TLS flags = -0x%x", esp_tls_code, esp_tls_flags);
        }
        goto cleanup;
    }

#ifdef CONFIG_EXAMPLE_CLIENT_SESSION_TICKETS
    /* The TLS session is successfully established, now saving the session ctx for reuse */
    if (save_client_session) {
        esp_tls_free_client_session(tls_client_session);
        tls_client_session = esp_tls_get_client_session(tls);
    }
#endif

    size_t written_bytes = 0;
    do {
        ret = esp_tls_conn_write(tls,
                                 REQUEST + written_bytes,
                                 strlen(REQUEST) - written_bytes);
        if (ret >= 0) {
            ESP_LOGI(TAG, "%d bytes written", ret);
            written_bytes += ret;
        } else if (ret != ESP_TLS_ERR_SSL_WANT_READ  && ret != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGE(TAG, "esp_tls_conn_write  returned: [0x%02X](%s)", ret, esp_err_to_name(ret));
            goto cleanup;
        }
    } while (written_bytes < strlen(REQUEST));

    ESP_LOGI(TAG, "Reading HTTP response...");
    do {
        len = sizeof(buf) - 1;
        memset(buf, 0x00, sizeof(buf));
        ret = esp_tls_conn_read(tls, (char *)buf, len);

        if (ret == ESP_TLS_ERR_SSL_WANT_WRITE  || ret == ESP_TLS_ERR_SSL_WANT_READ) {
            continue;
        } else if (ret < 0) {
            ESP_LOGE(TAG, "esp_tls_conn_read  returned [-0x%02X](%s)", -ret, esp_err_to_name(ret));
            break;
        } else if (ret == 0) {
            ESP_LOGI(TAG, "connection closed");
            break;
        }

        len = ret;
        ESP_LOGD(TAG, "%d bytes read", len);
        /* Print response directly to stdout as it is read */
        for (int i = 0; i < len; i++) {
            putchar(buf[i]);
        }
        putchar('\n'); // JSON output doesn't have a newline at end
    } while (1);

cleanup:
    esp_tls_conn_destroy(tls);
exit:
    for (int countdown = 10; countdown >= 0; countdown--) {
        ESP_LOGI(TAG, "%d...", countdown);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE && CONFIG_EXAMPLE_USING_ESP_TLS_MBEDTLS
static void https_get_request_using_crt_bundle(void)
{
    ESP_LOGI(TAG, "https_request using crt bundle");
    esp_tls_cfg_t cfg = {
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    https_get_request(cfg, WEB_URL, HOWSMYSSL_REQUEST);
}
#endif // CONFIG_MBEDTLS_CERTIFICATE_BUNDLE && CONFIG_EXAMPLE_USING_ESP_TLS_MBEDTLS

static void https_get_request_using_cacert_buf(void)
{
    ESP_LOGI(TAG, "https_request using cacert_buf");
    esp_tls_cfg_t cfg = {
        .cacert_buf = (const unsigned char *) server_root_cert_pem_start,
        .cacert_bytes = server_root_cert_pem_end - server_root_cert_pem_start,
    };
    https_get_request(cfg, WEB_URL, HOWSMYSSL_REQUEST);
}

static void https_get_request_using_specified_ciphersuites(void)
{
#if CONFIG_EXAMPLE_USING_ESP_TLS_MBEDTLS

    ESP_LOGI(TAG, "https_request using server supported ciphersuites");
    esp_tls_cfg_t cfg = {
        .cacert_buf = (const unsigned char *) server_root_cert_pem_start,
        .cacert_bytes = server_root_cert_pem_end - server_root_cert_pem_start,
        .ciphersuites_list = server_supported_ciphersuites,
    };

    https_get_request(cfg, WEB_URL, HOWSMYSSL_REQUEST);

    ESP_LOGI(TAG, "https_request using server unsupported ciphersuites");

    cfg.ciphersuites_list = server_unsupported_ciphersuites;

    https_get_request(cfg, WEB_URL, HOWSMYSSL_REQUEST);
#endif
}

static void https_get_request_using_global_ca_store(void)
{
    esp_err_t esp_ret = ESP_FAIL;
    ESP_LOGI(TAG, "https_request using global ca_store");
    esp_ret = esp_tls_set_global_ca_store(server_root_cert_pem_start, server_root_cert_pem_end - server_root_cert_pem_start);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Error in setting the global ca store: [%02X] (%s),could not complete the https_request using global_ca_store", esp_ret, esp_err_to_name(esp_ret));
        return;
    }
    esp_tls_cfg_t cfg = {
        .use_global_ca_store = true,
    };
    https_get_request(cfg, WEB_URL, HOWSMYSSL_REQUEST);
    esp_tls_free_global_ca_store();
}

#ifdef CONFIG_EXAMPLE_CLIENT_SESSION_TICKETS
static void https_get_request_to_local_server(const char* url)
{
    ESP_LOGI(TAG, "https_request to local server");
    esp_tls_cfg_t cfg = {
        .cacert_buf = (const unsigned char *) local_server_cert_pem_start,
        .cacert_bytes = local_server_cert_pem_end - local_server_cert_pem_start,
        .skip_common_name = true,
    };
    save_client_session = true;
    https_get_request(cfg, url, LOCAL_SRV_REQUEST);
}

static void https_get_request_using_already_saved_session(const char *url)
{
    ESP_LOGI(TAG, "https_request using saved client session");
    esp_tls_cfg_t cfg = {
        .client_session = tls_client_session,
    };
    https_get_request(cfg, url, LOCAL_SRV_REQUEST);
    esp_tls_free_client_session(tls_client_session);
    save_client_session = false;
    tls_client_session = NULL;
}
#endif

static void https_request_task(void *pvparameters)
{
    ESP_LOGI(TAG, "Start https_request example");

#ifdef CONFIG_EXAMPLE_CLIENT_SESSION_TICKETS
    char *server_url = NULL;
#ifdef CONFIG_EXAMPLE_LOCAL_SERVER_URL_FROM_STDIN
    char url_buf[SERVER_URL_MAX_SZ];
    if (strcmp(CONFIG_EXAMPLE_LOCAL_SERVER_URL, "FROM_STDIN") == 0) {
        example_configure_stdin_stdout();
        fgets(url_buf, SERVER_URL_MAX_SZ, stdin);
        int len = strlen(url_buf);
        url_buf[len - 1] = '\0';
        server_url = url_buf;
    } else {
        ESP_LOGE(TAG, "Configuration mismatch: invalid url for local server");
        abort();
    }
    printf("\nServer URL obtained is %s\n", url_buf);
#else
    server_url = CONFIG_EXAMPLE_LOCAL_SERVER_URL;
#endif /* CONFIG_EXAMPLE_LOCAL_SERVER_URL_FROM_STDIN */
    https_get_request_to_local_server(server_url);
    https_get_request_using_already_saved_session(server_url);
#endif

#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE && CONFIG_EXAMPLE_USING_ESP_TLS_MBEDTLS
    https_get_request_using_crt_bundle();
#endif
    ESP_LOGI(TAG, "Minimum free heap size: %" PRIu32 " bytes", esp_get_minimum_free_heap_size());
    https_get_request_using_cacert_buf();
    https_get_request_using_global_ca_store();
    https_get_request_using_specified_ciphersuites();
    ESP_LOGI(TAG, "Finish https_request example");
    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    if (esp_reset_reason() == ESP_RST_POWERON) {
        ESP_LOGI(TAG, "Updating time from NVS");
        ESP_ERROR_CHECK(update_time_from_nvs());
    }

    const esp_timer_create_args_t nvs_update_timer_args = {
            .callback = (void *)&fetch_and_store_time_in_nvs,
    };

    esp_timer_handle_t nvs_update_timer;
    ESP_ERROR_CHECK(esp_timer_create(&nvs_update_timer_args, &nvs_update_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(nvs_update_timer, TIME_PERIOD));

    xTaskCreate(&https_request_task, "https_get_task", 8192, NULL, 5, NULL);
}
