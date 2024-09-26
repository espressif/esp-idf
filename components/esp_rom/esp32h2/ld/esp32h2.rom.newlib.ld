/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
 * THIS FILE WAS AUTOMATICALLY GENERATED. !!! BUT EDITED !!!
 * The file was originally generated for use with newlib, but it was split into
 * multiple files to make it compatible with picolibc.
 */

/***************************************
 Group newlib
 ***************************************/

/* Functions */
_isatty_r = 0x400004cc;
strdup = 0x40000528;
strndup = 0x4000054c;
rand_r = 0x40000584;
rand = 0x40000588;
srand = 0x4000058c;
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
