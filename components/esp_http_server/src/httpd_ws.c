/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */



#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <esp_log.h>
#include <esp_err.h>
#include <psa/crypto.h>
#include <mbedtls/base64.h>
#include <mbedtls/error.h>

#include <esp_http_server.h>
#include "esp_httpd_priv.h"
#include "freertos/event_groups.h"
#include "sdkconfig.h"

#ifdef CONFIG_HTTPD_WS_SUPPORT

#define WS_SEND_OK                      (1 << 0)
#define WS_SEND_FAILED                  (1 << 1)

#define SEC_WEBSOCKET_VERSION_HDR_MAX_LEN 255
#define SEC_WEBSOCKET_KEY_HDR_MAX_LEN     255
#define SEC_WEBSOCKET_VERSION             "13"

typedef struct {
    httpd_ws_frame_t frame;
    httpd_handle_t handle;
    int socket;
    transfer_complete_cb callback;
    void *arg;
    bool blocking;
    EventGroupHandle_t transfer_done;
} async_transfer_t;

static const char *TAG="httpd_ws";

/*
 * Bit masks for WebSocket frames.
 * Please refer to RFC6455 Section 5.2 for more details.
 */
#define HTTPD_WS_CONTINUE       0x00U
#define HTTPD_WS_FIN_BIT        0x80U
#define HTTPD_WS_RSV1_BIT       0x40U
#define HTTPD_WS_RSV2_BIT       0x20U
#define HTTPD_WS_RSV3_BIT       0x10U
#define HTTPD_WS_OPCODE_BITS    0x0fU
#define HTTPD_WS_MASK_BIT       0x80U
#define HTTPD_WS_LENGTH_BITS    0x7fU

/* RFC 6455 §7.4 close status codes used for protocol-error Close frames */
#define HTTPD_WS_CLOSE_CODE_PROTOCOL_ERROR 1002U
#define HTTPD_WS_CLOSE_CODE_TOO_BIG       1009U

/*
 * The magic GUID string used for handshake
 * Please refer to RFC6455 Section 1.3 for more details.
 */
