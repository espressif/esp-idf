/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32h2.rom.newlib.ld for esp32h2
 *
 *
 * Generated from ./target/esp32h2/interface-esp32h2.yml md5sum c0ad4e113e5b29bb9d799f10f03edbc1
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib
 ***************************************/

/* Functions */
esp_rom_newlib_init_common_mutexes = 0x4000049c;
memset = 0x400004a0;
memcpy = 0x400004a4;
memmove = 0x400004a8;
memcmp = 0x400004ac;
strcpy = 0x400004b0;
strncpy = 0x400004b4;
strcmp = 0x400004b8;
strncmp = 0x400004bc;
strlen = 0x400004c0;
strstr = 0x400004c4;
bzero = 0x400004c8;
_isatty_r = 0x400004cc;
sbrk = 0x400004d0;
isalnum = 0x400004d4;
isalpha = 0x400004d8;
isascii = 0x400004dc;
isblank = 0x400004e0;
iscntrl = 0x400004e4;
isdigit = 0x400004e8;
islower = 0x400004ec;
isgraph = 0x400004f0;
isprint = 0x400004f4;
ispunct = 0x400004f8;
isspace = 0x400004fc;
isupper = 0x40000500;
toupper = 0x40000504;
tolower = 0x40000508;
toascii = 0x4000050c;
memccpy = 0x40000510;
memchr = 0x40000514;
memrchr = 0x40000518;
strcasecmp = 0x4000051c;
strcasestr = 0x40000520;
strcat = 0x40000524;
strdup = 0x40000528;
strchr = 0x4000052c;
strcspn = 0x40000530;
strcoll = 0x40000534;
strlcat = 0x40000538;
strlcpy = 0x4000053c;
strlwr = 0x40000540;
strncasecmp = 0x40000544;
strncat = 0x40000548;
strndup = 0x4000054c;
strnlen = 0x40000550;
strrchr = 0x40000554;
strsep = 0x40000558;
strspn = 0x4000055c;
strtok_r = 0x40000560;
strupr = 0x40000564;
longjmp = 0x40000568;
setjmp = 0x4000056c;
abs = 0x40000570;
div = 0x40000574;
labs = 0x40000578;
ldiv = 0x4000057c;
qsort = 0x40000580;
rand_r = 0x40000584;
rand = 0x40000588;
srand = 0x4000058c;
utoa = 0x40000590;
itoa = 0x40000594;
atoi = 0x40000598;
atol = 0x4000059c;
strtol = 0x400005a0;
strtoul = 0x400005a4;
fflush = 0x400005a8;
_fflush_r = 0x400005ac;
_fwalk = 0x400005b0;
_fwalk_reent = 0x400005b4;
__smakebuf_r = 0x400005b8;
__swhatbuf_r = 0x400005bc;
__swbuf_r = 0x400005c0;
__swbuf = 0x400005c4;
__swsetup_r = 0x400005c8;
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x4084ffd4;
_global_impure_ptr = 0x4084ffd0;
