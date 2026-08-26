/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_NIMBLE_PROFILE_VCS_H_
#define HOST_NIMBLE_PROFILE_VCS_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int bt_le_nimble_vcs_attr_handle_set(void);

int bt_le_nimble_vcs_init(void *vcp_inc);

#ifdef __cplusplus
}
#endif

#endif /* HOST_NIMBLE_PROFILE_VCS_H_ */
