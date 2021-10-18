/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* ROM function interface esp32h2.rom.newlib-nano.ld for esp32h2
 *
 *
 * Generated from ./interface-esp32h2.yml md5sum a4343bd6a9a68319e4e3cc26aea38574
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib_nano_format
 ***************************************/

/* Functions */
__sprint_r = 0x40000674;
_fiprintf_r = 0x40000678;
_fprintf_r = 0x4000067c;
_printf_common = 0x40000680;
_printf_i = 0x40000684;
_vfiprintf_r = 0x40000688;
_vfprintf_r = 0x4000068c;
fiprintf = 0x40000690;
fprintf = 0x40000694;
printf = 0x40000698;
vfiprintf = 0x4000069c;
vfprintf = 0x400006a0;
asprintf = 0x400006a4;
sprintf = 0x400006a8;
snprintf = 0x400006ac;
siprintf = 0x400006b0;
sniprintf = 0x400006b4;
vprintf = 0x400006b8;
viprintf = 0x400006bc;
vsnprintf = 0x400006c0;
vsniprintf = 0x400006c4;
__rom_printf_float = 0x400006c8;
__rom_scanf_float = 0x400006cc;
_scanf_i = 0x400006d0;
_scanf_chars = 0x400006d4;
sscanf = 0x400006d8;
siscanf = 0x400006dc;
