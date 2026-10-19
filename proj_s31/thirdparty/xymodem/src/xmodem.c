/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "sdkconfig.h"
#if CONFIG_XYMODEM_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for X/YMODEM driver
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "xymodem.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "esp_crc.h"

#define TAG_TX "xmodem_tx"
#define TAG_RX "xmodem_rx"

#define NUL 0x00  // YMODEM packet 0 padding byte
#define SOH 0x01  // start of header, 128-byte data size
#define STX 0x02  // 1024-byte data size
#define EOT 0x04  // end of transmission
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18  // abort/cancel session
#define SUB 0x1A  // XMODEM packet padding byte
#define CRC 0x43  // request CRC16 mode
#define GMD 0x47  // G mode, streaming transfer

#define XYMODEM_MALLOC_CAPS (MALLOC_CAP_8BIT)

typedef struct {
    uint8_t soh_stx;
    uint8_t blk;
    uint8_t blk_inv;
    uint8_t data[];
    /**
     * uint8_t checksum; // obtained by xymodem_packet_checksum()
     * or
     * uint8_t crc[2]; // obtained by xymodem_packet_crc(), big-endian on the wire
     */
} xmodem_packet_header_t;

static inline size_t xymodem_packet_size(xymodem_block_size_t block_size, xymodem_check_type_t check_type)
{
    return sizeof(xmodem_packet_header_t) + (block_size == XYMODEM_BLOCK_SIZE_MIXED ? 1024 : block_size) + (check_type == XYMODEM_CHECK_SUM8 ? 1 : 2);
}

static inline xymodem_block_size_t xymodem_block_size(uint8_t soh_stx)
{
    return soh_stx == SOH ? XYMODEM_BLOCK_SIZE_128 : XYMODEM_BLOCK_SIZE_1024;
}

static inline uint8_t* xymodem_packet_checksum(xmodem_packet_header_t *packet)
{
    return packet->data + xymodem_block_size(packet->soh_stx);
}

static inline uint8_t* xymodem_packet_crc(xmodem_packet_header_t *packet)
{
    return packet->data + xymodem_block_size(packet->soh_stx);
}

/**
 * @brief Send the transmission cancellation sequence (on a best-effort basis).
 *
 * CAN bytes are sent as pairs so a receiver that purges RX is less likely
 * to swallow the entire abort sequence in one drain. A delay between the pairs
 * may be added later.
 */
static void send_cancel(const xymodem_transport_t *transport, uint8_t count)
{
    uint8_t can_pair[2] = { CAN, CAN };
    while (count > 0) {
        uint8_t n = count > 2 ? 2 : count;
        transport->send(transport->ctx, can_pair, n);
        count -= n;
    }
}

/**
 * @brief Drain the RX buffer until the line has been idle for at least idle_timeout_ms milliseconds.
 *
 * @param overall_timeout_ms Prevent a continuous stream of incoming data from causing the program to hang indefinitely.
 */
static esp_err_t purge_rx(const xymodem_transport_t *transport, uint32_t idle_timeout_ms, uint32_t overall_timeout_ms, const char *tag)
{
    ESP_LOGD(tag, "Purging RX buffer");
    uint8_t dummy[32];
    const int64_t deadline = esp_timer_get_time() + (int64_t)overall_timeout_ms * 1000;
    for (size_t recv_len = 1; recv_len > 0;) {
        ESP_RETURN_ON_FALSE(esp_timer_get_time() < deadline, ESP_ERR_TIMEOUT, tag, "Purge timeout");
        ESP_RETURN_ON_ERROR(transport->recv(transport->ctx, dummy, sizeof(dummy), &recv_len, idle_timeout_ms), tag, "Recv failed");
    }
    return ESP_OK;
}

/*******************************************************
 * XMODEM Sender functions
 *******************************************************/

/**
 * @brief Used by the sender. Send a packet and wait for a response. Uses a receiver-driven retransmission model.
 *
 * @note The original XMODEM Protocol Specification recommends a receiver-driven model. Under this model, after
 *       sending a packet, the sender should not retransmit it unless it explicitly receives a NAK. If data is
 *       corrupted or lost, retransmission should always be initiated by the receiver using a relatively short
 *       timeout. The sender only uses a much longer timeout; if no valid response is received within that period,
 *       it assumes that the link has been completely lost. This approach helps avoid the confusion that can arise
 *       if both sides attempt retransmission independently.
 *
 * @return
 *   - ESP_OK: successfully sent the packet and received a ACK
 *   - ESP_ERR_XYMODEM_NO_RESPONSE: timeout waiting for a response
 *   - ESP_ERR_XYMODEM_CANCEL_BY_PEER: transmission cancelled by the peer
 *   - ESP_ERR_XYMODEM_RETRY_EXCEEDED: maximum retry count exceeded
 *   - other errors
 */
