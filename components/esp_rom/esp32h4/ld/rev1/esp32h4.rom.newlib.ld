/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* ROM function interface esp32b1z.rom.newlib.ld for esp32b1z
 *
 *
 * Generated from ./interface-esp32b1z.yml md5sum a8cce65aa1422e5781ad0d729ef0a0a6
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib
 ***************************************/

/* Functions */
esp_rom_newlib_init_common_mutexes = 0x40000358;
memset = 0x4000035c;
memcpy = 0x40000360;
memmove = 0x40000364;
memcmp = 0x40000368;
strcpy = 0x4000036c;
strncpy = 0x40000370;
strcmp = 0x40000374;
strncmp = 0x40000378;
strlen = 0x4000037c;
strstr = 0x40000380;
bzero = 0x40000384;
sbrk = 0x4000038c;
isalnum = 0x40000390;
isalpha = 0x40000394;
isascii = 0x40000398;
isblank = 0x4000039c;
iscntrl = 0x400003a0;
isdigit = 0x400003a4;
islower = 0x400003a8;
isgraph = 0x400003ac;
isprint = 0x400003b0;
ispunct = 0x400003b4;
isspace = 0x400003b8;
isupper = 0x400003bc;
toupper = 0x400003c0;
tolower = 0x400003c4;
toascii = 0x400003c8;
memccpy = 0x400003cc;
memchr = 0x400003d0;
memrchr = 0x400003d4;
strcasecmp = 0x400003d8;
strcasestr = 0x400003dc;
strcat = 0x400003e0;
strdup = 0x400003e4;
strchr = 0x400003e8;
strcspn = 0x400003ec;
strcoll = 0x400003f0;
strlcat = 0x400003f4;
strlcpy = 0x400003f8;
strlwr = 0x400003fc;
strncasecmp = 0x40000400;
strncat = 0x40000404;
strndup = 0x40000408;
strnlen = 0x4000040c;
strrchr = 0x40000410;
strsep = 0x40000414;
strspn = 0x40000418;
strtok_r = 0x4000041c;
strupr = 0x40000420;
longjmp = 0x40000424;
setjmp = 0x40000428;
abs = 0x4000042c;
div = 0x40000430;
labs = 0x40000434;
ldiv = 0x40000438;
qsort = 0x4000043c;
rand_r = 0x40000440;
rand = 0x40000444;
srand = 0x40000448;
utoa = 0x4000044c;
itoa = 0x40000450;
atoi = 0x40000454;
atol = 0x40000458;
strtol = 0x4000045c;
strtoul = 0x40000460;
PROVIDE( fflush = 0x40000464 );
PROVIDE( _fflush_r = 0x40000468 );
PROVIDE( _fwalk = 0x4000046c );
PROVIDE( _fwalk_reent = 0x40000470 );
PROVIDE( __swbuf_r = 0x4000047c );
__swbuf = 0x40000480;
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x3fcdffdc;
_global_impure_ptr = 0x3fcdffd8;
