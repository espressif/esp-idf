/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32p4lp.rom.newlib.ld for esp32p4lp
 *
 *
 * Generated from ./target/esp32p4lp/interface-esp32p4lp.yml md5sum 0751c805e21bd23f11b74dcc1f7e8007
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib
 ***************************************/

/* Functions */
memset = 0x501000f0;
memcpy = 0x501000f4;
memmove = 0x501000f8;
memcmp = 0x501000fc;
strcpy = 0x50100100;
strncpy = 0x50100104;
strcmp = 0x50100108;
strncmp = 0x5010010c;
strlen = 0x50100110;
strstr = 0x50100114;
bzero = 0x50100118;
isalnum = 0x5010011c;
isalpha = 0x50100120;
isascii = 0x50100124;
isblank = 0x50100128;
iscntrl = 0x5010012c;
isdigit = 0x50100130;
islower = 0x50100134;
isgraph = 0x50100138;
isprint = 0x5010013c;
ispunct = 0x50100140;
isspace = 0x50100144;
isupper = 0x50100148;
toupper = 0x5010014c;
tolower = 0x50100150;
toascii = 0x50100154;
memccpy = 0x50100158;
memchr = 0x5010015c;
memrchr = 0x50100160;
strcasecmp = 0x50100164;
strcasestr = 0x50100168;
strcat = 0x5010016c;
strchr = 0x50100170;
strcspn = 0x50100174;
strcoll = 0x50100178;
strlcat = 0x5010017c;
strlcpy = 0x50100180;
strlwr = 0x50100184;
strncasecmp = 0x50100188;
strncat = 0x5010018c;
strnlen = 0x50100190;
strrchr = 0x50100194;
strsep = 0x50100198;
strspn = 0x5010019c;
strtok_r = 0x501001a0;
strupr = 0x501001a4;
longjmp = 0x501001a8;
setjmp = 0x501001ac;
abs = 0x501001b0;
div = 0x501001b4;
labs = 0x501001b8;
ldiv = 0x501001bc;
qsort = 0x501001c0;
atoi = 0x501001c4;
atol = 0x501001c8;
itoa = 0x501001cc;
utoa = 0x501001d0;
