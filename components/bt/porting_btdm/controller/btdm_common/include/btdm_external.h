/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BTDM_EXTERNAL_H_
#define _BTDM_EXTERNAL_H_

#include <stdbool.h>
#include <stdint.h>

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BTDM_EXTERNAL_WR_FUNC(f)        wr_ ## f

#define btdm_external_bb_get_tx_pwr_table       BTDM_EXTERNAL_WR_FUNC(btdm_external_bb_get_tx_pwr_table)

/*
 * Function declarations for BTDM EXTERNAL
 */
const int8_t *wr_btdm_external_bb_get_tx_pwr_table(uint8_t *length, uint8_t modem_cfg);

int btdm_external_init(void);
void btdm_external_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _BTDM_EXTERNAL_H_ */
