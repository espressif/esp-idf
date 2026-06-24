/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* These functions are not well optimized for misaligned memory access.
 * See details in the https://sourceware.org/pipermail/newlib/2025/021470.html
 * patch series.  */
memcpy = 0x400004bc;
memmove = 0x400004c0;
memcmp = 0x400004c4;
strcpy = 0x400004c8;
strncpy = 0x400004cc;
strcmp = 0x400004d0;
strncmp = 0x400004d4;
