/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void ets_apb_backup_init_lock_func(void(* _apb_backup_lock)(void), void(* _apb_backup_unlock)(void));

#ifdef __cplusplus
}
#endif
