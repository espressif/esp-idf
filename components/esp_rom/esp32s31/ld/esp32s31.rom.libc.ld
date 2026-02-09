/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32s31.rom.libc.ld for esp32s31
 *
 *
 * Generated from ../../rom/target/esp32s31/interface-esp32s31.yml md5sum 0cb775308c66eebf0757e406021ad5be
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group libc
 ***************************************/

/* Functions */
esp_rom_newlib_init_common_mutexes = 0x2f8004bc;
memset = 0x2f8004c0;
memcpy = 0x2f8004c4;
memmove = 0x2f8004c8;
memcmp = 0x2f8004cc;
strcpy = 0x2f8004d0;
strncpy = 0x2f8004d4;
strcmp = 0x2f8004d8;
strncmp = 0x2f8004dc;
strlen = 0x2f8004e0;
strstr = 0x2f8004e4;
bzero = 0x2f8004e8;
sbrk = 0x2f8004ec;
isalnum = 0x2f8004f0;
isalpha = 0x2f8004f4;
isascii = 0x2f8004f8;
isblank = 0x2f8004fc;
iscntrl = 0x2f800500;
isdigit = 0x2f800504;
islower = 0x2f800508;
isgraph = 0x2f80050c;
isprint = 0x2f800510;
ispunct = 0x2f800514;
isspace = 0x2f800518;
isupper = 0x2f80051c;
toupper = 0x2f800520;
tolower = 0x2f800524;
toascii = 0x2f800528;
memccpy = 0x2f80052c;
memchr = 0x2f800530;
memrchr = 0x2f800534;
strcasecmp = 0x2f800538;
strcasestr = 0x2f80053c;
strcat = 0x2f800540;
strchr = 0x2f800544;
strcspn = 0x2f800548;
strcoll = 0x2f80054c;
strlcat = 0x2f800550;
strlcpy = 0x2f800554;
strlwr = 0x2f800558;
strncasecmp = 0x2f80055c;
strncat = 0x2f800560;
strnlen = 0x2f800564;
strrchr = 0x2f800568;
strsep = 0x2f80056c;
strspn = 0x2f800570;
strtok_r = 0x2f800574;
strupr = 0x2f800578;
longjmp = 0x2f80057c;
setjmp = 0x2f800580;
abs = 0x2f800584;
div = 0x2f800588;
labs = 0x2f80058c;
ldiv = 0x2f800590;
qsort = 0x2f800594;
utoa = 0x2f800598;
itoa = 0x2f80059c;
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x2f07ffc8;
_global_impure_ptr = 0x2f07ffc4;
