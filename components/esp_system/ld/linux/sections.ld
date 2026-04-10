/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
SECTIONS
{
    .esp_sys_init_fn :
    {
        PROVIDE(_esp_sys_init_fn_start = .);
        KEEP(*(SORT(.esp_sys_init_fn*)))
        PROVIDE(_esp_sys_init_fn_end = .);
    }
}
INSERT AFTER .data;