static esp_err_t send_packet(const xymodem_transport_t *transport, const xymodem_send_config_t *config, const void *data, size_t len)
{
    for (uint32_t attempt = 0; attempt <= config->max_retry_cnt; attempt++) {
        ESP_RETURN_ON_ERROR(transport->send(transport->ctx, data, len), TAG_TX, "Send failed");
        const int64_t deadline = esp_timer_get_time() + (int64_t)config->response_timeout_ms * 1000;

        while (true) {
            int64_t now = esp_timer_get_time();
            if (now >= deadline) {
                ESP_LOGE(TAG_TX, "Wait for response timeout");
                return ESP_ERR_XYMODEM_NO_RESPONSE;
            }

            uint32_t remaining_ms = (deadline - now + 999) / 1000;
            uint8_t response;
            size_t recv_len = 0;
            ESP_RETURN_ON_ERROR(transport->recv(transport->ctx, &response, 1, &recv_len, remaining_ms), TAG_TX, "Recv failed");
            if (recv_len == 0) {
                // timeout, check deadline again
                continue;
            }

            if (response == CAN) {
                recv_len = 0;
                ESP_RETURN_ON_ERROR(transport->recv(transport->ctx, &response, 1, &recv_len, config->can_interval_ms), TAG_TX, "Recv failed");
                if (recv_len == 0) {
                    // a single CAN is considered noise
                    ESP_LOGW(TAG_TX, "Spurious CAN");
                    continue;
                } else if (response == CAN) {
                    ESP_LOGD(TAG_TX, "Cancelled by peer");
                    return ESP_ERR_XYMODEM_CANCEL_BY_PEER;
                }
                // treat the first CAN as noise and process the second byte normally
                ESP_LOGW(TAG_TX, "Spurious CAN");
            }

            if (response == ACK) {
                return ESP_OK;
            } else if (response == NAK) {
                ESP_LOGD(TAG_TX, "NAK, retry %u", (unsigned)(attempt + 1));
                break;
            }
            ESP_LOGW(TAG_TX, "Unknown response: %02X", response);
        }
    }
    return ESP_ERR_XYMODEM_RETRY_EXCEEDED;
}

