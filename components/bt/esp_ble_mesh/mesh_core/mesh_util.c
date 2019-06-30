/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2016 Vinayak Kariappa Chettimada
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "sdkconfig.h"
#include "mesh_util.h"
#include "mesh_kernel.h"
#include "mesh_aes_encrypt.h"

#define MASK_TWENTY_SEVEN 0x1b

const char *bt_hex(const void *buf, size_t len)
{
    static const char hex[] = "0123456789abcdef";
    static char hexbufs[4][129];
    static u8_t curbuf;
    const u8_t *b = buf;
    unsigned int mask;
    char *str;
    int i;

    mask = bt_mesh_irq_lock();
    str = hexbufs[curbuf++];
    curbuf %= ARRAY_SIZE(hexbufs);
    bt_mesh_irq_unlock(mask);

    len = MIN(len, (sizeof(hexbufs[0]) - 1) / 2);

    for (i = 0; i < len; i++) {
        str[i * 2]     = hex[b[i] >> 4];
        str[i * 2 + 1] = hex[b[i] & 0xf];
    }

    str[i * 2] = '\0';

    return str;
}

void mem_rcopy(u8_t *dst, u8_t const *src, u16_t len)
{
    src += len;
    while (len--) {
        *dst++ = *--src;
    }
}

unsigned int _copy(uint8_t *to, unsigned int to_len,
                   const uint8_t *from, unsigned int from_len)
{
    if (from_len <= to_len) {
        (void)memcpy(to, from, from_len);
        return from_len;
    } else {
        return TC_CRYPTO_FAIL;
    }
}

void _set(void *to, uint8_t val, unsigned int len)
{
    (void)memset(to, val, len);
}

/*
 * Doubles the value of a byte for values up to 127.
 */
uint8_t _double_byte(uint8_t a)
{
    return ((a << 1) ^ ((a >> 7) * MASK_TWENTY_SEVEN));
}

int _compare(const uint8_t *a, const uint8_t *b, size_t size)
{
    const uint8_t *tempa = a;
    const uint8_t *tempb = b;
    uint8_t result = 0;

    for (unsigned int i = 0; i < size; i++) {
        result |= tempa[i] ^ tempb[i];
    }
    return result;
}
