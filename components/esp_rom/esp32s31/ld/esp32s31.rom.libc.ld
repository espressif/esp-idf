/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32s31.rom.libc.ld for esp32s31
 *
 *
 * Generated from ../../rom/target/esp32s31/interface-esp32s31.yml md5sum e8647a01196578fbaee1887babf31d38
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group libc
 ***************************************/

/* Functions */
esp_rom_newlib_init_common_mutexes = 0x2f8004b4;
memset = 0x2f8004b8;
memcpy = 0x2f8004bc;
memmove = 0x2f8004c0;
memcmp = 0x2f8004c4;
strcpy = 0x2f8004c8;
strncpy = 0x2f8004cc;
strcmp = 0x2f8004d0;
strncmp = 0x2f8004d4;
strlen = 0x2f8004d8;
strstr = 0x2f8004dc;
bzero = 0x2f8004e0;
sbrk = 0x2f8004e4;
isascii = 0x2f8004f0;
isblank = 0x2f8004f4;
iscntrl = 0x2f8004f8;
isgraph = 0x2f800504;
isprint = 0x2f800508;
ispunct = 0x2f80050c;
toascii = 0x2f800520;
memccpy = 0x2f800524;
memchr = 0x2f800528;
memrchr = 0x2f80052c;
strcasestr = 0x2f800534;
strcat = 0x2f800538;
strchr = 0x2f80053c;
strcspn = 0x2f800540;
strlcat = 0x2f800548;
strlcpy = 0x2f80054c;
strncat = 0x2f800558;
strnlen = 0x2f80055c;
strrchr = 0x2f800560;
strsep = 0x2f800564;
strspn = 0x2f800568;
strtok_r = 0x2f80056c;
longjmp = 0x2f800574;
setjmp = 0x2f800578;
abs = 0x2f80057c;
div = 0x2f800580;
labs = 0x2f800584;
ldiv = 0x2f800588;
qsort = 0x2f80058c;
utoa = 0x2f800590;
itoa = 0x2f800594;
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x2f07ffc8;
_global_impure_ptr = 0x2f07ffc4;
