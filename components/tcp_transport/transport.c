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


#include <stdlib.h>
#include <string.h>
#include <esp_tls.h>

#include "sys/queue.h"
#include "esp_log.h"

#include "esp_transport.h"
#include "esp_transport_internal.h"
#include "esp_transport_utils.h"

static const char *TAG = "TRANSPORT";

/**
 * Transport layer error structure including
 * * esp-tls last error storage
 * * sock-errno
 */
struct esp_transport_error_storage {
    struct esp_tls_last_error esp_tls_err_h_base;   /*!< esp-tls last error container */
    // additional fields
    int    sock_errno;                              /*!< last socket error captured for this transport */
};

/**
 * This list will hold all transport available
 */
STAILQ_HEAD(esp_transport_list_t, esp_transport_item_t);

struct transport_esp_tls;

/**
 * Internal transport structure holding list of transports and other data common to all transports
 */
typedef struct esp_transport_internal {
    struct esp_transport_list_t list;                      /*!< List of transports */
    struct esp_foundation_transport *base;       /*!< Base transport pointer shared for each list item */
} esp_transport_internal_t;

static esp_foundation_transport_t * esp_transport_init_foundation_transport(void)
{
    esp_foundation_transport_t *foundation = calloc(1, sizeof(esp_foundation_transport_t));
    ESP_TRANSPORT_MEM_CHECK(TAG, foundation, return NULL);
    foundation->error_handle = calloc(1, sizeof(struct esp_transport_error_storage));
    ESP_TRANSPORT_MEM_CHECK(TAG, foundation->error_handle,
                        free(foundation);
                        return NULL);
    foundation->transport_esp_tls = esp_transport_esp_tls_create();
    ESP_TRANSPORT_MEM_CHECK(TAG, foundation->transport_esp_tls,
                        free(foundation->error_handle);
                        free(foundation);
                        return NULL);
    return foundation;
}

static void esp_transport_destroy_foundation_transport(esp_foundation_transport_t *foundation)
{
    esp_transport_esp_tls_destroy(foundation->transport_esp_tls);
    free(foundation->error_handle);
    free(foundation);
}

static esp_transport_handle_t esp_transport_get_default_parent(esp_transport_handle_t t)
{
    /*
    * By default, the underlying transport layer handle is the handle itself
    */
    return t;
}

esp_transport_list_handle_t esp_transport_list_init(void)
{
    esp_transport_list_handle_t transport = calloc(1, sizeof(esp_transport_internal_t));
    ESP_TRANSPORT_MEM_CHECK(TAG, transport, return NULL);
    STAILQ_INIT(&transport->list);
    transport->base = esp_transport_init_foundation_transport();
    ESP_TRANSPORT_MEM_CHECK(TAG, transport->base,
                            free(transport);
                            return NULL);
    return transport;
}

