/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
esp_rom_newlib_init_common_mutexes = 0x400011dc;
memset = 0x400011e8;
memcpy = 0x400011f4;
memmove = 0x40001200;
memcmp = 0x4000120c;
strcpy = 0x40001218;
strncpy = 0x40001224;
strcmp = 0x40001230;
strncmp = 0x4000123c;
strlen = 0x40001248;
strstr = 0x40001254;
bzero = 0x40001260;
sbrk = 0x40001278;
isascii = 0x4000129c;
isblank = 0x400012a8;
iscntrl = 0x400012b4;
isgraph = 0x400012d8;
isprint = 0x400012e4;
ispunct = 0x400012f0;
toascii = 0x4000132c;
memccpy = 0x40001338;
memchr = 0x40001344;
memrchr = 0x40001350;
strcasestr = 0x40001368;
strcat = 0x40001374;
strchr = 0x4000138c;
strcspn = 0x40001398;
strlcat = 0x400013b0;
strlcpy = 0x400013bc;
strncat = 0x400013e0;
strnlen = 0x400013f8;
strrchr = 0x40001404;
strsep = 0x40001410;
strspn = 0x4000141c;
strtok_r = 0x40001428;
longjmp = 0x40001440;
setjmp = 0x4000144c;
abs = 0x40001458;
div = 0x40001464;
labs = 0x40001470;
ldiv = 0x4000147c;
qsort = 0x40001488;
rand_r = 0x40001494;
utoa = 0x400014b8;
itoa = 0x400014c4;
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x3fceffd4;
_global_impure_ptr = 0x3fceffd0;
