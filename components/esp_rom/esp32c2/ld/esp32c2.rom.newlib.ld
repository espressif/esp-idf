/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c2.rom.newlib.ld for esp32c2
 *
 *
 * Generated from ./interface-esp32c2.yml md5sum 6c4d0f3a9f2d0c93477024a1a8f13746
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib
 ***************************************/

/* Functions */
esp_rom_newlib_init_common_mutexes = 0x40000480;
memset = 0x40000484;
memcpy = 0x40000488;
memmove = 0x4000048c;
memcmp = 0x40000490;
strcpy = 0x40000494;
strncpy = 0x40000498;
strcmp = 0x4000049c;
strncmp = 0x400004a0;
strlen = 0x400004a4;
strstr = 0x400004a8;
bzero = 0x400004ac;
sbrk = 0x400004b4;
isalnum = 0x400004b8;
isalpha = 0x400004bc;
isascii = 0x400004c0;
isblank = 0x400004c4;
iscntrl = 0x400004c8;
isdigit = 0x400004cc;
islower = 0x400004d0;
isgraph = 0x400004d4;
isprint = 0x400004d8;
ispunct = 0x400004dc;
isspace = 0x400004e0;
isupper = 0x400004e4;
toupper = 0x400004e8;
tolower = 0x400004ec;
toascii = 0x400004f0;
memccpy = 0x400004f4;
memchr = 0x400004f8;
memrchr = 0x400004fc;
strcasecmp = 0x40000500;
strcasestr = 0x40000504;
strcat = 0x40000508;
strdup = 0x4000050c;
strchr = 0x40000510;
strcspn = 0x40000514;
strcoll = 0x40000518;
strlcat = 0x4000051c;
strlcpy = 0x40000520;
strlwr = 0x40000524;
strncasecmp = 0x40000528;
strncat = 0x4000052c;
strndup = 0x40000530;
strnlen = 0x40000534;
strrchr = 0x40000538;
strsep = 0x4000053c;
strspn = 0x40000540;
strtok_r = 0x40000544;
strupr = 0x40000548;
longjmp = 0x4000054c;
setjmp = 0x40000550;
abs = 0x40000554;
div = 0x40000558;
labs = 0x4000055c;
ldiv = 0x40000560;
qsort = 0x40000564;
rand_r = 0x40000568;
rand = 0x4000056c;
srand = 0x40000570;
utoa = 0x40000574;
itoa = 0x40000578;
atoi = 0x4000057c;
atol = 0x40000580;
strtol = 0x40000584;
strtoul = 0x40000588;
fflush = 0x4000058c;
_fflush_r = 0x40000590;
_fwalk = 0x40000594;
_fwalk_reent = 0x40000598;
__swbuf_r = 0x400005a4;
__swbuf = 0x400005a8;
_strtod_l = 0x400005b0;
_strtod_r = 0x400005b4;
strtod_l = 0x400005b8;
strtod = 0x400005bc;
strtof_l = 0x400005c0;
strtof = 0x400005c4;
_strtol_r = 0x400005c8;
strtol_l = 0x400005cc;
_strtoul_r = 0x400005d0;
strtoul_l = 0x400005d4;
__match = 0x400005d8;
__hexnan = 0x400005dc;
__hexdig_fun = 0x400005e0;
__gethex = 0x400005e4;
_Balloc = 0x400005e8;
_Bfree = 0x400005ec;
__multadd = 0x400005f0;
__s2b = 0x400005f4;
__hi0bits = 0x400005f8;
__lo0bits = 0x400005fc;
__i2b = 0x40000600;
__multiply = 0x40000604;
__pow5mult = 0x40000608;
__lshift = 0x4000060c;
__mcmp = 0x40000610;
__mdiff = 0x40000614;
__ulp = 0x40000618;
__b2d = 0x4000061c;
__d2b = 0x40000620;
__ratio = 0x40000624;
_mprec_log10 = 0x40000628;
__copybits = 0x4000062c;
__any_on = 0x40000630;
asctime = 0x40000634;
asctime_r = 0x40000638;
atof = 0x4000063c;
atoff = 0x40000640;
_dtoa_r = 0x40000644;
_wctomb_r = 0x40000648;
__ascii_wctomb = 0x4000064c;
_mbtowc_r = 0x40000650;
__ascii_mbtowc = 0x40000654;
puts = 0x40000658;
putc = 0x4000065c;
putchar = 0x40000660;
nan = 0x40000664;
nanf = 0x40000668;
__errno = 0x4000066c;
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x3fcdffd8;
_global_impure_ptr = 0x3fcdffd4;
