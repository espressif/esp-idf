/* ROM function interface esp32s3.rom.newlib.ld for esp32s3
 *
 *
 * Generated from ./interface-esp32s3.yml md5sum 39c4ce259b11323b9404c192b01b712b
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib
 ***************************************/

/* Functions */
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
isalnum = 0x40001284;
isalpha = 0x40001290;
isascii = 0x4000129c;
isblank = 0x400012a8;
iscntrl = 0x400012b4;
isdigit = 0x400012c0;
islower = 0x400012cc;
isgraph = 0x400012d8;
isprint = 0x400012e4;
ispunct = 0x400012f0;
isspace = 0x400012fc;
isupper = 0x40001308;
toupper = 0x40001314;
tolower = 0x40001320;
toascii = 0x4000132c;
memccpy = 0x40001338;
memchr = 0x40001344;
memrchr = 0x40001350;
strcasecmp = 0x4000135c;
strcasestr = 0x40001368;
strcat = 0x40001374;
strdup = 0x40001380;
strchr = 0x4000138c;
strcspn = 0x40001398;
strcoll = 0x400013a4;
strlcat = 0x400013b0;
strlcpy = 0x400013bc;
strlwr = 0x400013c8;
strncasecmp = 0x400013d4;
strncat = 0x400013e0;
strndup = 0x400013ec;
strnlen = 0x400013f8;
strrchr = 0x40001404;
strsep = 0x40001410;
strspn = 0x4000141c;
strtok_r = 0x40001428;
strupr = 0x40001434;
longjmp = 0x40001440;
setjmp = 0x4000144c;
abs = 0x40001458;
div = 0x40001464;
labs = 0x40001470;
ldiv = 0x4000147c;
qsort = 0x40001488;
rand_r = 0x40001494;
rand = 0x400014a0;
srand = 0x400014ac;
utoa = 0x400014b8;
itoa = 0x400014c4;
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
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x3fceffd4;
_global_impure_ptr = 0x3fceffd0;
