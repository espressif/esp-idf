/*
 * SPDX-FileCopyrightText: 2017 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2016 Vinayak Kariappa Chettimada
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "mesh/types.h"
#include "mesh/utils.h"
#include "mesh/trace.h"

const char *bt_hex(const void *buf, size_t len)
{
    static const char hex[] = "0123456789abcdef";
    static char hexbufs[2][129];
    static uint8_t curbuf;
    const uint8_t *b = buf;
    char *str = NULL;
    int i;

    str = hexbufs[curbuf++];
    curbuf %= ARRAY_SIZE(hexbufs);

    len = MIN(len, (sizeof(hexbufs[0]) - 1) / 2);

    for (i = 0; i < len; i++) {
        str[i * 2]     = hex[b[i] >> 4];
        str[i * 2 + 1] = hex[b[i] & 0xf];
    }

    str[i * 2] = '\0';

    return str;
}

void mem_rcopy(uint8_t *dst, uint8_t const *src, uint16_t len)
{
    src += len;
    while (len--) {
        *dst++ = *--src;
    }
}

#ifdef CONFIG_BLE_MESH_BQB_TEST_LOG

enum BLE_MESH_BQB_TEST_FLAG_OP {
    BLE_MESH_BQB_TEST_FLAG_OP_GET = 0,
    BLE_MESH_BQB_TEST_FLAG_OP_SET,
};

static uint32_t bt_mesh_bqb_test_flag(uint8_t op, uint32_t value)
{
    static uint32_t bqb_log_flag = 0;

    switch (op) {
    case BLE_MESH_BQB_TEST_FLAG_OP_GET:
        break;
    case BLE_MESH_BQB_TEST_FLAG_OP_SET:
        bqb_log_flag = value;
        break;
    default:
        BT_ERR("Unknown BQB test flag opcode 0x%02x", op);
        break;
    }

    return bqb_log_flag;
}

uint32_t bt_mesh_bqb_test_flag_get(void)
{
    return bt_mesh_bqb_test_flag(BLE_MESH_BQB_TEST_FLAG_OP_GET, 0);
}

int bt_mesh_bqb_test_flag_set(uint32_t flag_mask)
{
    if (flag_mask > BLE_MESH_BQB_TEST_LOG_LEVEL_OUTPUT_NONE) {
        BT_ERR("Invalid BQB test flag mask 0x%08x", flag_mask);
        return -EINVAL;
    }

    return (bt_mesh_bqb_test_flag(BLE_MESH_BQB_TEST_FLAG_OP_SET, flag_mask) == flag_mask) ? 0 : -EINVAL;
}

bool bt_mesh_bqb_test_flag_check(uint32_t flag_mask)
{
    if (flag_mask > BLE_MESH_BQB_TEST_LOG_LEVEL_OUTPUT_NONE) {
        BT_ERR("Invalid BQB test flag mask 0x%08x", flag_mask);
        return false;
    }

    return ((bt_mesh_bqb_test_flag_get() & flag_mask) == flag_mask);
}

#endif /* CONFIG_BLE_MESH_BQB_TEST_LOG */
