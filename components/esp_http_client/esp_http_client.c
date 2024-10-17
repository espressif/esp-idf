/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <string.h>
#include <inttypes.h>

#include "esp_log.h"
#include "esp_assert.h"
#include "esp_check.h"
#include "http_parser.h"
#include "http_header.h"
#include "esp_transport.h"
#include "esp_transport_tcp.h"
#include "http_utils.h"
#include "http_auth.h"
#include "sdkconfig.h"
#include "esp_http_client.h"
#include "errno.h"
#include "esp_random.h"
#include "esp_tls.h"

#ifdef CONFIG_ESP_HTTP_CLIENT_ENABLE_HTTPS
#include "esp_transport_ssl.h"
#endif

ESP_EVENT_DEFINE_BASE(ESP_HTTP_CLIENT_EVENT);

static const char *TAG = "HTTP_CLIENT";

ESP_STATIC_ASSERT((int)ESP_HTTP_CLIENT_TLS_VER_ANY == (int)ESP_TLS_VER_ANY, "Enum mismatch in esp_http_client and esp-tls");
ESP_STATIC_ASSERT((int)ESP_HTTP_CLIENT_TLS_VER_MAX <= (int)ESP_TLS_VER_TLS_MAX, "HTTP client supported TLS is not supported in esp-tls");

#if CONFIG_ESP_HTTP_CLIENT_EVENT_POST_TIMEOUT == -1
#define ESP_HTTP_CLIENT_EVENT_POST_TIMEOUT portMAX_DELAY
#else
#define ESP_HTTP_CLIENT_EVENT_POST_TIMEOUT pdMS_TO_TICKS(CONFIG_ESP_HTTP_CLIENT_EVENT_POST_TIMEOUT)
#endif
/**
 * HTTP Buffer
 */
typedef struct {
    char *data;         /*!< The HTTP data received from the server */
    int len;            /*!< The HTTP data len received from the server */
    char *raw_data;     /*!< The HTTP data after decoding */
    char *orig_raw_data;/*!< The Original pointer to HTTP data after decoding */
    int raw_len;        /*!< The HTTP data len after decoding */
    char *output_ptr;   /*!< The destination address of the data to be copied to after decoding */
} esp_http_buffer_t;

/**
 * private HTTP Data structure
 */
typedef struct {
    http_header_handle_t headers;       /*!< http header */
    esp_http_buffer_t   *buffer;        /*!< data buffer as linked list */
    int                 status_code;    /*!< status code (integer) */
    int64_t             content_length; /*!< data length */
    int                 chunk_length;   /*!< chunk length */
    int                 data_offset;    /*!< offset to http data (Skip header) */
    int64_t             data_process;   /*!< data processed */
    int                 method;         /*!< http method */
    bool                is_chunked;
} esp_http_data_t;

typedef struct {
    char                         *url;
    char                         *scheme;
    char                         *host;
    int                          port;
    char                         *username;
    char                         *password;
    char                         *path;
    char                         *query;
    char                         *cert_pem;
    esp_http_client_method_t     method;
    esp_http_client_auth_type_t  auth_type;
    esp_http_client_transport_t  transport_type;
    int                          max_store_header_size;
} connection_info_t;

typedef enum {
    HTTP_STATE_UNINIT = 0,
    HTTP_STATE_INIT,
    HTTP_STATE_CONNECTED,
    HTTP_STATE_REQ_COMPLETE_HEADER,
    HTTP_STATE_REQ_COMPLETE_DATA,
    HTTP_STATE_RES_COMPLETE_HEADER,
    HTTP_STATE_RES_ON_DATA_START,
    HTTP_STATE_RES_COMPLETE_DATA,
    HTTP_STATE_CLOSE
} esp_http_state_t;

typedef enum {
    SESSION_TICKET_UNUSED = 0,
    SESSION_TICKET_NOT_SAVED,
    SESSION_TICKET_SAVED,
} session_ticket_state_t;

/**
 * HTTP client class
 */
struct esp_http_client {
    int                         redirect_counter;
    int                         max_redirection_count;
    int                         max_authorization_retries;
    int                         process_again;
    struct http_parser          *parser;
    struct http_parser_settings *parser_settings;
    esp_transport_list_handle_t     transport_list;
    esp_transport_handle_t          transport;
    esp_http_data_t                 *request;
    esp_http_data_t                 *response;
    void                        *user_data;
    esp_http_auth_data_t        *auth_data;
    char                        *post_data;
    char                        *location;
    char                        *auth_header;
    char                        *current_header_key;
    char                        *current_header_value;
    int                         post_len;
    connection_info_t           connection_info;
    bool                        is_chunk_complete;
    esp_http_state_t            state;
    http_event_handle_cb        event_handler;
    int                         timeout_ms;
    int                         buffer_size_rx;
    int                         buffer_size_tx;
    bool                        disable_auto_redirect;
    esp_http_client_event_t     event;
    int                         data_written_index;
    int                         data_write_left;
    bool                        first_line_prepared;
    int                         header_index;
    bool                        is_async;
    esp_transport_keep_alive_t  keep_alive_cfg;
    struct ifreq                *if_name;
    unsigned                    cache_data_in_fetch_hdr: 1;
#ifdef CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
    session_ticket_state_t      session_ticket_state;
#endif
};

typedef struct esp_http_client esp_http_client_t;

static esp_err_t _clear_connection_info(esp_http_client_handle_t client);
/**
 * Default settings
 */
#define DEFAULT_HTTP_PORT (80)
#define DEFAULT_HTTPS_PORT (443)

#define ASYNC_TRANS_CONNECT_FAIL -1
#define ASYNC_TRANS_CONNECTING 0
#define ASYNC_TRANS_CONNECT_PASS 1

static const char *DEFAULT_HTTP_USER_AGENT = "ESP32 HTTP Client/1.0";
static const char *DEFAULT_HTTP_PROTOCOL = "HTTP/1.1";
static const char *DEFAULT_HTTP_PATH = "/";
static const int DEFAULT_MAX_REDIRECT = 10;
static const int DEFAULT_MAX_AUTH_RETRIES = 10;
static const int DEFAULT_TIMEOUT_MS = 5000;
static const int DEFAULT_KEEP_ALIVE_IDLE = 5;
static const int DEFAULT_KEEP_ALIVE_INTERVAL= 5;
static const int DEFAULT_KEEP_ALIVE_COUNT= 3;

static const char *HTTP_METHOD_MAPPING[] = {
    "GET",
    "POST",
    "PUT",
    "PATCH",
    "DELETE",
    "HEAD",
    "NOTIFY",
    "SUBSCRIBE",
    "UNSUBSCRIBE",
    "OPTIONS",
    "COPY",
    "MOVE",
    "LOCK",
    "UNLOCK",
    "PROPFIND",
    "PROPPATCH",
    "MKCOL",
    "REPORT"
};

static esp_err_t esp_http_client_request_send(esp_http_client_handle_t client, int write_len);
static esp_err_t esp_http_client_connect(esp_http_client_handle_t client);
static esp_err_t esp_http_client_send_post_data(esp_http_client_handle_t client);

static esp_err_t http_dispatch_event(esp_http_client_t *client, esp_http_client_event_id_t event_id, void *data, int len)
{
    esp_http_client_event_t *event = &client->event;

    if (client->event_handler) {
        event->event_id = event_id;
        event->user_data = client->user_data;
        event->data = data;
        event->data_len = len;
        return client->event_handler(event);
    }
    return ESP_OK;
}

static void http_dispatch_event_to_event_loop(int32_t event_id, const void* event_data, size_t event_data_size)
{
    esp_err_t err = esp_event_post(ESP_HTTP_CLIENT_EVENT, event_id, event_data, event_data_size, ESP_HTTP_CLIENT_EVENT_POST_TIMEOUT);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to post http_client event: %"PRId32", error: %s", event_id, esp_err_to_name(err));
    }
}

static int http_on_message_begin(http_parser *parser)
{
    esp_http_client_t *client = parser->data;
    ESP_LOGD(TAG, "on_message_begin");

    client->response->is_chunked = false;
    client->is_chunk_complete = false;
    return 0;
}

static int http_on_url(http_parser *parser, const char *at, size_t length)
{
    ESP_LOGD(TAG, "http_on_url");
    return 0;
}

static int http_on_status(http_parser *parser, const char *at, size_t length)
{
    return 0;
}

static int http_on_header_event(esp_http_client_handle_t client)
{
    if (client->current_header_key != NULL && client->current_header_value != NULL) {
        ESP_LOGD(TAG, "HEADER=%s:%s", client->current_header_key, client->current_header_value);
        client->event.header_key = client->current_header_key;
        client->event.header_value = client->current_header_value;
        http_dispatch_event(client, HTTP_EVENT_ON_HEADER, NULL, 0);
        http_dispatch_event_to_event_loop(HTTP_EVENT_ON_HEADER, &client, sizeof(esp_http_client_handle_t));
        free(client->current_header_key);
        free(client->current_header_value);
        client->current_header_key = NULL;
        client->current_header_value = NULL;
    }
    return 0;
}

