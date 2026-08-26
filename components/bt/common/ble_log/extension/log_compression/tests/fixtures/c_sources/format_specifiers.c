/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* Test fixture: all format specifier -> size type mappings */

void test_u32_types(int a, unsigned int b, int c, int d, int e) {
    APPL_TRACE_DEBUG("int:%d unsigned:%u hex:%x HEX:%X octal:%o", a, b, c, d, e);
}

void test_u64_float(double f_val) {
    APPL_TRACE_DEBUG("float: %f", f_val);
}

void test_u64_long_long(long long ll_val, long long llx_val) {
    APPL_TRACE_DEBUG("long long: %lld hex_ll: %llx", ll_val, llx_val);
}

void test_str_type(const char *str1, const char *str2) {
    APPL_TRACE_DEBUG("string: %s, another: %s", str1, str2);
}

void test_mixed_types(int i, const char *s, long long ll, double f, int h) {
    APPL_TRACE_DEBUG("mixed: %d %s %lld %f %x", i, s, ll, f, h);
}

void test_width_flags(int val, int num, const char *str) {
    APPL_TRACE_DEBUG("padded: %08x, width: %10d, left: %-20s", val, num, str);
}
