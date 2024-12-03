/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * These are the newlib functions present in ESP32-S2 ROM.
 * See also esp32s2.rom.newlib-data.ld for the list of .data/.bss symbols used by these functions.

 * Unlike other ROM functions which are exported using PROVIDE, which declare weak symbols,
 * newlib related functions are exported using assignment, which declares strong symbols.
 * This is done so that ROM functions are always used instead of the ones provided by libc.a.
 */

abs = 0x40000618;
PROVIDE ( __assert = 0x4001a430 );
PROVIDE ( __assert_func = 0x4001a408 );
bzero = 0x400078c8;
div = 0x40000620;
isalnum = 0x400078d8;
isalpha = 0x400078e8;
isascii = 0x4001aaec;
isblank = 0x400078f8;
iscntrl = 0x40007918;
isdigit = 0x40007930;
isgraph = 0x40007968;
islower = 0x40007948;
isprint = 0x40007980;
ispunct = 0x40007994;
isspace = 0x400079ac;
isupper = 0x400079c4;
labs = 0x40000648;
ldiv = 0x40000650;
longjmp = 0x400005a4;
memccpy = 0x4001ab00;
memchr = 0x4001ab24;
memcmp = 0x4001ab40;
memcpy = 0x4001aba8;
memmove = 0x4001acb0;
memrchr = 0x4001acec;
memset = 0x4001ad3c;
qsort = 0x400006f4;
setjmp = 0x40000540;
strcat = 0x4001ad90;
strchr = 0x4001adb0;
strcmp = 0x40007be4;
strcoll = 0x40007ce8;
strcpy = 0x40007cfc;
strcspn = 0x4001adcc;
strlcat = 0x40007db8;
strlcpy = 0x4001adf8;
strlen = 0x40007e08;
strlwr = 0x40007e68;
strncasecmp = 0x40007e94;
strncat = 0x4001ae34;
strncmp = 0x4001ae64;
strncpy = 0x40007f20;
strnlen = 0x4001ae9c;
strrchr = 0x40008040;
strsep = 0x4000806c;
strspn = 0x4001aebc;
strstr = 0x4001aee8;
__strtok_r = 0x4001af18;
strtok_r = 0x4001af7c;
strupr = 0x40008084;
toascii = 0x4001af90;
tolower = 0x40008158;
toupper = 0x40008174;
