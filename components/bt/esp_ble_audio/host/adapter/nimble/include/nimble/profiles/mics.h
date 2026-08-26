/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_NIMBLE_PROFILE_MICS_H_
#define HOST_NIMBLE_PROFILE_MICS_H_

#include <stdint.h>
#include <stdbool.h>

#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/audio/micp.h>

#ifdef __cplusplus
extern "C" {
#endif

int bt_le_nimble_mics_attr_handle_set(void);

int bt_le_nimble_mics_init(void *micp_inc);

#ifdef __cplusplus
}
#endif

#endif /* HOST_NIMBLE_PROFILE_MICS_H_ */
