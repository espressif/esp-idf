/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c5.rom.newlib-normal.ld for esp32c5
 *
 *
 * Generated from ./target/esp32c5/interface-esp32c5.yml md5sum 2476337377df636dda217b0b3c1a63db
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib_normal_format
 ***************************************/

/* Functions */
__sprint_r = 0x400005d0;
_fiprintf_r = 0x400005d4;
_fprintf_r = 0x400005d8;
_vfiprintf_r = 0x400005dc;
_vfprintf_r = 0x400005e0;
fiprintf = 0x400005e4;
fprintf = 0x400005e8;
printf = 0x400005ec;
vfiprintf = 0x400005f0;
vfprintf = 0x400005f4;
asprintf = 0x400005f8;
sprintf = 0x400005fc;
snprintf = 0x40000600;
siprintf = 0x40000604;
sniprintf = 0x40000608;
vprintf = 0x4000060c;
viprintf = 0x40000610;
vsnprintf = 0x40000614;
vsniprintf = 0x40000618;
sscanf = 0x4000061c;
siscanf = 0x40000620;
