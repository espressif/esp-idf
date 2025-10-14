/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
esp_rom_newlib_init_common_mutexes = 0x40000494;
memset = 0x40000498;
strlen = 0x400004b8;
strstr = 0x400004bc;
bzero = 0x400004c0;
sbrk = 0x400004c8;
isalnum = 0x400004cc;
isalpha = 0x400004d0;
isascii = 0x400004d4;
isblank = 0x400004d8;
iscntrl = 0x400004dc;
isdigit = 0x400004e0;
islower = 0x400004e4;
isgraph = 0x400004e8;
isprint = 0x400004ec;
ispunct = 0x400004f0;
isspace = 0x400004f4;
isupper = 0x400004f8;
toupper = 0x400004fc;
tolower = 0x40000500;
toascii = 0x40000504;
memccpy = 0x40000508;
memchr = 0x4000050c;
memrchr = 0x40000510;
strcasecmp = 0x40000514;
strcasestr = 0x40000518;
strcat = 0x4000051c;
strchr = 0x40000524;
strcspn = 0x40000528;
strcoll = 0x4000052c;
strlcat = 0x40000530;
strlcpy = 0x40000534;
strlwr = 0x40000538;
strncasecmp = 0x4000053c;
strncat = 0x40000540;
strnlen = 0x40000548;
strrchr = 0x4000054c;
strsep = 0x40000550;
strspn = 0x40000554;
strtok_r = 0x40000558;
strupr = 0x4000055c;
longjmp = 0x40000560;
setjmp = 0x40000564;
abs = 0x40000568;
div = 0x4000056c;
labs = 0x40000570;
ldiv = 0x40000574;
qsort = 0x40000578;
utoa = 0x40000588;
itoa = 0x4000058c;
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x4084ffd4;
_global_impure_ptr = 0x4084ffd0;
