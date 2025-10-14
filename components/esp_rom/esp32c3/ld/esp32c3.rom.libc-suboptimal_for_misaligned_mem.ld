/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* These functions are not well optimized for misaligned memory access.
 * See details in the https://sourceware.org/pipermail/newlib/2025/021470.html
 * patch series.  */
memcpy = 0x40000358;
memmove = 0x4000035c;
memcmp = 0x40000360;
strcpy = 0x40000364;
strncpy = 0x40000368;
strcmp = 0x4000036c;
strncmp = 0x40000370;
