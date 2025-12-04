/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32s31.rom.newlib.ld for esp32s31
 *
 *
 * Generated from ../../rom/target/esp32s31/interface-esp32s31.yml md5sum e8647a01196578fbaee1887babf31d38
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib
 ***************************************/

/* Functions */
_isatty_r = 0x2f800468;
strdup = 0x2f80046c;
strndup = 0x2f800470;
rand_r = 0x2f800474;
rand = 0x2f800478;
srand = 0x2f80047c;
atoi = 0x2f800480;
atol = 0x2f800484;
strtol = 0x2f800488;
strtoul = 0x2f80048c;
fflush = 0x2f800490;
_fflush_r = 0x2f800494;
_fwalk = 0x2f800498;
_fwalk_reent = 0x2f80049c;
__smakebuf_r = 0x2f8004a0;
__swhatbuf_r = 0x2f8004a4;
__swbuf_r = 0x2f8004a8;
__swbuf = 0x2f8004ac;
__swsetup_r = 0x2f8004b0;
toupper = 0x2f800518;
tolower = 0x2f80051c;
isalnum = 0x2f8004e8;
isalpha = 0x2f8004ec;
isdigit = 0x2f8004fc;
islower = 0x2f800500;
isspace = 0x2f800510;
isupper = 0x2f800514;
strcasecmp = 0x2f800530;
strcoll = 0x2f800544;
strlwr = 0x2f800550;
strncasecmp = 0x2f800554;
strupr = 0x2f800570;
