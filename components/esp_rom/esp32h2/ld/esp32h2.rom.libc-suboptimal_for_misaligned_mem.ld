/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* These functions are not well optimized for misaligned memory access.
 * See details in the https://sourceware.org/pipermail/newlib/2025/021470.html
 * patch series.  */
memcpy = 0x400004a4;
memmove = 0x400004a8;
memcmp = 0x400004ac;
strcpy = 0x400004b0;
strncpy = 0x400004b4;
strcmp = 0x400004b8;
strncmp = 0x400004bc;
