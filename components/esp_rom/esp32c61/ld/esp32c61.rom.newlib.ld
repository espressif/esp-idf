/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c6lite.rom.newlib.ld for esp32c6lite
 *
 *
 * Generated from ./target/esp32c6lite/interface-esp32c6lite.yml md5sum d875746dfc29a51e6d327cc923b7a095
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib
 ***************************************/

/* Functions */
esp_rom_newlib_init_common_mutexes = 0x400004b4;
memset = 0x400004b8;
memcpy = 0x400004bc;
memmove = 0x400004c0;
memcmp = 0x400004c4;
strcpy = 0x400004c8;
strncpy = 0x400004cc;
strcmp = 0x400004d0;
strncmp = 0x400004d4;
strlen = 0x400004d8;
strstr = 0x400004dc;
bzero = 0x400004e0;
_isatty_r = 0x400004e4;
sbrk = 0x400004e8;
isalnum = 0x400004ec;
isalpha = 0x400004f0;
isascii = 0x400004f4;
isblank = 0x400004f8;
iscntrl = 0x400004fc;
isdigit = 0x40000500;
islower = 0x40000504;
isgraph = 0x40000508;
isprint = 0x4000050c;
ispunct = 0x40000510;
isspace = 0x40000514;
isupper = 0x40000518;
toupper = 0x4000051c;
tolower = 0x40000520;
toascii = 0x40000524;
memccpy = 0x40000528;
memchr = 0x4000052c;
memrchr = 0x40000530;
strcasecmp = 0x40000534;
strcasestr = 0x40000538;
strcat = 0x4000053c;
strdup = 0x40000540;
strchr = 0x40000544;
strcspn = 0x40000548;
strcoll = 0x4000054c;
strlcat = 0x40000550;
strlcpy = 0x40000554;
strlwr = 0x40000558;
strncasecmp = 0x4000055c;
strncat = 0x40000560;
strndup = 0x40000564;
strnlen = 0x40000568;
strrchr = 0x4000056c;
strsep = 0x40000570;
strspn = 0x40000574;
strtok_r = 0x40000578;
strupr = 0x4000057c;
longjmp = 0x40000580;
setjmp = 0x40000584;
abs = 0x40000588;
div = 0x4000058c;
labs = 0x40000590;
ldiv = 0x40000594;
qsort = 0x40000598;
rand_r = 0x4000059c;
rand = 0x400005a0;
srand = 0x400005a4;
utoa = 0x400005a8;
itoa = 0x400005ac;
atoi = 0x400005b0;
atol = 0x400005b4;
strtol = 0x400005b8;
strtoul = 0x400005bc;
fflush = 0x400005c0;
_fflush_r = 0x400005c4;
_fwalk = 0x400005c8;
_fwalk_reent = 0x400005cc;
__smakebuf_r = 0x400005d0;
__swhatbuf_r = 0x400005d4;
__swbuf_r = 0x400005d8;
__swbuf = 0x400005dc;
__swsetup_r = 0x400005e0;
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x4084ffd4;
_global_impure_ptr = 0x4084ffd0;
