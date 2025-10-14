/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "obex_tl.h"

#if (OBEX_INCLUDED == TRUE)

#define OBEX_TL_L2CAP_BT_HDR_MIN_OFFSET         13      /* equal to L2CAP_MIN_OFFSET */
#define OBEX_TL_L2CAP_BT_HDR_RESERVE_LEN        0       /* not require any additional byte */

tOBEX_TL_OPS *obex_tl_l2cap_ops_get(void);

#endif /* #if (OBEX_INCLUDED == TRUE) */
