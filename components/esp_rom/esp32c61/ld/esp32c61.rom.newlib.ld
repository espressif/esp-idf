/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c61.rom.newlib.ld for esp32c61
 *
 *
 * Generated from ./target/esp32c6lite/interface-esp32c6lite.yml md5sum 27eb0efac0883ee622c22767242c9457
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
_isatty_r = 0x400004e4;
strdup = 0x40000540;
strndup = 0x40000564;
rand_r = 0x4000059c;
rand = 0x400005a0;
srand = 0x400005a4;
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
