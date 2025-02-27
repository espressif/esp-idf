/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32h4.rom.newlib.ld for esp32h4
 *
 *
 * Generated from ./target/esp32h4/interface-esp32h4.yml md5sum 14f04fa4b1cf69c4e6eec57d641026c4
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
_isatty_r = 0x400004a8;
strdup = 0x40000504;
strndup = 0x40000528;
rand_r = 0x40000560;
rand = 0x40000564;
srand = 0x40000568;
atoi = 0x40000574;
atol = 0x40000578;
strtol = 0x4000057c;
strtoul = 0x40000580;
fflush = 0x40000584;
_fflush_r = 0x40000588;
_fwalk = 0x4000058c;
_fwalk_reent = 0x40000590;
__smakebuf_r = 0x40000594;
__swhatbuf_r = 0x40000598;
__swbuf_r = 0x4000059c;
__swbuf = 0x400005a0;
__swsetup_r = 0x400005a4;
