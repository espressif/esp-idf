/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32h4.rom.libc.ld for esp32h4
 *
 *
 * Generated from ../../rom/target/esp32h4/interface-esp32h4.yml md5sum bb9ee317148d3c5b10610936a3675a65
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group libc
 ***************************************/

/* Functions */
esp_rom_newlib_init_common_mutexes = 0x40000494;
memset = 0x40000498;
memcpy = 0x4000049c;
memmove = 0x400004a0;
memcmp = 0x400004a4;
strcpy = 0x400004a8;
strncpy = 0x400004ac;
strcmp = 0x400004b0;
strncmp = 0x400004b4;
strlen = 0x400004b8;
strstr = 0x400004bc;
bzero = 0x400004c0;
sbrk = 0x400004c4;
isalnum = 0x400004c8;
isalpha = 0x400004cc;
isascii = 0x400004d0;
isblank = 0x400004d4;
iscntrl = 0x400004d8;
isdigit = 0x400004dc;
islower = 0x400004e0;
isgraph = 0x400004e4;
isprint = 0x400004e8;
ispunct = 0x400004ec;
isspace = 0x400004f0;
isupper = 0x400004f4;
toupper = 0x400004f8;
tolower = 0x400004fc;
toascii = 0x40000500;
memccpy = 0x40000504;
memchr = 0x40000508;
memrchr = 0x4000050c;
strcasecmp = 0x40000510;
strcasestr = 0x40000514;
strcat = 0x40000518;
strchr = 0x4000051c;
strcspn = 0x40000520;
strcoll = 0x40000524;
strlcat = 0x40000528;
strlcpy = 0x4000052c;
strlwr = 0x40000530;
strncasecmp = 0x40000534;
strncat = 0x40000538;
strnlen = 0x4000053c;
strrchr = 0x40000540;
strsep = 0x40000544;
strspn = 0x40000548;
strtok_r = 0x4000054c;
strupr = 0x40000550;
longjmp = 0x40000554;
setjmp = 0x40000558;
abs = 0x4000055c;
div = 0x40000560;
labs = 0x40000564;
ldiv = 0x40000568;
qsort = 0x4000056c;
utoa = 0x40000570;
itoa = 0x40000574;
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x4085ffd0;
_global_impure_ptr = 0x4085ffcc;
