/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
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
 * THIS FILE WAS AUTOMATICALLY GENERATED. !!! BUT EDITED !!!
 * The file was originally generated for use with newlib, but it was split into
 * multiple files to make it compatible with picolibc.
 */


/***************************************
 Group newlib
 ***************************************/

/* Functions */
_isatty_r = 0x400004b4;
strdup = 0x40000510;
strndup = 0x40000534;
rand_r = 0x4000056c;
rand = 0x40000570;
srand = 0x40000574;
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
__errno = 0x40000670;
