/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* ROM function interface esp32c2.rom.newlib-nano.ld for esp32c2
 *
 *
 * Generated from ./interface-esp32c2.yml md5sum 6c4d0f3a9f2d0c93477024a1a8f13746
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib_nano_format
 ***************************************/

/* Functions */
__sprint_r = 0x40000670;
_fiprintf_r = 0x40000674;
_fprintf_r = 0x40000678;
_printf_common = 0x4000067c;
_printf_i = 0x40000680;
_vfiprintf_r = 0x40000684;
_vfprintf_r = 0x40000688;
fiprintf = 0x4000068c;
fprintf = 0x40000690;
printf = 0x40000694;
vfiprintf = 0x40000698;
vfprintf = 0x4000069c;
asprintf = 0x400006a0;
sprintf = 0x400006a4;
snprintf = 0x400006a8;
siprintf = 0x400006ac;
sniprintf = 0x400006b0;
vprintf = 0x400006b4;
viprintf = 0x400006b8;
vsnprintf = 0x400006bc;
vsniprintf = 0x400006c0;
__rom_printf_float = 0x400006c4;
__rom_scanf_float = 0x400006c8;
_scanf_i = 0x400006cc;
_scanf_chars = 0x400006d0;
sscanf = 0x400006d4;
siscanf = 0x400006d8;
