/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c5.rom.newlib-nano.ld for esp32c5
 *
 *
 * Generated from ./target/esp32c5/interface-esp32c5.yml md5sum 4dfe6ccc311ca3575012c23e3753353c
 *
 * Compatible with ROM where ECO version equal or greater to 2.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib_nano_format
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
_printf_common = 0x40000630;
_printf_i = 0x40000634;
