/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* These are the newlib functions present in ESP32 ROM.
   They should not be used when compiling with PSRAM cache workaround enabled.
   See also esp32.rom.newlib-data.ld for the list of .data/.bss symbols
   used by these functions, and esp32.rom.newlib-nano.ld for "nano" versions
   of printf/scanf family of functions.

   Unlike other ROM functions which are exported using PROVIDE, which declares
   weak symbols, newlib related functions are exported using assignment,
   which declares strong symbols. This is done so that ROM functions are always
   used instead of the ones provided by libc.a.

   Time functions were moved to the esp32.rom.newlib-time.ld file.
 */

abs = 0x40056340;
bzero = 0x4000c1f4;
div = 0x40056348;
__dummy_lock = 0x4000c728;
__dummy_lock_try = 0x4000c730;
isalnum = 0x40000f04;
isalpha = 0x40000f18;
isascii = 0x4000c20c;
isblank = 0x40000f2c;
iscntrl = 0x40000f50;
isdigit = 0x40000f64;
isgraph = 0x40000f94;
islower = 0x40000f78;
isprint = 0x40000fa8;
ispunct = 0x40000fc0;
isspace = 0x40000fd4;
isupper = 0x40000fe8;
__itoa = 0x40056678;
itoa = 0x400566b4;
labs = 0x40056370;
ldiv = 0x40056378;
longjmp = 0x400562cc;
memccpy = 0x4000c220;
memchr = 0x4000c244;
memcmp = 0x4000c260;
memcpy = 0x4000c2c8;
memmove = 0x4000c3c0;
memrchr = 0x4000c400;
memset = 0x4000c44c;
qsort = 0x40056424;
__sccl = 0x4000c498;
setjmp = 0x40056268;
strcasecmp = 0x400011cc;
strcasestr = 0x40001210;
strcat = 0x4000c518;
strchr = 0x4000c53c;
strcmp = 0x40001274;
strcoll = 0x40001398;
strcpy = 0x400013ac;
strcspn = 0x4000c558;
strlcat = 0x40001470;
strlcpy = 0x4000c584;
strlen = 0x400014c0;
strlwr = 0x40001524;
strncasecmp = 0x40001550;
strncat = 0x4000c5c4;
strncmp = 0x4000c5f4;
strncpy = 0x400015d4;
strnlen = 0x4000c628;
strrchr = 0x40001708;
strsep = 0x40001734;
strspn = 0x4000c648;
strstr = 0x4000c674;
strupr = 0x4000174c;
__submore = 0x40058f3c;
toascii = 0x4000c720;
tolower = 0x40001868;
toupper = 0x40001884;
__utoa = 0x400561f0;
utoa = 0x40056258;
