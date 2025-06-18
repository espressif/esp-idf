/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
esp_rom_newlib_init_common_mutexes = 0x4fc00264;
memset = 0x4fc00268;
strlen = 0x4fc00288;
strstr = 0x4fc0028c;
bzero = 0x4fc00290;
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
strchr = 0x4fc002f4;
strcspn = 0x4fc002f8;
strcoll = 0x4fc002fc;
strlcat = 0x4fc00300;
strlcpy = 0x4fc00304;
strlwr = 0x4fc00308;
strncasecmp = 0x4fc0030c;
strncat = 0x4fc00310;
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
utoa = 0x4fc00358;
itoa = 0x4fc0035c;
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x4ffbffe4;
_global_impure_ptr = 0x4ffbffe0;
