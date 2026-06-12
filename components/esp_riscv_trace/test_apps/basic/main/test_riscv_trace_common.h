/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "esp_riscv_trace.h"

#ifdef __cplusplus
extern "C" {
#endif

/* RISC-V trace packet framing: 1-byte header + 2-byte index, then payload (1-10B). */
#define TRACE_PACKET_HEADER_SIZE     1    /* length + format/reserved byte */
#define TRACE_PACKET_INDEX_SIZE      2    /* monotonic packet index */
#define TRACE_PACKET_PREFIX_SIZE     (TRACE_PACKET_HEADER_SIZE + TRACE_PACKET_INDEX_SIZE) /* bytes before payload */
#define TRACE_PACKET_MIN_LEN         4    /* prefix (3) + >=1 payload byte */
#define TRACE_PACKET_MAX_LEN         13   /* prefix (3) + <=10 payload bytes */
#define TRACE_SYNC_PAYLOAD_BYTES     5
#define TRACE_ANCHOR_ZERO_RUN_BYTES  14   /* an anchor tag is >=14 zero bytes */
#define TRACE_TEST_STOP_TIMEOUT_US   1000000

/* If payload is a Format 3 / Subformat 0 (sync) packet with at least TRACE_SYNC_PAYLOAD_BYTES
 * payload bytes, decode its full PC into *pc and return true. Otherwise return false.
 * payload_len is the number of valid payload bytes (packet length minus the 3-byte header). */
bool decode_sync_pc(const uint8_t *payload, size_t payload_len, uint32_t *pc);

/* True if any byte after the write head is non-zero, i.e. a wrap overwrote that region (the
 * driver pre-zeros the buffer, so there is no hardware "wrapped" flag in loop mode). */
bool buffer_has_wrapped(const uint8_t *buf, size_t capacity, size_t head);

/* Assert the encoder stopped cleanly: FIFO empty, no overflow, not in the lost state. */
void validate_trace_status(esp_riscv_trace_core_t core);

#ifdef __cplusplus
}
#endif