static const char ws_magic_uuid[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

/* Checks if any subprotocols from the comma separated list matches the supported one
 *
 * Returns true if the response should contain a protocol field
*/

/**
 * @brief Checks if any subprotocols from the comma separated list matches the supported one
 *
 * @param supported_subprotocol[in] The subprotocol supported by the URI
 * @param subprotocol[in],  [in]: A comma separate list of subprotocols requested
 * @param buf_len Length of the buffer
 * @return true: found a matching subprotocol
 * @return false
 */
static bool httpd_ws_get_response_subprotocol(const char *supported_subprotocol, char *subprotocol, size_t buf_len)
{
    /* Request didn't contain any subprotocols */
    if (strnlen(subprotocol, buf_len) == 0) {
        return false;
    }

    if (supported_subprotocol == NULL) {
        ESP_LOGW(TAG, "Sec-WebSocket-Protocol %s not supported, URI do not support any subprotocols", subprotocol);
        return false;
    }

    /* Get first subprotocol from comma separated list */
    char *rest = NULL;
    char *s = strtok_r(subprotocol, ", ", &rest);
    int supported_subprotocol_len = strlen(supported_subprotocol);
    while (s != NULL) {
        if (strlen(s) == supported_subprotocol_len &&
                strncmp(s, supported_subprotocol, supported_subprotocol_len) == 0) {
            ESP_LOGD(TAG, "Requested subprotocol supported: %s", s);
            return true;
        }
        s = strtok_r(NULL, ", ", &rest);
    }

    ESP_LOGW(TAG, "Sec-WebSocket-Protocol %s not supported, supported subprotocol is %s", subprotocol, supported_subprotocol);

    /* No matches */
    return false;

}

/* Send an HTTP error response for a rejected WS handshake.
 * Optionally adds a Sec-WebSocket-Version header (RFC 6455 §4.4).
 * Returns ESP_FAIL so callers can write: return httpd_ws_send_handshake_error(...).
 */
static esp_err_t httpd_ws_send_handshake_error(httpd_req_t *req, const char *status,
        const char *message, const char *supported_version)
{
    if (supported_version != NULL) {
        if (httpd_resp_set_hdr(req, "Sec-WebSocket-Version", supported_version) != ESP_OK) {
            ESP_LOGE(TAG, LOG_FMT("Failed to set Sec-WebSocket-Version header"));
            return ESP_FAIL;
        }
    }
    esp_err_t ret = httpd_resp_send_custom_err(req, status, message);
    return (ret == ESP_OK) ? ESP_FAIL : ret;
}

esp_err_t httpd_ws_respond_server_handshake(httpd_req_t *req, const char *supported_subprotocol)
{
    /* Probe if input parameters are valid or not */
    if (!req || !req->aux) {
        ESP_LOGW(TAG, LOG_FMT("Argument is invalid"));
        return ESP_ERR_INVALID_ARG;
    }

    /* Detect handshake - reject if handshake was ALREADY performed */
    struct httpd_req_aux *req_aux = req->aux;
    if (req_aux->sd->ws_handshake_done) {
        ESP_LOGW(TAG, LOG_FMT("State is invalid - Handshake has been performed"));
        return ESP_ERR_INVALID_STATE;
    }

#if CONFIG_HTTPD_WS_STRICTER_RFC6455
    /* RFC 6455 §4.2.1: Host header MUST be present */
    size_t host_hdr_len = httpd_req_get_hdr_value_len(req, "Host");
    if (host_hdr_len == 0) {
        ESP_LOGW(TAG, LOG_FMT("\"Host\" is not found"));
        return httpd_ws_send_handshake_error(req, "400 Bad Request", "Missing Host header", NULL);
    }
#endif

    /* RFC 6455 §4.2.1: Sec-WebSocket-Version must be present and equal "13" */
    size_t version_hdr_len = httpd_req_get_hdr_value_len(req, "Sec-WebSocket-Version");
    if (version_hdr_len == 0) {
        ESP_LOGW(TAG, LOG_FMT("\"Sec-WebSocket-Version\" is not found"));
        return httpd_ws_send_handshake_error(req, "400 Bad Request",
                                             "Missing Sec-WebSocket-Version header", NULL);
    }
#if CONFIG_HTTPD_WS_STRICTER_RFC6455
    if (version_hdr_len > SEC_WEBSOCKET_VERSION_HDR_MAX_LEN) {
        ESP_LOGW(TAG, LOG_FMT("\"Sec-WebSocket-Version\" is too long"));
        return httpd_ws_send_handshake_error(req, "400 Bad Request",
                                             "Invalid Sec-WebSocket-Version header", NULL);
    }
#endif
    char *version_val = calloc(1, version_hdr_len + 1);
    if (version_val == NULL) {
        ESP_LOGE(TAG, "Failed to allocate version header buffer");
        return ESP_FAIL;
    }
    if (httpd_req_get_hdr_value_str(req, "Sec-WebSocket-Version", version_val, version_hdr_len + 1) != ESP_OK) {
        free(version_val);
        return httpd_ws_send_handshake_error(req, "400 Bad Request",
                                             "Invalid Sec-WebSocket-Version header", NULL);
    }
    /* WS version must be 13. Please refer to RFC6455 Section 4.1, Page 18 for more details. */
    if (strcasecmp(version_val, SEC_WEBSOCKET_VERSION) != 0) {
        ESP_LOGW(TAG, LOG_FMT("\"Sec-WebSocket-Version\" is not \"13\", it is: %s"), version_val);
        free(version_val);
        return httpd_ws_send_handshake_error(req, "426 Upgrade Required",
                                             "WebSocket version not supported", "13");
    }
    free(version_val);

    /* RFC 6455 §4.2.1 / §4.3: Sec-WebSocket-Key must be present (strict mode
     * additionally validates that it base64-decodes to a 16-byte nonce). */
    size_t sec_key_hdr_len = httpd_req_get_hdr_value_len(req, "Sec-WebSocket-Key");
    if (sec_key_hdr_len == 0) {
        ESP_LOGW(TAG, LOG_FMT("Cannot find client key"));
        return httpd_ws_send_handshake_error(req, "400 Bad Request",
                                             "Missing Sec-WebSocket-Key header", NULL);
    }
#if CONFIG_HTTPD_WS_STRICTER_RFC6455
    if (sec_key_hdr_len > SEC_WEBSOCKET_KEY_HDR_MAX_LEN) {
        ESP_LOGW(TAG, LOG_FMT("Sec-WebSocket-Key header is too long"));
        return httpd_ws_send_handshake_error(req, "400 Bad Request",
                                             "Invalid Sec-WebSocket-Key header", NULL);
    }
#endif
    char *sec_key_encoded = calloc(1, sec_key_hdr_len + 1);
    if (sec_key_encoded == NULL) {
        ESP_LOGE(TAG, "Failed to allocate Sec-WebSocket-Key buffer");
        return ESP_FAIL;
    }
    if (httpd_req_get_hdr_value_str(req, "Sec-WebSocket-Key", sec_key_encoded, sec_key_hdr_len + 1) != ESP_OK) {
        free(sec_key_encoded);
        return httpd_ws_send_handshake_error(req, "400 Bad Request",
                                             "Invalid Sec-WebSocket-Key header", NULL);
    }
#if CONFIG_HTTPD_WS_STRICTER_RFC6455
    uint8_t decoded_key[16] = { 0 };
    size_t decoded_key_len = 0;
    if (mbedtls_base64_decode(decoded_key, sizeof(decoded_key), &decoded_key_len,
                              (const unsigned char *)sec_key_encoded, strlen(sec_key_encoded)) != 0 ||
        decoded_key_len != sizeof(decoded_key)) {
        free(sec_key_encoded);
        ESP_LOGW(TAG, LOG_FMT("Sec-WebSocket-Key is not a valid base64-encoded 16-byte nonce"));
        return httpd_ws_send_handshake_error(req, "400 Bad Request",
                                             "Invalid Sec-WebSocket-Key header", NULL);
    }
#endif

    /* Prepare server key (Sec-WebSocket-Accept), concat the string */
    char server_key_encoded[33] = { '\0' };
    uint8_t server_key_hash[20] = { 0 };
    size_t server_raw_text_len = strlen(sec_key_encoded) + strlen(ws_magic_uuid) + 1;
    char *server_raw_text = calloc(1, server_raw_text_len);
    if (server_raw_text == NULL) {
        free(sec_key_encoded);
        ESP_LOGE(TAG, "Failed to allocate handshake hash input buffer");
        return ESP_FAIL;
    }

    strcpy(server_raw_text, sec_key_encoded);
    strcat(server_raw_text, ws_magic_uuid);

    ESP_LOGD(TAG, LOG_FMT("Server key before encoding: %s"), server_raw_text);

    /* Generate SHA-1 hash */
    psa_hash_operation_t sha1_operation = PSA_HASH_OPERATION_INIT;
    psa_status_t status = psa_hash_setup(&sha1_operation, PSA_ALG_SHA_1);
    if (status != PSA_SUCCESS) {
        free(server_raw_text);
        free(sec_key_encoded);
        ESP_LOGE(TAG, "Failed to setup SHA-1 operation");
        return ESP_FAIL;
    }

    status = psa_hash_update(&sha1_operation, (uint8_t *)server_raw_text, strlen(server_raw_text));
    if (status != PSA_SUCCESS) {
        free(server_raw_text);
        free(sec_key_encoded);
        ESP_LOGE(TAG, "Failed to update SHA-1 hash");
        psa_hash_abort(&sha1_operation);
        return ESP_FAIL;
    }

    size_t hash_length;
    status = psa_hash_finish(&sha1_operation, server_key_hash, sizeof(server_key_hash), &hash_length);
    free(server_raw_text);
    if (status != PSA_SUCCESS || hash_length != sizeof(server_key_hash)) {
        free(sec_key_encoded);
        ESP_LOGE(TAG, "Failed to finish SHA-1 hash");
        return ESP_FAIL;
    }

    /* Encode to Base64 */
    size_t encoded_len = 0;
    mbedtls_base64_encode((uint8_t *)server_key_encoded, sizeof(server_key_encoded), &encoded_len,
                          server_key_hash, sizeof(server_key_hash));

    free(sec_key_encoded);

    ESP_LOGD(TAG, LOG_FMT("Generated server key: %s"), server_key_encoded);

    char subprotocol[50] = { '\0' };
    if (httpd_req_get_hdr_value_str(req, "Sec-WebSocket-Protocol", subprotocol, sizeof(subprotocol) - 1) == ESP_ERR_HTTPD_RESULT_TRUNC) {
        ESP_LOGW(TAG, "Sec-WebSocket-Protocol length exceeded buffer size of %"NEWLIB_NANO_COMPAT_FORMAT", was truncated", NEWLIB_NANO_COMPAT_CAST(sizeof(subprotocol)));
    }


    /* Prepare the Switching Protocol response */
    char tx_buf[192] = { '\0' };
    int fmt_len = snprintf(tx_buf, sizeof(tx_buf),
                           "HTTP/1.1 101 Switching Protocols\r\n"
                           "Upgrade: websocket\r\n"
                           "Connection: Upgrade\r\n"
                           "Sec-WebSocket-Accept: %s\r\n", server_key_encoded);

    if (fmt_len < 0 || fmt_len > sizeof(tx_buf)) {
        ESP_LOGW(TAG, LOG_FMT("Failed to prepare Tx buffer"));
        return ESP_FAIL;
    }

    if ( httpd_ws_get_response_subprotocol(supported_subprotocol, subprotocol, sizeof(subprotocol))) {
        ESP_LOGD(TAG, "subprotocol: %s", subprotocol);
        int r = snprintf(tx_buf + fmt_len, sizeof(tx_buf) - fmt_len, "Sec-WebSocket-Protocol: %s\r\n", supported_subprotocol);
        if (r <= 0) {
            ESP_LOGE(TAG, "Error in response generation"
                          "(snprintf of subprotocol returned %d, buffer size: %"NEWLIB_NANO_COMPAT_FORMAT, r, NEWLIB_NANO_COMPAT_CAST(sizeof(tx_buf)));
            return ESP_FAIL;
        }

        fmt_len += r;

        if (fmt_len >= sizeof(tx_buf)) {
            ESP_LOGE(TAG, "Error in response generation"
                          "(snprintf of subprotocol returned %d, desired response len: %d, buffer size: %"NEWLIB_NANO_COMPAT_FORMAT, r, fmt_len, NEWLIB_NANO_COMPAT_CAST(sizeof(tx_buf)));
            return ESP_FAIL;
        }
    }

    int r = snprintf(tx_buf + fmt_len, sizeof(tx_buf) - fmt_len, "\r\n");
    if (r <= 0) {
        ESP_LOGE(TAG, "Error in response generation"
                        "(snprintf of subprotocol returned %d, buffer size: %"NEWLIB_NANO_COMPAT_FORMAT, r, NEWLIB_NANO_COMPAT_CAST(sizeof(tx_buf)));
        return ESP_FAIL;
    }
    fmt_len += r;
    if (fmt_len >= sizeof(tx_buf)) {
        ESP_LOGE(TAG, "Error in response generation"
                       "(snprintf of header terminal returned %d, desired response len: %d, buffer size: %"NEWLIB_NANO_COMPAT_FORMAT, r, fmt_len, NEWLIB_NANO_COMPAT_CAST(sizeof(tx_buf)));
        return ESP_FAIL;
    }

    /* Send off the response */
    if (httpd_send(req, tx_buf, fmt_len) < 0) {
        ESP_LOGW(TAG, LOG_FMT("Failed to send the response"));
        return ESP_FAIL;
    }

    return ESP_OK;
}

static esp_err_t httpd_ws_check_req(httpd_req_t *req)
{
    /* Probe if input parameters are valid or not */
    if (!req || !req->aux) {
        ESP_LOGW(TAG, LOG_FMT("Argument is null"));
        return ESP_ERR_INVALID_ARG;
    }

    /* Detect handshake - reject if handshake was NOT YET performed */
    struct httpd_req_aux *req_aux = req->aux;
    if (!req_aux->sd->ws_handshake_done) {
        ESP_LOGW(TAG, LOG_FMT("State is invalid - No handshake performed"));
        return ESP_ERR_INVALID_STATE;
    }

    return ESP_OK;
}

/* RFC 6455 §5.5: opcodes 0x8-0xA (CLOSE, PING, PONG) are control frames.
 * Reserved control opcodes 0xB-0xF are rejected earlier in httpd_ws_get_frame_type,
 * so an explicit whitelist is used here instead of a `>= CLOSE` range compare. */
static inline bool httpd_ws_is_control_opcode(httpd_ws_type_t type)
{
    return type == HTTPD_WS_TYPE_CLOSE ||
           type == HTTPD_WS_TYPE_PING  ||
           type == HTTPD_WS_TYPE_PONG;
}

/* Mark the session's pending frame as a CLOSE so the request layer tears the
 * socket down after the current request completes. */
static inline void httpd_ws_mark_closing(struct httpd_req_aux *aux)
{
    aux->ws_final = true;
    aux->ws_type = HTTPD_WS_TYPE_CLOSE;
}

/* Send a Close frame with the given status code and mark the session as closing.
 * Always returns ESP_FAIL so callers can write: return httpd_ws_fail_connection(...).
 * RFC 6455 §7.1.7
 */
static esp_err_t httpd_ws_fail_connection(httpd_req_t *req, uint16_t close_code)
{
    esp_err_t ret = ESP_FAIL;
    if (!req || !req->aux) {
        return ret;
    }

    struct httpd_req_aux *aux = req->aux;
    if (!aux->sd) {
        return ret;
    }

    httpd_ws_mark_closing(aux);

    bool already_closing = aux->sd->ws_close;
    aux->sd->ws_close = true;

    if (aux->sd->ws_handshake_done && !already_closing) {
        uint8_t close_payload[2] = {
            (uint8_t)(close_code >> 8U),
            (uint8_t)(close_code & 0xffU),
        };
        httpd_ws_frame_t close_frame = {
            .final = true,
            .fragmented = false,
            .type = HTTPD_WS_TYPE_CLOSE,
            .payload = close_payload,
            .len = sizeof(close_payload),
        };
        esp_err_t ret = httpd_ws_send_frame(req, &close_frame);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, LOG_FMT("Failed to send CLOSE frame with code %u"), close_code);
        }
    }

    return ret;
}

