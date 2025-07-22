/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_bit_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Number of paths for each supported APM controller */
#define APM_CTRL_HP_APM_PATH_NUM          (4)
#define APM_CTRL_LP_APM_PATH_NUM          (1)
/* Number of regions for each supported APM controller */
#define APM_CTRL_HP_APM_REGION_NUM        (16)
#define APM_CTRL_LP_APM_REGION_NUM        (2)

/* Register offset for TEE mode control */
#define APM_TEE_MODE_CTRL_OFFSET        (0x04)
/* Register offset between region bound address registers */
#define APM_REGION_ADDR_OFFSET          (0x0C)
/* Register offset between region pms attribute registers */
#define APM_REGION_PMS_ATTR_OFFSET      (0x0C)
/* Register offset between exception info registers */
#define APM_EXCP_INFO_OFFSET            (0x10)

/* Bit to clear exception status */
#define APM_EXCP_STATUS_CLR_BIT         (BIT(0))

/* APM controller masters mask */
#define APM_MASTER_MASK_ALL             (0x03CD0075U)

#ifdef __cplusplus
}
#endif
