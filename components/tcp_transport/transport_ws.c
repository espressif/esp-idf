/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/random.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "esp_log.h"
#include "esp_transport.h"
#include "esp_transport_tcp.h"
#include "esp_transport_ws.h"
#include "esp_transport_internal.h"
#include "errno.h"
#include "esp_tls_crypto.h"
#include <arpa/inet.h>

static const char *TAG = "transport_ws";

#define WS_BUFFER_SIZE              CONFIG_WS_BUFFER_SIZE
#define WS_FIN                      0x80
#define WS_OPCODE_CONT              0x00
#define WS_OPCODE_TEXT              0x01
#define WS_OPCODE_BINARY            0x02
#define WS_OPCODE_CLOSE             0x08
#define WS_OPCODE_PING              0x09
#define WS_OPCODE_PONG              0x0a
#define WS_OPCODE_CONTROL_FRAME     0x08

// Second byte
#define WS_MASK                     0x80
#define WS_SIZE16                   126
#define WS_SIZE64                   127
#define MAX_WEBSOCKET_HEADER_SIZE   16
#define WS_RESPONSE_OK              101
#define WS_TRANSPORT_MAX_CONTROL_FRAME_BUFFER_LEN 125


typedef struct {
    uint8_t opcode;
    bool fin;                           /*!< Frame fin flag, for continuations */
    char mask_key[4];                   /*!< Mask key for this payload */
    int payload_len;                    /*!< Total length of the payload */
    int bytes_remaining;                /*!< Bytes left to read of the payload  */
    bool header_received;               /*!< Flag to indicate that a new message header was received */
} ws_transport_frame_state_t;

typedef struct {
    char *path;
    char *sub_protocol;
    char *user_agent;
    char *headers;
    char *auth;
    char *buffer;             /*!< Initial HTTP connection buffer, which may include data beyond the handshake headers, such as the next WebSocket packet*/
    size_t buffer_len;        /*!< The buffer length */
    int http_status_code;
    bool propagate_control_frames;
    ws_transport_frame_state_t frame_state;
    esp_transport_handle_t parent;
} transport_ws_t;

/**
 * @brief               Handles control frames
 *
 * This API is used internally to handle control frames at the transport layer.
 * The API could be possibly promoted to a public API if needed by some clients
 *
 * @param t             Websocket transport handle
 * @param buffer        Buffer with the actual payload of the control packet to be processed
 * @param len           Length of the buffer (typically the same as the payload buffer)
 * @param timeout_ms    The timeout milliseconds
 * @param client_closed To indicate that the connection has been closed by the client
*                       (to prevent echoing the CLOSE packet if true, as this is the actual echo from the server)
 *
 * @return
 *      0 - no activity, or successfully responded to PING
 *      -1 - Failure: Error on read or the actual payload longer then buffer
 *      1 - Close handshake success
 *      2 - Got PONG message
 */

static int esp_transport_ws_handle_control_frames(esp_transport_handle_t t, char *buffer, int len, int timeout_ms, bool client_closed);

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

static int esp_transport_read_internal(transport_ws_t *ws, char *buffer, int len, int timeout_ms)
{
    // No buffered data to read from, directly attempt to read from the transport.
    if (ws->buffer_len == 0) {
        return esp_transport_read(ws->parent, buffer, len, timeout_ms);
    }

    // At this point, buffer_len is guaranteed to be > 0.
    int to_read = (ws->buffer_len >= len) ? len : ws->buffer_len;

    // Copy the available or requested data to the buffer.
    memcpy(buffer, ws->buffer, to_read);

    if (to_read < ws->buffer_len) {
        // Shift remaining data if not all was read.
        memmove(ws->buffer, ws->buffer + to_read, ws->buffer_len - to_read);
        ws->buffer_len -= to_read;
    } else {
        // All buffer data was consumed.
#ifdef CONFIG_WS_DYNAMIC_BUFFER
        free(ws->buffer);
        ws->buffer = NULL;
#endif
        ws->buffer_len = 0;
    }

    return to_read;
}

