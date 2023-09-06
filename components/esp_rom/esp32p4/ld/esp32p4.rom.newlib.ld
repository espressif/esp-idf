/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32p4.rom.newlib.ld for esp32p4
 *
 *
 * Generated from ./target/esp32p4/interface-esp32p4.yml md5sum f6516bd9708d890f63db87f8aed53ca7
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib
 ***************************************/

/* Functions */
esp_rom_newlib_init_common_mutexes = 0x4fc00264;
memset = 0x4fc00268;
memcpy = 0x4fc0026c;
memmove = 0x4fc00270;
memcmp = 0x4fc00274;
strcpy = 0x4fc00278;
strncpy = 0x4fc0027c;
strcmp = 0x4fc00280;
strncmp = 0x4fc00284;
strlen = 0x4fc00288;
strstr = 0x4fc0028c;
bzero = 0x4fc00290;
_isatty_r = 0x4fc00294;
sbrk = 0x4fc00298;
isalnum = 0x4fc0029c;
isalpha = 0x4fc002a0;
isascii = 0x4fc002a4;
isblank = 0x4fc002a8;
iscntrl = 0x4fc002ac;
isdigit = 0x4fc002b0;
islower = 0x4fc002b4;
isgraph = 0x4fc002b8;
isprint = 0x4fc002bc;
ispunct = 0x4fc002c0;
isspace = 0x4fc002c4;
isupper = 0x4fc002c8;
toupper = 0x4fc002cc;
tolower = 0x4fc002d0;
toascii = 0x4fc002d4;
memccpy = 0x4fc002d8;
memchr = 0x4fc002dc;
memrchr = 0x4fc002e0;
strcasecmp = 0x4fc002e4;
strcasestr = 0x4fc002e8;
strcat = 0x4fc002ec;
strdup = 0x4fc002f0;
strchr = 0x4fc002f4;
strcspn = 0x4fc002f8;
strcoll = 0x4fc002fc;
strlcat = 0x4fc00300;
strlcpy = 0x4fc00304;
strlwr = 0x4fc00308;
strncasecmp = 0x4fc0030c;
strncat = 0x4fc00310;
strndup = 0x4fc00314;
strnlen = 0x4fc00318;
strrchr = 0x4fc0031c;
strsep = 0x4fc00320;
strspn = 0x4fc00324;
strtok_r = 0x4fc00328;
strupr = 0x4fc0032c;
longjmp = 0x4fc00330;
setjmp = 0x4fc00334;
abs = 0x4fc00338;
div = 0x4fc0033c;
labs = 0x4fc00340;
ldiv = 0x4fc00344;
qsort = 0x4fc00348;
rand_r = 0x4fc0034c;
rand = 0x4fc00350;
srand = 0x4fc00354;
utoa = 0x4fc00358;
itoa = 0x4fc0035c;
atoi = 0x4fc00360;
atol = 0x4fc00364;
strtol = 0x4fc00368;
strtoul = 0x4fc0036c;
fflush = 0x4fc00370;
_fflush_r = 0x4fc00374;
_fwalk = 0x4fc00378;
_fwalk_reent = 0x4fc0037c;
__smakebuf_r = 0x4fc00380;
__swhatbuf_r = 0x4fc00384;
__swbuf_r = 0x4fc00388;
__swbuf = 0x4fc0038c;
__swsetup_r = 0x4fc00390;
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x4ff3ffe4;
_global_impure_ptr = 0x4ff3ffe0;
