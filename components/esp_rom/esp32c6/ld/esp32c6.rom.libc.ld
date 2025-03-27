/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
esp_rom_newlib_init_common_mutexes = 0x400004a4;
memset = 0x400004a8;
strlen = 0x400004c8;
strstr = 0x400004cc;
bzero = 0x400004d0;
sbrk = 0x400004d8;
isalnum = 0x400004dc;
isalpha = 0x400004e0;
isascii = 0x400004e4;
isblank = 0x400004e8;
iscntrl = 0x400004ec;
isdigit = 0x400004f0;
islower = 0x400004f4;
isgraph = 0x400004f8;
isprint = 0x400004fc;
ispunct = 0x40000500;
isspace = 0x40000504;
isupper = 0x40000508;
toupper = 0x4000050c;
tolower = 0x40000510;
toascii = 0x40000514;
memccpy = 0x40000518;
memchr = 0x4000051c;
memrchr = 0x40000520;
strcasecmp = 0x40000524;
strcasestr = 0x40000528;
strcat = 0x4000052c;
strchr = 0x40000534;
strcspn = 0x40000538;
strcoll = 0x4000053c;
strlcat = 0x40000540;
strlcpy = 0x40000544;
strlwr = 0x40000548;
strncasecmp = 0x4000054c;
strncat = 0x40000550;
strnlen = 0x40000558;
strrchr = 0x4000055c;
strsep = 0x40000560;
strspn = 0x40000564;
strtok_r = 0x40000568;
strupr = 0x4000056c;
longjmp = 0x40000570;
setjmp = 0x40000574;
abs = 0x40000578;
div = 0x4000057c;
labs = 0x40000580;
ldiv = 0x40000584;
qsort = 0x40000588;
utoa = 0x40000598;
itoa = 0x4000059c;
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x4087ffd4;
_global_impure_ptr = 0x4087ffd0;
