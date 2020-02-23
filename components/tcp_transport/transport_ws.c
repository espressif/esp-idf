#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/random.h>
#include "esp_log.h"
#include "esp_transport.h"
#include "esp_transport_tcp.h"
#include "esp_transport_ws.h"
#include "esp_transport_utils.h"
#include "mbedtls/base64.h"
#include "mbedtls/sha1.h"

static const char *TAG = "TRANSPORT_WS";

#define DEFAULT_WS_BUFFER (1024)
#define WS_FIN            0x80
#define WS_OPCODE_CONT    0x00
#define WS_OPCODE_TEXT    0x01
#define WS_OPCODE_BINARY  0x02
#define WS_OPCODE_CLOSE   0x08
#define WS_OPCODE_PING    0x09
#define WS_OPCODE_PONG    0x0a

// Second byte
#define WS_MASK           0x80
#define WS_SIZE16         126
#define WS_SIZE64         127
#define MAX_WEBSOCKET_HEADER_SIZE 16
#define WS_RESPONSE_OK    101


typedef struct {
    uint8_t opcode;
    char mask_key[4];                   /*!< Mask key for this payload */
    int payload_len;                    /*!< Total length of the payload */
    int bytes_remaining;                /*!< Bytes left to read of the payload  */
} ws_transport_frame_state_t;

typedef struct {
    char *path;
    char *buffer;
    char *sub_protocol;
    char *user_agent;
    char *headers;
    ws_transport_frame_state_t frame_state;
    esp_transport_handle_t parent;
} transport_ws_t;

static inline uint8_t ws_get_bin_opcode(ws_transport_opcodes_t opcode)
{
    return (uint8_t)opcode;
}