esp_err_t xmodem_send(const xymodem_transport_t *transport, const xymodem_send_config_t *config, const xmodem_data_source_t *source)
{
    ESP_RETURN_ON_FALSE(transport && transport->recv && transport->send && config && source && source->read,
                        ESP_ERR_INVALID_ARG, TAG_TX, "invalid argument");
    ESP_RETURN_ON_FALSE(config->block_size == XYMODEM_BLOCK_SIZE_MIXED || config->block_size == XYMODEM_BLOCK_SIZE_128 ||
                        config->block_size == XYMODEM_BLOCK_SIZE_1024, ESP_ERR_INVALID_ARG, TAG_TX, "invalid block size");
    ESP_RETURN_ON_FALSE(config->response_timeout_ms > 0, ESP_ERR_INVALID_ARG, TAG_TX, "response timeout must > 0");
    ESP_RETURN_ON_FALSE(config->cancel_can_count >= 2, ESP_ERR_INVALID_ARG, TAG_TX, "cancel CAN count must >= 2");
    ESP_RETURN_ON_FALSE(config->can_interval_ms > 0, ESP_ERR_INVALID_ARG, TAG_TX, "CAN interval must > 0");

    // handshake
    bool handshake_success = false;
    xymodem_check_type_t check_type;
    for (uint32_t attempt = 0; attempt <= config->max_retry_cnt; attempt++) {
        uint8_t response;
        size_t recv_len = 0;
        ESP_RETURN_ON_ERROR(transport->recv(transport->ctx, &response, 1, &recv_len, config->response_timeout_ms), TAG_TX, "Recv failed");
        ESP_RETURN_ON_FALSE(recv_len > 0, ESP_ERR_XYMODEM_NO_RESPONSE, TAG_TX, "Wait for handshake timeout");

        if (response == NAK) {
            check_type = XYMODEM_CHECK_SUM8;
            handshake_success = true;
            break;
        } else if (response == CRC) {
            check_type = XYMODEM_CHECK_CRC16;
            handshake_success = true;
            break;
        } else {
            ESP_LOGW(TAG_TX, "Unknown handshake code: %02X", response);
            // clear the RX buffer and retry
            ESP_RETURN_ON_ERROR(purge_rx(transport, config->purge_idle_timeout_ms, config->response_timeout_ms, TAG_TX), TAG_TX, "Failed to purge RX buffer");
        }
    }
    ESP_RETURN_ON_FALSE(handshake_success, ESP_ERR_XYMODEM_RETRY_EXCEEDED, TAG_TX, "Handshake failed");
    ESP_LOGD(TAG_TX, "Got handshake %s", check_type == XYMODEM_CHECK_CRC16 ? "CRC16" : "SUM8");

    // send packets
    esp_err_t ret = ESP_OK;
    xmodem_packet_header_t* packet = heap_caps_malloc(xymodem_packet_size(config->block_size, check_type), XYMODEM_MALLOC_CAPS);
    ESP_GOTO_ON_FALSE(packet, ESP_ERR_NO_MEM, cleanup, TAG_TX, "No memory");

    for (uint8_t blk = 1; ; blk++) {
        const size_t request_len = config->block_size == XYMODEM_BLOCK_SIZE_MIXED ? 1024 : config->block_size;
        size_t data_len = 0;
        ESP_GOTO_ON_ERROR(source->read(source->ctx, packet->data, request_len, &data_len), cleanup, TAG_TX, "Read data failed");
        ESP_GOTO_ON_FALSE(data_len <= request_len, ESP_ERR_INVALID_SIZE, cleanup, TAG_TX, "Read length exceeds request");
        if (data_len == 0) {
            // end of data, send EOT
            break;
        } else if (data_len > 128 || config->block_size == XYMODEM_BLOCK_SIZE_1024) {
            // construct a 1024-byte packet
            memset(packet->data + data_len, SUB, 1024 - data_len);
            packet->soh_stx = STX;
            data_len = 1024;
        } else {
            // construct a 128-byte packet
            memset(packet->data + data_len, SUB, 128 - data_len);
            packet->soh_stx = SOH;
            data_len = 128;
        }

        packet->blk = blk;
        packet->blk_inv = ~blk;

        // calculate checksum/CRC
        if (check_type == XYMODEM_CHECK_SUM8) {
            uint8_t checksum = 0;
            for (size_t i = 0; i < data_len; i++) {
                checksum += packet->data[i];
            }
            *xymodem_packet_checksum(packet) = checksum;
        } else {  // XYMODEM_CHECK_CRC16
            uint16_t crc = ~esp_crc16_be(~0, packet->data, data_len);
            uint8_t *crc_bytes = xymodem_packet_crc(packet);
            crc_bytes[0] = (uint8_t)(crc >> 8);
            crc_bytes[1] = (uint8_t)crc;
        }

        ESP_LOGD(TAG_TX, "Send blk=%u %u bytes", blk, (unsigned)data_len);
        ESP_GOTO_ON_ERROR(send_packet(transport, config, packet, xymodem_packet_size(data_len, check_type)), cleanup, TAG_TX, "Send packet failed");
    }

    // send EOT
    ESP_LOGD(TAG_TX, "Send EOT");
    uint8_t eot = EOT;
    ESP_GOTO_ON_ERROR(send_packet(transport, config, &eot, 1), cleanup, TAG_TX, "Send EOT failed");

cleanup:
    if (ret != ESP_OK && ret != ESP_ERR_XYMODEM_CANCEL_BY_PEER) {
        send_cancel(transport, config->cancel_can_count);
    }
    heap_caps_free(packet);
    return ret;
}

/*******************************************************
 * XMODEM Receiver functions
 *******************************************************/

/**
 * @brief This function attempts to receive a single packet. It does not handle retries or flush the RX buffer on failure.
 * @return
 *   - ESP_OK: successfully received and validated a packet, including either a regular data packet or EOT
 *   - ESP_ERR_XYMODEM_NO_RESPONSE: no data was received within the timeout
 *   - ESP_ERR_XYMODEM_CANCEL_BY_PEER: transmission cancelled by the peer
 *   - ESP_ERR_XYMODEM_BAD_PACKET: received data does not form a valid packet
 *   - ESP_ERR_INVALID_CRC: blk or checksum/CRC verification failed
 *   - other errors
 */
