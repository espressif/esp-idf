/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

SECTIONS
{
    .esp_err_msg_tbl :
    {
        PROVIDE(_esp_err_msg_tbl_start = .);
        KEEP(*(SORT(.esp_err_msg_tbl*)))
        PROVIDE(_esp_err_msg_tbl_end = .);
    }
}
INSERT AFTER .rodata;
