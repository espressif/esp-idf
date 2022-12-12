/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32h4.rom.newlib.ld for esp32h4
 *
 *
 * Generated from ./target/esp32h4/interface-esp32h4.yml md5sum da4c474a48c097d4ac9acad67f70fda6
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
__swbuf_r = 0x400005b8;
__swbuf = 0x400005bc;
_strtod_l = 0x400005c4;
_strtod_r = 0x400005c8;
strtod_l = 0x400005cc;
strtod = 0x400005d0;
strtof_l = 0x400005d4;
strtof = 0x400005d8;
_strtol_r = 0x400005dc;
strtol_l = 0x400005e0;
_strtoul_r = 0x400005e4;
strtoul_l = 0x400005e8;
__match = 0x400005ec;
__hexnan = 0x400005f0;
__hexdig_fun = 0x400005f4;
__gethex = 0x400005f8;
_Balloc = 0x400005fc;
_Bfree = 0x40000600;
__multadd = 0x40000604;
__s2b = 0x40000608;
__hi0bits = 0x4000060c;
__lo0bits = 0x40000610;
__i2b = 0x40000614;
__multiply = 0x40000618;
__pow5mult = 0x4000061c;
__lshift = 0x40000620;
__mcmp = 0x40000624;
__mdiff = 0x40000628;
__ulp = 0x4000062c;
__b2d = 0x40000630;
__d2b = 0x40000634;
__ratio = 0x40000638;
_mprec_log10 = 0x4000063c;
__copybits = 0x40000640;
__any_on = 0x40000644;
asctime = 0x40000648;
asctime_r = 0x4000064c;
atof = 0x40000650;
atoff = 0x40000654;
_dtoa_r = 0x40000658;
_wctomb_r = 0x4000065c;
__ascii_wctomb = 0x40000660;
_mbtowc_r = 0x40000664;
__ascii_mbtowc = 0x40000668;
puts = 0x4000066c;
putc = 0x40000670;
putchar = 0x40000674;
nan = 0x40000678;
nanf = 0x4000067c;
__errno = 0x40000680;
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x3fcdffd4;
_global_impure_ptr = 0x3fcdffd0;