static esp_err_t recv_packet_once(const xymodem_transport_t *transport, const xymodem_recv_config_t *config, uint8_t *buffer, xymodem_check_type_t check_type)
{
    size_t recv_len = 0;
    ESP_RETURN_ON_ERROR(transport->recv(transport->ctx, buffer, 1, &recv_len, config->packet_timeout_ms), TAG_RX, "Recv failed");
    if (recv_len == 0) {
        ESP_LOGD(TAG_RX, "Wait for packet timeout");
        return ESP_ERR_XYMODEM_NO_RESPONSE;
    }

    size_t remaining_size;
    switch (buffer[0]) {
    case EOT:
        ESP_LOGD(TAG_RX, "Recv EOT");
        return ESP_OK;
    case CAN:
        recv_len = 0;
        ESP_RETURN_ON_ERROR(transport->recv(transport->ctx, buffer, 1, &recv_len, config->can_interval_ms), TAG_RX, "Recv failed");
        if (recv_len == 1 && buffer[0] == CAN) {
            ESP_LOGD(TAG_RX, "Cancelled by peer");
            return ESP_ERR_XYMODEM_CANCEL_BY_PEER;
        }
        ESP_LOGD(TAG_RX, "Spurious CAN");
        return ESP_ERR_XYMODEM_BAD_PACKET;
    case SOH:
    case STX:
        remaining_size = xymodem_packet_size(xymodem_block_size(buffer[0]), check_type) - 1;
        break;
    default:
        ESP_LOGD(TAG_RX, "Bad header %02X", buffer[0]);
        return ESP_ERR_XYMODEM_BAD_PACKET;
    }

    ESP_RETURN_ON_ERROR(transport->recv(transport->ctx, buffer + 1, remaining_size, &recv_len, config->packet_timeout_ms), TAG_RX, "Recv failed");
    if (recv_len != remaining_size) {
        ESP_LOGD(TAG_RX, "Incomplete packet (%u/%u)", (unsigned)recv_len, (unsigned)remaining_size);
        return ESP_ERR_XYMODEM_BAD_PACKET;
    }

    xmodem_packet_header_t* packet = (xmodem_packet_header_t*)buffer;
    if ((packet->blk ^ packet->blk_inv) != 0xFF) {
        ESP_LOGD(TAG_RX, "Blk complement mismatch %u/%u", packet->blk, packet->blk_inv);
        return ESP_ERR_INVALID_CRC;
    }

    if (check_type == XYMODEM_CHECK_SUM8) {
        uint8_t checksum = 0;
        for (size_t i = 0; i < xymodem_block_size(packet->soh_stx); i++) {
            checksum += packet->data[i];
        }
        if (checksum != *xymodem_packet_checksum(packet)) {
            ESP_LOGD(TAG_RX, "SUM8 mismatch");
            return ESP_ERR_INVALID_CRC;
        }
    } else {
        uint16_t crc = ~esp_crc16_be(~0, packet->data, xymodem_block_size(packet->soh_stx) + 2);
        if (crc != 0) {
            ESP_LOGD(TAG_RX, "CRC16 mismatch");
            return ESP_ERR_INVALID_CRC;
        }
    }
    ESP_LOGD(TAG_RX, "Recv blk=%u %u bytes", packet->blk, (unsigned)xymodem_block_size(packet->soh_stx));
    return ESP_OK;
}

/**
 * @brief Receive and validate an XMODEM packet with retries.
 * @param initial_request is sent before the first attempt to receive a packet.
 * @param idle_request is sent when no response is received within the timeout. Once a packet is started,
 *                     packet errors and subsequent timeouts will be handled by sending NAK to request retransmission.
 * @note
 * @return
 *   - ESP_OK: successfully received and validated a packet
 *   - ESP_ERR_XYMODEM_CANCEL_BY_PEER: transmission cancelled by the peer
 *   - ESP_ERR_XYMODEM_NO_RESPONSE: peer did not respond at all
 *   - ESP_ERR_XYMODEM_RETRY_EXCEEDED: some data was received, but the reception could not be completed within the retry limit
 *   - other errors
 */
