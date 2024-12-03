/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c3.rom.newlib.ld for esp32c3
 *
 *
 * Generated from ./interface-esp32c3.yml md5sum 93b28a9e1fe42d212018eb4336849208
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
strdup = 0x400003dc;
strndup = 0x40000400;
rand_r = 0x40000438;
rand = 0x4000043c;
srand = 0x40000440;
atoi = 0x4000044c;
atol = 0x40000450;
strtol = 0x40000454;
strtoul = 0x40000458;
PROVIDE( fflush = 0x4000045c );
PROVIDE( _fflush_r = 0x40000460 );
PROVIDE( _fwalk = 0x40000464 );
PROVIDE( _fwalk_reent = 0x40000468 );
PROVIDE( __swbuf_r = 0x40000474 );
__swbuf = 0x40000478;
