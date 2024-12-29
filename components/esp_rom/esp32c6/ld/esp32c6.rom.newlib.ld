/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c6.rom.newlib.ld for esp32c6
 *
 *
 * Generated from ./target/esp32c6/interface-esp32c6.yml md5sum 06c13e133e0743d09b87aba30d3e213b
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
_isatty_r = 0x400004d4;
strdup = 0x40000530;
strndup = 0x40000554;
rand_r = 0x4000058c;
rand = 0x40000590;
srand = 0x40000594;
atoi = 0x400005a0;
atol = 0x400005a4;
strtol = 0x400005a8;
strtoul = 0x400005ac;
fflush = 0x400005b0;
_fflush_r = 0x400005b4;
_fwalk = 0x400005b8;
_fwalk_reent = 0x400005bc;
__smakebuf_r = 0x400005c0;
__swhatbuf_r = 0x400005c4;
__swbuf_r = 0x400005c8;
__swbuf = 0x400005cc;
__swsetup_r = 0x400005d0;