static esp_err_t recv_packet(const xymodem_transport_t *transport, const xymodem_recv_config_t *config,
                             void *buffer, xymodem_check_type_t check_type, uint8_t initial_request, uint8_t idle_request)
{
    bool packet_started = false;
    uint8_t request = initial_request;
    for (uint32_t attempt = 0; attempt <= config->max_retry_cnt; attempt++) {
        ESP_RETURN_ON_ERROR(transport->send(transport->ctx, &request, 1), TAG_RX, "Send failed");
        esp_err_t ret = recv_packet_once(transport, config, buffer, check_type);
        switch (ret) {
        case ESP_OK:
        case ESP_ERR_XYMODEM_CANCEL_BY_PEER:
            return ret;
        case ESP_ERR_XYMODEM_NO_RESPONSE:
            ESP_LOGD(TAG_RX, "No packet, retry %u", (unsigned)(attempt + 1));
            request = packet_started ? NAK : idle_request;
            break;
        case ESP_ERR_XYMODEM_BAD_PACKET:
        case ESP_ERR_INVALID_CRC:
            ESP_LOGD(TAG_RX, "Bad packet, retry %u", (unsigned)(attempt + 1));
            packet_started = true;
            request = NAK;
            ESP_RETURN_ON_ERROR(purge_rx(transport, config->purge_idle_timeout_ms, config->packet_timeout_ms, TAG_RX), TAG_RX, "Failed to purge RX buffer");
            break;
        default:
            return ret;
        }
    }
    return packet_started ? ESP_ERR_XYMODEM_RETRY_EXCEEDED : ESP_ERR_XYMODEM_NO_RESPONSE;
}

esp_err_t xmodem_recv(const xymodem_transport_t *transport, const xymodem_recv_config_t *config, const xmodem_data_sink_t *sink)
{
    ESP_RETURN_ON_FALSE(transport && transport->recv && transport->send && config && sink && sink->write,
                        ESP_ERR_INVALID_ARG, TAG_RX, "invalid argument");
    ESP_RETURN_ON_FALSE(config->check_type == XYMODEM_CHECK_AUTO || config->check_type == XYMODEM_CHECK_SUM8 ||
                        config->check_type == XYMODEM_CHECK_CRC16, ESP_ERR_INVALID_ARG, TAG_RX, "invalid check type");
    ESP_RETURN_ON_FALSE(config->packet_timeout_ms > 0, ESP_ERR_INVALID_ARG, TAG_RX, "packet timeout must > 0");
    ESP_RETURN_ON_FALSE(config->cancel_can_count >= 2, ESP_ERR_INVALID_ARG, TAG_RX, "cancel CAN count must >= 2");
    ESP_RETURN_ON_FALSE(config->can_interval_ms > 0, ESP_ERR_INVALID_ARG, TAG_RX, "CAN interval must > 0");

    // prepare a buffer of the maximum possible length.
    xmodem_packet_header_t* packet = heap_caps_malloc(xymodem_packet_size(XYMODEM_BLOCK_SIZE_1024, XYMODEM_CHECK_CRC16), XYMODEM_MALLOC_CAPS);
    ESP_RETURN_ON_FALSE(packet, ESP_ERR_NO_MEM, TAG_RX, "No memory");
    esp_err_t ret = ESP_OK;

    // handshake
    xymodem_check_type_t check_type;
    switch (config->check_type) {
    case XYMODEM_CHECK_AUTO:
    case XYMODEM_CHECK_CRC16:
        ESP_LOGD(TAG_RX, "Try handshake CRC16");
        ret = recv_packet(transport, config, packet, XYMODEM_CHECK_CRC16, CRC, CRC);
        if (ret == ESP_OK) {
            check_type = XYMODEM_CHECK_CRC16;
            ESP_LOGD(TAG_RX, "Handshake CRC16 success");
            break;
        }
        if (config->check_type == XYMODEM_CHECK_AUTO && ret == ESP_ERR_XYMODEM_NO_RESPONSE) {
            ESP_LOGD(TAG_RX, "Peer may not support CRC16, fallback to SUM8");
        } else {
            ESP_GOTO_ON_ERROR(ret, cleanup, TAG_RX, "CRC16 handshake failed");
        }
        __attribute__((fallthrough));
    case XYMODEM_CHECK_SUM8:
        ESP_LOGD(TAG_RX, "Try handshake SUM8");
        ret = recv_packet(transport, config, packet, XYMODEM_CHECK_SUM8, NAK, NAK);
        if (ret == ESP_OK) {
            check_type = XYMODEM_CHECK_SUM8;
            ESP_LOGD(TAG_RX, "Handshake SUM8 success");
            break;
        }
        ESP_GOTO_ON_ERROR(ret, cleanup, TAG_RX, "SUM8 handshake failed");
        __attribute__((fallthrough));
    default:
        __builtin_unreachable();
    }

    // main receive loop
    for (uint8_t blk = 1, last_blk = 1; ;) {
        if (packet->soh_stx == EOT) {
            // end of transmission
            ESP_LOGD(TAG_RX, "ACK");
            uint8_t ack = ACK;
            ESP_GOTO_ON_ERROR(transport->send(transport->ctx, &ack, 1), cleanup, TAG_RX, "Send failed");
            break;
        } else {  // SOH or STX
            if (packet->blk == blk) {
                ESP_GOTO_ON_ERROR(sink->write(sink->ctx, packet->data, xymodem_block_size(packet->soh_stx)), cleanup, TAG_RX, "Write failed");
                last_blk = blk;
                blk++;
            } else if (packet->blk == last_blk) {
                // outdated packet
                ESP_LOGD(TAG_RX, "Outdated packet got=%u expect=%u", packet->blk, blk);
            } else {
                // The packet number is neither the expected one nor the previous one.
                // The transfer is seriously out of sync and must be aborted according to the protocol specification.
                ESP_GOTO_ON_ERROR(ESP_ERR_XYMODEM_OUT_OF_SYNC, cleanup, TAG_RX, "Out of sync");
            }
            ESP_LOGD(TAG_RX, "ACK");
            ESP_GOTO_ON_ERROR(recv_packet(transport, config, packet, check_type, ACK, NAK), cleanup, TAG_RX, "Recv packet failed");
        }
    }

cleanup:
    if (ret != ESP_OK && ret != ESP_ERR_XYMODEM_CANCEL_BY_PEER) {
        send_cancel(transport, config->cancel_can_count);
    }
    heap_caps_free(packet);
    return ret;

}