static esp_transport_handle_t ws_get_payload_transport_handle(esp_transport_handle_t t)
{
    transport_ws_t *ws = esp_transport_get_context_data(t);

    /* Reading parts of a frame directly will disrupt the WS internal frame state,
        reset bytes_remaining to prepare for reading a new frame */
    ws->frame_state.bytes_remaining = 0;

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
    char *found = strcasestr(buffer, key);
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

static int ws_connect(esp_transport_handle_t t, const char *host, int port, int timeout_ms)
{
    transport_ws_t *ws = esp_transport_get_context_data(t);
    if (esp_transport_connect(ws->parent, host, port, timeout_ms) < 0) {
        ESP_LOGE(TAG, "Error connecting to host %s:%d", host, port);
        return -1;
    }

    unsigned char random_key[16];
    getrandom(random_key, sizeof(random_key), 0);

    // Size of base64 coded string is equal '((input_size * 4) / 3) + (input_size / 96) + 6' including Z-term
    unsigned char client_key[28] = {0};

    const char *user_agent_ptr = (ws->user_agent)?(ws->user_agent):"ESP32 Websocket Client";

    size_t outlen = 0;
    mbedtls_base64_encode(client_key, sizeof(client_key), &outlen, random_key, sizeof(random_key));
    int len = snprintf(ws->buffer, DEFAULT_WS_BUFFER,
                         "GET %s HTTP/1.1\r\n"
                         "Connection: Upgrade\r\n"
                         "Host: %s:%d\r\n"
                         "User-Agent: %s\r\n"
                         "Upgrade: websocket\r\n"
                         "Sec-WebSocket-Version: 13\r\n"
                         "Sec-WebSocket-Key: %s\r\n",
                         ws->path,
                         host, port, user_agent_ptr,
                         client_key);
    if (len <= 0 || len >= DEFAULT_WS_BUFFER) {
        ESP_LOGE(TAG, "Error in request generation, %d", len);
        return -1;
    }
    if (ws->sub_protocol) {
        ESP_LOGD(TAG, "sub_protocol: %s", ws->sub_protocol);
        int r = snprintf(ws->buffer + len, DEFAULT_WS_BUFFER - len, "Sec-WebSocket-Protocol: %s\r\n", ws->sub_protocol);
        len += r;
        if (r <= 0 || len >= DEFAULT_WS_BUFFER) {
            ESP_LOGE(TAG, "Error in request generation"
                          "(snprintf of subprotocol returned %d, desired request len: %d, buffer size: %d", r, len, DEFAULT_WS_BUFFER);
            return -1;
        }
    }
    if (ws->headers) {
        ESP_LOGD(TAG, "headers: %s", ws->headers);
        int r = snprintf(ws->buffer + len, DEFAULT_WS_BUFFER - len, "%s", ws->headers);
        len += r;
        if (r <= 0 || len >= DEFAULT_WS_BUFFER) {
            ESP_LOGE(TAG, "Error in request generation"
                          "(strncpy of headers returned %d, desired request len: %d, buffer size: %d", r, len, DEFAULT_WS_BUFFER);
            return -1;
        }
    }
    int r = snprintf(ws->buffer + len, DEFAULT_WS_BUFFER - len, "\r\n");
    len += r;
    if (r <= 0 || len >= DEFAULT_WS_BUFFER) {
        ESP_LOGE(TAG, "Error in request generation"
                       "(snprintf of header terminal returned %d, desired request len: %d, buffer size: %d", r, len, DEFAULT_WS_BUFFER);
        return -1;
    }
    ESP_LOGD(TAG, "Write upgrate request\r\n%s", ws->buffer);
    if (esp_transport_write(ws->parent, ws->buffer, len, timeout_ms) <= 0) {
        ESP_LOGE(TAG, "Error write Upgrade header %s", ws->buffer);
        return -1;
    }
    int header_len = 0;
    do {
        if ((len = esp_transport_read(ws->parent, ws->buffer + header_len, DEFAULT_WS_BUFFER - header_len, timeout_ms)) <= 0) {
            ESP_LOGE(TAG, "Error read response for Upgrade header %s", ws->buffer);
            return -1;
        }
        header_len += len;
        ws->buffer[header_len] = '\0';
        ESP_LOGD(TAG, "Read header chunk %d, current header size: %d", len, header_len);
    } while (NULL == strstr(ws->buffer, "\r\n\r\n") && header_len < DEFAULT_WS_BUFFER);

    char *server_key = get_http_header(ws->buffer, "Sec-WebSocket-Accept:");
    if (server_key == NULL) {
        ESP_LOGE(TAG, "Sec-WebSocket-Accept not found");
        return -1;
    }

    // See mbedtls_sha1_ret() arg size
    unsigned char expected_server_sha1[20];
    // Size of base64 coded string see above
    unsigned char expected_server_key[33] = {0};
    // If you are interested, see https://tools.ietf.org/html/rfc6455
    const char expected_server_magic[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    unsigned char expected_server_text[sizeof(client_key) + sizeof(expected_server_magic) + 1];
    strcpy((char*)expected_server_text, (char*)client_key);
    strcat((char*)expected_server_text, expected_server_magic);

    size_t key_len = strlen((char*)expected_server_text);
    mbedtls_sha1_ret(expected_server_text, key_len, expected_server_sha1);
    mbedtls_base64_encode(expected_server_key, sizeof(expected_server_key),  &outlen, expected_server_sha1, sizeof(expected_server_sha1));
    expected_server_key[ (outlen < sizeof(expected_server_key)) ? outlen : (sizeof(expected_server_key) - 1) ] = 0;
    ESP_LOGD(TAG, "server key=%s, send_key=%s, expected_server_key=%s", (char *)server_key, (char*)client_key, expected_server_key);
    if (strcmp((char*)expected_server_key, (char*)server_key) != 0) {
        ESP_LOGE(TAG, "Invalid websocket key");
        return -1;
    }
    return 0;
}

static int _ws_write(esp_transport_handle_t t, int opcode, int mask_flag, const char *b, int len, int timeout_ms)
{
    transport_ws_t *ws = esp_transport_get_context_data(t);
    char *buffer = (char *)b;
    char ws_header[MAX_WEBSOCKET_HEADER_SIZE];
    char *mask;
    int header_len = 0, i;

    int poll_write;
    if ((poll_write = esp_transport_poll_write(ws->parent, timeout_ms)) <= 0) {
        ESP_LOGE(TAG, "Error transport_poll_write");
        return poll_write;
    }
    ws_header[header_len++] = opcode;

    if (len <= 125) {
        ws_header[header_len++] = (uint8_t)(len | mask_flag);
    } else if (len < 65536) {
        ws_header[header_len++] = WS_SIZE16 | mask_flag;
        ws_header[header_len++] = (uint8_t)(len >> 8);
        ws_header[header_len++] = (uint8_t)(len & 0xFF);
    } else {
        ws_header[header_len++] = WS_SIZE64 | mask_flag;
        /* Support maximum 4 bytes length */
        ws_header[header_len++] = 0; //(uint8_t)((len >> 56) & 0xFF);
        ws_header[header_len++] = 0; //(uint8_t)((len >> 48) & 0xFF);
        ws_header[header_len++] = 0; //(uint8_t)((len >> 40) & 0xFF);
        ws_header[header_len++] = 0; //(uint8_t)((len >> 32) & 0xFF);
        ws_header[header_len++] = (uint8_t)((len >> 24) & 0xFF);
        ws_header[header_len++] = (uint8_t)((len >> 16) & 0xFF);
        ws_header[header_len++] = (uint8_t)((len >> 8) & 0xFF);
        ws_header[header_len++] = (uint8_t)((len >> 0) & 0xFF);
    }

    if (mask_flag) {
        mask = &ws_header[header_len];
        getrandom(ws_header + header_len, 4, 0);
        header_len += 4;

        for (i = 0; i < len; ++i) {
            buffer[i] = (buffer[i] ^ mask[i % 4]);
        }
    }

    if (esp_transport_write(ws->parent, ws_header, header_len, timeout_ms) != header_len) {
        ESP_LOGE(TAG, "Error write header");
        return -1;
    }
    if (len == 0) {
        return 0;
    }

    int ret = esp_transport_write(ws->parent, buffer, len, timeout_ms);
    // in case of masked transport we have to revert back to the original data, as ws layer
    // does not create its own copy of data to be sent
    if (mask_flag) {
        mask = &ws_header[header_len-4];
        for (i = 0; i < len; ++i) {
            buffer[i] = (buffer[i] ^ mask[i % 4]);
        }
    }
    return ret;
}

int esp_transport_ws_send_raw(esp_transport_handle_t t, ws_transport_opcodes_t opcode, const char *b, int len, int timeout_ms)
{
    uint8_t op_code = ws_get_bin_opcode(opcode);
    if (t == NULL) {
        ESP_LOGE(TAG, "Transport must be a valid ws handle");
        return ESP_ERR_INVALID_ARG;
    }
    ESP_LOGD(TAG, "Sending raw ws message with opcode %d", op_code);
    return _ws_write(t, op_code | WS_FIN, WS_MASK, b, len, timeout_ms);
}

static int ws_write(esp_transport_handle_t t, const char *b, int len, int timeout_ms)
{
    if (len == 0) {
        // Default transport write of zero length in ws layer sends out a ping message.
        // This behaviour could however be altered in IDF 5.0, since a separate API for sending
        // messages with user defined opcodes has been introduced.
        ESP_LOGD(TAG, "Write PING message");
        return _ws_write(t, WS_OPCODE_PING | WS_FIN, WS_MASK, NULL, 0, timeout_ms);
    }
    return _ws_write(t, WS_OPCODE_BINARY | WS_FIN, WS_MASK, b, len, timeout_ms);
}


static int ws_read_payload(esp_transport_handle_t t, char *buffer, int len, int timeout_ms)
{
    transport_ws_t *ws = esp_transport_get_context_data(t);

    int bytes_to_read;
    int rlen = 0;

    if (ws->frame_state.bytes_remaining > len) {
        ESP_LOGD(TAG, "Actual data to receive (%d) are longer than ws buffer (%d)", ws->frame_state.bytes_remaining, len);
        bytes_to_read = len;

    } else {
        bytes_to_read = ws->frame_state.bytes_remaining;
    }

    // Receive and process payload
    if (bytes_to_read != 0 && (rlen = esp_transport_read(ws->parent, buffer, bytes_to_read, timeout_ms)) <= 0) {
        ESP_LOGE(TAG, "Error read data");
        return rlen;
    }
    ws->frame_state.bytes_remaining -= rlen;

    if (ws->frame_state.mask_key) {
        for (int i = 0; i < bytes_to_read; i++) {
            buffer[i] = (buffer[i] ^ ws->frame_state.mask_key[i % 4]);
        }
    }
    return rlen;
}


/* Read and parse the WS header, determine length of payload */
static int ws_read_header(esp_transport_handle_t t, char *buffer, int len, int timeout_ms)
{
    transport_ws_t *ws = esp_transport_get_context_data(t);
    int payload_len;

    char ws_header[MAX_WEBSOCKET_HEADER_SIZE];
    char *data_ptr = ws_header, mask;
    int rlen;
    int poll_read;
    if ((poll_read = esp_transport_poll_read(ws->parent, timeout_ms)) <= 0) {
        return poll_read;
    }

    // Receive and process header first (based on header size)
    int header = 2;
    int mask_len = 4;
    if ((rlen = esp_transport_read(ws->parent, data_ptr, header, timeout_ms)) <= 0) {
        ESP_LOGE(TAG, "Error read data");
        return rlen;
    }
    ws->frame_state.opcode = (*data_ptr & 0x0F);
    data_ptr ++;
    mask = ((*data_ptr >> 7) & 0x01);
    payload_len = (*data_ptr & 0x7F);
    data_ptr++;
    ESP_LOGD(TAG, "Opcode: %d, mask: %d, len: %d\r\n", ws->frame_state.opcode, mask, payload_len);
    if (payload_len == 126) {
        // headerLen += 2;
        if ((rlen = esp_transport_read(ws->parent, data_ptr, header, timeout_ms)) <= 0) {
            ESP_LOGE(TAG, "Error read data");
            return rlen;
        }
        payload_len = data_ptr[0] << 8 | data_ptr[1];
    } else if (payload_len == 127) {
        // headerLen += 8;
        header = 8;
        if ((rlen = esp_transport_read(ws->parent, data_ptr, header, timeout_ms)) <= 0) {
            ESP_LOGE(TAG, "Error read data");
            return rlen;
        }

        if (data_ptr[0] != 0 || data_ptr[1] != 0 || data_ptr[2] != 0 || data_ptr[3] != 0) {
            // really too big!
            payload_len = 0xFFFFFFFF;
        } else {
            payload_len = data_ptr[4] << 24 | data_ptr[5] << 16 | data_ptr[6] << 8 | data_ptr[7];
        }
    }

    if (mask) {
        // Read and store mask
        if (payload_len != 0 && (rlen = esp_transport_read(ws->parent, buffer, mask_len, timeout_ms)) <= 0) {
            ESP_LOGE(TAG, "Error read data");
            return rlen;
        }
        memcpy(ws->frame_state.mask_key, buffer, mask_len);
    } else {
        memset(ws->frame_state.mask_key, 0, mask_len);
    }

    ws->frame_state.payload_len = payload_len;
    ws->frame_state.bytes_remaining = payload_len;

    return payload_len;
}

static int ws_read(esp_transport_handle_t t, char *buffer, int len, int timeout_ms)
{
    int rlen = 0;
    transport_ws_t *ws = esp_transport_get_context_data(t);

    // If message exceeds buffer len then subsequent reads will skip reading header and read whatever is left of the payload
    if (ws->frame_state.bytes_remaining <= 0) {
        if ( (rlen = ws_read_header(t, buffer, len, timeout_ms)) <= 0) {
            // If something when wrong then we prepare for reading a new header
            ws->frame_state.bytes_remaining = 0;
            return rlen;
        }
    }
    if (ws->frame_state.payload_len) {
        if ( (rlen = ws_read_payload(t, buffer, len, timeout_ms)) <= 0) {
            ESP_LOGE(TAG, "Error reading payload data");
            ws->frame_state.bytes_remaining = 0;
            return rlen;
        }
    }

    return rlen;
}


static int ws_poll_read(esp_transport_handle_t t, int timeout_ms)
{
    transport_ws_t *ws = esp_transport_get_context_data(t);
    return esp_transport_poll_read(ws->parent, timeout_ms);
}

static int ws_poll_write(esp_transport_handle_t t, int timeout_ms)
{
    transport_ws_t *ws = esp_transport_get_context_data(t);
    return esp_transport_poll_write(ws->parent, timeout_ms);;
}

static int ws_close(esp_transport_handle_t t)
{
    transport_ws_t *ws = esp_transport_get_context_data(t);
    return esp_transport_close(ws->parent);
}

static esp_err_t ws_destroy(esp_transport_handle_t t)
{
    transport_ws_t *ws = esp_transport_get_context_data(t);
    free(ws->buffer);
    free(ws->path);
    free(ws->sub_protocol);
    free(ws->user_agent);
    free(ws->headers);
    free(ws);
    return 0;
}
void esp_transport_ws_set_path(esp_transport_handle_t t, const char *path)
{
    transport_ws_t *ws = esp_transport_get_context_data(t);
    ws->path = realloc(ws->path, strlen(path) + 1);
    strcpy(ws->path, path);
}

esp_transport_handle_t esp_transport_ws_init(esp_transport_handle_t parent_handle)
{
    esp_transport_handle_t t = esp_transport_init();
    transport_ws_t *ws = calloc(1, sizeof(transport_ws_t));
    ESP_TRANSPORT_MEM_CHECK(TAG, ws, return NULL);
    ws->parent = parent_handle;

    ws->path = strdup("/");
    ESP_TRANSPORT_MEM_CHECK(TAG, ws->path, {
        free(ws);
        return NULL;
    });
    ws->buffer = malloc(DEFAULT_WS_BUFFER);
    ESP_TRANSPORT_MEM_CHECK(TAG, ws->buffer, {
        free(ws->path);
        free(ws);
        return NULL;
    });

    esp_transport_set_func(t, ws_connect, ws_read, ws_write, ws_close, ws_poll_read, ws_poll_write, ws_destroy);
    // webocket underlying transfer is the payload transfer handle
    esp_transport_set_parent_transport_func(t, ws_get_payload_transport_handle);

    esp_transport_set_context_data(t, ws);
    return t;
}

esp_err_t esp_transport_ws_set_subprotocol(esp_transport_handle_t t, const char *sub_protocol)
{
    if (t == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    transport_ws_t *ws = esp_transport_get_context_data(t);
    if (ws->sub_protocol) {
        free(ws->sub_protocol);
    }
    if (sub_protocol == NULL) {
        ws->sub_protocol = NULL;
        return ESP_OK;
    }
    ws->sub_protocol = strdup(sub_protocol);
    if (ws->sub_protocol == NULL) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t esp_transport_ws_set_user_agent(esp_transport_handle_t t, const char *user_agent)
{
    if (t == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    transport_ws_t *ws = esp_transport_get_context_data(t);
    if (ws->user_agent) {
        free(ws->user_agent);
    }
    if (user_agent == NULL) {
        ws->user_agent = NULL;
        return ESP_OK;
    }
    ws->user_agent = strdup(user_agent);
    if (ws->user_agent == NULL) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t esp_transport_ws_set_headers(esp_transport_handle_t t, const char *headers)
{
    if (t == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    transport_ws_t *ws = esp_transport_get_context_data(t);
    if (ws->headers) {
        free(ws->headers);
    }
    if (headers == NULL) {
        ws->headers = NULL;
        return ESP_OK;
    }
    ws->headers = strdup(headers);
    if (ws->headers == NULL) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

ws_transport_opcodes_t esp_transport_ws_get_read_opcode(esp_transport_handle_t t)
{
    transport_ws_t *ws = esp_transport_get_context_data(t);
    return ws->frame_state.opcode;
}

int esp_transport_ws_get_read_payload_len(esp_transport_handle_t t)
{
    transport_ws_t *ws = esp_transport_get_context_data(t);
    return ws->frame_state.payload_len;
}