static int http_on_header_field(http_parser *parser, const char *at, size_t length)
{
    esp_http_client_t *client = parser->data;
    http_on_header_event(client);
    http_utils_append_string(&client->current_header_key, at, length);

    return 0;
}

static int http_on_header_value(http_parser *parser, const char *at, size_t length)
{
    esp_http_client_handle_t client = parser->data;
    if (client->current_header_key == NULL) {
        return 0;
    }
    if (strcasecmp(client->current_header_key, "Location") == 0) {
        http_utils_append_string(&client->location, at, length);
    } else if (strcasecmp(client->current_header_key, "Transfer-Encoding") == 0
               && memcmp(at, "chunked", length) == 0) {
        client->response->is_chunked = true;
    } else if (strcasecmp(client->current_header_key, "WWW-Authenticate") == 0) {
        http_utils_append_string(&client->auth_header, at, length);
    }
    http_utils_append_string(&client->current_header_value, at, length);
    return 0;
}

static int http_on_headers_complete(http_parser *parser)
{
    esp_http_client_handle_t client = parser->data;
    http_on_header_event(client);
    client->response->status_code = parser->status_code;
    client->response->data_offset = parser->nread;
    client->response->content_length = parser->content_length;
    client->response->data_process = 0;
    ESP_LOGD(TAG, "http_on_headers_complete, status=%d, offset=%d, nread=%" PRId32, parser->status_code, client->response->data_offset, parser->nread);
    client->state = HTTP_STATE_RES_COMPLETE_HEADER;
    if (client->connection_info.method == HTTP_METHOD_HEAD) {
        /* In a HTTP_RESPONSE parser returning '1' from on_headers_complete will tell the
           parser that it should not expect a body. This is used when receiving a response
           to a HEAD request which may contain 'Content-Length' or 'Transfer-Encoding: chunked'
           headers that indicate the presence of a body.*/
        return 1;
    }
    return 0;
}

static int http_on_body(http_parser *parser, const char *at, size_t length)
{
    esp_http_client_t *client = parser->data;
    ESP_LOGD(TAG, "http_on_body %zu", length);

    if (client->response->buffer->output_ptr) {
        memcpy(client->response->buffer->output_ptr, (char *)at, length);
        client->response->buffer->output_ptr += length;
    } else {
        /* Do not cache body when http_on_body is called from esp_http_client_perform */
        if (client->state < HTTP_STATE_RES_ON_DATA_START && client->cache_data_in_fetch_hdr) {
            ESP_LOGD(TAG, "Body received in fetch header state, %p, %zu", at, length);
            esp_http_buffer_t *res_buffer = client->response->buffer;
            assert(res_buffer->orig_raw_data == res_buffer->raw_data);
            res_buffer->orig_raw_data = (char *)realloc(res_buffer->orig_raw_data, res_buffer->raw_len + length);
            if (!res_buffer->orig_raw_data) {
                ESP_LOGE(TAG, "Failed to allocate memory for storing decoded data");
                return -1;
            }
            memcpy(res_buffer->orig_raw_data + res_buffer->raw_len, at, length);
            res_buffer->raw_data = res_buffer->orig_raw_data;
        }
    }

    client->response->data_process += length;
    client->response->buffer->raw_len += length;
    http_dispatch_event(client, HTTP_EVENT_ON_DATA, (void *)at, length);
    esp_http_client_on_data_t evt_data = {};
    evt_data.data_process = client->response->data_process;
    evt_data.client = client;
    http_dispatch_event_to_event_loop(HTTP_EVENT_ON_DATA, &evt_data, sizeof(esp_http_client_on_data_t));
    return 0;
}

static int http_on_message_complete(http_parser *parser)
{
    ESP_LOGD(TAG, "http_on_message_complete, parser=%p", parser);
    esp_http_client_handle_t client = parser->data;
    client->is_chunk_complete = true;
    return 0;
}

static int http_on_chunk_complete(http_parser *parser)
{
    ESP_LOGD(TAG, "http_on_chunk_complete");
    return 0;
}

static int http_on_chunk_header(http_parser *parser)
{
    esp_http_client_handle_t client = parser->data;
    client->response->chunk_length = parser->content_length;
    ESP_LOGD(TAG, "http_on_chunk_header, chunk_length");
    return 0;
}

