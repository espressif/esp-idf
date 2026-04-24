/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* Test fixture: basic log patterns */

void simple_no_args(void) {
    APPL_TRACE_DEBUG("Simple message with no args");
}

void simple_one_int(int val) {
    APPL_TRACE_ERROR("Value is %d", val);
}

void simple_multi_int(int a, int b, int c) {
    APPL_TRACE_API("Values: %d, %d, %d", a, b, c);
}

void simple_string(const char *name) {
    APPL_TRACE_WARNING("Name is %s", name);
}
