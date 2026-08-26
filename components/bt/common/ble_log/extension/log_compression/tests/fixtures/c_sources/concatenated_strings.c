/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* Test fixture: PRIu64 and other macro-concatenated format strings */

void test_pri_u64(uint64_t val64) {
    APPL_TRACE_DEBUG("val %" PRIu64, val64);
}

void test_pri_d64(int64_t sval64) {
    APPL_TRACE_DEBUG("signed %" PRId64, sval64);
}

void test_pri_x64(uint64_t hval64) {
    APPL_TRACE_DEBUG("hex %" PRIx64, hval64);
}
