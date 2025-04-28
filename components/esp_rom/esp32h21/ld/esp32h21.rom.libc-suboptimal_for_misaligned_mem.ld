/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* These functions are not well optimized for misaligned memory access.
 * See details in the https://sourceware.org/pipermail/newlib/2025/021470.html
 * patch series.  */
memcpy = 0x4000049c;
memmove = 0x400004a0;
memcmp = 0x400004a4;
strcpy = 0x400004a8;
strncpy = 0x400004ac;
strcmp = 0x400004b0;
strncmp = 0x400004b4;
