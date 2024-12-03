/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32s3.rom.newlib.ld for esp32s3
 *
 *
 * Generated from ./interface-esp32s3.yml md5sum 39c4ce259b11323b9404c192b01b712b
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. !!! BUT EDITED !!!
 * The file was originally generated for use with newlib, but it was split into
 * multiple files to make it compatible with picolibc.
 */

strdup = 0x40001380;
strndup = 0x400013ec;
rand = 0x400014a0;
srand = 0x400014ac;
atoi = 0x400014d0;
atol = 0x400014dc;
strtol = 0x400014e8;
strtoul = 0x400014f4;
PROVIDE( fflush = 0x40001500 );
PROVIDE( _fflush_r = 0x4000150c );
PROVIDE( _fwalk = 0x40001518 );
PROVIDE( _fwalk_reent = 0x40001524 );
PROVIDE( __swbuf_r = 0x40001548 );
__swbuf = 0x40001554;
