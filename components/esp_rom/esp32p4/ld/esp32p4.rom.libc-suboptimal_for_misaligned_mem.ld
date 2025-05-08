/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* These functions are not well optimized for misaligned memory access.
 * See details in the https://sourceware.org/pipermail/newlib/2025/021470.html
 * patch series.  */
memcpy = 0x4fc0026c;
memmove = 0x4fc00270;
memcmp = 0x4fc00274;
strcpy = 0x4fc00278;
strncpy = 0x4fc0027c;
strcmp = 0x4fc00280;
strncmp = 0x4fc00284;
