/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "obex_tl.h"

#if (OBEX_INCLUDED == TRUE && RFCOMM_INCLUDED == TRUE)

#define OBEX_TL_RFCOMM_BT_HDR_MIN_OFFSET        18      /* RFCOMM_MIN_OFFSET + L2CAP_MIN_OFFSET */
#define OBEX_TL_RFCOMM_BT_HDR_RESERVE_LEN       1       /* reserve 1 byte for rfcomm fcs */

tOBEX_TL_OPS *obex_tl_rfcomm_ops_get(void);

#endif /* #if (OBEX_INCLUDED == TRUE && RFCOMM_INCLUDED == TRUE) */
