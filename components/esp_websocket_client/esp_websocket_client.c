// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>

#include "esp_websocket_client.h"
#include "esp_transport.h"
#include "esp_transport_tcp.h"
#include "esp_transport_ssl.h"
#include "esp_transport_ws.h"
/* using uri parser */
#include "http_parser.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "WEBSOCKET_CLIENT";

#define WEBSOCKET_TCP_DEFAULT_PORT      (80)
#define WEBSOCKET_SSL_DEFAULT_PORT      (443)
#define WEBSOCKET_BUFFER_SIZE_BYTE      (1024)
#define WEBSOCKET_RECONNECT_TIMEOUT_MS  (10*1000)
#define WEBSOCKET_TASK_PRIORITY         (5)
#define WEBSOCKET_TASK_STACK            (4*1024)
#define WEBSOCKET_NETWORK_TIMEOUT_MS    (10*1000)
#define WEBSOCKET_PING_INTERVAL_SEC     (10)
#define WEBSOCKET_EVENT_QUEUE_SIZE      (1)
#define WEBSOCKET_PINGPONG_TIMEOUT_SEC  (120)
#define WEBSOCKET_KEEP_ALIVE_IDLE       (5)
#define WEBSOCKET_KEEP_ALIVE_INTERVAL   (5)
#define WEBSOCKET_KEEP_ALIVE_COUNT      (3)

#define ESP_WS_CLIENT_MEM_CHECK(TAG, a, action) if (!(a)) {                                         \
        ESP_LOGE(TAG,"%s(%d): %s", __FUNCTION__, __LINE__, "Memory exhausted");                     \
        action;                                                                                     \
        }

#define ESP_WS_CLIENT_ERR_OK_CHECK(TAG, err, action)  { \
        esp_err_t _esp_ws_err_to_check = err;           \
        if (_esp_ws_err_to_check != ESP_OK) {           \
            ESP_LOGE(TAG,"%s(%d): Expected ESP_OK; reported: %d", __FUNCTION__, __LINE__, _esp_ws_err_to_check); \
            action;                                     \
            }                                           \
        }

#define ESP_WS_CLIENT_STATE_CHECK(TAG, a, action) if ((a->state) < WEBSOCKET_STATE_INIT) {                                         \
        ESP_LOGE(TAG,"%s(%d): %s", __FUNCTION__, __LINE__, "Websocket already stop"); \
        action;                                                                                     \
        }

const static int STOPPED_BIT = BIT0;
const static int CLOSE_FRAME_SENT_BIT = BIT1;   // Indicates that a close frame was sent by the client
                                        // and we are waiting for the server to continue with clean close

ESP_EVENT_DEFINE_BASE(WEBSOCKET_EVENTS);

typedef struct {
    int                         task_stack;
    int                         task_prio;
    char                        *uri;
    char                        *host;
    char                        *path;
    char                        *scheme;
    char                        *username;
    char                        *password;
    int                         port;
    bool                        auto_reconnect;
    void                        *user_context;
    int                         network_timeout_ms;
    char                        *subprotocol;
    char                        *user_agent;
    char                        *headers;
    int                         pingpong_timeout_sec;
    size_t                      ping_interval_sec;
} websocket_config_storage_t;

typedef enum {
    WEBSOCKET_STATE_ERROR = -1,
    WEBSOCKET_STATE_UNKNOW = 0,
    WEBSOCKET_STATE_INIT,
    WEBSOCKET_STATE_CONNECTED,
    WEBSOCKET_STATE_WAIT_TIMEOUT,
    WEBSOCKET_STATE_CLOSING,
} websocket_client_state_t;

struct esp_websocket_client {
    esp_event_loop_handle_t     event_handle;
    TaskHandle_t                task_handle;
    esp_transport_list_handle_t transport_list;
    esp_transport_handle_t      transport;
    websocket_config_storage_t *config;
    websocket_client_state_t    state;
    uint64_t                    keepalive_tick_ms;
    uint64_t                    reconnect_tick_ms;
    uint64_t                    ping_tick_ms;
    uint64_t                    pingpong_tick_ms;
    int                         wait_timeout_ms;
    int                         auto_reconnect;
    bool                        run;
    bool                        wait_for_pong_resp;
    EventGroupHandle_t          status_bits;
    xSemaphoreHandle            lock;
    char                        *rx_buffer;
    char                        *tx_buffer;
    int                         buffer_size;
    ws_transport_opcodes_t      last_opcode;
    int                         payload_len;
    int                         payload_offset;
    esp_transport_keep_alive_t  keep_alive_cfg;
    struct ifreq                *if_name;
};

static uint64_t _tick_get_ms(void)
{
    return esp_timer_get_time()/1000;
}

