/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_COMMON_HOST_H_
#define HOST_COMMON_HOST_H_

#include <stdint.h>

#include <zephyr/autoconf.h>
#include <zephyr/bluetooth/iso.h>
#include <zephyr/bluetooth/bluetooth.h>

#include "common/adv.h"
#include "common/conn.h"
#include "common/iso.h"
#include "common/l2cap.h"
#include "common/scan.h"
#include "common/gatt.h"
#include "common/task.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HOST_LOCK_DEBUG     0

#if HOST_LOCK_DEBUG
void bt_le_host_lock_debug(const char *func, int line);
void bt_le_host_unlock_debug(const char *func, int line);

#define bt_le_host_lock(...)    bt_le_host_lock_debug(__func__, __LINE__)
#define bt_le_host_unlock(...)  bt_le_host_unlock_debug(__func__, __LINE__)
#else /* HOST_LOCK_DEBUG */
void bt_le_host_lock(void);
void bt_le_host_unlock(void);
#endif /* HOST_LOCK_DEBUG */

int bt_le_host_init(void);

void bt_le_host_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* HOST_COMMON_HOST_H_ */
