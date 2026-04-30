/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/byteorder.h>

#include <zephyr/bluetooth/uuid.h>

#define UUID_16_BASE_OFFSET 12

/* TODO: Decide whether to continue using BLE format or switch to RFC 4122 */

/* Base UUID : 0000[0000]-0000-1000-8000-00805F9B34FB
 * 0x2800    : 0000[2800]-0000-1000-8000-00805F9B34FB
 *  little endian 0x2800 : [00 28] -> no swapping required
 *  big endian 0x2800    : [28 00] -> swapping required
 */
static const struct bt_uuid_128 uuid128_base = {
    .uuid = { BT_UUID_TYPE_128 },
    .val = {
        BT_UUID_128_ENCODE(
            0x00000000, 0x0000, 0x1000, 0x8000, 0x00805F9B34FB)
    }
};

static void uuid_to_uuid128(const struct bt_uuid *src, struct bt_uuid_128 *dst)
{
    switch (src->type) {
    case BT_UUID_TYPE_16:
        *dst = uuid128_base;
        sys_put_le16(BT_UUID_16(src)->val,
                     &dst->val[UUID_16_BASE_OFFSET]);
        return;
    case BT_UUID_TYPE_32:
        *dst = uuid128_base;
        sys_put_le32(BT_UUID_32(src)->val,
                     &dst->val[UUID_16_BASE_OFFSET]);
        return;
    case BT_UUID_TYPE_128:
        memcpy(dst, src, sizeof(*dst));
        return;
    default:
        memset(dst, 0, sizeof(*dst));
        return;
    }
}

static int uuid128_cmp(const struct bt_uuid *u1, const struct bt_uuid *u2)
{
    struct bt_uuid_128 uuid1, uuid2;

    uuid_to_uuid128(u1, &uuid1);
    uuid_to_uuid128(u2, &uuid2);

    return memcmp(uuid1.val, uuid2.val, 16);
}

int bt_uuid_cmp(const struct bt_uuid *u1, const struct bt_uuid *u2)
{
    /* Convert to 128 bit if types don't match */
    if (u1->type != u2->type) {
        return uuid128_cmp(u1, u2);
    }

    switch (u1->type) {
    case BT_UUID_TYPE_16:
        return (int)BT_UUID_16(u1)->val - (int)BT_UUID_16(u2)->val;
    case BT_UUID_TYPE_32:
        if (BT_UUID_32(u1)->val > BT_UUID_32(u2)->val) {
            return 1;
        } else if (BT_UUID_32(u1)->val < BT_UUID_32(u2)->val) {
            return -1;
        }
        return 0;
    case BT_UUID_TYPE_128:
        return memcmp(BT_UUID_128(u1)->val, BT_UUID_128(u2)->val, 16);
    }

    return -EINVAL;
}

bool bt_uuid_create(struct bt_uuid *uuid, const uint8_t *data, uint8_t data_len)
{
    /* Copy UUID from packet data/internal variable to internal bt_uuid */
    switch (data_len) {
    case BT_UUID_SIZE_16:
        uuid->type = BT_UUID_TYPE_16;
        BT_UUID_16(uuid)->val = sys_get_le16(data);
        break;
    case BT_UUID_SIZE_32:
        uuid->type = BT_UUID_TYPE_32;
        BT_UUID_32(uuid)->val = sys_get_le32(data);
        break;
    case BT_UUID_SIZE_128:
        uuid->type = BT_UUID_TYPE_128;
        memcpy(&BT_UUID_128(uuid)->val, data, 16);
        break;
    default:
        return false;
    }
    return true;
}

void bt_uuid_to_str(const struct bt_uuid *uuid, char *str, size_t len)
{
    uint32_t tmp1, tmp5;
    uint16_t tmp0, tmp2, tmp3, tmp4;

    switch (uuid->type) {
    case BT_UUID_TYPE_16:
        snprintf(str, len, "%04x", BT_UUID_16(uuid)->val);
        break;
    case BT_UUID_TYPE_32:
        snprintf(str, len, "%08x", BT_UUID_32(uuid)->val);
        break;
    case BT_UUID_TYPE_128:
        memcpy(&tmp0, &BT_UUID_128(uuid)->val[0], sizeof(tmp0));
        memcpy(&tmp1, &BT_UUID_128(uuid)->val[2], sizeof(tmp1));
        memcpy(&tmp2, &BT_UUID_128(uuid)->val[6], sizeof(tmp2));
        memcpy(&tmp3, &BT_UUID_128(uuid)->val[8], sizeof(tmp3));
        memcpy(&tmp4, &BT_UUID_128(uuid)->val[10], sizeof(tmp4));
        memcpy(&tmp5, &BT_UUID_128(uuid)->val[12], sizeof(tmp5));

        snprintf(str, len, "%08x-%04x-%04x-%04x-%08x%04x",
                 sys_le32_to_cpu(tmp5), sys_le16_to_cpu(tmp4),
                 sys_le16_to_cpu(tmp3), sys_le16_to_cpu(tmp2),
                 sys_le32_to_cpu(tmp1), sys_le16_to_cpu(tmp0));
        break;
    default:
        (void)memset(str, 0, len);
        return;
    }
}
