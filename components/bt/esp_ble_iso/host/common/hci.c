/*
 * SPDX-FileCopyrightText: 2017-2025 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileCopyrightText: 2025 NXP
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/buf.h>
#include <zephyr/bluetooth/hci.h>

#include <../host/hci_core.h>
#include <../host/iso_internal.h>

#include "common/host.h"

struct cmd_data {
    /** HCI status of the command completion */
    uint8_t  status;

    /** The command OpCode that the buffer contains */
    uint16_t opcode;

    /** The state to update when command completes with success. */
    struct bt_hci_cmd_state_set *state;
};

static struct cmd_data cmd_data;

#define cmd(buf)    (&cmd_data)

NET_BUF_POOL_FIXED_DEFINE(hci_cmd_pool, 1, 255, 0, NULL);

_IDF_ONLY
struct net_buf *bt_hci_cmd_create(uint16_t opcode, uint8_t param_len)
{
    struct bt_hci_cmd_hdr *hdr;
    struct net_buf *buf;

    LOG_DBG("HciCmdCreate[%04x][%u]", opcode, param_len);

    buf = net_buf_alloc(&hci_cmd_pool, K_NO_WAIT);
    assert(buf);

    net_buf_reserve(buf, BT_BUF_RESERVE);

    cmd(buf)->opcode = opcode;
    cmd(buf)->state = NULL;

    hdr = net_buf_add(buf, sizeof(*hdr));
    hdr->opcode = sys_cpu_to_le16(opcode);
    hdr->param_len = param_len;

    return buf;
}

_IDF_ONLY
int bt_hci_cmd_send_sync(uint16_t opcode,
                         struct net_buf *buf,
                         struct net_buf **rsp)
{
    LOG_DBG("HciCmdSendSync[%04x]", opcode);

    return bt_le_nimble_iso_cmd_send_sync(opcode, buf, rsp);
}
