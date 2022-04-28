/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* These are the newlib functions and the .bss/.data symbols which are related to 'time_t'
   or other structures which include 'time_t' (like 'struct stat').
   These ROM functions were compiled with sizeof(time_t) == 4.
   When compiling with sizeof(time_t) == 8, these functions should be excluded from the build.
 */

__swsetup_r = 0x40001690;
__smakebuf_r = 0x40001954;
__swhatbuf_r = 0x400018f8;
__sfvwrite_r = 0x40001310;
_isatty_r = 0x400078a0;
