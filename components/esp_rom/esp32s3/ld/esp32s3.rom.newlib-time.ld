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

_isatty_r = 0x4000126c;
PROVIDE( __smakebuf_r = 0x40001530 );
PROVIDE( __swsetup_r = 0x40001560 );
PROVIDE( __swhatbuf_r = 0x4000153c );
