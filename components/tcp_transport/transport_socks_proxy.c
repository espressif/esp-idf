/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_check.h"
#include "esp_transport_socks_proxy.h"
#include "netdb.h"
#include "string.h"

#include "esp_log.h"
#include "include/esp_transport.h"

static const char *TAG = "transport_proxy";
static const uint32_t SOCKS4_FIX_MESSAGE_SIZE = 8;
#define SOCKS4_RESPONSE_SIZE 8

#define SOCKS_ERROR_IF(cond, err_code, format, ...) do {\
 ESP_GOTO_ON_FALSE(!(cond), err_code, Error, TAG, format , ##__VA_ARGS__);\
}while(0)

typedef enum {CONNECT = 0x01, BIND = 0x02} command_socks_t;

typedef struct {
    union {
        char *user_id;
    };
} socks_authentication_data_t;

typedef struct transport_socks_t {
    socks_version_t version;
    char *proxy_address;
    uint16_t proxy_port;
    socks_authentication_data_t authentication;
    esp_transport_handle_t parent;
} transport_socks_t;

typedef struct __attribute((packed)) socks_request {
    uint8_t version;
    uint8_t command;
    uint16_t destination_port;
    uint32_t destination_ip;
} socks_request_t;

typedef struct __attribute((packed)) socks_response {
    uint8_t version;
    uint8_t code;
    uint16_t destination_port;
    uint32_t destination_ip;
} socks_response_t;

static uint32_t get_IP(const char *const host)
{
    struct addrinfo *address_info;
    struct addrinfo hints;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int res = getaddrinfo(host, NULL, &hints, &address_info);
    if (res) {
        // IP_ADDR_ANY equivalent returned as error here because it doesn't make
        // sense to connect to ANY
        return 0;
    }
    uint32_t ip = ((struct sockaddr_in *)address_info->ai_addr)->sin_addr.s_addr;
    freeaddrinfo(address_info);
    return ip;
}

static int64_t get_tick(void)
{
    return esp_timer_get_time() / (int64_t)1000;
}

static int socks_connect(esp_transport_handle_t transport, const char *const host, int port, int timeout_ms)
{
    transport_socks_t *socks_transport = esp_transport_get_context_data(transport);
    int64_t initial_tick = get_tick();
    int64_t remaining_time = timeout_ms;
    int ret = 0;
    uint32_t request_message_len = SOCKS4_FIX_MESSAGE_SIZE + 1;
    char *request_message = NULL;

    int proxy_connected  = esp_transport_connect(socks_transport->parent, socks_transport->proxy_address, socks_transport->proxy_port, remaining_time);
    SOCKS_ERROR_IF(proxy_connected == -1, esp_transport_get_errno(socks_transport->parent), "Error connecting to proxy");

    if (remaining_time > -1) {
        remaining_time = (int64_t)timeout_ms - (get_tick() - initial_tick);
        SOCKS_ERROR_IF(remaining_time < 0, SOCKS_TIMEOUT, "Connection Timeout");
    }

    socks_request_t request = {};
    request.version = socks_transport->version;
    request.command = CONNECT;
    request.destination_port = htons(port);
    request.destination_ip = get_IP(host);
    SOCKS_ERROR_IF(request.destination_ip == 0, SOCKS_RESPONSE_TARGET_NOT_FOUND, "Failed to resolve target address");

    if (socks_transport->authentication.user_id) {
        request_message_len += strlen(socks_transport->authentication.user_id);
        request_message = calloc(request_message_len, sizeof(char));
        if (request_message) {
            strcpy(request_message + sizeof(socks_request_t) +1, socks_transport->authentication.user_id);
        }
    } else {
        request_message = calloc(request_message_len, sizeof(char));
    }
    SOCKS_ERROR_IF(request_message == NULL, ESP_ERR_NO_MEM, "Failed to allocate request message");
    memcpy(request_message, (char *)&request, sizeof(socks_request_t));

    SOCKS_ERROR_IF(esp_transport_write(socks_transport->parent, request_message, request_message_len, remaining_time) < 0, esp_transport_get_errno(socks_transport->parent), "Failed to write the request message");

    if (remaining_time > -1) {
        remaining_time = (int64_t)timeout_ms - (get_tick() - initial_tick);
        SOCKS_ERROR_IF(remaining_time < 0, SOCKS_TIMEOUT, "Connection Timeout");
    }

    char proxy_response[SOCKS4_RESPONSE_SIZE];
    SOCKS_ERROR_IF(esp_transport_read(socks_transport->parent, proxy_response, SOCKS4_RESPONSE_SIZE, remaining_time) < 0, esp_transport_get_errno(socks_transport->parent), "Failed to get a response");

    socks_response_t *response = (socks_response_t *)proxy_response;
    SOCKS_ERROR_IF(response->code != SOCKS_RESPONSE_SUCCESS, response->code, "Request Rejected with : %02x", response->code);
    free(request_message);
    return 0;
Error:
    free(request_message);
    errno = ret;
    return -1;
}

