/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
esp_rom_newlib_init_common_mutexes = 0x4000049c;
memset = 0x400004a0;
strlen = 0x400004c0;
strstr = 0x400004c4;
bzero = 0x400004c8;
sbrk = 0x400004d0;
isascii = 0x400004dc;
isblank = 0x400004e0;
iscntrl = 0x400004e4;
isgraph = 0x400004f0;
isprint = 0x400004f4;
ispunct = 0x400004f8;
toascii = 0x4000050c;
memccpy = 0x40000510;
memchr = 0x40000514;
memrchr = 0x40000518;
strcasestr = 0x40000520;
strcat = 0x40000524;
strchr = 0x4000052c;
strcspn = 0x40000530;
strlcat = 0x40000538;
strlcpy = 0x4000053c;
strncat = 0x40000548;
strnlen = 0x40000550;
strrchr = 0x40000554;
strsep = 0x40000558;
strspn = 0x4000055c;
strtok_r = 0x40000560;
longjmp = 0x40000568;
setjmp = 0x4000056c;
abs = 0x40000570;
div = 0x40000574;
labs = 0x40000578;
ldiv = 0x4000057c;
qsort = 0x40000580;
utoa = 0x40000590;
itoa = 0x40000594;
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x4084ffd4;
_global_impure_ptr = 0x4084ffd0;
