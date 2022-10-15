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

_isatty_r = 0x40000380;
PROVIDE( __smakebuf_r = 0x4000046c );
PROVIDE( __swhatbuf_r = 0x40000470 );
PROVIDE( __swsetup_r = 0x4000047c );
