/* MQTT connect test

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdint.h>
#include "esp_netif.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"

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

static const char *TAG = "CONNECT_TEST";
static esp_mqtt_client_handle_t mqtt_client = NULL;
static int running_test_case = 0;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    ESP_LOGD(TAG, "Event: %d, Test case: %d", event->event_id, running_test_case);
    switch (event->event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED: Test=%d", running_test_case);
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

static void create_client(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtts://127.0.0.1:1234"
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    mqtt_client = client;
    esp_mqtt_client_start(client);
    ESP_LOGI(TAG, "mqtt client created for connection tests");
}

static void destroy_client(void)
{
    if (mqtt_client) {
        esp_mqtt_client_stop(mqtt_client);
        esp_mqtt_client_destroy(mqtt_client);
        mqtt_client = NULL;
        ESP_LOGI(TAG, "mqtt client for connection tests destroyed");
    }
}

static void connect_no_certs(const char *host, const int port)
{
    char uri[64];
    sprintf(uri, "mqtts://%s:%d", host, port);
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri
    };
    esp_mqtt_set_config(mqtt_client, &mqtt_cfg);
    esp_mqtt_client_disconnect(mqtt_client);
    esp_mqtt_client_reconnect(mqtt_client);
}

static void connect_with_client_key_password(const char *host, const int port)
{
    char uri[64];
    sprintf(uri, "mqtts://%s:%d", host, port);
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
        .broker.verification.certificate = (const char *)ca_local_crt,
        .credentials.authentication.certificate = (const char *)client_pwd_crt,
        .credentials.authentication.key = (const char *)client_pwd_key,
        .credentials.authentication.key_password = "esp32",
        .credentials.authentication.key_password_len = 5
    };
    esp_mqtt_set_config(mqtt_client, &mqtt_cfg);
    esp_mqtt_client_disconnect(mqtt_client);
    esp_mqtt_client_reconnect(mqtt_client);
}

static void connect_with_server_der_cert(const char *host, const int port)
{
    char uri[64];
    sprintf(uri, "mqtts://%s:%d", host, port);
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
        .broker.verification.certificate = (const char *)ca_der_start,
        .broker.verification.certificate_len = ca_der_end - ca_der_start,
        .credentials.authentication.certificate = "NULL",
        .credentials.authentication.key = "NULL"
    };
    esp_mqtt_set_config(mqtt_client, &mqtt_cfg);
    esp_mqtt_client_disconnect(mqtt_client);
    esp_mqtt_client_reconnect(mqtt_client);
}

static void connect_with_wrong_server_cert(const char *host, const int port)
{
    char uri[64];
    sprintf(uri, "mqtts://%s:%d", host, port);
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
        .broker.verification.certificate = (const char *)client_pwd_crt,
        .credentials.authentication.certificate = "NULL",
        .credentials.authentication.key = "NULL"
    };
    esp_mqtt_set_config(mqtt_client, &mqtt_cfg);
    esp_mqtt_client_disconnect(mqtt_client);
    esp_mqtt_client_reconnect(mqtt_client);
}

static void connect_with_server_cert(const char *host, const int port)
{
    char uri[64];
    sprintf(uri, "mqtts://%s:%d", host, port);
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
        .broker.verification.certificate = (const char *)ca_local_crt,
    };
    esp_mqtt_set_config(mqtt_client, &mqtt_cfg);
    esp_mqtt_client_disconnect(mqtt_client);
    esp_mqtt_client_reconnect(mqtt_client);
}

static void connect_with_server_client_certs(const char *host, const int port)
{
    char uri[64];
    sprintf(uri, "mqtts://%s:%d", host, port);
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
        .broker.verification.certificate = (const char *)ca_local_crt,
        .credentials.authentication.certificate = (const char *)client_pwd_crt,
        .credentials.authentication.key = (const char *)client_no_pwd_key
    };
    esp_mqtt_set_config(mqtt_client, &mqtt_cfg);
    esp_mqtt_client_disconnect(mqtt_client);
    esp_mqtt_client_reconnect(mqtt_client);
}

static void connect_with_invalid_client_certs(const char *host, const int port)
{
    char uri[64];
    sprintf(uri, "mqtts://%s:%d", host, port);
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
        .broker.verification.certificate = (const char *)ca_local_crt,
        .credentials.authentication.certificate = (const char *)client_inv_crt,
        .credentials.authentication.key = (const char *)client_no_pwd_key
    };
    esp_mqtt_set_config(mqtt_client, &mqtt_cfg);
    esp_mqtt_client_disconnect(mqtt_client);
    esp_mqtt_client_reconnect(mqtt_client);
}

static void connect_with_alpn(const char *host, const int port)
{
    char uri[64];
    const char *alpn_protos[] = { "mymqtt", NULL };
    sprintf(uri, "mqtts://%s:%d", host, port);
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
        .broker.verification.alpn_protos = alpn_protos
    };
    esp_mqtt_set_config(mqtt_client, &mqtt_cfg);
    esp_mqtt_client_disconnect(mqtt_client);
    esp_mqtt_client_reconnect(mqtt_client);
}

void connection_test(const char *line)
{
    char test_type[32];
    char host[32];
    int port;
    int test_case;

    sscanf(line, "%s %s %d %d", test_type, host, &port, &test_case);
    if (mqtt_client == NULL) {
        create_client();
    }
    if (strcmp(host, "teardown") == 0) {
        destroy_client();;
    }
    ESP_LOGI(TAG, "CASE:%d, connecting to mqtts://%s:%d ", test_case, host, port);
    running_test_case = test_case;
    switch (test_case) {
    case CONFIG_EXAMPLE_CONNECT_CASE_NO_CERT:
        connect_no_certs(host, port);
        break;
    case CONFIG_EXAMPLE_CONNECT_CASE_SERVER_CERT:
        connect_with_server_cert(host, port);
        break;
    case CONFIG_EXAMPLE_CONNECT_CASE_MUTUAL_AUTH:
        connect_with_server_client_certs(host, port);
        break;
    case CONFIG_EXAMPLE_CONNECT_CASE_INVALID_SERVER_CERT:
        connect_with_wrong_server_cert(host, port);
        break;
    case CONFIG_EXAMPLE_CONNECT_CASE_SERVER_DER_CERT:
        connect_with_server_der_cert(host, port);
        break;
    case CONFIG_EXAMPLE_CONNECT_CASE_MUTUAL_AUTH_KEY_PWD:
        connect_with_client_key_password(host, port);
        break;
    case CONFIG_EXAMPLE_CONNECT_CASE_MUTUAL_AUTH_BAD_CRT:
        connect_with_invalid_client_certs(host, port);
        break;
    case CONFIG_EXAMPLE_CONNECT_CASE_NO_CERT_ALPN:
        connect_with_alpn(host, port);
        break;
    default:
        ESP_LOGE(TAG, "Unknown test case %d ", test_case);
        break;
    }
    ESP_LOGI(TAG, "Test case:%d started", test_case);
}
