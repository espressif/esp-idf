/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

SECTIONS
{
  .eh_frame_hdr   :
    {
        __eh_frame_hdr = ABSOLUTE(.);
        *(.eh_frame_hdr) *(.eh_frame_entry .eh_frame_entry.*)
    }
  .eh_frame       : ONLY_IF_RO
    {
        __eh_frame = ABSOLUTE(.);
        KEEP (*(.eh_frame)) *(.eh_frame.*)
    }
}

INSERT BEFORE .gcc_except_table;
