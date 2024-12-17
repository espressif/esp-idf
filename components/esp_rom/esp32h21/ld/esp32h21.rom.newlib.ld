/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32h21.rom.newlib.ld for esp32h21
 *
 *
 * Generated from ./target/esp32h21/interface-esp32h21.yml md5sum dee73289acb146ae14dd9544143527f0
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib
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
_isatty_r = 0x400004c4;
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
strdup = 0x40000520;
strchr = 0x40000524;
strcspn = 0x40000528;
strcoll = 0x4000052c;
strlcat = 0x40000530;
strlcpy = 0x40000534;
strlwr = 0x40000538;
strncasecmp = 0x4000053c;
strncat = 0x40000540;
strndup = 0x40000544;
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
rand_r = 0x4000057c;
rand = 0x40000580;
srand = 0x40000584;
utoa = 0x40000588;
itoa = 0x4000058c;
atoi = 0x40000590;
atol = 0x40000594;
strtol = 0x40000598;
strtoul = 0x4000059c;
fflush = 0x400005a0;
_fflush_r = 0x400005a4;
_fwalk = 0x400005a8;
_fwalk_reent = 0x400005ac;
__smakebuf_r = 0x400005b0;
__swhatbuf_r = 0x400005b4;
__swbuf_r = 0x400005b8;
__swbuf = 0x400005bc;
__swsetup_r = 0x400005c0;
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x4084ffd4;
_global_impure_ptr = 0x4084ffd0;