esp_err_t esp_http_client_set_header(esp_http_client_handle_t client, const char *key, const char *value)
{
    if (client == NULL || client->request == NULL || client->request->headers == NULL || key == NULL || value == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return http_header_set(client->request->headers, key, value);
}

esp_err_t esp_http_client_get_header(esp_http_client_handle_t client, const char *key, char **value)
{
    if (client == NULL || client->request == NULL || client->request->headers == NULL || key == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return http_header_get(client->request->headers, key, value);
}

esp_err_t esp_http_client_delete_header(esp_http_client_handle_t client, const char *key)
{
    if (client == NULL || client->request == NULL || client->request->headers == NULL || key == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return http_header_delete(client->request->headers, key);
}

esp_err_t esp_http_client_delete_all_headers(esp_http_client_handle_t client)
{
    if (client == NULL || client->request == NULL || client->request->headers == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return http_header_clean(client->request->headers);
}

esp_err_t esp_http_client_get_username(esp_http_client_handle_t client, char **value)
{
    if (client == NULL || value == NULL) {
        ESP_LOGE(TAG, "client or value must not be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    *value = client->connection_info.username;
    return ESP_OK;
}

esp_err_t esp_http_client_set_username(esp_http_client_handle_t client, const char *username)
{
    if (client == NULL) {
        ESP_LOGE(TAG, "client must not be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    if (client->connection_info.username != NULL) {
        free(client->connection_info.username);
    }
    client->connection_info.username = username ? strdup(username) : NULL;
    return ESP_OK;
}

esp_err_t esp_http_client_get_password(esp_http_client_handle_t client, char **value)
{
    if (client == NULL || value == NULL) {
        ESP_LOGE(TAG, "client or value must not be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    *value = client->connection_info.password;
    return ESP_OK;
}

esp_err_t esp_http_client_cancel_request(esp_http_client_handle_t client)
{
    if (client == NULL) {
        ESP_LOGD(TAG, "Client handle is NULL");
        return ESP_ERR_INVALID_ARG;
    }
    if (client->state < HTTP_STATE_CONNECTED) {
        ESP_LOGD(TAG, "Invalid State: %d", client->state);
        return ESP_ERR_INVALID_STATE;
    }
    if (esp_transport_close(client->transport) != 0) {
        return ESP_FAIL;
    }

    esp_err_t err = esp_http_client_connect(client);
    // esp_http_client_connect() will return ESP_ERR_HTTP_CONNECTING in case of non-blocking mode and if the connection has not been established.
    if (err == ESP_OK || (client->is_async && err == ESP_ERR_HTTP_CONNECTING)) {
        client->response->data_process = client->response->content_length;
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t esp_http_client_set_password(esp_http_client_handle_t client, const char *password)
{
    if (client == NULL) {
        ESP_LOGE(TAG, "client must not be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    if (client->connection_info.password != NULL) {
        memset(client->connection_info.password, 0, strlen(client->connection_info.password));
        free(client->connection_info.password);
    }
    client->connection_info.password = password ? strdup(password) : NULL;
    return ESP_OK;
}

esp_err_t esp_http_client_set_authtype(esp_http_client_handle_t client, esp_http_client_auth_type_t auth_type)
{
    if (client == NULL) {
        ESP_LOGE(TAG, "client must not be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    client->connection_info.auth_type = auth_type;
    return ESP_OK;
}

esp_err_t esp_http_client_get_user_data(esp_http_client_handle_t client, void **data)
{
    if (NULL == client || NULL == data) {
        ESP_LOGE(TAG, "client or data must not be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    *data = client->user_data;
    return ESP_OK;
}

esp_err_t esp_http_client_set_user_data(esp_http_client_handle_t client, void *data)
{
    if (NULL == client) {
        ESP_LOGE(TAG, "client must not be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    client->user_data = data;
    return ESP_OK;
}

static esp_err_t _set_config(esp_http_client_handle_t client, const esp_http_client_config_t *config)
{
    esp_err_t ret = ESP_OK;
    client->connection_info.method = config->method;
    client->connection_info.port = config->port;
    client->connection_info.auth_type = config->auth_type;
    client->event_handler = config->event_handler;
    client->timeout_ms = config->timeout_ms;
    client->max_redirection_count = config->max_redirection_count;
    client->max_authorization_retries = config->max_authorization_retries;
    client->user_data = config->user_data;
    client->buffer_size_rx = config->buffer_size;
    client->buffer_size_tx = config->buffer_size_tx;
    client->disable_auto_redirect = config->disable_auto_redirect;

    if (config->buffer_size == 0) {
        client->buffer_size_rx = DEFAULT_HTTP_BUF_SIZE;
    }

    if (config->buffer_size_tx == 0) {
        client->buffer_size_tx = DEFAULT_HTTP_BUF_SIZE;
    }

    if (client->max_redirection_count == 0) {
        client->max_redirection_count = DEFAULT_MAX_REDIRECT;
    }

    if (client->max_authorization_retries == 0) {
        client->max_authorization_retries = DEFAULT_MAX_AUTH_RETRIES;
    } else if (client->max_authorization_retries == -1) {
        client->max_authorization_retries = 0;
    }

    if (config->path) {
        client->connection_info.path = strdup(config->path);
    } else {
        client->connection_info.path = strdup(DEFAULT_HTTP_PATH);
    }
    ESP_RETURN_ON_FALSE(client->connection_info.path, ESP_ERR_NO_MEM, TAG, "Memory exhausted");

    if (config->host) {
        client->connection_info.host = strdup(config->host);

        ESP_GOTO_ON_FALSE(client->connection_info.host, ESP_ERR_NO_MEM, error, TAG, "Memory exhausted");
    }

    if (config->query) {
        client->connection_info.query = strdup(config->query);
        ESP_GOTO_ON_FALSE(client->connection_info.query, ESP_ERR_NO_MEM, error, TAG, "Memory exhausted");
    }

    if (config->username) {
        client->connection_info.username = strdup(config->username);
        ESP_GOTO_ON_FALSE(client->connection_info.username, ESP_ERR_NO_MEM, error, TAG, "Memory exhausted");
    }

    if (config->password) {
        client->connection_info.password = strdup(config->password);
        ESP_GOTO_ON_FALSE(client->connection_info.password, ESP_ERR_NO_MEM, error, TAG, "Memory exhausted");
    }

    if (config->transport_type == HTTP_TRANSPORT_OVER_SSL) {
        http_utils_assign_string(&client->connection_info.scheme, "https", -1);
        if (client->connection_info.port == 0) {
            client->connection_info.port = DEFAULT_HTTPS_PORT;
        }
    } else {
        http_utils_assign_string(&client->connection_info.scheme, "http", -1);
        if (client->connection_info.port == 0) {
            client->connection_info.port = DEFAULT_HTTP_PORT;
        }
    }
    if (client->timeout_ms == 0) {
        client->timeout_ms = DEFAULT_TIMEOUT_MS;
    }
    if (config->is_async) {
        client->is_async = true;
    }

    return ret;

error:
    _clear_connection_info(client);
    return ret;
}

static esp_err_t _clear_connection_info(esp_http_client_handle_t client)
{
    free(client->connection_info.path);
    free(client->connection_info.host);
    free(client->connection_info.query);
    free(client->connection_info.username);
    if (client->connection_info.password) {
        memset(client->connection_info.password, 0, strlen(client->connection_info.password));
        free(client->connection_info.password);
    }
    free(client->connection_info.scheme);
    free(client->connection_info.url);
    memset(&client->connection_info, 0, sizeof(connection_info_t));
    return ESP_OK;
}

static esp_err_t _clear_auth_data(esp_http_client_handle_t client)
{
    if (client->auth_data == NULL) {
        return ESP_FAIL;
    }

    free(client->auth_data->method);
    free(client->auth_data->realm);
    free(client->auth_data->algorithm);
    free(client->auth_data->qop);
    free(client->auth_data->nonce);
    free(client->auth_data->opaque);
    free(client->auth_data->uri);
    memset(client->auth_data, 0, sizeof(esp_http_auth_data_t));
    return ESP_OK;
}

static esp_err_t esp_http_client_prepare(esp_http_client_handle_t client)
{
    client->process_again = 0;
    client->response->data_process = 0;
    client->first_line_prepared = false;
    /**
     * Clear location field before making a new HTTP request. Location
     * field should not be cleared in http_on_header* callbacks because
     * callbacks can be invoked multiple times for same header, and
     * hence can lead to data corruption.
     */
    if (client->location != NULL) {
        free(client->location);
        client->location = NULL;
    }
    if (client->auth_header != NULL) {
        free(client->auth_header);
        client->auth_header = NULL;
    }
    http_parser_init(client->parser, HTTP_RESPONSE);
    if (client->connection_info.username) {
        char *auth_response = NULL;

        if (client->connection_info.auth_type == HTTP_AUTH_TYPE_BASIC) {
            auth_response = http_auth_basic(client->connection_info.username, client->connection_info.password);
#ifdef CONFIG_ESP_HTTP_CLIENT_ENABLE_DIGEST_AUTH
        } else if (client->connection_info.auth_type == HTTP_AUTH_TYPE_DIGEST && client->auth_data) {
            client->auth_data->uri = NULL;
            http_utils_assign_string(&client->auth_data->uri,client->connection_info.path,-1);
            if (client->connection_info.query){
                http_utils_append_string(&client->auth_data->uri,"?",-1);
                http_utils_append_string(&client->auth_data->uri,client->connection_info.query,-1);
            }
            client->auth_data->cnonce = ((uint64_t)esp_random() << 32) + esp_random();
            auth_response = http_auth_digest(client->connection_info.username, client->connection_info.password, client->auth_data);
#endif
        }

        if (auth_response) {
            ESP_LOGD(TAG, "auth_response=%s", auth_response);
            esp_http_client_set_header(client, "Authorization", auth_response);
            free(auth_response);
        }
    }
    return ESP_OK;
}

static char *_get_host_header(char *host, int port)
{
    int err = 0;
    char *host_name;
    if (port != DEFAULT_HTTP_PORT && port != DEFAULT_HTTPS_PORT) {
        err = asprintf(&host_name, "%s:%d", host, port);
    } else {
        err = asprintf(&host_name, "%s", host);
    }
    if (err == -1) {
        return NULL;
    }
    return host_name;
}

static bool init_common_tcp_transport(esp_http_client_handle_t client, const esp_http_client_config_t *config, esp_transport_handle_t transport)
{
    if (config->keep_alive_enable == true) {
        client->keep_alive_cfg.keep_alive_enable = true;
        client->keep_alive_cfg.keep_alive_idle = (config->keep_alive_idle == 0) ? DEFAULT_KEEP_ALIVE_IDLE : config->keep_alive_idle;
        client->keep_alive_cfg.keep_alive_interval = (config->keep_alive_interval == 0) ? DEFAULT_KEEP_ALIVE_INTERVAL : config->keep_alive_interval;
        client->keep_alive_cfg.keep_alive_count =  (config->keep_alive_count == 0) ? DEFAULT_KEEP_ALIVE_COUNT : config->keep_alive_count;
        esp_transport_tcp_set_keep_alive(transport, &client->keep_alive_cfg);
    }

    if (config->if_name) {
        if (client->if_name == NULL) {
            client->if_name = calloc(1, sizeof(struct ifreq));
            ESP_RETURN_ON_FALSE(client->if_name, false, TAG, "Memory exhausted");
            memcpy(client->if_name, config->if_name, sizeof(struct ifreq));
        }
        esp_transport_tcp_set_interface_name(transport, client->if_name);
    }
    return true;
}

esp_http_client_handle_t esp_http_client_init(const esp_http_client_config_t *config)
{

    esp_http_client_handle_t client;
    esp_err_t ret = ESP_OK;
    esp_transport_handle_t tcp = NULL;
    char *host_name;
    bool _success;

    _success = (
                   (client                         = calloc(1, sizeof(esp_http_client_t)))           &&
                   (client->parser                 = calloc(1, sizeof(struct http_parser)))          &&
                   (client->parser_settings        = calloc(1, sizeof(struct http_parser_settings))) &&
                   (client->auth_data              = calloc(1, sizeof(esp_http_auth_data_t)))        &&
                   (client->request                = calloc(1, sizeof(esp_http_data_t)))             &&
                   (client->request->headers       = http_header_init())                             &&
                   (client->request->buffer        = calloc(1, sizeof(esp_http_buffer_t)))           &&
                   (client->response               = calloc(1, sizeof(esp_http_data_t)))             &&
                   (client->response->headers      = http_header_init())                             &&
                   (client->response->buffer       = calloc(1, sizeof(esp_http_buffer_t)))
               );

    if (!_success) {
        ESP_LOGE(TAG, "Error allocate memory");
        goto error;
    }

    _success = (
                   (client->transport_list = esp_transport_list_init()) &&
                   (tcp = esp_transport_tcp_init()) &&
                   (esp_transport_set_default_port(tcp, DEFAULT_HTTP_PORT) == ESP_OK) &&
                   (esp_transport_list_add(client->transport_list, tcp, "http") == ESP_OK)
               );
    if (!_success) {
        ESP_LOGE(TAG, "Error initialize transport");
        goto error;
    }

    ESP_GOTO_ON_FALSE(init_common_tcp_transport(client, config, tcp), ESP_FAIL, error, TAG, "Failed to set TCP config");

#ifdef CONFIG_ESP_HTTP_CLIENT_ENABLE_HTTPS
    esp_transport_handle_t ssl = NULL;
    _success = (
                   (ssl = esp_transport_ssl_init()) &&
                   (esp_transport_set_default_port(ssl, DEFAULT_HTTPS_PORT) == ESP_OK) &&
                   (esp_transport_list_add(client->transport_list, ssl, "https") == ESP_OK)
               );

    if (!_success) {
        ESP_LOGE(TAG, "Error initialize SSL Transport");
        goto error;
    }

    ESP_GOTO_ON_FALSE(init_common_tcp_transport(client, config, ssl), ESP_FAIL, error, TAG, "Failed to set SSL config");

    if (config->crt_bundle_attach != NULL) {
#ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
        esp_transport_ssl_crt_bundle_attach(ssl, config->crt_bundle_attach);
#else //CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
        ESP_LOGE(TAG, "use_crt_bundle configured but not enabled in menuconfig: Please enable MBEDTLS_CERTIFICATE_BUNDLE option");
#endif
    } else if (config->use_global_ca_store == true) {
        esp_transport_ssl_enable_global_ca_store(ssl);
    } else if (config->cert_pem) {
        if (!config->cert_len) {
            esp_transport_ssl_set_cert_data(ssl, config->cert_pem, strlen(config->cert_pem));
        } else {
            esp_transport_ssl_set_cert_data_der(ssl, config->cert_pem, config->cert_len);
        }
    }

    if (config->client_cert_pem) {
        if (!config->client_cert_len) {
            esp_transport_ssl_set_client_cert_data(ssl, config->client_cert_pem, strlen(config->client_cert_pem));
        } else {
            esp_transport_ssl_set_client_cert_data_der(ssl, config->client_cert_pem, config->client_cert_len);
        }
    }
    esp_transport_ssl_set_tls_version(ssl, config->tls_version);

#if CONFIG_ESP_TLS_USE_SECURE_ELEMENT
    if (config->use_secure_element) {
        esp_transport_ssl_use_secure_element(ssl);
    }
#endif

#if CONFIG_ESP_TLS_USE_DS_PERIPHERAL
    if (config->ds_data != NULL) {
        esp_transport_ssl_set_ds_data(ssl, config->ds_data);
    }
#endif

#if CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
    if (config->save_client_session) {
        client->session_ticket_state = SESSION_TICKET_NOT_SAVED;
    }
#endif

#if CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT
    if (config->transport) {
        client->transport = config->transport;
    }
#endif

    if (config->client_key_pem) {
        if (!config->client_key_len) {
            esp_transport_ssl_set_client_key_data(ssl, config->client_key_pem, strlen(config->client_key_pem));
        } else {
            esp_transport_ssl_set_client_key_data_der(ssl, config->client_key_pem, config->client_key_len);
        }
    }
#ifdef CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
    if (config->use_ecdsa_peripheral) {
        esp_transport_ssl_set_client_key_ecdsa_peripheral(ssl, config->ecdsa_key_efuse_blk);
    }
#endif
    if (config->client_key_password && config->client_key_password_len > 0) {
        esp_transport_ssl_set_client_key_password(ssl, config->client_key_password, config->client_key_password_len);
    }

    if (config->skip_cert_common_name_check) {
        esp_transport_ssl_skip_common_name_check(ssl);
    }

    if (config->common_name) {
        esp_transport_ssl_set_common_name(ssl, config->common_name);
    }
#endif

    if (_set_config(client, config) != ESP_OK) {
        ESP_LOGE(TAG, "Error set configurations");
        goto error;
    }
    _success = (
                   (client->request->buffer->data  = malloc(client->buffer_size_tx))  &&
                   (client->response->buffer->data = malloc(client->buffer_size_rx))
               );

    if (!_success) {
        ESP_LOGE(TAG, "Allocation failed");
        goto error;
    }

    const char *user_agent = config->user_agent == NULL ? DEFAULT_HTTP_USER_AGENT : config->user_agent;

    if (config->host != NULL && config->path != NULL) {
        if (client->connection_info.host == NULL) {
            ESP_LOGE(TAG, "invalid host");
            goto error;
        }
        host_name = _get_host_header(client->connection_info.host, client->connection_info.port);
        if (host_name == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for host header");
            goto error;
        }
        _success = (
            (esp_http_client_set_header(client, "User-Agent", user_agent) == ESP_OK) &&
            (esp_http_client_set_header(client, "Host", host_name) == ESP_OK)
        );
        free(host_name);
        if (!_success) {
            ESP_LOGE(TAG, "Error while setting default configurations");
            goto error;
        }
    } else if (config->url != NULL) {
        if (esp_http_client_set_url(client, config->url) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set URL");
            goto error;
        }
        if (client->connection_info.host == NULL) {
            ESP_LOGE(TAG, "invalid host");
            goto error;
        }
        host_name = _get_host_header(client->connection_info.host, client->connection_info.port);
        if (host_name == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for host header");
            goto error;
        }

        _success = (
                    (esp_http_client_set_header(client, "User-Agent", user_agent) == ESP_OK) &&
                    (esp_http_client_set_header(client, "Host", host_name) == ESP_OK)
                );

        free(host_name);
        if (!_success) {
            ESP_LOGE(TAG, "Error while setting default configurations");
            goto error;
        }
    } else {
        ESP_LOGE(TAG, "config should have either URL or host & path");
        goto error;
    }

    /* As default behavior, cache data received in fetch header state. This will be
     * used in esp_http_client_read API only. For esp_http_perform we shall disable
     * this as data will be processed by event handler */
    client->cache_data_in_fetch_hdr = 1;

    client->parser_settings->on_message_begin = http_on_message_begin;
    client->parser_settings->on_url = http_on_url;
    client->parser_settings->on_status = http_on_status;
    client->parser_settings->on_header_field = http_on_header_field;
    client->parser_settings->on_header_value = http_on_header_value;
    client->parser_settings->on_headers_complete = http_on_headers_complete;
    client->parser_settings->on_body = http_on_body;
    client->parser_settings->on_message_complete = http_on_message_complete;
    client->parser_settings->on_chunk_complete = http_on_chunk_complete;
    client->parser_settings->on_chunk_header = http_on_chunk_header;
    client->parser->data = client;
    client->event.client = client;

    client->state = HTTP_STATE_INIT;

    if (ret == ESP_OK) {
        return client;
    }

error:
    esp_http_client_cleanup(client);
    return NULL;
}

static void esp_http_client_cached_buf_cleanup(esp_http_buffer_t *res_buffer)
{
    /* Free cached data if any, that was received during fetch header stage */
    if (res_buffer && res_buffer->orig_raw_data) {
        free(res_buffer->orig_raw_data);
        res_buffer->orig_raw_data = NULL;
        res_buffer->raw_data = NULL;
        res_buffer->raw_len = 0;
    }
}

esp_err_t esp_http_client_cleanup(esp_http_client_handle_t client)
{
    if (client == NULL) {
        return ESP_FAIL;
    }
    esp_http_client_close(client);
    if (client->transport_list) {
        esp_transport_list_destroy(client->transport_list);
    }
    if (client->request) {
        http_header_destroy(client->request->headers);
        if (client->request->buffer) {
            free(client->request->buffer->data);
        }
        free(client->request->buffer);
        free(client->request);
    }
    if (client->response) {
        http_header_destroy(client->response->headers);
        if (client->response->buffer) {
            free(client->response->buffer->data);
            esp_http_client_cached_buf_cleanup(client->response->buffer);
        }
        free(client->response->buffer);
        free(client->response);
    }
    if (client->if_name) {
        free(client->if_name);
    }
    free(client->parser);
    free(client->parser_settings);
    _clear_connection_info(client);
    _clear_auth_data(client);
    free(client->auth_data);
    free(client->current_header_key);
    free(client->location);
    free(client->auth_header);
    free(client);
    return ESP_OK;
}

esp_err_t esp_http_client_set_redirection(esp_http_client_handle_t client)
{
    if (client == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (client->location == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_LOGD(TAG, "Redirect to %s", client->location);
    esp_err_t err = esp_http_client_set_url(client, client->location);
    if (err == ESP_OK) {
        client->redirect_counter ++;
        client->process_again = 1;  // used only in the blocking mode (when esp_http_client_perform() is called)
    }
    return err;
}

esp_err_t esp_http_client_reset_redirect_counter(esp_http_client_handle_t client)
{
    if (client == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    client->redirect_counter = 0;
    return ESP_OK;
}

static esp_err_t esp_http_check_response(esp_http_client_handle_t client)
{
    if (client->response->status_code >= HttpStatus_Ok && client->response->status_code < HttpStatus_MultipleChoices) {
        client->redirect_counter = 0;
        return ESP_OK;
    }
    if (client->redirect_counter >= client->max_redirection_count) {
        ESP_LOGE(TAG, "Error, reach max_redirection_count count=%d", client->redirect_counter);
        return ESP_ERR_HTTP_MAX_REDIRECT;
    }
    switch (client->response->status_code) {
        case HttpStatus_MovedPermanently:
        case HttpStatus_Found:
        case HttpStatus_SeeOther:
        case HttpStatus_TemporaryRedirect:
        case HttpStatus_PermanentRedirect:
            if (client->disable_auto_redirect) {
                http_dispatch_event(client, HTTP_EVENT_REDIRECT, NULL, 0);
            } else {
                if (esp_http_client_set_redirection(client) != ESP_OK){
                    return ESP_FAIL;
                };
            }
            esp_http_client_redirect_event_data_t evt_data = {
                .status_code = client->response->status_code,
                .client = client,
            };
            http_dispatch_event_to_event_loop(HTTP_EVENT_REDIRECT, &evt_data, sizeof(esp_http_client_redirect_event_data_t));
            break;
        case HttpStatus_Unauthorized:
            esp_http_client_add_auth(client);
    }
    return ESP_OK;
}

esp_err_t esp_http_client_set_url(esp_http_client_handle_t client, const char *url)
{
    esp_err_t ret = ESP_OK;
    char *old_host = NULL;
    struct http_parser_url purl;
    int old_port;

    if (client == NULL || url == NULL) {
        ESP_LOGE(TAG, "client or url must not NULL");
        return ESP_ERR_INVALID_ARG;
    }

    http_parser_url_init(&purl);

    int parser_status = http_parser_parse_url(url, strlen(url), 0, &purl);

    if (parser_status != 0) {
        ESP_LOGE(TAG, "Error parse url %s", url);
        return ESP_ERR_INVALID_ARG;
    }
    if (client->connection_info.host) {
        old_host = strdup(client->connection_info.host);
    }
    old_port = client->connection_info.port;

    if (purl.field_data[UF_HOST].len) {
        http_utils_assign_string(&client->connection_info.host, url + purl.field_data[UF_HOST].off, purl.field_data[UF_HOST].len);
        ESP_GOTO_ON_FALSE(client->connection_info.host, ESP_ERR_NO_MEM, error, TAG, "Memory exhausted");
    }
    // Close the connection if host was changed
    if (old_host && client->connection_info.host
            && strcasecmp(old_host, (const void *)client->connection_info.host) != 0) {
        ESP_LOGD(TAG, "New host assign = %s", client->connection_info.host);
        if (esp_http_client_set_header(client, "Host", client->connection_info.host) != ESP_OK) {
            free(old_host);
            return ESP_ERR_NO_MEM;
        }
        /* Free cached data if any, as we are closing this connection */
        esp_http_client_cached_buf_cleanup(client->response->buffer);
        esp_http_client_close(client);
    }

    if (old_host) {
        free(old_host);
        old_host = NULL;
    }

    if (purl.field_data[UF_SCHEMA].len) {
        http_utils_assign_string(&client->connection_info.scheme, url + purl.field_data[UF_SCHEMA].off, purl.field_data[UF_SCHEMA].len);
        ESP_RETURN_ON_FALSE(client->connection_info.scheme, ESP_ERR_NO_MEM, TAG, "Memory exhausted");

        if (strcasecmp(client->connection_info.scheme, "http") == 0) {
            client->connection_info.port = DEFAULT_HTTP_PORT;
        } else if (strcasecmp(client->connection_info.scheme, "https") == 0) {
            client->connection_info.port = DEFAULT_HTTPS_PORT;
        }
    }

    if (purl.field_data[UF_PORT].len) {
        client->connection_info.port = strtol((const char*)(url + purl.field_data[UF_PORT].off), NULL, 10);
    }

    if (old_port != client->connection_info.port) {
        /* Free cached data if any, as we are closing this connection */
        esp_http_client_cached_buf_cleanup(client->response->buffer);
        esp_http_client_close(client);
    }

    if (purl.field_data[UF_USERINFO].len) {
        char *user_info = NULL;
        http_utils_assign_string(&user_info, url + purl.field_data[UF_USERINFO].off, purl.field_data[UF_USERINFO].len);
        if (user_info) {
            char *username = user_info;
            char *password = strchr(user_info, ':');
            if (password) {
                *password = 0;
                password ++;
                http_utils_assign_string(&client->connection_info.password, password, -1);
                ESP_RETURN_ON_FALSE(client->connection_info.password, ESP_ERR_NO_MEM, TAG, "Memory exhausted");
            }
            http_utils_assign_string(&client->connection_info.username, username, -1);
            ESP_RETURN_ON_FALSE(client->connection_info.username, ESP_ERR_NO_MEM, TAG, "Memory exhausted");
            free(user_info);
        } else {
            return ESP_ERR_NO_MEM;
        }
    }

    //Reset path and query if there are no information
    if (purl.field_data[UF_PATH].len) {
        http_utils_assign_string(&client->connection_info.path, url + purl.field_data[UF_PATH].off, purl.field_data[UF_PATH].len);
    } else {
        http_utils_assign_string(&client->connection_info.path, "/", -1);
    }
    ESP_RETURN_ON_FALSE(client->connection_info.path, ESP_ERR_NO_MEM, TAG, "Memory exhausted");

    if (purl.field_data[UF_QUERY].len) {
        http_utils_assign_string(&client->connection_info.query, url + purl.field_data[UF_QUERY].off, purl.field_data[UF_QUERY].len);
        ESP_RETURN_ON_FALSE(client->connection_info.query, ESP_ERR_NO_MEM, TAG, "Memory exhausted");
    } else if (client->connection_info.query) {
        free(client->connection_info.query);
        client->connection_info.query = NULL;
    }

    return ret;

error:
    free(old_host);
    return ret;
}

int esp_http_client_get_errno(esp_http_client_handle_t client)
{
    if (!client) {
        ESP_LOGE(TAG, "Invalid client handle");
        return -1;
    }
    return esp_transport_get_errno(client->transport);
}

esp_err_t esp_http_client_set_method(esp_http_client_handle_t client, esp_http_client_method_t method)
{
    client->connection_info.method = method;
    return ESP_OK;
}

esp_err_t esp_http_client_set_timeout_ms(esp_http_client_handle_t client, int timeout_ms)
{
    if (client == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    client->timeout_ms = timeout_ms;
    return ESP_OK;
}

static int esp_http_client_get_data(esp_http_client_handle_t client)
{
    if (client->state < HTTP_STATE_RES_ON_DATA_START) {
        return ESP_FAIL;
    }

    if (client->connection_info.method == HTTP_METHOD_HEAD) {
        return 0;
    }

    esp_http_buffer_t *res_buffer = client->response->buffer;

    ESP_LOGD(TAG, "data_process=%"PRId64", content_length=%"PRId64, client->response->data_process, client->response->content_length);
    errno = 0;
    int rlen = esp_transport_read(client->transport, res_buffer->data, client->buffer_size_rx, client->timeout_ms);
    if (rlen >= 0) {
        // When tls error is ESP_TLS_ERR_SSL_WANT_READ (-0x6900), esp_trasnport_read returns ERR_TCP_TRANSPORT_CONNECTION_TIMEOUT (0x0).
        // We should not execute http_parser_execute() on this condition as it sets the internal state machine in an
        // invalid state.
        if (!(client->is_async && rlen == 0)) {
            http_parser_execute(client->parser, client->parser_settings, res_buffer->data, rlen);
        }
    }
    return rlen;
}

bool esp_http_client_is_complete_data_received(esp_http_client_handle_t client)
{
    if (client->response->is_chunked) {
        if (!client->is_chunk_complete) {
            ESP_LOGD(TAG, "Chunks were not completely read");
            return false;
        }
    } else {
        if (client->response->data_process != client->response->content_length) {
            ESP_LOGD(TAG, "Data processed %"PRId64" != Data specified in content length %"PRId64, client->response->data_process, client->response->content_length);
            return false;
        }
    }
    return true;
}

int esp_http_client_read(esp_http_client_handle_t client, char *buffer, int len)
{
    esp_http_buffer_t *res_buffer = client->response->buffer;

    int rlen = ESP_FAIL, ridx = 0;
    if (res_buffer->raw_len) {
        int remain_len = client->response->buffer->raw_len;
        if (remain_len > len) {
            remain_len = len;
        }
        memcpy(buffer, res_buffer->raw_data, remain_len);
        res_buffer->raw_len -= remain_len;
        res_buffer->raw_data += remain_len;
        ridx = remain_len;
        if (res_buffer->raw_len == 0) {
            esp_http_client_cached_buf_cleanup(res_buffer);
        }
    }
    int need_read = len - ridx;
    bool is_data_remain = true;
    while (need_read > 0 && is_data_remain) {
        if (client->response->is_chunked) {
            is_data_remain = !client->is_chunk_complete;
        } else {
            is_data_remain = client->response->data_process < client->response->content_length;
        }
        ESP_LOGD(TAG, "is_data_remain=%d, is_chunked=%d, content_length=%"PRId64, is_data_remain, client->response->is_chunked, client->response->content_length);
        if (!is_data_remain) {
            break;
        }
        int byte_to_read = need_read;
        if (byte_to_read > client->buffer_size_rx) {
            byte_to_read = client->buffer_size_rx;
        }
        errno = 0;
        rlen = esp_transport_read(client->transport, res_buffer->data, byte_to_read, client->timeout_ms);
        ESP_LOGD(TAG, "need_read=%d, byte_to_read=%d, rlen=%d, ridx=%d", need_read, byte_to_read, rlen, ridx);

        if (rlen <= 0) {
            esp_log_level_t sev = ESP_LOG_WARN;
            /* Check for cleanly closed connection */
            if (rlen == ERR_TCP_TRANSPORT_CONNECTION_CLOSED_BY_FIN && client->response->is_chunked) {
                /* Explicit call to parser for invoking `message_complete` callback */
                http_parser_execute(client->parser, client->parser_settings, res_buffer->data, 0);
                /* ...and lowering the message severity, as closed connection from server side is expected in chunked transport */
                sev = ESP_LOG_DEBUG;
            }
            if (errno != 0) {
                ESP_LOG_LEVEL(sev, TAG, "esp_transport_read returned:%d and errno:%d ", rlen, errno);
            }

            if (rlen == ERR_TCP_TRANSPORT_CONNECTION_TIMEOUT) {
                ESP_LOGD(TAG, "Connection timed out before data was ready!");
                /* Returning the number of bytes read upto the point where connection timed out */
                if (ridx) {
                    return ridx;
                }
                return -ESP_ERR_HTTP_EAGAIN;
            }

            if (rlen != ERR_TCP_TRANSPORT_CONNECTION_CLOSED_BY_FIN) {
                esp_err_t err = esp_transport_translate_error(rlen);
                ESP_LOGE(TAG, "transport_read: error - %d | %s", err, esp_err_to_name(err));
            }

            if (rlen < 0 && ridx == 0 && !esp_http_client_is_complete_data_received(client)) {
                http_dispatch_event(client, HTTP_EVENT_ERROR, esp_transport_get_error_handle(client->transport), 0);
                http_dispatch_event_to_event_loop(HTTP_EVENT_ERROR, &client, sizeof(esp_http_client_handle_t));
                return ESP_FAIL;
            }
            return ridx;
        }
        res_buffer->output_ptr = buffer + ridx;
        http_parser_execute(client->parser, client->parser_settings, res_buffer->data, rlen);
        ridx += res_buffer->raw_len;
        need_read -= res_buffer->raw_len;

        res_buffer->raw_len = 0; //clear
        res_buffer->output_ptr = NULL;
    }

    return ridx;
}

esp_err_t esp_http_client_perform(esp_http_client_handle_t client)
{
    esp_err_t err;
    do {
        if (client->process_again) {
            esp_http_client_prepare(client);
        }
        switch (client->state) {
        /* In case of blocking esp_http_client_perform(), the following states will fall through one after the after;
           in case of non-blocking esp_http_client_perform(), if there is an error condition, like EINPROGRESS or EAGAIN,
           then the esp_http_client_perform() API will return ESP_ERR_HTTP_EAGAIN error. The user may call
           esp_http_client_perform API again, and for this reason, we maintain the states */
            case HTTP_STATE_INIT:
                if ((err = esp_http_client_connect(client)) != ESP_OK) {
                    if (client->is_async && err == ESP_ERR_HTTP_CONNECTING) {
                        return ESP_ERR_HTTP_EAGAIN;
                    }
                    http_dispatch_event(client, HTTP_EVENT_ERROR, esp_transport_get_error_handle(client->transport), 0);
                    http_dispatch_event_to_event_loop(HTTP_EVENT_ERROR, &client, sizeof(esp_http_client_handle_t));
                    return err;
                }
                /* falls through */
            case HTTP_STATE_CONNECTED:
                if ((err = esp_http_client_request_send(client, client->post_len)) != ESP_OK) {
                    if (client->is_async && errno == EAGAIN) {
                        return ESP_ERR_HTTP_EAGAIN;
                    }
                    http_dispatch_event(client, HTTP_EVENT_ERROR, esp_transport_get_error_handle(client->transport), 0);
                    http_dispatch_event_to_event_loop(HTTP_EVENT_ERROR, &client, sizeof(esp_http_client_handle_t));
                    return err;
                }
                /* falls through */
            case HTTP_STATE_REQ_COMPLETE_HEADER:
                if ((err = esp_http_client_send_post_data(client)) != ESP_OK) {
                    if (client->is_async && errno == EAGAIN) {
                        return ESP_ERR_HTTP_EAGAIN;
                    }
                    http_dispatch_event(client, HTTP_EVENT_ERROR, esp_transport_get_error_handle(client->transport), 0);
                    http_dispatch_event_to_event_loop(HTTP_EVENT_ERROR, &client, sizeof(esp_http_client_handle_t));
                    return err;
                }
                /* falls through */
            case HTTP_STATE_REQ_COMPLETE_DATA:
                /* Disable caching response body, as data should
                 * be handled by application event handler */
                client->cache_data_in_fetch_hdr = 0;
                int64_t ret = esp_http_client_fetch_headers(client);
                if (ret < 0) {
                    if ((client->is_async && errno == EAGAIN) || ret == -ESP_ERR_HTTP_EAGAIN) {
                        return ESP_ERR_HTTP_EAGAIN;
                    }
                    /* Enable caching after error condition because next
                     * request could be performed using native APIs */
                    client->cache_data_in_fetch_hdr = 1;
                    if (esp_transport_get_errno(client->transport) == ENOTCONN) {
                        ESP_LOGW(TAG, "Close connection due to FIN received");
                        esp_http_client_close(client);
                        http_dispatch_event(client, HTTP_EVENT_ERROR, esp_transport_get_error_handle(client->transport), 0);
                        http_dispatch_event_to_event_loop(HTTP_EVENT_ERROR, &client, sizeof(esp_http_client_handle_t));
                        return ESP_ERR_HTTP_CONNECTION_CLOSED;
                    }
                    http_dispatch_event(client, HTTP_EVENT_ERROR, esp_transport_get_error_handle(client->transport), 0);
                    http_dispatch_event_to_event_loop(HTTP_EVENT_ERROR, &client, sizeof(esp_http_client_handle_t));
                    return ESP_ERR_HTTP_FETCH_HEADER;
                }
                /* falls through */
            case HTTP_STATE_RES_ON_DATA_START:
                /* Enable caching after fetch headers state because next
                 * request could be performed using native APIs */
                client->cache_data_in_fetch_hdr = 1;
                if ((err = esp_http_check_response(client)) != ESP_OK) {
                    ESP_LOGE(TAG, "Error response");
                    http_dispatch_event(client, HTTP_EVENT_ERROR, esp_transport_get_error_handle(client->transport), 0);
                    http_dispatch_event_to_event_loop(HTTP_EVENT_ERROR, &client, sizeof(esp_http_client_handle_t));
                    return err;
                }
                while (client->response->is_chunked && !client->is_chunk_complete) {
                    if (esp_http_client_get_data(client) <= 0) {
                        if (client->is_async && errno == EAGAIN) {
                            return ESP_ERR_HTTP_EAGAIN;
                        }
                        ESP_LOGD(TAG, "Read finish or server requests close");
                        break;
                    }
                }
                while (client->response->data_process < client->response->content_length) {
                    if (esp_http_client_get_data(client) <= 0) {
                        if (client->is_async && errno == EAGAIN) {
                            return ESP_ERR_HTTP_EAGAIN;
                        }
                        ESP_LOGD(TAG, "Read finish or server requests close");
                        break;
                    }
                }
                http_dispatch_event(client, HTTP_EVENT_ON_FINISH, NULL, 0);
                http_dispatch_event_to_event_loop(HTTP_EVENT_ON_FINISH, &client, sizeof(esp_http_client_handle_t));

                client->response->buffer->raw_len = 0;
                if (!http_should_keep_alive(client->parser)) {
                    ESP_LOGD(TAG, "Close connection");
                    esp_http_client_close(client);
                } else {
                    if (client->state > HTTP_STATE_CONNECTED) {
                        client->state = HTTP_STATE_CONNECTED;
                        client->first_line_prepared = false;
                    }
                }
                break;
                default:
                break;
        }
    } while (client->process_again);
    return ESP_OK;
}

int64_t esp_http_client_fetch_headers(esp_http_client_handle_t client)
{
    if (client->state < HTTP_STATE_REQ_COMPLETE_HEADER) {
        return ESP_FAIL;
    }

    client->state = HTTP_STATE_REQ_COMPLETE_DATA;
    esp_http_buffer_t *buffer = client->response->buffer;
    client->response->status_code = -1;

    while (client->state < HTTP_STATE_RES_COMPLETE_HEADER) {
        errno = 0;
        buffer->len = esp_transport_read(client->transport, buffer->data, client->buffer_size_rx, client->timeout_ms);
        if (buffer->len <= 0) {
            if (buffer->len == ERR_TCP_TRANSPORT_CONNECTION_TIMEOUT) {
                ESP_LOGW(TAG, "Connection timed out before data was ready!");
                return -ESP_ERR_HTTP_EAGAIN;
            }
            return ESP_FAIL;
        }
        http_parser_execute(client->parser, client->parser_settings, buffer->data, buffer->len);
    }
    client->state = HTTP_STATE_RES_ON_DATA_START;
    ESP_LOGD(TAG, "content_length = %"PRId64, client->response->content_length);
    if (client->response->content_length <= 0) {
        client->response->is_chunked = true;
        return 0;
    }
    return client->response->content_length;
}

static esp_err_t esp_http_client_connect(esp_http_client_handle_t client)
{
    esp_err_t err;

    if (client->state == HTTP_STATE_UNINIT) {
        ESP_LOGE(TAG, "Client has not been initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if ((err = esp_http_client_prepare(client)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize request data");
        esp_http_client_close(client);
        return err;
    }

    if (client->state < HTTP_STATE_CONNECTED) {
#ifdef CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT
        // If the custom transport is enabled and defined, we skip the selection of appropriate transport from the list
        // based on the scheme, since we already have the transport
        if (!client->transport)
#endif
        {
            ESP_LOGD(TAG, "Begin connect to: %s://%s:%d", client->connection_info.scheme, client->connection_info.host, client->connection_info.port);
            client->transport = esp_transport_list_get_transport(client->transport_list, client->connection_info.scheme);
        }

#ifdef CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
        if (client->session_ticket_state == SESSION_TICKET_SAVED) {
            esp_transport_ssl_session_ticket_operation(client->transport, ESP_TRANSPORT_SESSION_TICKET_USE);
        }
#endif
        if (client->transport == NULL) {
            ESP_LOGE(TAG, "No transport found");
#ifndef CONFIG_ESP_HTTP_CLIENT_ENABLE_HTTPS
            if (strcasecmp(client->connection_info.scheme, "https") == 0) {
                ESP_LOGE(TAG, "Please enable HTTPS at menuconfig to allow requesting via https");
            }
#endif
            return ESP_ERR_HTTP_INVALID_TRANSPORT;
        }
        if (!client->is_async) {
            if (esp_transport_connect(client->transport, client->connection_info.host, client->connection_info.port, client->timeout_ms) < 0) {
                ESP_LOGE(TAG, "Connection failed, sock < 0");
                return ESP_ERR_HTTP_CONNECT;
            }
        } else {
            int ret = esp_transport_connect_async(client->transport, client->connection_info.host, client->connection_info.port, client->timeout_ms);
            if (ret == ASYNC_TRANS_CONNECT_FAIL) {
                ESP_LOGE(TAG, "Connection failed");
                if (strcasecmp(client->connection_info.scheme, "http") == 0) {
                    ESP_LOGE(TAG, "Asynchronous mode doesn't work for HTTP based connection");
                    return ESP_ERR_INVALID_ARG;
                }
                return ESP_ERR_HTTP_CONNECT;
            } else if (ret == ASYNC_TRANS_CONNECTING) {
                ESP_LOGD(TAG, "Connection not yet established");
                return ESP_ERR_HTTP_CONNECTING;
            }
        }
        client->state = HTTP_STATE_CONNECTED;
        http_dispatch_event(client, HTTP_EVENT_ON_CONNECTED, NULL, 0);
        http_dispatch_event_to_event_loop(HTTP_EVENT_ON_CONNECTED, &client, sizeof(esp_http_client_handle_t));
#ifdef CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
        if (client->session_ticket_state != SESSION_TICKET_UNUSED) {
            esp_transport_ssl_session_ticket_operation(client->transport, ESP_TRANSPORT_SESSION_TICKET_SAVE);
            client->session_ticket_state = SESSION_TICKET_SAVED;
        }
#endif

    }
    return ESP_OK;
}

static int http_client_prepare_first_line(esp_http_client_handle_t client, int write_len)
{
    if (write_len >= 0) {
        const bool length_required = (client->connection_info.method != HTTP_METHOD_GET &&
                                      client->connection_info.method != HTTP_METHOD_HEAD &&
                                      client->connection_info.method != HTTP_METHOD_DELETE);
        if (write_len != 0 || length_required) {
            http_header_set_format(client->request->headers, "Content-Length", "%d", write_len);
        }
    } else {
        esp_http_client_set_header(client, "Transfer-Encoding", "chunked");
    }

    const char *method = HTTP_METHOD_MAPPING[client->connection_info.method];

    int first_line_len = snprintf(client->request->buffer->data,
                                  client->buffer_size_tx, "%s %s",
                                  method,
                                  client->connection_info.path);
    if (first_line_len >= client->buffer_size_tx) {
        ESP_LOGE(TAG, "Out of buffer");
        return -1;
    }

    if (client->connection_info.query) {
        first_line_len += snprintf(client->request->buffer->data + first_line_len,
                                   client->buffer_size_tx - first_line_len, "?%s", client->connection_info.query);
        if (first_line_len >= client->buffer_size_tx) {
            ESP_LOGE(TAG, "Out of buffer");
            return -1;

        }
    }
    first_line_len += snprintf(client->request->buffer->data + first_line_len,
                               client->buffer_size_tx - first_line_len, " %s\r\n", DEFAULT_HTTP_PROTOCOL);
    if (first_line_len >= client->buffer_size_tx) {
        ESP_LOGE(TAG, "Out of buffer");
        return -1;
    }
    return first_line_len;
}

static esp_err_t esp_http_client_request_send(esp_http_client_handle_t client, int write_len)
{
    int first_line_len = 0;
    if (!client->first_line_prepared) {
        if ((first_line_len = http_client_prepare_first_line(client, write_len)) < 0) {
            return first_line_len;
        }
        client->first_line_prepared = true;
        client->header_index = 0;
        client->data_written_index = 0;
        client->data_write_left = 0;
    }

    if (client->data_write_left > 0) {
        /* sending leftover data from previous call to esp_http_client_request_send() API */
        int wret = 0;
        if (((wret = esp_http_client_write(client, client->request->buffer->data + client->data_written_index, client->data_write_left)) < 0)) {
            ESP_LOGE(TAG, "Error write request");
            return ESP_ERR_HTTP_WRITE_DATA;
        }
        client->data_write_left -= wret;
        client->data_written_index += wret;
        if (client->is_async && client->data_write_left > 0) {
            return ESP_ERR_HTTP_WRITE_DATA;      /* In case of EAGAIN error, we return ESP_ERR_HTTP_WRITE_DATA,
                                                 and the handling of EAGAIN should be done in the higher level APIs. */
        }
    }

    int wlen = client->buffer_size_tx - first_line_len;
    while ((client->header_index = http_header_generate_string(client->request->headers, client->header_index, client->request->buffer->data + first_line_len, &wlen))) {
        if (wlen <= 0) {
            break;
        }
        if (first_line_len) {
            wlen += first_line_len;
            first_line_len = 0;
        }
        client->request->buffer->data[wlen] = 0;
        ESP_LOGD(TAG, "Write header[%d]: %s", client->header_index, client->request->buffer->data);

        client->data_write_left = wlen;
        client->data_written_index = 0;
        while (client->data_write_left > 0) {
            int wret = esp_transport_write(client->transport, client->request->buffer->data + client->data_written_index, client->data_write_left, client->timeout_ms);
            if (wret <= 0) {
                ESP_LOGE(TAG, "Error write request");
                esp_http_client_close(client);
                return ESP_ERR_HTTP_WRITE_DATA;
            }
            client->data_write_left -= wret;
            client->data_written_index += wret;
        }
        wlen = client->buffer_size_tx;
    }

    client->data_written_index = 0;
    client->data_write_left = client->post_len;
    client->state = HTTP_STATE_REQ_COMPLETE_HEADER;
    http_dispatch_event(client, HTTP_EVENT_HEADERS_SENT, NULL, 0);
    http_dispatch_event_to_event_loop(HTTP_EVENT_HEADERS_SENT, &client, sizeof(esp_http_client_handle_t));
    return ESP_OK;
}

static esp_err_t esp_http_client_send_post_data(esp_http_client_handle_t client)
{
    if (client->state != HTTP_STATE_REQ_COMPLETE_HEADER) {
        ESP_LOGE(TAG, "Invalid state");
        return ESP_ERR_INVALID_STATE;
    }
    if (!(client->post_data && client->post_len)) {
        goto success;
    }

    int wret = esp_http_client_write(client, client->post_data + client->data_written_index, client->data_write_left);
    if (wret < 0) {
        return wret;
    }
    client->data_write_left -= wret;
    client->data_written_index += wret;

    if (client->data_write_left <= 0) {
        goto success;
    } else {
        return ESP_ERR_HTTP_WRITE_DATA;
    }

success:
    client->state = HTTP_STATE_REQ_COMPLETE_DATA;
    return ESP_OK;
}

esp_err_t esp_http_client_open(esp_http_client_handle_t client, int write_len)
{
    client->post_len = write_len;
    esp_err_t err;
    if ((err = esp_http_client_connect(client)) != ESP_OK) {
        http_dispatch_event(client, HTTP_EVENT_ERROR, esp_transport_get_error_handle(client->transport), 0);
        http_dispatch_event_to_event_loop(HTTP_EVENT_ERROR, &client, sizeof(esp_http_client_handle_t));
        return err;
    }
    if ((err = esp_http_client_request_send(client, write_len)) != ESP_OK) {
        http_dispatch_event(client, HTTP_EVENT_ERROR, esp_transport_get_error_handle(client->transport), 0);
        http_dispatch_event_to_event_loop(HTTP_EVENT_ERROR, &client, sizeof(esp_http_client_handle_t));
        return err;
    }
    return ESP_OK;
}

int esp_http_client_write(esp_http_client_handle_t client, const char *buffer, int len)
{
    if (client->state < HTTP_STATE_REQ_COMPLETE_HEADER) {
        return ESP_FAIL;
    }

    int wlen = 0, widx = 0;
    while (len > 0) {
        wlen = esp_transport_write(client->transport, buffer + widx, len, client->timeout_ms);
        /* client->async_block is initialised in case of non-blocking IO, and in this case we return how
           much ever data was written by the esp_transport_write() API. */
        if (client->is_async || wlen <= 0) {
            return wlen;
        }
        widx += wlen;
        len -= wlen;
    }
    return widx;
}

esp_err_t esp_http_client_close(esp_http_client_handle_t client)
{
    if (client->state >= HTTP_STATE_INIT) {
        http_dispatch_event(client, HTTP_EVENT_DISCONNECTED, esp_transport_get_error_handle(client->transport), 0);
        http_dispatch_event_to_event_loop(HTTP_EVENT_DISCONNECTED, &client, sizeof(esp_http_client_handle_t));
        client->state = HTTP_STATE_INIT;
        return esp_transport_close(client->transport);
    }
    return ESP_OK;
}

esp_err_t esp_http_client_set_post_field(esp_http_client_handle_t client, const char *data, int len)
{
    esp_err_t err = ESP_OK;
    client->post_data = (char *)data;
    client->post_len = len;
    ESP_LOGD(TAG, "set post file length = %d", len);
    if (client->post_data) {
        char *value = NULL;
        if ((err = esp_http_client_get_header(client, "Content-Type", &value)) != ESP_OK) {
            return err;
        }
        if (value == NULL) {
            err = esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
        }
    } else {
        client->post_len = 0;
        err = esp_http_client_set_header(client, "Content-Type", NULL);
    }
    return err;
}

int esp_http_client_get_post_field(esp_http_client_handle_t client, char **data)
{
    if (client->post_data) {
        *data = client->post_data;
        return client->post_len;
    }
    return 0;
}

int esp_http_client_get_status_code(esp_http_client_handle_t client)
{
    return client->response->status_code;
}

int64_t esp_http_client_get_content_length(esp_http_client_handle_t client)
{
    return client->response->content_length;
}

bool esp_http_client_is_chunked_response(esp_http_client_handle_t client)
{
    return client->response->is_chunked;
}

esp_http_client_transport_t esp_http_client_get_transport_type(esp_http_client_handle_t client)
{
    if (!strcasecmp(client->connection_info.scheme, "https") ) {
        return HTTP_TRANSPORT_OVER_SSL;
    } else if (!strcasecmp(client->connection_info.scheme, "http")) {
        return HTTP_TRANSPORT_OVER_TCP;
    } else {
        return HTTP_TRANSPORT_UNKNOWN;
    }
}

esp_err_t esp_http_client_set_auth_data(esp_http_client_handle_t client, const char *auth_data, int len)
{
    if (client == NULL || auth_data == NULL || len <= 0) {
        return ESP_ERR_INVALID_ARG;
    }
    http_utils_append_string(&client->auth_header, auth_data, len);
    return ESP_OK;
}

void esp_http_client_add_auth(esp_http_client_handle_t client)
{
    if (client == NULL) {
        return;
    }
    if (client->state != HTTP_STATE_RES_ON_DATA_START) {
        return;
    }
    if (client->redirect_counter >= client->max_authorization_retries) {
        ESP_LOGE(TAG, "Error, reached max_authorization_retries count=%d", client->redirect_counter);
        return;
    }

    char *auth_header = client->auth_header;
    if (auth_header) {
        http_utils_trim_whitespace(&auth_header);
        ESP_LOGD(TAG, "UNAUTHORIZED: %s", auth_header);
        client->redirect_counter++;
#ifdef CONFIG_ESP_HTTP_CLIENT_ENABLE_DIGEST_AUTH
        if (http_utils_str_starts_with(auth_header, "Digest") == 0) {
            ESP_LOGD(TAG, "type = Digest");
            client->connection_info.auth_type = HTTP_AUTH_TYPE_DIGEST;
        } else {
#endif
#ifdef CONFIG_ESP_HTTP_CLIENT_ENABLE_BASIC_AUTH
        if (http_utils_str_starts_with(auth_header, "Basic") == 0) {
            ESP_LOGD(TAG, "type = Basic");
            client->connection_info.auth_type = HTTP_AUTH_TYPE_BASIC;
        } else {
#endif
            client->connection_info.auth_type = HTTP_AUTH_TYPE_NONE;
            ESP_LOGE(TAG, "This authentication method is not supported: %s", auth_header);
            return;
#ifdef CONFIG_ESP_HTTP_CLIENT_ENABLE_BASIC_AUTH
        }
#endif
#ifdef CONFIG_ESP_HTTP_CLIENT_ENABLE_DIGEST_AUTH
        }
#endif

        _clear_auth_data(client);

        client->auth_data->method = strdup(HTTP_METHOD_MAPPING[client->connection_info.method]);

        client->auth_data->nc = 1;
        client->auth_data->realm = http_utils_get_string_between(auth_header, "realm=\"", "\"");
        client->auth_data->algorithm = http_utils_get_string_between(auth_header, "algorithm=", ",");
        if (client->auth_data->algorithm == NULL) {
            client->auth_data->algorithm = http_utils_get_string_after(auth_header, "algorithm=");
        }
        if (client->auth_data->algorithm == NULL) {
            client->auth_data->algorithm = strdup("MD5");
        }
        client->auth_data->qop = http_utils_get_string_between(auth_header, "qop=\"", "\"");
        client->auth_data->nonce = http_utils_get_string_between(auth_header, "nonce=\"", "\"");
        client->auth_data->opaque = http_utils_get_string_between(auth_header, "opaque=\"", "\"");
        client->process_again = 1;
    } else {
        client->connection_info.auth_type = HTTP_AUTH_TYPE_NONE;
        ESP_LOGW(TAG, "This request requires authentication, but does not provide header information for that");
    }
}

int esp_http_client_read_response(esp_http_client_handle_t client, char *buffer, int len)
{
    int read_len = 0;
    while (read_len < len) {
        int data_read = esp_http_client_read(client, buffer + read_len, len - read_len);
        if (data_read <= 0) {
            return read_len;
        }
        read_len += data_read;
    }
    return read_len;
}

esp_err_t esp_http_client_flush_response(esp_http_client_handle_t client, int *len)
{
    if (client == NULL) {
        ESP_LOGE(TAG, "client must not be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    int read_len = 0;
    while (!esp_http_client_is_complete_data_received(client)) {
        int data_read = esp_http_client_get_data(client);
        if (data_read < 0) {
            return ESP_FAIL;
        }
        read_len += data_read;
    }
    if (len) {
        *len = read_len;
    }
    return ESP_OK;
}

esp_err_t esp_http_client_get_url(esp_http_client_handle_t client, char *url, const int len)
{
    if (client == NULL || url == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (client->connection_info.host && client->connection_info.scheme && client->connection_info.path) {
        snprintf(url, len, "%s://%s%s", client->connection_info.scheme, client->connection_info.host, client->connection_info.path);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to get URL");
    }
    return ESP_FAIL;
}

esp_err_t esp_http_client_get_chunk_length(esp_http_client_handle_t client, int *len)
{
    if (client == NULL || len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (esp_http_client_is_chunked_response(client)) {
        *len = client->response->chunk_length;
    } else {
        ESP_LOGE(TAG, "Response is not chunked");
        return ESP_FAIL;
    }
    return ESP_OK;
}
