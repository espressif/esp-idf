/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "btdm_user_cfg.h"

#if UC_BT_CTRL_BLE_IS_ENABLE
#include "../../common/btdm_le_vs.h"
#endif // UC_BT_CTRL_BLE_IS_ENABLE

#if UC_BT_CTRL_BR_EDR_IS_ENABLE
#include "../../common/btdm_bredr_vs.h"
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