static esp_err_t httpd_ws_unmask_payload(uint8_t *payload, size_t len, const uint8_t *mask_key, size_t mask_offset)
{
    if (len < 1 || !payload) {
        ESP_LOGW(TAG, LOG_FMT("Invalid payload provided"));
        return ESP_ERR_INVALID_ARG;
    }

    for (size_t idx = 0; idx < len; idx++) {
        payload[idx] = (payload[idx] ^ mask_key[(idx + mask_offset) % 4]);
    }

    return ESP_OK;
}

static esp_err_t httpd_ws_recv_frame_internal(httpd_req_t *req, httpd_ws_frame_t *frame, size_t max_len, bool partial)
{
    esp_err_t ret = httpd_ws_check_req(req);
    if (ret != ESP_OK) {
        return ret;
    }

    struct httpd_req_aux *aux = req->aux;
    if (aux == NULL) {
        ESP_LOGW(TAG, LOG_FMT("Invalid Aux pointer"));
        return ESP_ERR_INVALID_ARG;
    }

    if (!frame) {
        ESP_LOGW(TAG, LOG_FMT("Frame pointer is invalid"));
        return ESP_ERR_INVALID_ARG;
    }
    /* If frame len is 0, will get frame len from req. Otherwise regard frame len already achieved by calling httpd_ws_recv_frame before */
    if (frame->len == 0) {
        /* Assign the frame info from the previous reading */
        frame->type = aux->ws_type;
        frame->final = aux->ws_final;

        /* Grab the second byte */
        uint8_t second_byte = 0;
        int recv_ret = httpd_recv_with_opt(req, (char *)&second_byte, sizeof(second_byte), HTTPD_RECV_OPT_BLOCKING);
        if (recv_ret != (int)sizeof(second_byte)) {
            ESP_LOGW(TAG, LOG_FMT("Failed to receive the second byte"));
            return ESP_FAIL;
        }

        /* Parse the second byte */
        /* Please refer to RFC6455 Section 5.2 for more details */
        bool masked = (second_byte & HTTPD_WS_MASK_BIT) != 0;

        /* Interpret length */
        uint8_t init_len = second_byte & HTTPD_WS_LENGTH_BITS;

#if CONFIG_HTTPD_WS_STRICTER_RFC6455
        /* RFC 6455 §5.5: control frames MUST have payload <= 125 bytes and
         * therefore MUST NOT use the 16- or 64-bit extended length encodings.
         */
        if (httpd_ws_is_control_opcode(frame->type) && init_len > 125) {
            ESP_LOGE(TAG, LOG_FMT("Invalid control frame length encoding"));
            return httpd_ws_fail_connection(req, HTTPD_WS_CLOSE_CODE_PROTOCOL_ERROR);
        }
#endif /* CONFIG_HTTPD_WS_STRICTER_RFC6455 */

        if (init_len < 126) {
            /* Case 1: If length is 0-125, then this length bit is 7 bits */
            frame->len = init_len;
        } else if (init_len == 126) {
            /* Case 2: If length byte is 126, then this frame's length bit is 16 bits */
            uint8_t length_bytes[2] = { 0 };
            recv_ret = httpd_recv_with_opt(req, (char *)length_bytes, sizeof(length_bytes), HTTPD_RECV_OPT_BLOCKING);
            if (recv_ret != (int)sizeof(length_bytes)) {
                ESP_LOGW(TAG, LOG_FMT("Failed to receive 2 bytes length"));
                return ESP_FAIL;
            }

            uint16_t length = ((uint16_t)(length_bytes[0] << 8U) | (length_bytes[1]));
#if CONFIG_HTTPD_WS_STRICTER_RFC6455
            /* RFC 6455 §5.2: encodings must be minimal; a value < 126 MUST use 7-bit form. */
            if (length < 126) {
                ESP_LOGE(TAG, LOG_FMT("Invalid WS frame length: non-minimal 16-bit encoding"));
                return httpd_ws_fail_connection(req, HTTPD_WS_CLOSE_CODE_PROTOCOL_ERROR);
            }
#endif /* CONFIG_HTTPD_WS_STRICTER_RFC6455 */
            frame->len = length;
        } else if (init_len == 127) {
            /* Case 3: If length is byte 127, then this frame's length bit is 64 bits */
            uint8_t length_bytes[8] = { 0 };
            recv_ret = httpd_recv_with_opt(req, (char *)length_bytes, sizeof(length_bytes), HTTPD_RECV_OPT_BLOCKING);
            if (recv_ret != (int)sizeof(length_bytes)) {
                ESP_LOGW(TAG, LOG_FMT("Failed to receive 8 bytes length"));
                return ESP_FAIL;
            }

#if CONFIG_HTTPD_WS_STRICTER_RFC6455
            /* RFC 6455 §5.2: MSB must be 0 */
            if (length_bytes[0] & 0x80) {
                ESP_LOGE(TAG, LOG_FMT("Invalid WS frame length: MSB must be 0"));
                return httpd_ws_fail_connection(req, HTTPD_WS_CLOSE_CODE_PROTOCOL_ERROR);
            }
#endif /* CONFIG_HTTPD_WS_STRICTER_RFC6455 */

            uint64_t length = (uint64_t)length_bytes[0] << 56U |
                    (uint64_t)length_bytes[1] << 48U |
                    (uint64_t)length_bytes[2] << 40U |
                    (uint64_t)length_bytes[3] << 32U |
                    (uint64_t)length_bytes[4] << 24U |
                    (uint64_t)length_bytes[5] << 16U |
                    (uint64_t)length_bytes[6] <<  8U |
                    (uint64_t)length_bytes[7];
#if CONFIG_HTTPD_WS_STRICTER_RFC6455
            /* Encoding must be minimal; values <= UINT16_MAX MUST use 16-bit form */
            if (length <= UINT16_MAX) {
                ESP_LOGE(TAG, LOG_FMT("Invalid WS frame length: non-minimal 64-bit encoding"));
                return httpd_ws_fail_connection(req, HTTPD_WS_CLOSE_CODE_PROTOCOL_ERROR);
            }
#endif /* CONFIG_HTTPD_WS_STRICTER_RFC6455 */
            if (length > SIZE_MAX) {
                ESP_LOGE(TAG, LOG_FMT("Invalid WS frame length: too large for platform"));
                return httpd_ws_fail_connection(req, HTTPD_WS_CLOSE_CODE_TOO_BIG);
            }
            frame->len = (size_t)length;
        }
        frame->left_len = frame->len;

        /* If this frame is masked, dump the mask as well */
        if (masked) {
            recv_ret = httpd_recv_with_opt(req, (char *)aux->mask_key, sizeof(aux->mask_key), HTTPD_RECV_OPT_BLOCKING);
            if (recv_ret != (int)sizeof(aux->mask_key)) {
                ESP_LOGW(TAG, LOG_FMT("Failed to receive mask key"));
                return ESP_FAIL;
            }
        } else {
            /* If the WS frame from client to server is not masked, it should be rejected.
             * Please refer to RFC6455 Section 5.2 for more details. */
            ESP_LOGE(TAG, LOG_FMT("WS frame is not properly masked."));
#if CONFIG_HTTPD_WS_STRICTER_RFC6455
            httpd_ws_fail_connection(req, HTTPD_WS_CLOSE_CODE_PROTOCOL_ERROR);
#endif /* CONFIG_HTTPD_WS_STRICTER_RFC6455 */
            return ESP_ERR_INVALID_STATE;
        }
    }
    /* If max_len is 0, regard it OK for userspace to get frame len */
    if (max_len == 0) {
        ESP_LOGD(TAG, "regard max_len == 0 is OK for user to get frame len");
        return ESP_OK;
    }
    if (frame->len > max_len) {
        /* When reading entire packet at once, we only accept the incoming packet length that is smaller than the max_len (or it will overflow the buffer!) */
        if (!partial) {
            ESP_LOGW(TAG, LOG_FMT("WS Message too long"));
            return ESP_ERR_INVALID_SIZE;
        }
        ESP_LOGD(TAG, LOG_FMT("WS Message too long. User will have to call read again"));
    }

    /* Receive buffer */
    /* If there's nothing to receive, return and stop here. */
    if (frame->left_len == 0) {
        return ESP_OK;
    }

    if (frame->payload == NULL) {
        ESP_LOGW(TAG, LOG_FMT("Payload buffer is null"));
        return ESP_FAIL;
    }

    size_t left_len = (max_len < frame->left_len) ? max_len : frame->left_len;
    size_t offset = 0;

    while (left_len > 0) {
        int read_len = httpd_recv_with_opt(req, (char *)frame->payload + offset, left_len, HTTPD_RECV_OPT_NONE);
        if (read_len <= 0) {
            ESP_LOGW(TAG, LOG_FMT("Failed to receive payload"));
            return ESP_FAIL;
        }
        offset += read_len;
        left_len -= read_len;

        ESP_LOGD(TAG, "Frame length: %"NEWLIB_NANO_COMPAT_FORMAT", Bytes Read: %"NEWLIB_NANO_COMPAT_FORMAT, NEWLIB_NANO_COMPAT_CAST(frame->len), NEWLIB_NANO_COMPAT_CAST(offset));
    }

    size_t mask_offset = frame->len - frame->left_len;
    frame->left_len -= offset;

    /* Unmask payload */
    httpd_ws_unmask_payload(frame->payload, offset, aux->mask_key, mask_offset);

    return ESP_OK;
}

