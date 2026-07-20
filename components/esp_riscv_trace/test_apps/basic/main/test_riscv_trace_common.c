/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "esp_riscv_trace.h"
#include "test_riscv_trace_common.h"

bool decode_sync_pc(const uint8_t *payload, size_t payload_len, uint32_t *pc)
{
    if (payload_len < TRACE_SYNC_PAYLOAD_BYTES) {
        return false;
    }
    if ((payload[0] & 0x3) != 3 || ((payload[0] >> 2) & 0x3) != 0) {
        return false;
    }
    /* Address is 31 bits starting at payload bit 6; byte address is addr << 1. */
    uint64_t v = 0;
    for (int i = 0; i < TRACE_SYNC_PAYLOAD_BYTES; i++) {
        v |= (uint64_t)payload[i] << (8 * i);
    }
    *pc = (uint32_t)((v >> 6) & 0x7FFFFFFFu) << 1;
    return true;
}

bool buffer_has_wrapped(const uint8_t *buf, size_t capacity, size_t head)
{
    for (size_t i = head; i < capacity; i++) {
        if (buf[i] != 0) {
            return true;
        }
    }
    return false;
}

void validate_trace_status(esp_riscv_trace_core_t core)
{
    esp_riscv_trace_status_t status = { 0 };
    TEST_ESP_OK(esp_riscv_trace_get_status(core, &status));
    TEST_ASSERT_TRUE_MESSAGE(status.fifo_empty, "trace FIFO is not empty after stop");
    TEST_ASSERT_FALSE_MESSAGE(status.fifo_overflowed, "trace FIFO overflowed");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(ESP_RISCV_TRACE_WORK_STATUS_LOST, status.work_status,
                                  "trace encoder entered lost state");
}
