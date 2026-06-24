/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32p4lp.rom.newlib.ld for esp32p4lp
 *
 *
 * Generated from ./target/esp32p4lp/interface-esp32p4lp.yml md5sum f827caf806e9f4faec384ffbde44b380
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib
 ***************************************/

/* Functions */
memset = 0x50100134;
memcpy = 0x50100138;
memmove = 0x5010013c;
memcmp = 0x50100140;
strcpy = 0x50100144;
strncpy = 0x50100148;
strcmp = 0x5010014c;
strncmp = 0x50100150;
strlen = 0x50100154;
strstr = 0x50100158;
bzero = 0x5010015c;
isalnum = 0x50100160;
isalpha = 0x50100164;
isascii = 0x50100168;
isblank = 0x5010016c;
iscntrl = 0x50100170;
isdigit = 0x50100174;
islower = 0x50100178;
isgraph = 0x5010017c;
isprint = 0x50100180;
ispunct = 0x50100184;
isspace = 0x50100188;
isupper = 0x5010018c;
toupper = 0x50100190;
tolower = 0x50100194;
toascii = 0x50100198;
memccpy = 0x5010019c;
memchr = 0x501001a0;
memrchr = 0x501001a4;
strcasecmp = 0x501001a8;
strcasestr = 0x501001ac;
strcat = 0x501001b0;
strchr = 0x501001b4;
strcspn = 0x501001b8;
strcoll = 0x501001bc;
strlcat = 0x501001c0;
strlcpy = 0x501001c4;
strlwr = 0x501001c8;
strncasecmp = 0x501001cc;
strncat = 0x501001d0;
strnlen = 0x501001d4;
strrchr = 0x501001d8;
strsep = 0x501001dc;
strspn = 0x501001e0;
strtok_r = 0x501001e4;
strupr = 0x501001e8;
longjmp = 0x501001ec;
setjmp = 0x501001f0;
abs = 0x501001f4;
div = 0x501001f8;
labs = 0x501001fc;
ldiv = 0x50100200;
qsort = 0x50100204;
atoi = 0x50100208;
atol = 0x5010020c;
itoa = 0x50100210;
utoa = 0x50100214;
