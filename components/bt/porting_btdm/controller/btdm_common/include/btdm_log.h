/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BTDM_LOG_H_
#define _BTDM_LOG_H_

int btdm_log_init(void);

/* Close admission and wait for writers before controller task deletion. */
void btdm_log_pause(void);

void btdm_log_deinit(void);

#endif
