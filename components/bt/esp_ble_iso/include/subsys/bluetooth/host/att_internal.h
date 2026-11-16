/* att_internal.h - Attribute protocol handling */

/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <zephyr/bluetooth/l2cap.h>
#include <zephyr/sys/util.h>

/* EATT is not supported, so only the unenhanced ATT MTU applies */
#define BT_LOCAL_ATT_MTU_UATT   MIN(BT_L2CAP_RX_MTU, BT_L2CAP_TX_MTU)

#define BT_ATT_BUF_SIZE         BT_LOCAL_ATT_MTU_UATT
