/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32p4.rom.newlib.ld for esp32p4
 *
 *
 * Generated from ./target/esp32p4/interface-esp32p4.yml md5sum f6516bd9708d890f63db87f8aed53ca7
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
_isatty_r = 0x4fc00294;
strdup = 0x4fc002f0;
strndup = 0x4fc00314;
rand_r = 0x4fc0034c;
rand = 0x4fc00350;
srand = 0x4fc00354;
atoi = 0x4fc00360;
atol = 0x4fc00364;
strtol = 0x4fc00368;
strtoul = 0x4fc0036c;
fflush = 0x4fc00370;
_fflush_r = 0x4fc00374;
_fwalk = 0x4fc00378;
_fwalk_reent = 0x4fc0037c;
__smakebuf_r = 0x4fc00380;
__swhatbuf_r = 0x4fc00384;
__swbuf_r = 0x4fc00388;
__swbuf = 0x4fc0038c;
__swsetup_r = 0x4fc00390;
toupper = 0x4fc002cc;
tolower = 0x4fc002d0;
isalnum = 0x4fc0029c;
isalpha = 0x4fc002a0;
isdigit = 0x4fc002b0;
islower = 0x4fc002b4;
isspace = 0x4fc002c4;
isupper = 0x4fc002c8;
strcasecmp = 0x4fc002e4;
strcoll = 0x4fc002fc;
strlwr = 0x4fc00308;
strncasecmp = 0x4fc0030c;
strupr = 0x4fc0032c;