esp_err_t httpd_ws_recv_frame(httpd_req_t *req, httpd_ws_frame_t *frame, size_t max_len) {
    return httpd_ws_recv_frame_internal(req, frame, max_len, false);
}

esp_err_t httpd_ws_recv_frame_part(httpd_req_t *req, httpd_ws_frame_t *frame, size_t max_len) {
    return httpd_ws_recv_frame_internal(req, frame, max_len, true);
}

esp_err_t httpd_ws_send_frame(httpd_req_t *req, httpd_ws_frame_t *frame)
{
    esp_err_t ret = httpd_ws_check_req(req);
    if (ret != ESP_OK) {
        return ret;
    }
    return httpd_ws_send_frame_async(req->handle, httpd_req_to_sockfd(req), frame);
}

esp_err_t httpd_ws_send_frame_async(httpd_handle_t hd, int fd, httpd_ws_frame_t *frame)
{
    if (!frame) {
        ESP_LOGW(TAG, LOG_FMT("Argument is invalid"));
        return ESP_ERR_INVALID_ARG;
    }

    struct sock_db *sess = httpd_sess_get(hd, fd);
    if (!sess) {
        return ESP_ERR_INVALID_ARG;
    }

#if CONFIG_HTTPD_WS_STRICTER_RFC6455
    /* RFC 6455 §1.4 / §5.5.1: once a CLOSE has been sent or received the
     * endpoint MUST NOT transmit any further data frames. Only the CLOSE
     * frame itself is permitted so that an in-progress fail-the-connection
     * or close-handshake response can still be emitted. */
    if (sess->ws_close && frame->type != HTTPD_WS_TYPE_CLOSE) {
        ESP_LOGW(TAG, LOG_FMT("Session is closing; refusing non-CLOSE frame (type=0x%02X)"), frame->type);
        return ESP_ERR_INVALID_STATE;
    }
#endif /* CONFIG_HTTPD_WS_STRICTER_RFC6455 */

    /* Prepare Tx buffer - maximum length is 14, which includes 2 bytes header, 8 bytes length, 4 bytes mask key */
    uint8_t tx_len = 0;
    uint8_t header_buf[10] = {0 };
    /* Set the `FIN` bit by default if message is not fragmented. Else, set it as per the `final` field */
    header_buf[0] |= (!frame->fragmented) ? HTTPD_WS_FIN_BIT : (frame->final? HTTPD_WS_FIN_BIT: HTTPD_WS_CONTINUE);
    header_buf[0] |= frame->type; /* Type (opcode): 4 bits */

    if (frame->len <= 125) {
        header_buf[1] = frame->len & 0x7fU; /* Length for 7 bits */
        tx_len = 2;
    } else if (frame->len > 125 && frame->len <= UINT16_MAX) {
        header_buf[1] = 126;                /* Length for 16 bits */
        header_buf[2] = (frame->len >> 8U) & 0xffU;
        header_buf[3] = frame->len & 0xffU;
        tx_len = 4;
    } else {
        header_buf[1] = 127;                /* Length for 64 bits */
        uint8_t shift_idx = sizeof(uint64_t) - 1; /* Shift index starts at 7 */
        uint64_t len64 = frame->len; /* Raise variable size to make sure we won't shift by more bits
                                      * than the length has (to avoid undefined behaviour) */
        for (int8_t idx = 2; idx <= 9; idx++) {
            /* Now do shifting (be careful of endianness, i.e. when buffer index is 2, frame length shift index is 7) */
            header_buf[idx] = (len64 >> (shift_idx * 8)) & 0xffU;
            shift_idx--;
        }
        tx_len = 10;
    }

    /* WebSocket server does not required to mask response payload, so leave the MASK bit as 0. */
    header_buf[1] &= (~HTTPD_WS_MASK_BIT);

    /* Send off header */
    if (sess->send_fn(hd, fd, (const char *)header_buf, tx_len, 0) < 0) {
        ESP_LOGW(TAG, LOG_FMT("Failed to send WS header"));
        return ESP_FAIL;
    }

    /* Send off payload */
    if(frame->len > 0 && frame->payload != NULL) {
        if (sess->send_fn(hd, fd, (const char *)frame->payload, frame->len, 0) < 0) {
            ESP_LOGW(TAG, LOG_FMT("Failed to send WS payload"));
            return ESP_FAIL;
        }
    }

    return ESP_OK;
}

