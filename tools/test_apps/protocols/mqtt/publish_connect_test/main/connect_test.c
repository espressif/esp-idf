/* MQTT connect test

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdint.h>
#include "esp_console.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "publish_connect_test.h"

#if (!defined(CONFIG_EXAMPLE_CONNECT_CASE_NO_CERT)) ||                  \
    (!defined(CONFIG_EXAMPLE_CONNECT_CASE_SERVER_CERT)) ||              \
    (!defined(CONFIG_EXAMPLE_CONNECT_CASE_MUTUAL_AUTH)) ||              \
    (!defined(CONFIG_EXAMPLE_CONNECT_CASE_INVALID_SERVER_CERT)) ||      \
    (!defined(CONFIG_EXAMPLE_CONNECT_CASE_SERVER_DER_CERT)) ||          \
    (!defined(CONFIG_EXAMPLE_CONNECT_CASE_MUTUAL_AUTH_KEY_PWD)) ||      \
    (!defined(CONFIG_EXAMPLE_CONNECT_CASE_MUTUAL_AUTH_BAD_CRT)) ||      \
    (!defined(CONFIG_EXAMPLE_CONNECT_CASE_NO_CERT_ALPN))
#error "Some mandatory test case not defined!"
#endif

extern const uint8_t ca_local_crt[]         asm("_binary_ca_crt_start");
extern const uint8_t ca_der_start[]         asm("_binary_ca_der_start");
extern const uint8_t ca_der_end[]           asm("_binary_ca_der_end");
extern const uint8_t client_pwd_crt[]       asm("_binary_client_pwd_crt_start");
extern const uint8_t client_pwd_key[]       asm("_binary_client_pwd_key_start");
extern const uint8_t client_inv_crt[]       asm("_binary_client_inv_crt_start");
extern const uint8_t client_no_pwd_key[]    asm("_binary_client_no_pwd_key_start");

static const char *TAG = "connect_test";
static int running_test_case = 0;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    (void)handler_args;
    (void)base;
    (void)event_id;
    esp_mqtt_event_handle_t event = event_data;
    ESP_LOGD(TAG, "Event: %d, Test case: %d", event->event_id, running_test_case);
    switch (event->event_id) {
    case MQTT_EVENT_BEFORE_CONNECT:
        break;
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED: Test=%d", running_test_case);
        break;
    case MQTT_EVENT_DISCONNECTED:
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR: Test=%d", running_test_case);
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_ESP_TLS) {
            ESP_LOGI(TAG, "ESP-TLS ERROR: %s", esp_err_to_name(event->error_handle->esp_tls_last_esp_err));
        } else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
            ESP_LOGI(TAG, "MQTT ERROR: 0x%x", event->error_handle->connect_return_code);
        } else {
            ESP_LOGW(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void connect_no_certs(esp_mqtt_client_handle_t client, const char *uri)
{
    ESP_LOGI(TAG, "Runnning :CONFIG_EXAMPLE_CONNECT_CASE_NO_CERT");
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri
    };
    esp_mqtt_set_config(client, &mqtt_cfg);
}

static void connect_with_client_key_password(esp_mqtt_client_handle_t client, const char *uri)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
        .broker.verification.certificate = (const char *)ca_local_crt,
        .credentials.authentication.certificate = (const char *)client_pwd_crt,
        .credentials.authentication.key = (const char *)client_pwd_key,
        .credentials.authentication.key_password = "esp32",
        .credentials.authentication.key_password_len = 5
    };
    esp_mqtt_set_config(client, &mqtt_cfg);
}

static void connect_with_server_der_cert(esp_mqtt_client_handle_t client, const char *uri)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
        .broker.verification.certificate = (const char *)ca_der_start,
        .broker.verification.certificate_len = ca_der_end - ca_der_start,
        .credentials.authentication.certificate = "NULL",
        .credentials.authentication.key = "NULL"
    };
    esp_mqtt_set_config(client, &mqtt_cfg);
}

static void connect_with_wrong_server_cert(esp_mqtt_client_handle_t client, const char *uri)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
        .broker.verification.certificate = (const char *)client_pwd_crt,
        .credentials.authentication.certificate = "NULL",
        .credentials.authentication.key = "NULL"
    };
    esp_mqtt_set_config(client, &mqtt_cfg);
}

static void connect_with_server_cert(esp_mqtt_client_handle_t client, const char *uri)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
        .broker.verification.certificate = (const char *)ca_local_crt,
    };
    esp_mqtt_set_config(client, &mqtt_cfg);
}

static void connect_with_server_client_certs(esp_mqtt_client_handle_t client, const char *uri)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
        .broker.verification.certificate = (const char *)ca_local_crt,
        .credentials.authentication.certificate = (const char *)client_pwd_crt,
        .credentials.authentication.key = (const char *)client_no_pwd_key
    };
    esp_mqtt_set_config(client, &mqtt_cfg);
}

static void connect_with_invalid_client_certs(esp_mqtt_client_handle_t client, const char *uri)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
        .broker.verification.certificate = (const char *)ca_local_crt,
        .credentials.authentication.certificate = (const char *)client_inv_crt,
        .credentials.authentication.key = (const char *)client_no_pwd_key
    };
    esp_mqtt_set_config(client, &mqtt_cfg);
}

static void connect_with_alpn(esp_mqtt_client_handle_t client, const char *uri)
{
    const char *alpn_protos[] = { "mymqtt", NULL };
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
        .broker.verification.alpn_protos = alpn_protos
    };
    esp_mqtt_set_config(client, &mqtt_cfg);
}

void connect_setup(command_context_t * ctx) {
    esp_mqtt_client_register_event(ctx->mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, ctx->data);
}

void connect_teardown(command_context_t * ctx) {
    esp_mqtt_client_unregister_event(ctx->mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler);
}
void connection_test(command_context_t * ctx, const char *uri, int test_case)
{
    ESP_LOGI(TAG, "CASE:%d, connecting to %s", test_case, uri);
    running_test_case = test_case;
    switch (test_case) {
    case CONFIG_EXAMPLE_CONNECT_CASE_NO_CERT:
        connect_no_certs(ctx->mqtt_client, uri);
        break;
    case CONFIG_EXAMPLE_CONNECT_CASE_SERVER_CERT:
        connect_with_server_cert(ctx->mqtt_client, uri);
        break;
    case CONFIG_EXAMPLE_CONNECT_CASE_MUTUAL_AUTH:
        connect_with_server_client_certs(ctx->mqtt_client, uri);
        break;
    case CONFIG_EXAMPLE_CONNECT_CASE_INVALID_SERVER_CERT:
        connect_with_wrong_server_cert(ctx->mqtt_client, uri);
        break;
    case CONFIG_EXAMPLE_CONNECT_CASE_SERVER_DER_CERT:
        connect_with_server_der_cert(ctx->mqtt_client, uri);
        break;
    case CONFIG_EXAMPLE_CONNECT_CASE_MUTUAL_AUTH_KEY_PWD:
        connect_with_client_key_password(ctx->mqtt_client, uri);
        break;
    case CONFIG_EXAMPLE_CONNECT_CASE_MUTUAL_AUTH_BAD_CRT:
        connect_with_invalid_client_certs(ctx->mqtt_client, uri);
        break;
    case CONFIG_EXAMPLE_CONNECT_CASE_NO_CERT_ALPN:
        connect_with_alpn(ctx->mqtt_client, uri);
        break;
    default:
        ESP_LOGE(TAG, "Unknown test case %d ", test_case);
        break;
    }
    ESP_LOGI(TAG, "Test case:%d started", test_case);
}
