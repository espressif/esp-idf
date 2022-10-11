/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c6.rom.newlib-normal.ld for esp32c6
 *
 *
 * Generated from ./target/esp32c6/interface-esp32c6.yml md5sum 06c13e133e0743d09b87aba30d3e213b
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib_normal_format
 ***************************************/

/* Functions */
__sprint_r = 0x400005d4;
_fiprintf_r = 0x400005d8;
_fprintf_r = 0x400005dc;
_vfiprintf_r = 0x400005e0;
_vfprintf_r = 0x400005e4;
fiprintf = 0x400005e8;
fprintf = 0x400005ec;
printf = 0x400005f0;
vfiprintf = 0x400005f4;
vfprintf = 0x400005f8;
asprintf = 0x400005fc;
sprintf = 0x40000600;
snprintf = 0x40000604;
siprintf = 0x40000608;
sniprintf = 0x4000060c;
vprintf = 0x40000610;
viprintf = 0x40000614;
vsnprintf = 0x40000618;
vsniprintf = 0x4000061c;
sscanf = 0x40000620;
siscanf = 0x40000624;