/*******************************************************
 * Convenience APIs
 *******************************************************/

typedef struct {
    const uint8_t *data;
    size_t len;
    size_t offset;
} buffer_source_t;

typedef struct {
    uint8_t *data;
    size_t cap;
    size_t copied;
} buffer_sink_t;

static esp_err_t buffer_read(void *ctx, void *buf, size_t len, size_t *read_len)
{
    buffer_source_t *src = (buffer_source_t *)ctx;
    size_t remain = src->len - src->offset;
    size_t n = remain < len ? remain : len;
    if (n > 0) {
        memcpy(buf, src->data + src->offset, n);
        src->offset += n;
    }
    *read_len = n;
    return ESP_OK;
}

static esp_err_t buffer_write(void *ctx, const void *buf, size_t len)
{
    buffer_sink_t *sink = (buffer_sink_t *)ctx;
    // A later packet after the buffer is already full is a real overflow.
    // Truncating the current packet is allowed: XMODEM cannot tell payload from
    // trailing padding, so the last packet may legitimately exceed the remaining space.
    if (sink->copied >= sink->cap) {
        return ESP_ERR_NO_MEM;
    }
    size_t remain = sink->cap - sink->copied;
    size_t n = remain < len ? remain : len;
    memcpy(sink->data + sink->copied, buf, n);
    sink->copied += n;
    return ESP_OK;
}

esp_err_t xmodem_send_buffer(const xymodem_transport_t *transport, const xymodem_send_config_t *config, const void *buf, size_t len)
{
    ESP_RETURN_ON_FALSE(buf || len == 0, ESP_ERR_INVALID_ARG, TAG_TX, "invalid argument");
    buffer_source_t src = {
        .data = (const uint8_t *)buf,
        .len = len,
        .offset = 0,
    };
    const xmodem_data_source_t source = {
        .read = buffer_read,
        .ctx = &src,
    };
    return xmodem_send(transport, config, &source);
}

esp_err_t xmodem_recv_buffer(const xymodem_transport_t *transport, const xymodem_recv_config_t *config, void *buf, size_t len, size_t *recv_len)
{
    ESP_RETURN_ON_FALSE(buf || len == 0, ESP_ERR_INVALID_ARG, TAG_RX, "invalid argument");
    buffer_sink_t sink_ctx = {
        .data = (uint8_t *)buf,
        .cap = len,
        .copied = 0,
    };
    const xmodem_data_sink_t sink = {
        .write = buffer_write,
        .ctx = &sink_ctx,
    };
    esp_err_t ret = xmodem_recv(transport, config, &sink);
    if (recv_len) {
        *recv_len = sink_ctx.copied;
    }
    return ret;
}

#if CONFIG_XYMODEM_ENABLE_DEBUG_LOG
__attribute__((constructor))
static void xymodem_override_default_log_level(void)
{
    esp_log_level_set(TAG_TX, ESP_LOG_DEBUG);
    esp_log_level_set(TAG_RX, ESP_LOG_DEBUG);
}
#endif