esp_err_t httpd_ws_recv_control_frame(httpd_req_t *req, httpd_ws_frame_t *frame,
                                      uint8_t *buf, size_t max_len)
{
    /* The opcode/FIN were already decoded by httpd_ws_get_frame_type(); a zeroed
     * frame (len == 0) makes httpd_ws_recv_frame() read the remaining header and
     * payload. Control payloads are <= 125 bytes (RFC 6455 §5.5). */
    memset(frame, 0, sizeof(*frame));
    frame->payload = buf;
    if (httpd_ws_recv_frame(req, frame, max_len) != ESP_OK) {
        ESP_LOGD(TAG, LOG_FMT("Cannot receive the full control frame"));
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

esp_err_t httpd_ws_reply_to_control_frame(httpd_req_t *req, httpd_ws_frame_t *frame)
{
    switch (frame->type) {
    case HTTPD_WS_TYPE_PING:
        /* Reply to a PING with a PONG echoing the payload (RFC 6455 §5.5.2/5.5.3) */
        ESP_LOGD(TAG, LOG_FMT("Got a WS PING frame, Replying PONG..."));
        frame->type = HTTPD_WS_TYPE_PONG;
        return httpd_ws_send_frame(req, frame);
    case HTTPD_WS_TYPE_CLOSE:
        /* Reply to a CLOSE with an empty CLOSE (RFC 6455 §5.5.1) */
        ESP_LOGD(TAG, LOG_FMT("Got a WS CLOSE frame, Replying CLOSE..."));
        frame->len = 0;
        frame->payload = NULL;
        return httpd_ws_send_frame(req, frame);
    default:
        /* PONG and any other control frame require no reply */
        return ESP_OK;
    }
}

esp_err_t httpd_ws_handle_control_frame(httpd_req_t *req)
{
    struct httpd_req_aux *aux = req->aux;
    if (aux == NULL || aux->sd == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    struct sock_db *sd = aux->sd;

    /* The server receives the control-frame body itself. Oversized or malformed
     * frames are rejected by the max_len cap (zero-trust on client input). */
    httpd_ws_frame_t frame;
    uint8_t frame_buf[HTTPD_WS_CTRL_FRAME_BUF_LEN] = { 0 };
    esp_err_t ret = httpd_ws_recv_control_frame(req, &frame, frame_buf, HTTPD_WS_CTRL_FRAME_MAX_LEN);
    if (ret != ESP_OK) {
        return ret;
    }

    /* Notify the user's control handler with a read-only view of the frame. */
    esp_err_t handler_ret = ESP_OK;
    if (sd->ws_control_handler != NULL) {
        handler_ret = sd->ws_control_handler(req, &frame);
    }

    /* The server always performs the protocol reply (PONG for PING, CLOSE for
     * CLOSE). If the handler failed, still reply, then propagate the error so the
     * caller closes the socket. */
    esp_err_t reply_ret = httpd_ws_reply_to_control_frame(req, &frame);
    return (handler_ret != ESP_OK) ? handler_ret : reply_ret;
}

esp_err_t httpd_ws_get_frame_type(httpd_req_t *req)
{
    esp_err_t ret = httpd_ws_check_req(req);
    if (ret != ESP_OK) {
        return ret;
    }

    struct httpd_req_aux *aux = req->aux;
    if (aux == NULL) {
        ESP_LOGW(TAG, LOG_FMT("Invalid Aux pointer"));
        return ESP_ERR_INVALID_ARG;
    }

    struct sock_db *sd = aux->sd;
    if (sd == NULL) {
        ESP_LOGW(TAG, LOG_FMT("Invalid sd pointer"));
        return ESP_ERR_INVALID_ARG;
    }

    /* Read the first byte from the frame to get the FIN flag and Opcode */
    /* Please refer to RFC6455 Section 5.2 for more details */
    uint8_t first_byte = 0;
    int recv_ret = httpd_recv_with_opt(req, (char *)&first_byte, sizeof(first_byte), HTTPD_RECV_OPT_BLOCKING);
    if (recv_ret != (int)sizeof(first_byte)) {
        /* If we fail to read exactly one byte, this socket FD is invalid or the frame header is incomplete. */
        /* Here we mark it as a Close message and close it later. */
        ESP_LOGW(TAG, LOG_FMT("Failed to read header byte (socket FD invalid), closing socket now"));
        httpd_ws_mark_closing(aux);
        return ESP_OK;
    }

    ESP_LOGD(TAG, LOG_FMT("First byte received: 0x%02X"), first_byte);

    /* Decode the FIN flag and Opcode from the byte */
    aux->ws_final = (first_byte & HTTPD_WS_FIN_BIT) != 0;
    aux->ws_type = (first_byte & HTTPD_WS_OPCODE_BITS);

#if CONFIG_HTTPD_WS_STRICTER_RFC6455
    /* RFC 6455 §5.2: RSV bits MUST be 0 unless an extension has been negotiated.
     * This implementation does not support extensions, so any set RSV bit is an error.
     */
    if (first_byte & (HTTPD_WS_RSV1_BIT | HTTPD_WS_RSV2_BIT | HTTPD_WS_RSV3_BIT)) {
        ESP_LOGE(TAG, LOG_FMT("RSV1, RSV2 or RSV3 bits are set, closing connection"));
        return httpd_ws_fail_connection(req, HTTPD_WS_CLOSE_CODE_PROTOCOL_ERROR);
    }

    /* RFC 6455 §5.2: opcodes 0x3–0x7 and 0xB–0xF are reserved and must not be used. */
    switch (aux->ws_type) {
    case HTTPD_WS_TYPE_CONTINUE:
    case HTTPD_WS_TYPE_TEXT:
    case HTTPD_WS_TYPE_BINARY:
    case HTTPD_WS_TYPE_CLOSE:
    case HTTPD_WS_TYPE_PING:
    case HTTPD_WS_TYPE_PONG:
        break;
    default:
        ESP_LOGE(TAG, LOG_FMT("Invalid WS frame type: 0x%02X"), aux->ws_type);
        return httpd_ws_fail_connection(req, HTTPD_WS_CLOSE_CODE_PROTOCOL_ERROR);
    }

    /* RFC 6455 §5.5: control frames MUST NOT be fragmented (FIN bit must be set). */
    if (httpd_ws_is_control_opcode(aux->ws_type) && !aux->ws_final) {
        ESP_LOGE(TAG, LOG_FMT("Invalid fragmented control frame: 0x%02X"), aux->ws_type);
        return httpd_ws_fail_connection(req, HTTPD_WS_CLOSE_CODE_PROTOCOL_ERROR);
    }
#endif /* CONFIG_HTTPD_WS_STRICTER_RFC6455 */

    /* If userspace requests control frames, do not deal with the control frames here */
    if (!sd->ws_control_frames) {
        ESP_LOGD(TAG, LOG_FMT("Handler not requests control frames"));

        /* Auto-handle PING and CLOSE: receive the body, then send the reply. PONG is
         * deliberately not consumed here; it is dispatched to the data handler elsewhere. */
        if (aux->ws_type == HTTPD_WS_TYPE_PING || aux->ws_type == HTTPD_WS_TYPE_CLOSE) {
            httpd_ws_frame_t frame;
            uint8_t frame_buf[HTTPD_WS_CTRL_FRAME_BUF_LEN] = { 0 };
            esp_err_t recv_frame_ret = httpd_ws_recv_control_frame(req, &frame, frame_buf,
                                                                   HTTPD_WS_CTRL_FRAME_MAX_LEN);
            if (recv_frame_ret != ESP_OK) {
                return recv_frame_ret;
            }
            return httpd_ws_reply_to_control_frame(req, &frame);
        }
    }
    return ESP_OK;
}

httpd_ws_client_info_t httpd_ws_get_fd_info(httpd_handle_t hd, int fd)
{
    struct sock_db *sess = httpd_sess_get(hd, fd);

    if (sess == NULL) {
        return HTTPD_WS_CLIENT_INVALID;
    }
    bool is_active_ws = sess->ws_handshake_done && (!sess->ws_close);
    return is_active_ws ? HTTPD_WS_CLIENT_WEBSOCKET : HTTPD_WS_CLIENT_HTTP;
}

static void httpd_ws_send_cb(void *arg)
{
    async_transfer_t *trans = arg;

    esp_err_t err = httpd_ws_send_frame_async(trans->handle, trans->socket, &trans->frame);

    if (trans->blocking) {
        xEventGroupSetBits(trans->transfer_done, err ? WS_SEND_FAILED : WS_SEND_OK);
    } else if (trans->callback) {
        trans->callback(err, trans->socket, trans->arg);
    }

    free(trans);
}

esp_err_t httpd_ws_send_data(httpd_handle_t handle, int socket, httpd_ws_frame_t *frame)
{
    async_transfer_t *transfer = calloc(1, sizeof(async_transfer_t));
    if (transfer == NULL) {
        return ESP_ERR_NO_MEM;
    }

    EventGroupHandle_t transfer_done = xEventGroupCreate();
    if (!transfer_done) {
        free(transfer);
        return ESP_ERR_NO_MEM;
    }

    transfer->blocking = true;
    transfer->handle = handle;
    transfer->socket = socket;
    transfer->transfer_done = transfer_done;
    memcpy(&transfer->frame, frame, sizeof(httpd_ws_frame_t));

    esp_err_t err = httpd_queue_work(handle, httpd_ws_send_cb, transfer);
    if (err != ESP_OK) {
        vEventGroupDelete(transfer_done);
        free(transfer);
        return err;
    }

    EventBits_t status = xEventGroupWaitBits(transfer_done, WS_SEND_OK | WS_SEND_FAILED,
                                             pdTRUE, pdFALSE, portMAX_DELAY);

    vEventGroupDelete(transfer_done);

    return (status & WS_SEND_OK) ? ESP_OK : ESP_FAIL;
}

esp_err_t httpd_ws_send_data_async(httpd_handle_t handle, int socket, httpd_ws_frame_t *frame,
                                   transfer_complete_cb callback, void *arg)
{
    async_transfer_t *transfer = calloc(1, sizeof(async_transfer_t));
    if (transfer == NULL) {
        return ESP_ERR_NO_MEM;
    }

    transfer->arg = arg;
    transfer->callback = callback;
    transfer->handle = handle;
    transfer->socket = socket;
    memcpy(&transfer->frame, frame, sizeof(httpd_ws_frame_t));

    esp_err_t err = httpd_queue_work(handle, httpd_ws_send_cb, transfer);

    if (err) {
        free(transfer);
        return err;
    }

    return ESP_OK;
}

#endif /* CONFIG_HTTPD_WS_SUPPORT */
