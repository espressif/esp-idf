/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "stdint.h"

void esp_ipc_isr_waiting_for_finish_cmd(void* ipc_isr_finish_cmd)
{
    while (*(volatile uint32_t *)ipc_isr_finish_cmd == 0) { };
}
