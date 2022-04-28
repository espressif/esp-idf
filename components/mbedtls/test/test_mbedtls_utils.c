/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdint.h>
#include "test_mbedtls_utils.h"

static inline char to_hex_digit(unsigned val)
{
    return (val < 10) ? ('0' + val) : ('a' + val - 10);
}


void utils_bin2hex(char *const hex, const size_t hex_maxlen, const unsigned char *const bin, const size_t bin_len)
{
    assert(bin_len < SIZE_MAX / 2);
    assert(hex_maxlen > bin_len * 2U);
    assert(hex);
    assert(bin);

    int i;
    for (i = 0; i < bin_len; i++) {
        hex[2*i] = to_hex_digit(bin[i] >> 4);
        hex[2*i + 1] = to_hex_digit(bin[i] & 0xf);
    }
    hex[i * 2U] = 0U;
}
