/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_NIMBLE_PROFILE_CAS_H_
#define HOST_NIMBLE_PROFILE_CAS_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int bt_le_nimble_cas_attr_handle_set(void);

int bt_le_nimble_cas_init(void *csis_svc);

#ifdef __cplusplus
}
#endif

#endif /* HOST_NIMBLE_PROFILE_CAS_H_ */