static int socks_close(esp_transport_handle_t transport)
{
    transport_socks_t *socks_transport = esp_transport_get_context_data(transport);
    return esp_transport_close(socks_transport->parent);

}

static int socks_write(esp_transport_handle_t transport, const char *buffer, int len, int timeout_ms)
{
    transport_socks_t *socks_transport = esp_transport_get_context_data(transport);
    return esp_transport_write(socks_transport->parent, buffer, len, timeout_ms);
}

static int socks_read(esp_transport_handle_t transport, char *buffer, int len, int timeout_ms)
{
    transport_socks_t *socks_transport = esp_transport_get_context_data(transport);
    return esp_transport_read(socks_transport->parent, buffer, len, timeout_ms);
}

static int socks_poll_read(esp_transport_handle_t transport, int timeout_ms)
{
    transport_socks_t *socks_transport = esp_transport_get_context_data(transport);
    return esp_transport_poll_read(socks_transport->parent, timeout_ms);
}

static int socks_poll_write(esp_transport_handle_t transport, int timeout_ms)
{
    transport_socks_t *socks_transport = esp_transport_get_context_data(transport);
    return esp_transport_poll_write(socks_transport->parent, timeout_ms);
}

static esp_err_t socks_destroy(esp_transport_handle_t transport)
{
    if (transport == NULL) {
        return ESP_OK;
    }
    transport_socks_t *socks_transport = esp_transport_get_context_data(transport);
    if (socks_transport == NULL) {
        return ESP_FAIL;
    }
    free(socks_transport->proxy_address);
    free(socks_transport);

    return ESP_OK;

}
static esp_err_t check_parameters(esp_transport_handle_t parent_handle, const esp_transport_socks_proxy_config_t *config)
{
    if (parent_handle == NULL) {
        ESP_LOGE(TAG, "Parent transport is invalid");
        return ESP_FAIL;
    }

    if (config == NULL || config->address == NULL) {
        ESP_LOGE(TAG, "Invalid Configuration");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_transport_handle_t esp_transport_socks_proxy_init(esp_transport_handle_t parent_handle, const esp_transport_socks_proxy_config_t *config)
{
    if (check_parameters(parent_handle, config) == ESP_FAIL) {
        return NULL;
    };

    int ret = 0;

    esp_transport_handle_t transport = esp_transport_init();
    SOCKS_ERROR_IF(transport == NULL, ESP_ERR_NO_MEM,  "Failed to allocate transport");

    transport_socks_t *socks_context = calloc(1, sizeof(transport_socks_t));
    SOCKS_ERROR_IF(socks_context == NULL, ESP_ERR_NO_MEM,  "Failed to allocate transport context");
    esp_transport_set_context_data(transport, socks_context);
    esp_transport_set_func(transport, socks_connect, socks_read, socks_write, socks_close, socks_poll_read, socks_poll_write, socks_destroy);

    socks_context->parent = parent_handle;
    socks_context->proxy_address = strdup(config->address);
    SOCKS_ERROR_IF(socks_context->proxy_address == NULL, ESP_ERR_NO_MEM, "Failed to copy proxy address");
    socks_context->proxy_port = config->port;
    socks_context->version = config->version;


    return transport;
Error:
    esp_transport_destroy(transport);
    errno = ret;
    return NULL;
}