esp_err_t esp_transport_list_add(esp_transport_list_handle_t h, esp_transport_handle_t t, const char *scheme)
{
    if (h == NULL || t == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    t->scheme = calloc(1, strlen(scheme) + 1);
    ESP_TRANSPORT_MEM_CHECK(TAG, t->scheme, return ESP_ERR_NO_MEM);
    strcpy(t->scheme, scheme);
    STAILQ_INSERT_TAIL(&h->list, t, next);
    // Each transport in a list to share the same error tracker
    t->base = h->base;
    return ESP_OK;
}

esp_transport_handle_t esp_transport_list_get_transport(esp_transport_list_handle_t h, const char *scheme)
{
    if (!h) {
        return NULL;
    }
    if (scheme == NULL) {
        return STAILQ_FIRST(&h->list);
    }
    esp_transport_handle_t item;
    STAILQ_FOREACH(item, &h->list, next) {
        if (strcasecmp(item->scheme, scheme) == 0) {
            return item;
        }
    }
    return NULL;
}

esp_err_t esp_transport_list_destroy(esp_transport_list_handle_t h)
{
    esp_transport_list_clean(h);
    esp_transport_destroy_foundation_transport(h->base);
    free(h);
    return ESP_OK;
}

esp_err_t esp_transport_list_clean(esp_transport_list_handle_t h)
{
    esp_transport_handle_t item = STAILQ_FIRST(&h->list);
    esp_transport_handle_t tmp;
    while (item != NULL) {
        tmp = STAILQ_NEXT(item, next);
        esp_transport_destroy(item);
        item = tmp;
    }
    STAILQ_INIT(&h->list);
    return ESP_OK;
}

esp_transport_handle_t esp_transport_init(void)
{
    esp_transport_handle_t t = calloc(1, sizeof(struct esp_transport_item_t));
    ESP_TRANSPORT_MEM_CHECK(TAG, t, return NULL);
    return t;
}

esp_transport_handle_t esp_transport_get_payload_transport_handle(esp_transport_handle_t t)
{
    if (t && t->_read) {
        return t->_parent_transfer(t);
    }
    return NULL;
}

esp_err_t esp_transport_destroy(esp_transport_handle_t t)
{
    if (t->_destroy) {
        t->_destroy(t);
    }
    if (t->scheme) {
        free(t->scheme);
    }
    free(t);
    return ESP_OK;
}

int esp_transport_connect(esp_transport_handle_t t, const char *host, int port, int timeout_ms)
{
    int ret = -1;
    if (t && t->_connect) {
        return t->_connect(t, host, port, timeout_ms);
    }
    return ret;
}

int esp_transport_connect_async(esp_transport_handle_t t, const char *host, int port, int timeout_ms)
{
    int ret = -1;
    if (t && t->_connect_async) {
        return t->_connect_async(t, host, port, timeout_ms);
    }
    return ret;
}

int esp_transport_read(esp_transport_handle_t t, char *buffer, int len, int timeout_ms)
{
    if (t && t->_read) {
        return t->_read(t, buffer, len, timeout_ms);
    }
    return -1;
}

int esp_transport_write(esp_transport_handle_t t, const char *buffer, int len, int timeout_ms)
{
    if (t && t->_write) {
        return t->_write(t, buffer, len, timeout_ms);
    }
    return -1;
}

int esp_transport_poll_read(esp_transport_handle_t t, int timeout_ms)
{
    if (t && t->_poll_read) {
        return t->_poll_read(t, timeout_ms);
    }
    return -1;
}

int esp_transport_poll_write(esp_transport_handle_t t, int timeout_ms)
{
    if (t && t->_poll_write) {
        return t->_poll_write(t, timeout_ms);
    }
    return -1;
}

int esp_transport_close(esp_transport_handle_t t)
{
    if (t && t->_close) {
        return t->_close(t);
    }
    return 0;
}

void *esp_transport_get_context_data(esp_transport_handle_t t)
{
    if (t) {
        return t->data;
    }
    return NULL;
}

esp_err_t esp_transport_set_context_data(esp_transport_handle_t t, void *data)
{
    if (t) {
        t->data = data;
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t esp_transport_set_func(esp_transport_handle_t t,
                             connect_func _connect,
                             io_read_func _read,
                             io_func _write,
                             trans_func _close,
                             poll_func _poll_read,
                             poll_func _poll_write,
                             trans_func _destroy)
{
    if (t == NULL) {
        return ESP_FAIL;
    }
    t->_connect = _connect;
    t->_read = _read;
    t->_write = _write;
    t->_close = _close;
    t->_poll_read = _poll_read;
    t->_poll_write = _poll_write;
    t->_destroy = _destroy;
    t->_connect_async = NULL;
    t->_parent_transfer = esp_transport_get_default_parent;
    return ESP_OK;
}

int esp_transport_get_default_port(esp_transport_handle_t t)
{
    if (t == NULL) {
        return -1;
    }
    return t->port;
}

esp_err_t esp_transport_set_default_port(esp_transport_handle_t t, int port)
{
    if (t == NULL) {
        return ESP_FAIL;
    }
    t->port = port;
    return ESP_OK;
}

esp_err_t esp_transport_set_async_connect_func(esp_transport_handle_t t, connect_async_func _connect_async_func)
{
    if (t == NULL) {
        return ESP_FAIL;
    }
    t->_connect_async = _connect_async_func;
    return ESP_OK;
}

esp_err_t esp_transport_set_parent_transport_func(esp_transport_handle_t t, payload_transfer_func _parent_transport)
{
    if (t == NULL) {
        return ESP_FAIL;
    }
    t->_parent_transfer = _parent_transport;
    return ESP_OK;
}

esp_tls_error_handle_t esp_transport_get_error_handle(esp_transport_handle_t t)
{
    if (t) {
        return &t->base->error_handle->esp_tls_err_h_base;
    }
    return NULL;
}

int esp_transport_get_errno(esp_transport_handle_t t)
{
    if (t && t->base && t->base->error_handle) {
        int actual_errno = t->base->error_handle->sock_errno;
        t->base->error_handle->sock_errno = 0;
        return actual_errno;
    }
    return -1;
}

void capture_tcp_transport_error(esp_transport_handle_t t, enum tcp_transport_errors error)
{
    esp_tls_last_error_t *err_handle = esp_transport_get_error_handle(t);
    switch (error) {
        case ERR_TCP_TRANSPORT_CONNECTION_TIMEOUT:
            err_handle->last_error = ESP_ERR_ESP_TLS_CONNECTION_TIMEOUT;
            break;
        case ERR_TCP_TRANSPORT_CANNOT_RESOLVE_HOSTNAME:
            err_handle->last_error = ESP_ERR_ESP_TLS_CANNOT_RESOLVE_HOSTNAME;
            break;
        case ERR_TCP_TRANSPORT_CONNECTION_CLOSED_BY_FIN:
            err_handle->last_error = ESP_ERR_ESP_TLS_TCP_CLOSED_FIN;
            break;
        case ERR_TCP_TRANSPORT_CONNECTION_FAILED:
            err_handle->last_error = ESP_ERR_ESP_TLS_FAILED_CONNECT_TO_HOST;
            break;
        case ERR_TCP_TRANSPORT_SETOPT_FAILED:
            err_handle->last_error = ESP_ERR_ESP_TLS_SOCKET_SETOPT_FAILED;
            break;
        case ERR_TCP_TRANSPORT_NO_MEM:
            err_handle->last_error = ESP_ERR_NO_MEM;
            break;
    }
}

void esp_transport_set_errors(esp_transport_handle_t t, const esp_tls_error_handle_t error_handle)
{
    if (t && t->base && t->base->error_handle) {
        memcpy(&t->base->error_handle->esp_tls_err_h_base, error_handle, sizeof(esp_tls_last_error_t));
        int sock_error;
        if (esp_tls_get_and_clear_error_type(error_handle, ESP_TLS_ERR_TYPE_SYSTEM, &sock_error) == ESP_OK) {
            t->base->error_handle->sock_errno = sock_error;
        }
    }
}

void esp_transport_capture_errno(esp_transport_handle_t t, int sock_errno)
{
    if (t && t->base && t->base->error_handle) {
        t->base->error_handle->sock_errno = sock_errno;
    }
}

int esp_transport_get_socket(esp_transport_handle_t t)
{
    if (t && t->_get_socket)  {
        return  t->_get_socket(t);
    }
    return -1;
}
