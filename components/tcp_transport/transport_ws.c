#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "esp_log.h"
#include "esp_transport.h"
#include "esp_transport_tcp.h"
#include "esp_transport_ws.h"
#include "esp_transport_utils.h"
#include "mbedtls/base64.h"
#include "mbedtls/sha1.h"

static const char *TAG = "TRANSPORT_WS";

#define DEFAULT_WS_BUFFER (1024)

typedef struct {
    char *path;
    char *buffer;
    transport_handle_t parent;
} transport_ws_t;

transport_handle_t ws_transport_get_payload_transport_handle(transport_handle_t t)
{
    transport_ws_t *ws = transport_get_context_data(t);
    return ws->parent;
}

static char *trimwhitespace(const char *str)
{
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) {
        return (char *)str;
    }

    // Trim trailing space
    end = (char *)(str + strlen(str) - 1);
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = 0;

    return (char *)str;
}


static char *get_http_header(const char *buffer, const char *key)
{
    char *found = strstr(buffer, key);
    if (found) {
        found += strlen(key);
        char *found_end = strstr(found, "\r\n");
        if (found_end) {
            found_end[0] = 0;//terminal string

            return trimwhitespace(found);
        }
    }
    return NULL;
}

static int ws_connect(transport_handle_t t, const char *host, int port, int timeout_ms)
{
    transport_ws_t *ws = transport_get_context_data(t);
    if (transport_connect(ws->parent, host, port, timeout_ms) < 0) {
        ESP_LOGE(TAG, "Error connect to ther server");
    }
    unsigned char random_key[16] = { 0 }, client_key[32] = {0};
    int i;
    for (i = 0; i < sizeof(random_key); i++) {
        random_key[i] = rand() & 0xFF;
    }
    size_t outlen = 0;
    mbedtls_base64_encode(client_key, 32,  &outlen, random_key, 16);
    int len =   snprintf(ws->buffer, DEFAULT_WS_BUFFER,
                         "GET %s HTTP/1.1\r\n"
                         "Connection: Upgrade\r\n"
                         "Host: %s:%d\r\n"
                         "Upgrade: websocket\r\n"
                         "Sec-WebSocket-Version: 13\r\n"
                         "Sec-WebSocket-Protocol: mqtt\r\n"
                         "Sec-WebSocket-Key: %s\r\n"
                         "User-Agent: ESP32 MQTT Client\r\n\r\n",
                         ws->path,
                         host, port,
                         client_key);
    ESP_LOGD(TAG, "Write upgrate request\r\n%s", ws->buffer);
    if (transport_write(ws->parent, ws->buffer, len, timeout_ms) <= 0) {
        ESP_LOGE(TAG, "Error write Upgrade header %s", ws->buffer);
        return -1;
    }
    if ((len = transport_read(ws->parent, ws->buffer, DEFAULT_WS_BUFFER, timeout_ms)) <= 0) {
        ESP_LOGE(TAG, "Error read response for Upgrade header %s", ws->buffer);
        return -1;
    }
    char *server_key = get_http_header(ws->buffer, "Sec-WebSocket-Accept:");
    if (server_key == NULL) {
        ESP_LOGE(TAG, "Sec-WebSocket-Accept not found");
        return -1;
    }

    unsigned char client_key_b64[64], valid_client_key[20], accept_key[32] = {0};
    int key_len = sprintf((char*)client_key_b64, "%s258EAFA5-E914-47DA-95CA-C5AB0DC85B11", (char*)client_key);
    mbedtls_sha1_ret(client_key_b64, (size_t)key_len, valid_client_key);
    mbedtls_base64_encode(accept_key, 32,  &outlen, valid_client_key, 20);
    accept_key[outlen] = 0;
    ESP_LOGD(TAG, "server key=%s, send_key=%s, accept_key=%s", (char *)server_key, (char*)client_key, accept_key);
    if (strcmp((char*)accept_key, (char*)server_key) != 0) {
        ESP_LOGE(TAG, "Invalid websocket key");
        return -1;
    }
    return 0;
}

static int ws_write(transport_handle_t t, const char *buff, int len, int timeout_ms)
{
    transport_ws_t *ws = transport_get_context_data(t);
    char ws_header[MAX_WEBSOCKET_HEADER_SIZE];
    char *mask;
    int header_len = 0, i;
    char *buffer = (char *)buff;
    int poll_write;
    if ((poll_write = transport_poll_write(ws->parent, timeout_ms)) <= 0) {
        return poll_write;
    }

    ws_header[header_len++] = WS_OPCODE_BINARY | WS_FIN;

    // NOTE: no support for > 16-bit sized messages
    if (len > 125) {
        ws_header[header_len++] = WS_SIZE16 | WS_MASK;
        ws_header[header_len++] = (uint8_t)(len >> 8);
        ws_header[header_len++] = (uint8_t)(len & 0xFF);
    } else {
        ws_header[header_len++] = (uint8_t)(len | WS_MASK);
    }
    mask = &ws_header[header_len];
    ws_header[header_len++] = rand() & 0xFF;
    ws_header[header_len++] = rand() & 0xFF;
    ws_header[header_len++] = rand() & 0xFF;
    ws_header[header_len++] = rand() & 0xFF;

    for (i = 0; i < len; ++i) {
        buffer[i] = (buffer[i] ^ mask[i % 4]);
    }
    if (transport_write(ws->parent, ws_header, header_len, timeout_ms) != header_len) {
        ESP_LOGE(TAG, "Error write header");
        return -1;
    }
    return transport_write(ws->parent, buffer, len, timeout_ms);
}

