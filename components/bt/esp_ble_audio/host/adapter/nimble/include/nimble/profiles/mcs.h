/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_NIMBLE_PROFILE_MCS_H_
#define HOST_NIMBLE_PROFILE_MCS_H_

#include <stdint.h>
#include <stdbool.h>

#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/audio/mcs.h>

#ifdef __cplusplus
extern "C" {
#endif

int bt_le_nimble_gmcs_attr_handle_set(void);

int bt_le_nimble_gmcs_init(bool ots_included);

#ifdef __cplusplus
}
#endif

#endif /* HOST_NIMBLE_PROFILE_MCS_H_ */