static char *trimwhitespace(const char *str)
{
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == 0) {
        return (char *)str;
    }

    // Trim trailing space
    end = (char *)(str + strlen(str) - 1);
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }

    // Write new null terminator
    *(end + 1) = 0;

    return (char *)str;
}

static int get_http_status_code(const char *buffer)
{
    const char http[] = "HTTP/";
    const char *found = strcasestr(buffer, http);
    char status_code[4];
    if (found) {
        found += sizeof(http)/sizeof(http[0]) - 1;
        found = strchr(found, ' ');
        if (found) {
            found++;
            strncpy(status_code, found, 4);
            status_code[3] = '\0';
            int code = atoi(status_code);
            ESP_LOGD(TAG, "HTTP status code is %d", code);
            return code == 0 ? -1 : code;
        }
    }
    return -1;
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
    const char delimiter[] = "\r\n\r\n";

    if (esp_transport_connect(ws->parent, host, port, timeout_ms) < 0) {
        ESP_LOGE(TAG, "Error connecting to host %s:%d", host, port);
        return -1;
    }

    unsigned char random_key[16];
    ssize_t rc;
    if ((rc = getrandom(random_key, sizeof(random_key), 0)) < 0) {
        ESP_LOGD(TAG, "getrandom() returned %zd", rc);
        return -1;
    }

    // Size of base64 coded string is equal '((input_size * 4) / 3) + (input_size / 96) + 6' including Z-term
    unsigned char client_key[28] = {0};

    const char *user_agent_ptr = (ws->user_agent) ? (ws->user_agent) : "ESP32 Websocket Client";
#ifdef CONFIG_WS_DYNAMIC_BUFFER
    if (!ws->buffer) {
        ws->buffer = malloc(WS_BUFFER_SIZE);
        if (!ws->buffer) {
            ESP_LOGE(TAG, "Cannot allocate buffer for connect, need-%d", WS_BUFFER_SIZE);
            return -1;
        }
    }
#endif

    size_t outlen = 0;
    esp_crypto_base64_encode(client_key, sizeof(client_key), &outlen, random_key, sizeof(random_key));
    int len = snprintf(ws->buffer, WS_BUFFER_SIZE,
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
    if (len <= 0 || len >= WS_BUFFER_SIZE) {
        ESP_LOGE(TAG, "Error in request generation, desired request len: %d, buffer size: %d", len, WS_BUFFER_SIZE);
        return -1;
    }
    if (ws->sub_protocol) {
        ESP_LOGD(TAG, "sub_protocol: %s", ws->sub_protocol);
        int r = snprintf(ws->buffer + len, WS_BUFFER_SIZE - len, "Sec-WebSocket-Protocol: %s\r\n", ws->sub_protocol);
        len += r;
        if (r <= 0 || len >= WS_BUFFER_SIZE) {
            ESP_LOGE(TAG, "Error in request generation"
                     "(snprintf of subprotocol returned %d, desired request len: %d, buffer size: %d", r, len, WS_BUFFER_SIZE);
            return -1;
        }
    }
    if (ws->auth) {
        ESP_LOGD(TAG, "Authorization: %s", ws->auth);
        int r = snprintf(ws->buffer + len, WS_BUFFER_SIZE - len, "Authorization: %s\r\n", ws->auth);
        len += r;
        if (r <= 0 || len >= WS_BUFFER_SIZE) {
            ESP_LOGE(TAG, "Error in request generation"
                     "(snprintf of authorization returned %d, desired request len: %d, buffer size: %d", r, len, WS_BUFFER_SIZE);
            return -1;
        }
    }
    if (ws->headers) {
        ESP_LOGD(TAG, "headers: %s", ws->headers);
        int r = snprintf(ws->buffer + len, WS_BUFFER_SIZE - len, "%s", ws->headers);
        len += r;
        if (r <= 0 || len >= WS_BUFFER_SIZE) {
            ESP_LOGE(TAG, "Error in request generation"
                     "(strncpy of headers returned %d, desired request len: %d, buffer size: %d", r, len, WS_BUFFER_SIZE);
            return -1;
        }
    }
    int r = snprintf(ws->buffer + len, WS_BUFFER_SIZE - len, "\r\n");
    len += r;
    if (r <= 0 || len >= WS_BUFFER_SIZE) {
        ESP_LOGE(TAG, "Error in request generation"
                 "(snprintf of header terminal returned %d, desired request len: %d, buffer size: %d", r, len, WS_BUFFER_SIZE);
        return -1;
    }
    ESP_LOGD(TAG, "Write upgrade request\r\n%s", ws->buffer);
    if (esp_transport_write(ws->parent, ws->buffer, len, timeout_ms) <= 0) {
        ESP_LOGE(TAG, "Error write Upgrade header %s", ws->buffer);
        return -1;
    }
    int header_len = 0;
    do {
        if ((len = esp_transport_read(ws->parent, ws->buffer + header_len, WS_BUFFER_SIZE - header_len, timeout_ms)) <= 0) {
            ESP_LOGE(TAG, "Error read response for Upgrade header %s", ws->buffer);
            return -1;
        }
        header_len += len;
        ws->buffer_len = header_len;
        ws->buffer[header_len] = '\0'; // We will mark the end of the header to ensure that strstr operations for parsing the headers don't fail.
        ESP_LOGD(TAG, "Read header chunk %d, current header size: %d", len, header_len);
    } while (NULL == strstr(ws->buffer, delimiter) && header_len < WS_BUFFER_SIZE);

    char* delim_ptr = strstr(ws->buffer, delimiter);

    ws->http_status_code = get_http_status_code(ws->buffer);
    if (ws->http_status_code == -1) {
        ESP_LOGE(TAG, "HTTP upgrade failed");
        return -1;
    }

    char *server_key = get_http_header(ws->buffer, "Sec-WebSocket-Accept:");
    if (server_key == NULL) {
        ESP_LOGE(TAG, "Sec-WebSocket-Accept not found");
        return -1;
    }

    if (delim_ptr != NULL) {
        size_t delim_pos = delim_ptr - ws->buffer + sizeof(delimiter) - 1;
        size_t remaining_len = ws->buffer_len - delim_pos;
        if (remaining_len > 0) {
            memmove(ws->buffer, ws->buffer + delim_pos, remaining_len);
            ws->buffer_len = remaining_len;
        } else {
#ifdef CONFIG_WS_DYNAMIC_BUFFER
            free(ws->buffer);
            ws->buffer = NULL;
#endif
            ws->buffer_len = 0;
        }
    }
    // See esp_crypto_sha1() arg size
    unsigned char expected_server_sha1[20];
    // Size of base64 coded string see above
    unsigned char expected_server_key[33] = {0};
    // If you are interested, see https://tools.ietf.org/html/rfc6455
    const char expected_server_magic[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    unsigned char expected_server_text[sizeof(client_key) + sizeof(expected_server_magic) + 1];
    strcpy((char *)expected_server_text, (char *)client_key);
    strcat((char *)expected_server_text, expected_server_magic);

    size_t key_len = strlen((char *)expected_server_text);
    esp_crypto_sha1(expected_server_text, key_len, expected_server_sha1);
    esp_crypto_base64_encode(expected_server_key, sizeof(expected_server_key),  &outlen, expected_server_sha1, sizeof(expected_server_sha1));
    expected_server_key[ (outlen < sizeof(expected_server_key)) ? outlen : (sizeof(expected_server_key) - 1) ] = 0;
    ESP_LOGD(TAG, "server key=%s, send_key=%s, expected_server_key=%s", (char *)server_key, (char *)client_key, expected_server_key);
    if (strcmp((char *)expected_server_key, (char *)server_key) != 0) {
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
        ssize_t rc;
        if ((rc = getrandom(ws_header + header_len, 4, 0)) < 0) {
            ESP_LOGD(TAG, "getrandom() returned %zd", rc);
            return -1;
        }
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
        mask = &ws_header[header_len - 4];
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
    return _ws_write(t, op_code, WS_MASK, b, len, timeout_ms);
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
    if (bytes_to_read != 0 && (rlen = esp_transport_read_internal(ws, buffer, bytes_to_read, timeout_ms)) <= 0) {
        ESP_LOGE(TAG, "Error read data");
        return rlen;
    }
    ws->frame_state.bytes_remaining -= rlen;

    for (int i = 0; i < bytes_to_read; i++) {
        buffer[i] = (buffer[i] ^ ws->frame_state.mask_key[i % 4]);
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
    ws->frame_state.header_received = false;
    if ((poll_read = esp_transport_poll_read(ws->parent, timeout_ms)) <= 0) {
        return poll_read;
    }

    // Receive and process header first (based on header size)
    int header = 2;
    int mask_len = 4;
    if ((rlen = esp_transport_read_internal(ws, data_ptr, header, timeout_ms)) <= 0) {
        ESP_LOGE(TAG, "Error read data");
        return rlen;
    }
    ws->frame_state.header_received = true;
    ws->frame_state.fin = (*data_ptr & 0x80) != 0;
    ws->frame_state.opcode = (*data_ptr & 0x0F);
    data_ptr ++;
    mask = ((*data_ptr >> 7) & 0x01);
    payload_len = (*data_ptr & 0x7F);
    data_ptr++;
    ESP_LOGD(TAG, "Opcode: %d, mask: %d, len: %d", ws->frame_state.opcode, mask, payload_len);
    if (payload_len == 126) {
        // headerLen += 2;
        if ((rlen = esp_transport_read_internal(ws, data_ptr, header, timeout_ms)) <= 0) {
            ESP_LOGE(TAG, "Error read data");
            return rlen;
        }
        payload_len = data_ptr[0] << 8 | data_ptr[1];
    } else if (payload_len == 127) {
        // headerLen += 8;
        header = 8;
        if ((rlen = esp_transport_read_internal(ws, data_ptr, header, timeout_ms)) <= 0) {
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
        if (payload_len != 0 && (rlen = esp_transport_read_internal(ws, buffer, mask_len, timeout_ms)) <= 0) {
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

static int ws_handle_control_frame_internal(esp_transport_handle_t t, int timeout_ms)
{
    transport_ws_t *ws = esp_transport_get_context_data(t);
    char *control_frame_buffer = NULL;
    int control_frame_buffer_len = 0;
    int payload_len = ws->frame_state.payload_len;
    int ret = 0;

    // If no new header reception in progress, or not a control frame
    // just pass 0 -> no need to handle control frames
    if (ws->frame_state.header_received == false ||
            !(ws->frame_state.opcode & WS_OPCODE_CONTROL_FRAME)) {
        return 0;
    }

    if (payload_len > WS_TRANSPORT_MAX_CONTROL_FRAME_BUFFER_LEN) {
        ESP_LOGE(TAG, "Not enough room for reading control frames (need=%d, max_allowed=%d)",
                 ws->frame_state.payload_len, WS_TRANSPORT_MAX_CONTROL_FRAME_BUFFER_LEN);
        return -1;
    }

    // Now we can handle the control frame correctly (either zero payload, or a short one for which we allocate mem)
    control_frame_buffer_len = payload_len;
    if (control_frame_buffer_len > 0) {
        control_frame_buffer = malloc(control_frame_buffer_len);
        if (control_frame_buffer == NULL) {
            ESP_LOGE(TAG, "Cannot allocate buffer for control frames, need-%d", control_frame_buffer_len);
            return -1;
        }
    } else {
        control_frame_buffer_len = 0;
    }

    // read the payload of the control frame
    int actual_len = ws_read_payload(t, control_frame_buffer, control_frame_buffer_len, timeout_ms);
    if (actual_len != payload_len) {
        ESP_LOGE(TAG, "Control frame (opcode=%d) payload read failed (payload_len=%d, read_len=%d)",
                 ws->frame_state.opcode, payload_len, actual_len);
        ret = -1;
        goto free_payload_buffer;
    }

    ret = esp_transport_ws_handle_control_frames(t, control_frame_buffer, control_frame_buffer_len, timeout_ms, false);

free_payload_buffer:
    free(control_frame_buffer);
    return ret > 0 ? 0 : ret; // We don't propagate control frames, pass 0 to upper layers

}

static int ws_read(esp_transport_handle_t t, char *buffer, int len, int timeout_ms)
{
    int rlen = 0;
    transport_ws_t *ws = esp_transport_get_context_data(t);

    // If message exceeds buffer len then subsequent reads will skip reading header and read whatever is left of the payload
    if (ws->frame_state.bytes_remaining <= 0) {

        if ( (rlen = ws_read_header(t, buffer, len, timeout_ms)) < 0) {
            // If something when wrong then we prepare for reading a new header
            ws->frame_state.bytes_remaining = 0;
            return rlen;
        }

        // If the new opcode is a control frame and we don't pass it to the app
        //  - try to handle it internally using the application buffer
        if (ws->frame_state.header_received && (ws->frame_state.opcode & WS_OPCODE_CONTROL_FRAME) &&
                ws->propagate_control_frames == false) {
            // automatically handle only 0 payload frames and make the transport read to return 0 on success
            // which might be interpreted as timeouts
            return ws_handle_control_frame_internal(t, timeout_ms);
        }

        if (rlen == 0) {
            ws->frame_state.bytes_remaining = 0;
            return 0; // timeout
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
    free(ws->auth);
    free(ws);
    return 0;
}
static esp_err_t internal_esp_transport_ws_set_path(esp_transport_handle_t t, const char *path)
{
    if (t == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    transport_ws_t *ws = esp_transport_get_context_data(t);
    if (ws->path) {
        free(ws->path);
    }
    if (path == NULL) {
        ws->path = NULL;
        return ESP_OK;
    }
    ws->path = strdup(path);
    if (ws->path == NULL) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

void esp_transport_ws_set_path(esp_transport_handle_t t, const char *path)
{
    esp_err_t err = internal_esp_transport_ws_set_path(t, path);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_transport_ws_set_path has internally failed with err=%d", err);
    }
}

static int ws_get_socket(esp_transport_handle_t t)
{
    if (t) {
        transport_ws_t *ws = t->data;
        if (ws && ws->parent && ws->parent->_get_socket) {
            return ws->parent->_get_socket(ws->parent);
        }
    }
    return -1;
}

esp_transport_handle_t esp_transport_ws_init(esp_transport_handle_t parent_handle)
{
    if (parent_handle == NULL || parent_handle->foundation == NULL) {
      ESP_LOGE(TAG, "Invalid parent ptotocol");
      return NULL;
    }
    esp_transport_handle_t t = esp_transport_init();
    if (t == NULL) {
        return NULL;
    }
    transport_ws_t *ws = calloc(1, sizeof(transport_ws_t));
    ESP_TRANSPORT_MEM_CHECK(TAG, ws, {
        esp_transport_destroy(t);
        return NULL;
    });
    ws->parent = parent_handle;
    t->foundation = parent_handle->foundation;


    ws->path = strdup("/");
    ESP_TRANSPORT_MEM_CHECK(TAG, ws->path, {
        free(ws);
        esp_transport_destroy(t);
        return NULL;
    });
    ws->buffer = malloc(WS_BUFFER_SIZE);
    ESP_TRANSPORT_MEM_CHECK(TAG, ws->buffer, {
        free(ws->path);
        free(ws);
        esp_transport_destroy(t);
        return NULL;
    });

    esp_transport_set_func(t, ws_connect, ws_read, ws_write, ws_close, ws_poll_read, ws_poll_write, ws_destroy);
    // websocket underlying transfer is the payload transfer handle
    esp_transport_set_parent_transport_func(t, ws_get_payload_transport_handle);

    esp_transport_set_context_data(t, ws);
    t->_get_socket = ws_get_socket;
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

esp_err_t esp_transport_ws_set_auth(esp_transport_handle_t t, const char *auth)
{
    if (t == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    transport_ws_t *ws = esp_transport_get_context_data(t);
    if (ws->auth) {
        free(ws->auth);
    }
    if (auth == NULL) {
        ws->auth = NULL;
        return ESP_OK;
    }
    ws->auth = strdup(auth);
    if (ws->auth == NULL) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t esp_transport_ws_set_config(esp_transport_handle_t t, const esp_transport_ws_config_t *config)
{
    if (t == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t err = ESP_OK;
    transport_ws_t *ws = esp_transport_get_context_data(t);
    if (config->ws_path) {
        err = internal_esp_transport_ws_set_path(t, config->ws_path);
        ESP_TRANSPORT_ERR_OK_CHECK(TAG, err, return err;)
    }
    if (config->sub_protocol) {
        err = esp_transport_ws_set_subprotocol(t, config->sub_protocol);
        ESP_TRANSPORT_ERR_OK_CHECK(TAG, err, return err;)
    }
    if (config->user_agent) {
        err = esp_transport_ws_set_user_agent(t, config->user_agent);
        ESP_TRANSPORT_ERR_OK_CHECK(TAG, err, return err;)
    }
    if (config->headers) {
        err = esp_transport_ws_set_headers(t, config->headers);
        ESP_TRANSPORT_ERR_OK_CHECK(TAG, err, return err;)
    }
    if (config->auth) {
        err = esp_transport_ws_set_auth(t, config->auth);
        ESP_TRANSPORT_ERR_OK_CHECK(TAG, err, return err;)
    }
    ws->propagate_control_frames = config->propagate_control_frames;

    return err;
}

bool esp_transport_ws_get_fin_flag(esp_transport_handle_t t)
{
  transport_ws_t *ws = esp_transport_get_context_data(t);
  return ws->frame_state.fin;
}

int esp_transport_ws_get_upgrade_request_status(esp_transport_handle_t t)
{
    transport_ws_t *ws = esp_transport_get_context_data(t);
    return ws->http_status_code;
}

ws_transport_opcodes_t esp_transport_ws_get_read_opcode(esp_transport_handle_t t)
{
    transport_ws_t *ws = esp_transport_get_context_data(t);
    if (ws->frame_state.header_received) {
        // convert the header byte to enum if correctly received
        return (ws_transport_opcodes_t)ws->frame_state.opcode;
    }
    return WS_TRANSPORT_OPCODES_NONE;
}

int esp_transport_ws_get_read_payload_len(esp_transport_handle_t t)
{
    transport_ws_t *ws = esp_transport_get_context_data(t);
    return ws->frame_state.payload_len;
}

static int esp_transport_ws_handle_control_frames(esp_transport_handle_t t, char *buffer, int len, int timeout_ms, bool client_closed)
{
    transport_ws_t *ws = esp_transport_get_context_data(t);

    // If no new header reception in progress, or not a control frame
    // just pass 0 -> no need to handle control frames
    if (ws->frame_state.header_received == false ||
            !(ws->frame_state.opcode & WS_OPCODE_CONTROL_FRAME)) {
        return 0;
    }
    int actual_len;
    int payload_len = ws->frame_state.payload_len;

    ESP_LOGD(TAG, "Handling control frame with %d bytes payload", payload_len);
    if (payload_len > len) {
        ESP_LOGE(TAG, "Not enough room for processing the payload (need=%d, available=%d)", payload_len, len);
        ws->frame_state.bytes_remaining = payload_len - len;
        return -1;
    }

    if (ws->frame_state.opcode == WS_OPCODE_PING) {
        // handle PING frames internally: just send a PONG with the same payload
        actual_len = _ws_write(t, WS_OPCODE_PONG | WS_FIN, WS_MASK, buffer,
                               payload_len, timeout_ms);
        if (actual_len != payload_len) {
            ESP_LOGE(TAG, "PONG send failed (payload_len=%d, written_len=%d)", payload_len, actual_len);
            return -1;
        }
        ESP_LOGD(TAG, "PONG sent correctly (payload_len=%d)", payload_len);

        // control frame handled correctly, reset the flag indicating new header received
        ws->frame_state.header_received = false;
        return 0;

    } else if (ws->frame_state.opcode == WS_OPCODE_CLOSE) {
        // handle CLOSE by the server: send a zero payload frame
        if (buffer && payload_len > 0) {     // if some payload, print out the status code
            uint16_t *code_network_order = (uint16_t *) buffer;
            ESP_LOGI(TAG, "Got CLOSE frame with status code=%u", ntohs(*code_network_order));
        }

        if (client_closed == false) {
            // Only echo the closing frame if not initiated by the client
            if (_ws_write(t, WS_OPCODE_CLOSE | WS_FIN, WS_MASK, NULL, 0, timeout_ms) < 0) {
                ESP_LOGE(TAG, "Sending CLOSE frame with 0 payload failed");
                return -1;
            }
            ESP_LOGD(TAG, "CLOSE frame with no payload sent correctly");
        }

        // control frame handled correctly, reset the flag indicating new header received
        ws->frame_state.header_received = false;
        int ret = esp_transport_ws_poll_connection_closed(t, timeout_ms);
        if (ret == 0) {
            ESP_LOGW(TAG, "Connection cannot be terminated gracefully within timeout=%d", timeout_ms);
            return -1;
        }
        if (ret < 0) {
            ESP_LOGW(TAG, "Connection terminated while waiting for clean TCP close");
            return -1;
        }
        ESP_LOGI(TAG, "Connection terminated gracefully");
        return 1;
    } else if (ws->frame_state.opcode == WS_OPCODE_PONG) {
        // handle PONG: just indicate return code
        ESP_LOGD(TAG, "Received PONG frame with payload=%d", payload_len);
        // control frame handled correctly, reset the flag indicating new header received
        ws->frame_state.header_received = false;
        return 2;
    }
    return 0;
}

int esp_transport_ws_poll_connection_closed(esp_transport_handle_t t, int timeout_ms)
{
    struct timeval timeout;
    int sock = esp_transport_get_socket(t);
    fd_set readset;
    fd_set errset;
    FD_ZERO(&readset);
    FD_ZERO(&errset);
    FD_SET(sock, &readset);
    FD_SET(sock, &errset);

    int ret = select(sock + 1, &readset, NULL, &errset, esp_transport_utils_ms_to_timeval(timeout_ms, &timeout));
    if (ret > 0) {
        if (FD_ISSET(sock, &readset)) {
            uint8_t buffer;
            if (recv(sock, &buffer, 1, MSG_PEEK) <= 0) {
                // socket is readable, but reads zero bytes -- connection cleanly closed by FIN flag
                return 1;
            }
            ESP_LOGW(TAG, "esp_transport_ws_poll_connection_closed: unexpected data readable on socket=%d", sock);
        } else if (FD_ISSET(sock, &errset)) {
            int sock_errno = 0;
            uint32_t optlen = sizeof(sock_errno);
            getsockopt(sock, SOL_SOCKET, SO_ERROR, &sock_errno, &optlen);
            ESP_LOGD(TAG, "esp_transport_ws_poll_connection_closed select error %d, errno = %s, fd = %d", sock_errno, strerror(sock_errno), sock);
            if (sock_errno == ENOTCONN || sock_errno == ECONNRESET || sock_errno == ECONNABORTED) {
                // the three err codes above might be caused by connection termination by RTS flag
                // which we still assume as expected closing sequence of ws-transport connection
                return 1;
            }
            ESP_LOGE(TAG, "esp_transport_ws_poll_connection_closed: unexpected errno=%d on socket=%d", sock_errno, sock);
        }
        return -1; // indicates error: socket unexpectedly reads an actual data, or unexpected errno code
    }
    return ret;

}
