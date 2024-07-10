/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c5.rom.newlib-normal.ld for esp32c5
 *
 *
 * Generated from ./target/esp32c5/interface-esp32c5.yml md5sum f5c146321f24f88ad1f27234da5aed11
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib_normal_format
 ***************************************/

/* Functions */
__sprint_r = 0x400005e4;
_fiprintf_r = 0x400005e8;
_fprintf_r = 0x400005ec;
_vfiprintf_r = 0x400005f0;
_vfprintf_r = 0x400005f4;
fiprintf = 0x400005f8;
fprintf = 0x400005fc;
printf = 0x40000600;
vfiprintf = 0x40000604;
vfprintf = 0x40000608;
asprintf = 0x4000060c;
sprintf = 0x40000610;
snprintf = 0x40000614;
siprintf = 0x40000618;
sniprintf = 0x4000061c;
vprintf = 0x40000620;
viprintf = 0x40000624;
vsnprintf = 0x40000628;
vsniprintf = 0x4000062c;
sscanf = 0x40000630;
siscanf = 0x40000634;
