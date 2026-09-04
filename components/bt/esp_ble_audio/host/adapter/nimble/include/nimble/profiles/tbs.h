/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_NIMBLE_PROFILE_TBS_H_
#define HOST_NIMBLE_PROFILE_TBS_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int bt_le_nimble_gtbs_attr_handle_set(void);

int bt_le_nimble_gtbs_init(void);

int bt_le_nimble_gtbs_deinit(void);

int bt_le_nimble_tbs_attr_handle_set(void);

int bt_le_nimble_tbs_init(void);

int bt_le_nimble_tbs_deinit(uint8_t bearer_index);

#ifdef __cplusplus
}
#endif

#endif /* HOST_NIMBLE_PROFILE_TBS_H_ */