static esp_err_t esp_websocket_client_dispatch_event(esp_websocket_client_handle_t client,
        esp_websocket_event_id_t event,
        const char *data,
        int data_len)
{
    esp_err_t err;
    esp_websocket_event_data_t event_data;

    event_data.client = client;
    event_data.user_context = client->config->user_context;
    event_data.data_ptr = data;
    event_data.data_len = data_len;
    event_data.op_code = client->last_opcode;
    event_data.payload_len = client->payload_len;
    event_data.payload_offset = client->payload_offset;

    if ((err = esp_event_post_to(client->event_handle,
                                 WEBSOCKET_EVENTS, event,
                                 &event_data,
                                 sizeof(esp_websocket_event_data_t),
                                 portMAX_DELAY)) != ESP_OK) {
        return err;
    }
    return esp_event_loop_run(client->event_handle, 0);
}

static esp_err_t esp_websocket_client_abort_connection(esp_websocket_client_handle_t client)
{
    ESP_WS_CLIENT_STATE_CHECK(TAG, client, return ESP_FAIL);
    esp_transport_close(client->transport);

    if (client->config->auto_reconnect) {
        client->wait_timeout_ms = WEBSOCKET_RECONNECT_TIMEOUT_MS;
        client->reconnect_tick_ms = _tick_get_ms();
        ESP_LOGI(TAG, "Reconnect after %d ms", client->wait_timeout_ms);
    }
    client->state = WEBSOCKET_STATE_WAIT_TIMEOUT;
    esp_websocket_client_dispatch_event(client, WEBSOCKET_EVENT_DISCONNECTED, NULL, 0);
    return ESP_OK;
}

static esp_err_t esp_websocket_client_set_config(esp_websocket_client_handle_t client, const esp_websocket_client_config_t *config)
{
    websocket_config_storage_t *cfg = client->config;
    cfg->task_prio = config->task_prio;
    if (cfg->task_prio <= 0) {
        cfg->task_prio = WEBSOCKET_TASK_PRIORITY;
    }

    cfg->task_stack = config->task_stack;
    if (cfg->task_stack == 0) {
        cfg->task_stack = WEBSOCKET_TASK_STACK;
    }

    if (config->host) {
        cfg->host = strdup(config->host);
        ESP_WS_CLIENT_MEM_CHECK(TAG, cfg->host, return ESP_ERR_NO_MEM);
    }

    if (config->port) {
        cfg->port = config->port;
    }

    if (config->username) {
        free(cfg->username);
        cfg->username = strdup(config->username);
        ESP_WS_CLIENT_MEM_CHECK(TAG, cfg->username, return ESP_ERR_NO_MEM);
    }

    if (config->password) {
        free(cfg->password);
        cfg->password = strdup(config->password);
        ESP_WS_CLIENT_MEM_CHECK(TAG, cfg->password, return ESP_ERR_NO_MEM);
    }

    if (config->uri) {
        free(cfg->uri);
        cfg->uri = strdup(config->uri);
        ESP_WS_CLIENT_MEM_CHECK(TAG, cfg->uri, return ESP_ERR_NO_MEM);
    }
    if (config->path) {
        free(cfg->path);
        cfg->path = strdup(config->path);
        ESP_WS_CLIENT_MEM_CHECK(TAG, cfg->path, return ESP_ERR_NO_MEM);
    }
    if (config->subprotocol) {
        free(cfg->subprotocol);
        cfg->subprotocol = strdup(config->subprotocol);
        ESP_WS_CLIENT_MEM_CHECK(TAG, cfg->subprotocol, return ESP_ERR_NO_MEM);
    }
    if (config->user_agent) {
        free(cfg->user_agent);
        cfg->user_agent = strdup(config->user_agent);
        ESP_WS_CLIENT_MEM_CHECK(TAG, cfg->user_agent, return ESP_ERR_NO_MEM);
    }
    if (config->headers) {
        free(cfg->headers);
        cfg->headers = strdup(config->headers);
        ESP_WS_CLIENT_MEM_CHECK(TAG, cfg->headers, return ESP_ERR_NO_MEM);
    }

    cfg->network_timeout_ms = WEBSOCKET_NETWORK_TIMEOUT_MS;
    cfg->user_context = config->user_context;
    cfg->auto_reconnect = true;
    if (config->disable_auto_reconnect) {
        cfg->auto_reconnect = false;
    }

    if (config->disable_pingpong_discon){
        cfg->pingpong_timeout_sec = 0;
    } else if (config->pingpong_timeout_sec) {
        cfg->pingpong_timeout_sec = config->pingpong_timeout_sec;
    } else {
        cfg->pingpong_timeout_sec = WEBSOCKET_PINGPONG_TIMEOUT_SEC;
    }

    if (config->ping_interval_sec == 0) {
        cfg->ping_interval_sec = WEBSOCKET_PING_INTERVAL_SEC;
    } else {
        cfg->ping_interval_sec = config->ping_interval_sec;
    }

    return ESP_OK;
}

