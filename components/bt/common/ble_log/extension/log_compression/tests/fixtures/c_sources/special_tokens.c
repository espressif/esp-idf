/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* Test fixture: __func__, __LINE__, MAC2STR, bt_hex */

void test_func_macro(void) {
    APPL_TRACE_DEBUG("%s starting", __func__);
}

void test_function_macro(void) {
    APPL_TRACE_DEBUG("%s called", __FUNCTION__);
}

void test_line_macro(int err_code) {
    APPL_TRACE_ERROR("%s error at line %d with code %d", __func__, __LINE__, err_code);
}

void test_mac2str(const uint8_t *bd_addr) {
    APPL_TRACE_DEBUG("addr=" MACSTR, MAC2STR(bd_addr));
}

void test_mixed_special(int handle, const uint8_t *addr) {
    APPL_TRACE_DEBUG("%s handle=0x%x addr=" MACSTR, __func__, handle, MAC2STR(addr));
}
