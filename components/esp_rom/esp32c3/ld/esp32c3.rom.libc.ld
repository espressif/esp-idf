/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
esp_rom_newlib_init_common_mutexes = 0x40000350;
memset = 0x40000354;
strlen = 0x40000374;
strstr = 0x40000378;
bzero = 0x4000037c;
sbrk = 0x40000384;
isalnum = 0x40000388;
isalpha = 0x4000038c;
isascii = 0x40000390;
isblank = 0x40000394;
iscntrl = 0x40000398;
isdigit = 0x4000039c;
islower = 0x400003a0;
isgraph = 0x400003a4;
isprint = 0x400003a8;
ispunct = 0x400003ac;
isspace = 0x400003b0;
isupper = 0x400003b4;
toupper = 0x400003b8;
tolower = 0x400003bc;
toascii = 0x400003c0;
memccpy = 0x400003c4;
memchr = 0x400003c8;
memrchr = 0x400003cc;
strcasecmp = 0x400003d0;
strcasestr = 0x400003d4;
strcat = 0x400003d8;
strchr = 0x400003e0;
strcspn = 0x400003e4;
strcoll = 0x400003e8;
strlcat = 0x400003ec;
strlcpy = 0x400003f0;
strlwr = 0x400003f4;
strncasecmp = 0x400003f8;
strncat = 0x400003fc;
strnlen = 0x40000404;
strrchr = 0x40000408;
strsep = 0x4000040c;
strspn = 0x40000410;
strtok_r = 0x40000414;
strupr = 0x40000418;
longjmp = 0x4000041c;
setjmp = 0x40000420;
abs = 0x40000424;
div = 0x40000428;
labs = 0x4000042c;
ldiv = 0x40000430;
qsort = 0x40000434;
utoa = 0x40000444;
itoa = 0x40000448;
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x3fcdffe0;
_global_impure_ptr = 0x3fcdffdc;