static esp_err_t esp_websocket_client_destroy_config(esp_websocket_client_handle_t client)
{
    if (client == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    websocket_config_storage_t *cfg = client->config;
    if (client->config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    free(cfg->host);
    free(cfg->uri);
    free(cfg->path);
    free(cfg->scheme);
    free(cfg->username);
    free(cfg->password);
    free(cfg->subprotocol);
    free(cfg->user_agent);
    free(cfg->headers);
    memset(cfg, 0, sizeof(websocket_config_storage_t));
    free(client->config);
    client->config = NULL;
    return ESP_OK;
}

static esp_err_t set_websocket_transport_optional_settings(esp_websocket_client_handle_t client, const char *scheme)
{
    esp_transport_handle_t trans = esp_transport_list_get_transport(client->transport_list, scheme);
    if (trans) {
        const esp_transport_ws_config_t config = {
                .ws_path = client->config->path,
                .sub_protocol = client->config->subprotocol,
                .user_agent = client->config->user_agent,
                .headers = client->config->headers,
                .propagate_control_frames = true
        };
        return esp_transport_ws_set_config(trans, &config);
    }
    return ESP_ERR_INVALID_ARG;
}

esp_websocket_client_handle_t esp_websocket_client_init(const esp_websocket_client_config_t *config)
{
    esp_websocket_client_handle_t client = calloc(1, sizeof(struct esp_websocket_client));
    ESP_WS_CLIENT_MEM_CHECK(TAG, client, return NULL);

    esp_event_loop_args_t event_args = {
        .queue_size = WEBSOCKET_EVENT_QUEUE_SIZE,
        .task_name = NULL // no task will be created
    };

    if (esp_event_loop_create(&event_args, &client->event_handle) != ESP_OK) {
        ESP_LOGE(TAG, "Error create event handler for websocket client");
        free(client);
        return NULL;
    }

    if (config->keep_alive_enable == true) {
        client->keep_alive_cfg.keep_alive_enable = true;
        client->keep_alive_cfg.keep_alive_idle = (config->keep_alive_idle == 0) ? WEBSOCKET_KEEP_ALIVE_IDLE : config->keep_alive_idle;
        client->keep_alive_cfg.keep_alive_interval = (config->keep_alive_interval == 0) ? WEBSOCKET_KEEP_ALIVE_INTERVAL : config->keep_alive_interval;
        client->keep_alive_cfg.keep_alive_count =  (config->keep_alive_count == 0) ? WEBSOCKET_KEEP_ALIVE_COUNT : config->keep_alive_count;
    }

    if (config->if_name) {
        client->if_name = calloc(1, sizeof(struct ifreq) + 1);
        ESP_WS_CLIENT_MEM_CHECK(TAG, client->if_name, goto _websocket_init_fail);
        memcpy(client->if_name, config->if_name, sizeof(struct ifreq));
    }

    client->lock = xSemaphoreCreateRecursiveMutex();
    ESP_WS_CLIENT_MEM_CHECK(TAG, client->lock, goto _websocket_init_fail);

    client->config = calloc(1, sizeof(websocket_config_storage_t));
    ESP_WS_CLIENT_MEM_CHECK(TAG, client->config, goto _websocket_init_fail);

    client->transport_list = esp_transport_list_init();
    ESP_WS_CLIENT_MEM_CHECK(TAG, client->transport_list, goto _websocket_init_fail);

    esp_transport_handle_t tcp = esp_transport_tcp_init();
    ESP_WS_CLIENT_MEM_CHECK(TAG, tcp, goto _websocket_init_fail);

    esp_transport_set_default_port(tcp, WEBSOCKET_TCP_DEFAULT_PORT);
    esp_transport_list_add(client->transport_list, tcp, "_tcp"); // need to save to transport list, for cleanup
    esp_transport_tcp_set_keep_alive(tcp, &client->keep_alive_cfg);
    esp_transport_tcp_set_interface_name(tcp, client->if_name);

    esp_transport_handle_t ws = esp_transport_ws_init(tcp);
    ESP_WS_CLIENT_MEM_CHECK(TAG, ws, goto _websocket_init_fail);

    esp_transport_set_default_port(ws, WEBSOCKET_TCP_DEFAULT_PORT);
    esp_transport_list_add(client->transport_list, ws, "ws");
    if (config->transport == WEBSOCKET_TRANSPORT_OVER_TCP) {
        asprintf(&client->config->scheme, "ws");
        ESP_WS_CLIENT_MEM_CHECK(TAG, client->config->scheme, goto _websocket_init_fail);
    }

    esp_transport_handle_t ssl = esp_transport_ssl_init();
    ESP_WS_CLIENT_MEM_CHECK(TAG, ssl, goto _websocket_init_fail);

    esp_transport_set_default_port(ssl, WEBSOCKET_SSL_DEFAULT_PORT);
    esp_transport_list_add(client->transport_list, ssl, "_ssl"); // need to save to transport list, for cleanup
    if (config->use_global_ca_store == true) {
        esp_transport_ssl_enable_global_ca_store(ssl);
    } else if (config->cert_pem) {
        if (!config->cert_len) {
            esp_transport_ssl_set_cert_data(ssl, config->cert_pem, strlen(config->cert_pem));
        } else {
            esp_transport_ssl_set_cert_data_der(ssl, config->cert_pem, config->cert_len);
        }
    }
    if (config->client_cert) {
        if (!config->client_cert_len) {
            esp_transport_ssl_set_client_cert_data(ssl, config->client_cert, strlen(config->client_cert));
        } else {
            esp_transport_ssl_set_client_cert_data_der(ssl, config->client_cert, config->client_cert_len);
        }
    }
    if (config->client_key) {
        if (!config->client_key_len) {
            esp_transport_ssl_set_client_key_data(ssl, config->client_key, strlen(config->client_key));
        } else {
            esp_transport_ssl_set_client_key_data_der(ssl, config->client_key, config->client_key_len);
        }
    }
    if (config->skip_cert_common_name_check) {
        esp_transport_ssl_skip_common_name_check(ssl);
    }

    esp_transport_handle_t wss = esp_transport_ws_init(ssl);
    ESP_WS_CLIENT_MEM_CHECK(TAG, wss, goto _websocket_init_fail);

    esp_transport_set_default_port(wss, WEBSOCKET_SSL_DEFAULT_PORT);

    esp_transport_list_add(client->transport_list, wss, "wss");
    if (config->transport == WEBSOCKET_TRANSPORT_OVER_SSL) {
        asprintf(&client->config->scheme, "wss");
        ESP_WS_CLIENT_MEM_CHECK(TAG, client->config->scheme, goto _websocket_init_fail);
    }

    if (config->uri) {
        if (esp_websocket_client_set_uri(client, config->uri) != ESP_OK) {
            ESP_LOGE(TAG, "Invalid uri");
            goto _websocket_init_fail;
        }
    }

    if (esp_websocket_client_set_config(client, config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set the configuration");
        goto _websocket_init_fail;
    }

    if (client->config->scheme == NULL) {
        asprintf(&client->config->scheme, "ws");
        ESP_WS_CLIENT_MEM_CHECK(TAG, client->config->scheme, goto _websocket_init_fail);
    }

    ESP_WS_CLIENT_ERR_OK_CHECK(TAG, set_websocket_transport_optional_settings(client, "ws"), goto _websocket_init_fail;)
    ESP_WS_CLIENT_ERR_OK_CHECK(TAG, set_websocket_transport_optional_settings(client, "wss"), goto _websocket_init_fail;)

    client->keepalive_tick_ms = _tick_get_ms();
    client->reconnect_tick_ms = _tick_get_ms();
    client->ping_tick_ms = _tick_get_ms();
    client->wait_for_pong_resp = false;

    int buffer_size = config->buffer_size;
    if (buffer_size <= 0) {
        buffer_size = WEBSOCKET_BUFFER_SIZE_BYTE;
    }
    client->rx_buffer = malloc(buffer_size);
    ESP_WS_CLIENT_MEM_CHECK(TAG, client->rx_buffer, {
        goto _websocket_init_fail;
    });
    client->tx_buffer = malloc(buffer_size);
    ESP_WS_CLIENT_MEM_CHECK(TAG, client->tx_buffer, {
        goto _websocket_init_fail;
    });
    client->status_bits = xEventGroupCreate();
    ESP_WS_CLIENT_MEM_CHECK(TAG, client->status_bits, {
        goto _websocket_init_fail;
    });

    client->buffer_size = buffer_size;
    return client;

_websocket_init_fail:
    esp_websocket_client_destroy(client);
    return NULL;
}

esp_err_t esp_websocket_client_destroy(esp_websocket_client_handle_t client)
{
    if (client == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (client->run) {
        esp_websocket_client_stop(client);
    }
    if (client->event_handle) {
        esp_event_loop_delete(client->event_handle);
    }
    if (client->if_name) {
        free(client->if_name);
    }
    esp_websocket_client_destroy_config(client);
    esp_transport_list_destroy(client->transport_list);
    vQueueDelete(client->lock);
    free(client->tx_buffer);
    free(client->rx_buffer);
    if (client->status_bits) {
        vEventGroupDelete(client->status_bits);
    }
    free(client);
    client = NULL;
    return ESP_OK;
}

esp_err_t esp_websocket_client_set_uri(esp_websocket_client_handle_t client, const char *uri)
{
    if (client == NULL || uri == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    struct http_parser_url puri;
    http_parser_url_init(&puri);
    int parser_status = http_parser_parse_url(uri, strlen(uri), 0, &puri);
    if (parser_status != 0) {
        ESP_LOGE(TAG, "Error parse uri = %s", uri);
        return ESP_FAIL;
    }
    if (puri.field_data[UF_SCHEMA].len) {
        free(client->config->scheme);
        asprintf(&client->config->scheme, "%.*s", puri.field_data[UF_SCHEMA].len, uri + puri.field_data[UF_SCHEMA].off);
        ESP_WS_CLIENT_MEM_CHECK(TAG, client->config->scheme, return ESP_ERR_NO_MEM);
    }

    if (puri.field_data[UF_HOST].len) {
        free(client->config->host);
        asprintf(&client->config->host, "%.*s", puri.field_data[UF_HOST].len, uri + puri.field_data[UF_HOST].off);
        ESP_WS_CLIENT_MEM_CHECK(TAG, client->config->host, return ESP_ERR_NO_MEM);
    }


    if (puri.field_data[UF_PATH].len || puri.field_data[UF_QUERY].len) {
        free(client->config->path);
        if (puri.field_data[UF_QUERY].len == 0) {
            asprintf(&client->config->path, "%.*s", puri.field_data[UF_PATH].len, uri + puri.field_data[UF_PATH].off);
        } else if (puri.field_data[UF_PATH].len == 0)  {
            asprintf(&client->config->path, "/?%.*s", puri.field_data[UF_QUERY].len, uri + puri.field_data[UF_QUERY].off);
        } else {
            asprintf(&client->config->path, "%.*s?%.*s", puri.field_data[UF_PATH].len, uri + puri.field_data[UF_PATH].off,
                    puri.field_data[UF_QUERY].len, uri + puri.field_data[UF_QUERY].off);
        }
        ESP_WS_CLIENT_MEM_CHECK(TAG, client->config->path, return ESP_ERR_NO_MEM);
    }
    if (puri.field_data[UF_PORT].off) {
        client->config->port = strtol((const char*)(uri + puri.field_data[UF_PORT].off), NULL, 10);
    }

    if (puri.field_data[UF_USERINFO].len) {
        char *user_info = NULL;
        asprintf(&user_info, "%.*s", puri.field_data[UF_USERINFO].len, uri + puri.field_data[UF_USERINFO].off);
        if (user_info) {
            char *pass = strchr(user_info, ':');
            if (pass) {
                pass[0] = 0; //terminal username
                pass ++;
                free(client->config->password);
                client->config->password = strdup(pass);
                ESP_WS_CLIENT_MEM_CHECK(TAG, client->config->password, return ESP_ERR_NO_MEM);
            }
            free(client->config->username);
            client->config->username = strdup(user_info);
            ESP_WS_CLIENT_MEM_CHECK(TAG, client->config->username, return ESP_ERR_NO_MEM);
            free(user_info);
        } else {
            return ESP_ERR_NO_MEM;
        }
    }
    return ESP_OK;
}

static esp_err_t esp_websocket_client_recv(esp_websocket_client_handle_t client)
{
    int rlen;
    client->payload_offset = 0;
    do {
        rlen = esp_transport_read(client->transport, client->rx_buffer, client->buffer_size, client->config->network_timeout_ms);
        if (rlen < 0) {
            ESP_LOGE(TAG, "Error read data");
            return ESP_FAIL;
        }
        client->payload_len = esp_transport_ws_get_read_payload_len(client->transport);
        client->last_opcode = esp_transport_ws_get_read_opcode(client->transport);

        if (rlen == 0 && client->last_opcode == WS_TRANSPORT_OPCODES_NONE ) {
            ESP_LOGV(TAG, "esp_transport_read timeouts");
            return ESP_OK;
        }

        esp_websocket_client_dispatch_event(client, WEBSOCKET_EVENT_DATA, client->rx_buffer, rlen);

        client->payload_offset += rlen;
    } while (client->payload_offset < client->payload_len);

    // if a PING message received -> send out the PONG, this will not work for PING messages with payload longer than buffer len
    if (client->last_opcode == WS_TRANSPORT_OPCODES_PING) {
        const char *data = (client->payload_len == 0) ? NULL : client->rx_buffer;
        ESP_LOGD(TAG, "Sending PONG with payload len=%d", client->payload_len);
        esp_transport_ws_send_raw(client->transport, WS_TRANSPORT_OPCODES_PONG | WS_TRANSPORT_OPCODES_FIN, data, client->payload_len,
                                  client->config->network_timeout_ms);
    } else if (client->last_opcode == WS_TRANSPORT_OPCODES_PONG) {
        client->wait_for_pong_resp = false;
    } else if (client->last_opcode == WS_TRANSPORT_OPCODES_CLOSE) {
        ESP_LOGD(TAG, "Received close frame");
        client->state = WEBSOCKET_STATE_CLOSING;
    }

    return ESP_OK;
}

static int esp_websocket_client_send_with_opcode(esp_websocket_client_handle_t client, ws_transport_opcodes_t opcode, const uint8_t *data, int len, TickType_t timeout);

static int esp_websocket_client_send_close(esp_websocket_client_handle_t client, int code, const char *additional_data, int total_len, TickType_t timeout);

static void esp_websocket_client_task(void *pv)
{
    const int lock_timeout = portMAX_DELAY;
    esp_websocket_client_handle_t client = (esp_websocket_client_handle_t) pv;
    client->run = true;

    //get transport by scheme
    client->transport = esp_transport_list_get_transport(client->transport_list, client->config->scheme);

    if (client->transport == NULL) {
        ESP_LOGE(TAG, "There are no transports valid, stop websocket client");
        client->run = false;
    }
    //default port
    if (client->config->port == 0) {
        client->config->port = esp_transport_get_default_port(client->transport);
    }

    client->state = WEBSOCKET_STATE_INIT;
    xEventGroupClearBits(client->status_bits, STOPPED_BIT | CLOSE_FRAME_SENT_BIT);
    int read_select = 0;
    while (client->run) {
        if (xSemaphoreTakeRecursive(client->lock, lock_timeout) != pdPASS) {
            ESP_LOGE(TAG, "Failed to lock ws-client tasks, exiting the task...");
            break;
        }
        switch ((int)client->state) {
            case WEBSOCKET_STATE_INIT:
                if (client->transport == NULL) {
                    ESP_LOGE(TAG, "There are no transport");
                    client->run = false;
                    break;
                }
                if (esp_transport_connect(client->transport,
                                          client->config->host,
                                          client->config->port,
                                          client->config->network_timeout_ms) < 0) {
                    ESP_LOGE(TAG, "Error transport connect");
                    esp_websocket_client_abort_connection(client);
                    break;
                }
                ESP_LOGD(TAG, "Transport connected to %s://%s:%d", client->config->scheme, client->config->host, client->config->port);

                client->state = WEBSOCKET_STATE_CONNECTED;
                client->wait_for_pong_resp = false;
                esp_websocket_client_dispatch_event(client, WEBSOCKET_EVENT_CONNECTED, NULL, 0);

                break;
            case WEBSOCKET_STATE_CONNECTED:
                if ((CLOSE_FRAME_SENT_BIT & xEventGroupGetBits(client->status_bits)) == 0) { // only send and check for PING
                                                                                                          // if closing hasn't been initiated
                    if (_tick_get_ms() - client->ping_tick_ms > client->config->ping_interval_sec*1000) {
                        client->ping_tick_ms = _tick_get_ms();
                        ESP_LOGD(TAG, "Sending PING...");
                        esp_transport_ws_send_raw(client->transport, WS_TRANSPORT_OPCODES_PING | WS_TRANSPORT_OPCODES_FIN, NULL, 0, client->config->network_timeout_ms);

                        if (!client->wait_for_pong_resp && client->config->pingpong_timeout_sec) {
                            client->pingpong_tick_ms = _tick_get_ms();
                            client->wait_for_pong_resp = true;
                        }
                    }

                    if ( _tick_get_ms() - client->pingpong_tick_ms > client->config->pingpong_timeout_sec*1000 ) {
                        if (client->wait_for_pong_resp) {
                            ESP_LOGE(TAG, "Error, no PONG received for more than %d seconds after PING", client->config->pingpong_timeout_sec);
                            esp_websocket_client_abort_connection(client);
                            break;
                        }
                    }
                }

                if (read_select == 0) {
                    ESP_LOGV(TAG, "Read poll timeout: skipping esp_transport_read()...");
                    break;
                }
                client->ping_tick_ms = _tick_get_ms();

                if (esp_websocket_client_recv(client) == ESP_FAIL) {
                    ESP_LOGE(TAG, "Error receive data");
                    esp_websocket_client_abort_connection(client);
                    break;
                }
                break;
            case WEBSOCKET_STATE_WAIT_TIMEOUT:

                if (!client->config->auto_reconnect) {
                    client->run = false;
                    break;
                }
                if (_tick_get_ms() - client->reconnect_tick_ms > client->wait_timeout_ms) {
                    client->state = WEBSOCKET_STATE_INIT;
                    client->reconnect_tick_ms = _tick_get_ms();
                    ESP_LOGD(TAG, "Reconnecting...");
                }
                break;
            case WEBSOCKET_STATE_CLOSING:
                // if closing not initiated by the client echo the close message back
                if ((CLOSE_FRAME_SENT_BIT & xEventGroupGetBits(client->status_bits)) == 0) {
                    ESP_LOGD(TAG, "Closing initiated by the server, sending close frame");
                    esp_transport_ws_send_raw(client->transport, WS_TRANSPORT_OPCODES_CLOSE | WS_TRANSPORT_OPCODES_FIN, NULL, 0, client->config->network_timeout_ms);
                    xEventGroupSetBits(client->status_bits, CLOSE_FRAME_SENT_BIT);
                }
                break;
            default:
                ESP_LOGD(TAG, "Client run iteration in a default state: %d", client->state);
                break;
        }
        xSemaphoreGiveRecursive(client->lock);
        if (WEBSOCKET_STATE_CONNECTED == client->state) {
            read_select = esp_transport_poll_read(client->transport, 1000); //Poll every 1000ms
            if (read_select < 0) {
                ESP_LOGE(TAG, "Network error: esp_transport_poll_read() returned %d, errno=%d", read_select, errno);
                esp_websocket_client_abort_connection(client);
            }
        } else if (WEBSOCKET_STATE_WAIT_TIMEOUT == client->state) {
            // waiting for reconnecting...
            vTaskDelay(client->wait_timeout_ms / 2 / portTICK_RATE_MS);
        } else if (WEBSOCKET_STATE_CLOSING == client->state &&
                  (CLOSE_FRAME_SENT_BIT & xEventGroupGetBits(client->status_bits))) {
            ESP_LOGD(TAG, " Waiting for TCP connection to be closed by the server");
            int ret = esp_transport_ws_poll_connection_closed(client->transport, 1000);
            if (ret == 0) {
                // still waiting
                break;
            }
            if (ret < 0) {
                ESP_LOGW(TAG, "Connection terminated while waiting for clean TCP close");
            }
            client->run = false;
            client->state = WEBSOCKET_STATE_UNKNOW;
            esp_websocket_client_dispatch_event(client, WEBSOCKET_EVENT_CLOSED, NULL, 0);
            break;
        }
    }

    esp_transport_close(client->transport);
    xEventGroupSetBits(client->status_bits, STOPPED_BIT);
    client->state = WEBSOCKET_STATE_UNKNOW;
    vTaskDelete(NULL);
}

esp_err_t esp_websocket_client_start(esp_websocket_client_handle_t client)
{
    if (client == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (client->state >= WEBSOCKET_STATE_INIT) {
        ESP_LOGE(TAG, "The client has started");
        return ESP_FAIL;
    }
    if (xTaskCreate(esp_websocket_client_task, "websocket_task", client->config->task_stack, client, client->config->task_prio, &client->task_handle) != pdTRUE) {
        ESP_LOGE(TAG, "Error create websocket task");
        return ESP_FAIL;
    }
    xEventGroupClearBits(client->status_bits, STOPPED_BIT | CLOSE_FRAME_SENT_BIT);
    return ESP_OK;
}

esp_err_t esp_websocket_client_stop(esp_websocket_client_handle_t client)
{
    if (client == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!client->run) {
        ESP_LOGW(TAG, "Client was not started");
        return ESP_FAIL;
    }

    /* A running client cannot be stopped from the websocket task/event handler */
    TaskHandle_t running_task = xTaskGetCurrentTaskHandle();
    if (running_task == client->task_handle) {
        ESP_LOGE(TAG, "Client cannot be stopped from websocket task");
        return ESP_FAIL;
    }


    client->run = false;
    xEventGroupWaitBits(client->status_bits, STOPPED_BIT, false, true, portMAX_DELAY);
    client->state = WEBSOCKET_STATE_UNKNOW;
    return ESP_OK;
}

static int esp_websocket_client_send_close(esp_websocket_client_handle_t client, int code, const char *additional_data, int total_len, TickType_t timeout)
{
    uint8_t *close_status_data = NULL;
    // RFC6455#section-5.5.1: The Close frame MAY contain a body (indicated by total_len >= 2)
    if (total_len >= 2) {
        close_status_data = calloc(1, total_len);
        ESP_WS_CLIENT_MEM_CHECK(TAG, close_status_data, return -1);
        // RFC6455#section-5.5.1: The first two bytes of the body MUST be a 2-byte representing a status
        uint16_t *code_network_order = (uint16_t *) close_status_data;
        *code_network_order = htons(code);
        memcpy(close_status_data + 2, additional_data, total_len - 2);
    }
    int ret = esp_websocket_client_send_with_opcode(client, WS_TRANSPORT_OPCODES_CLOSE, close_status_data, total_len, timeout);
    free(close_status_data);
    return ret;
}


static esp_err_t esp_websocket_client_close_with_optional_body(esp_websocket_client_handle_t client, bool send_body, int code, const char *data, int len, TickType_t timeout)
{
    if (client == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!client->run) {
        ESP_LOGW(TAG, "Client was not started");
        return ESP_FAIL;
    }

    /* A running client cannot be stopped from the websocket task/event handler */
    TaskHandle_t running_task = xTaskGetCurrentTaskHandle();
    if (running_task == client->task_handle) {
        ESP_LOGE(TAG, "Client cannot be stopped from websocket task");
        return ESP_FAIL;
    }

    if (send_body) {
        esp_websocket_client_send_close(client, code, data, len + 2, portMAX_DELAY); // len + 2 -> always sending the code
    } else {
        esp_websocket_client_send_close(client, 0, NULL, 0, portMAX_DELAY); // only opcode frame
    }

    // Set closing bit to prevent from sending PING frames while connected
    xEventGroupSetBits(client->status_bits, CLOSE_FRAME_SENT_BIT);

    if (STOPPED_BIT & xEventGroupWaitBits(client->status_bits, STOPPED_BIT, false, true, timeout)) {
        return ESP_OK;
    }

    // If could not close gracefully within timeout, stop the client and disconnect
    client->run = false;
    xEventGroupWaitBits(client->status_bits, STOPPED_BIT, false, true, portMAX_DELAY);
    client->state = WEBSOCKET_STATE_UNKNOW;
    return ESP_OK;
}

esp_err_t esp_websocket_client_close_with_code(esp_websocket_client_handle_t client, int code, const char *data, int len, TickType_t timeout)
{
    return esp_websocket_client_close_with_optional_body(client, true, code, data, len, timeout);
}

esp_err_t esp_websocket_client_close(esp_websocket_client_handle_t client, TickType_t timeout)
{
    return esp_websocket_client_close_with_optional_body(client, false, 0, NULL, 0, timeout);
}

int esp_websocket_client_send_text(esp_websocket_client_handle_t client, const char *data, int len, TickType_t timeout)
{
    return esp_websocket_client_send_with_opcode(client, WS_TRANSPORT_OPCODES_TEXT, (const uint8_t *)data, len, timeout);
}

int esp_websocket_client_send(esp_websocket_client_handle_t client, const char *data, int len, TickType_t timeout)
{
    return esp_websocket_client_send_with_opcode(client, WS_TRANSPORT_OPCODES_BINARY, (const uint8_t *)data, len, timeout);
}

int esp_websocket_client_send_bin(esp_websocket_client_handle_t client, const char *data, int len, TickType_t timeout)
{
    return esp_websocket_client_send_with_opcode(client, WS_TRANSPORT_OPCODES_BINARY, (const uint8_t *)data, len, timeout);
}

static int esp_websocket_client_send_with_opcode(esp_websocket_client_handle_t client, ws_transport_opcodes_t opcode, const uint8_t *data, int len, TickType_t timeout)
{
    int need_write = len;
    int wlen = 0, widx = 0;
    int ret = ESP_FAIL;

    if (client == NULL || len < 0 ||
        (opcode != WS_TRANSPORT_OPCODES_CLOSE && (data == NULL || len <= 0))) {
        ESP_LOGE(TAG, "Invalid arguments");
        return ESP_FAIL;
    }

    if (xSemaphoreTakeRecursive(client->lock, timeout) != pdPASS) {
        ESP_LOGE(TAG, "Could not lock ws-client within %d timeout", timeout);
        return ESP_FAIL;
    }

    if (!esp_websocket_client_is_connected(client)) {
        ESP_LOGE(TAG, "Websocket client is not connected");
        goto unlock_and_return;
    }

    if (client->transport == NULL) {
        ESP_LOGE(TAG, "Invalid transport");
        goto unlock_and_return;
    }
    uint32_t current_opcode = opcode;
    while (widx < len || current_opcode) {  // allow for sending "current_opcode" only message with len==0
        if (need_write > client->buffer_size) {
            need_write = client->buffer_size;
        } else {
            current_opcode |= WS_TRANSPORT_OPCODES_FIN;
        }
        memcpy(client->tx_buffer, data + widx, need_write);
        // send with ws specific way and specific opcode
        wlen = esp_transport_ws_send_raw(client->transport, current_opcode, (char *)client->tx_buffer, need_write,
                                        (timeout==portMAX_DELAY)? -1 : timeout * portTICK_PERIOD_MS);
        if (wlen < 0 || (wlen == 0 && need_write != 0)) {
            ret = wlen;
            ESP_LOGE(TAG, "Network error: esp_transport_write() returned %d, errno=%d", ret, errno);
            esp_websocket_client_abort_connection(client);
            goto unlock_and_return;
        }
        current_opcode = 0;
        widx += wlen;
        need_write = len - widx;

    }
    ret = widx;
unlock_and_return:
    xSemaphoreGiveRecursive(client->lock);
    return ret;
}

bool esp_websocket_client_is_connected(esp_websocket_client_handle_t client)
{
    if (client == NULL) {
        return false;
    }
    return client->state == WEBSOCKET_STATE_CONNECTED;
}

esp_err_t esp_websocket_register_events(esp_websocket_client_handle_t client,
                                        esp_websocket_event_id_t event,
                                        esp_event_handler_t event_handler,
                                        void *event_handler_arg)
{
    if (client == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    return esp_event_handler_register_with(client->event_handle, WEBSOCKET_EVENTS, event, event_handler, event_handler_arg);
}
