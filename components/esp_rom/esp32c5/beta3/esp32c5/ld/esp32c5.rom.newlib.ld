/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c5.rom.newlib.ld for esp32c5
 *
 *
 * Generated from ./target/esp32c5/interface-esp32c5.yml md5sum 2476337377df636dda217b0b3c1a63db
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib
 ***************************************/

/* Functions */
esp_rom_newlib_init_common_mutexes = 0x400004a0;
memset = 0x400004a4;
memcpy = 0x400004a8;
memmove = 0x400004ac;
memcmp = 0x400004b0;
strcpy = 0x400004b4;
strncpy = 0x400004b8;
strcmp = 0x400004bc;
strncmp = 0x400004c0;
strlen = 0x400004c4;
strstr = 0x400004c8;
bzero = 0x400004cc;
_isatty_r = 0x400004d0;
sbrk = 0x400004d4;
isalnum = 0x400004d8;
isalpha = 0x400004dc;
isascii = 0x400004e0;
isblank = 0x400004e4;
iscntrl = 0x400004e8;
isdigit = 0x400004ec;
islower = 0x400004f0;
isgraph = 0x400004f4;
isprint = 0x400004f8;
ispunct = 0x400004fc;
isspace = 0x40000500;
isupper = 0x40000504;
toupper = 0x40000508;
tolower = 0x4000050c;
toascii = 0x40000510;
memccpy = 0x40000514;
memchr = 0x40000518;
memrchr = 0x4000051c;
strcasecmp = 0x40000520;
strcasestr = 0x40000524;
strcat = 0x40000528;
strdup = 0x4000052c;
strchr = 0x40000530;
strcspn = 0x40000534;
strcoll = 0x40000538;
strlcat = 0x4000053c;
strlcpy = 0x40000540;
strlwr = 0x40000544;
strncasecmp = 0x40000548;
strncat = 0x4000054c;
strndup = 0x40000550;
strnlen = 0x40000554;
strrchr = 0x40000558;
strsep = 0x4000055c;
strspn = 0x40000560;
strtok_r = 0x40000564;
strupr = 0x40000568;
longjmp = 0x4000056c;
setjmp = 0x40000570;
abs = 0x40000574;
div = 0x40000578;
labs = 0x4000057c;
ldiv = 0x40000580;
qsort = 0x40000584;
rand_r = 0x40000588;
rand = 0x4000058c;
srand = 0x40000590;
utoa = 0x40000594;
itoa = 0x40000598;
atoi = 0x4000059c;
atol = 0x400005a0;
strtol = 0x400005a4;
strtoul = 0x400005a8;
fflush = 0x400005ac;
_fflush_r = 0x400005b0;
_fwalk = 0x400005b4;
_fwalk_reent = 0x400005b8;
__smakebuf_r = 0x400005bc;
__swhatbuf_r = 0x400005c0;
__swbuf_r = 0x400005c4;
__swbuf = 0x400005c8;
__swsetup_r = 0x400005cc;
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x4087ffd4;
_global_impure_ptr = 0x4087ffd0;
