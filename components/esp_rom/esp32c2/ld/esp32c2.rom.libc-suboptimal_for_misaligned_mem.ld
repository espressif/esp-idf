/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* These functions are not well optimized for misaligned memory access.
 * See details in the https://sourceware.org/pipermail/newlib/2025/021470.html
 * patch series.  */
memcpy = 0x4000048c;
memmove = 0x40000490;
memcmp = 0x40000494;
strcpy = 0x40000498;
strncpy = 0x4000049c;
strcmp = 0x400004a0;
strncmp = 0x400004a4;
