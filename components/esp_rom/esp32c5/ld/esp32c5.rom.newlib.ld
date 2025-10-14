/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c5.rom.newlib.ld for esp32c5
 *
 *
 * Generated from ./target/esp32c5/interface-esp32c5.yml md5sum 4dfe6ccc311ca3575012c23e3753353c
 *
 * Compatible with ROM where ECO version equal or greater to 2.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
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
