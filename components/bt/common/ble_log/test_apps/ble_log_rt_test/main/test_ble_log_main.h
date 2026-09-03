/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ble_log.h"

typedef struct {
    ble_log_src_t src;
    uint32_t sn;
    const uint8_t *payload;
    size_t payload_len;
} test_ble_log_frame_t;

typedef void (*test_ble_log_frame_observer_t)(const test_ble_log_frame_t *frame, void *ctx);

/* Walks a captured transport buffer, validating frame headers and checksums.
 * Returns true when the whole buffer consists of valid frames. */
bool test_ble_log_walk_frames(const uint8_t *data, size_t len,
                              test_ble_log_frame_observer_t observer, void *ctx);
