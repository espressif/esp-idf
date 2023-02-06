/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c2.rom.newlib.ld for esp32c2
 *
 *
 * Generated from ./interface-esp32c2.yml md5sum c679b6ed5e9f0a9c3e7b93e5e0f2a1a3
 *
 * Compatible with ROM where ECO version equal or greater to 1.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib
 ***************************************/

/* Functions */
esp_rom_newlib_init_common_mutexes = 0x40000484;
memset = 0x40000488;
memcpy = 0x4000048c;
memmove = 0x40000490;
memcmp = 0x40000494;
strcpy = 0x40000498;
strncpy = 0x4000049c;
strcmp = 0x400004a0;
strncmp = 0x400004a4;
strlen = 0x400004a8;
strstr = 0x400004ac;
bzero = 0x400004b0;
_isatty_r = 0x400004b4;
sbrk = 0x400004b8;
isalnum = 0x400004bc;
isalpha = 0x400004c0;
isascii = 0x400004c4;
isblank = 0x400004c8;
iscntrl = 0x400004cc;
isdigit = 0x400004d0;
islower = 0x400004d4;
isgraph = 0x400004d8;
isprint = 0x400004dc;
ispunct = 0x400004e0;
isspace = 0x400004e4;
isupper = 0x400004e8;
toupper = 0x400004ec;
tolower = 0x400004f0;
toascii = 0x400004f4;
memccpy = 0x400004f8;
memchr = 0x400004fc;
memrchr = 0x40000500;
strcasecmp = 0x40000504;
strcasestr = 0x40000508;
strcat = 0x4000050c;
strdup = 0x40000510;
strchr = 0x40000514;
strcspn = 0x40000518;
strcoll = 0x4000051c;
strlcat = 0x40000520;
strlcpy = 0x40000524;
strlwr = 0x40000528;
strncasecmp = 0x4000052c;
strncat = 0x40000530;
strndup = 0x40000534;
strnlen = 0x40000538;
strrchr = 0x4000053c;
strsep = 0x40000540;
strspn = 0x40000544;
strtok_r = 0x40000548;
strupr = 0x4000054c;
longjmp = 0x40000550;
setjmp = 0x40000554;
abs = 0x40000558;
div = 0x4000055c;
labs = 0x40000560;
ldiv = 0x40000564;
qsort = 0x40000568;
rand_r = 0x4000056c;
rand = 0x40000570;
srand = 0x40000574;
utoa = 0x40000578;
itoa = 0x4000057c;
atoi = 0x40000580;
atol = 0x40000584;
strtol = 0x40000588;
strtoul = 0x4000058c;
fflush = 0x40000590;
_fflush_r = 0x40000594;
_fwalk = 0x40000598;
_fwalk_reent = 0x4000059c;
__smakebuf_r = 0x400005a0;
__swhatbuf_r = 0x400005a4;
__swbuf_r = 0x400005a8;
__swbuf = 0x400005ac;
__swsetup_r = 0x400005b0;
_strtod_l = 0x400005b4;
_strtod_r = 0x400005b8;
strtod_l = 0x400005bc;
strtod = 0x400005c0;
strtof_l = 0x400005c4;
strtof = 0x400005c8;
_strtol_r = 0x400005cc;
strtol_l = 0x400005d0;
_strtoul_r = 0x400005d4;
strtoul_l = 0x400005d8;
__match = 0x400005dc;
__hexnan = 0x400005e0;
__hexdig_fun = 0x400005e4;
__gethex = 0x400005e8;
_Balloc = 0x400005ec;
_Bfree = 0x400005f0;
__multadd = 0x400005f4;
__s2b = 0x400005f8;
__hi0bits = 0x400005fc;
__lo0bits = 0x40000600;
__i2b = 0x40000604;
__multiply = 0x40000608;
__pow5mult = 0x4000060c;
__lshift = 0x40000610;
__mcmp = 0x40000614;
__mdiff = 0x40000618;
__ulp = 0x4000061c;
__b2d = 0x40000620;
__d2b = 0x40000624;
__ratio = 0x40000628;
_mprec_log10 = 0x4000062c;
__copybits = 0x40000630;
__any_on = 0x40000634;
asctime = 0x40000638;
asctime_r = 0x4000063c;
atof = 0x40000640;
atoff = 0x40000644;
_dtoa_r = 0x40000648;
_wctomb_r = 0x4000064c;
__ascii_wctomb = 0x40000650;
_mbtowc_r = 0x40000654;
__ascii_mbtowc = 0x40000658;
puts = 0x4000065c;
putc = 0x40000660;
putchar = 0x40000664;
nan = 0x40000668;
nanf = 0x4000066c;
__errno = 0x40000670;
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x3fcdffd8;
_global_impure_ptr = 0x3fcdffd4;
