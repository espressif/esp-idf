/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32h21.rom.newlib.ld for esp32h21
 *
 *
 * Generated from ./target/esp32h21/interface-esp32h21.yml md5sum dee73289acb146ae14dd9544143527f0
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
_isatty_r = 0x400004c4;
strdup = 0x40000520;
strndup = 0x40000544;
rand_r = 0x4000057c;
rand = 0x40000580;
srand = 0x40000584;
atoi = 0x40000590;
atol = 0x40000594;
strtol = 0x40000598;
strtoul = 0x4000059c;
fflush = 0x400005a0;
_fflush_r = 0x400005a4;
_fwalk = 0x400005a8;
_fwalk_reent = 0x400005ac;
__smakebuf_r = 0x400005b0;
__swhatbuf_r = 0x400005b4;
__swbuf_r = 0x400005b8;
__swbuf = 0x400005bc;
__swsetup_r = 0x400005c0;