static int ws_read(transport_handle_t t, char *buffer, int len, int timeout_ms)
{
    transport_ws_t *ws = transport_get_context_data(t);
    int payload_len;
    int payload_len_buff = len;
    char *data_ptr = buffer, opcode, mask, *mask_key = NULL;
    int rlen;
    int poll_read;
    if ((poll_read = transport_poll_read(ws->parent, timeout_ms)) <= 0) {
        return poll_read;
    }
    if ((rlen = transport_read(ws->parent, buffer, len, timeout_ms)) <= 0) {
        ESP_LOGE(TAG, "Error read data");
        return rlen;
    }
    opcode = (*data_ptr & 0x0F);
    data_ptr ++;
    mask = ((*data_ptr >> 7) & 0x01);
    payload_len = (*data_ptr & 0x7F);
    data_ptr++;
    ESP_LOGD(TAG, "Opcode: %d, mask: %d, len: %d\r\n", opcode, mask, payload_len);
    if (payload_len == 126) {
        // headerLen += 2;
        payload_len = data_ptr[0] << 8 | data_ptr[1];
        payload_len_buff = len - 4;
        data_ptr += 2;
    } else if (payload_len == 127) {
        // headerLen += 8;

        if (data_ptr[0] != 0 || data_ptr[1] != 0 || data_ptr[2] != 0 || data_ptr[3] != 0) {
            // really too big!
            payload_len = 0xFFFFFFFF;
        } else {
            payload_len = data_ptr[4] << 24 | data_ptr[5] << 16 | data_ptr[6] << 8 | data_ptr[7];
        }
        data_ptr += 8;
        payload_len_buff = len - 10;
    }
    if (payload_len > payload_len_buff) {
        ESP_LOGD(TAG, "Actual data received (%d) are longer than mqtt buffer (%d)", payload_len, payload_len_buff);
        payload_len = payload_len_buff;
    }

    if (mask) {
        mask_key = data_ptr;
        data_ptr += 4;
        for (int i = 0; i < payload_len; i++) {
            buffer[i] = (data_ptr[i] ^ mask_key[i % 4]);
        }
    } else {
        memmove(buffer, data_ptr, payload_len);
    }
    return payload_len;
}

static int ws_poll_read(transport_handle_t t, int timeout_ms)
{
    transport_ws_t *ws = transport_get_context_data(t);
    return transport_poll_read(ws->parent, timeout_ms);
}

static int ws_poll_write(transport_handle_t t, int timeout_ms)
{
    transport_ws_t *ws = transport_get_context_data(t);
    return transport_poll_write(ws->parent, timeout_ms);;
}

static int ws_close(transport_handle_t t)
{
    transport_ws_t *ws = transport_get_context_data(t);
    return transport_close(ws->parent);
}

static esp_err_t ws_destroy(transport_handle_t t)
{
    transport_ws_t *ws = transport_get_context_data(t);
    free(ws->buffer);
    free(ws->path);
    free(ws);
    return 0;
}
void transport_ws_set_path(transport_handle_t t, const char *path)
{
    transport_ws_t *ws = transport_get_context_data(t);
    ws->path = realloc(ws->path, strlen(path) + 1);
    strcpy(ws->path, path);
}
transport_handle_t transport_ws_init(transport_handle_t parent_handle)
{
    transport_handle_t t = transport_init();
    transport_ws_t *ws = calloc(1, sizeof(transport_ws_t));
    TRANSPORT_MEM_CHECK(TAG, ws, return NULL);
    ws->parent = parent_handle;

    ws->path = strdup("/");
    TRANSPORT_MEM_CHECK(TAG, ws->path, return NULL);
    ws->buffer = malloc(DEFAULT_WS_BUFFER);
    TRANSPORT_MEM_CHECK(TAG, ws->buffer, {
        free(ws->path);
        free(ws);
        return NULL;
    });

    transport_set_func(t, ws_connect, ws_read, ws_write, ws_close, ws_poll_read, ws_poll_write, ws_destroy, ws_transport_get_payload_transport_handle);
    transport_set_context_data(t, ws);
    return t;
}

