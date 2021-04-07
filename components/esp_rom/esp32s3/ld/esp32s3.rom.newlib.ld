/* ROM function interface esp32s3.rom.newlib.ld for esp32s3
 *
 *
 * Generated from ./interface-esp32s3.yml md5sum 36d43c36b9d0f4f082f71c819ad53470
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib
 ***************************************/

/* Functions */
esp_rom_newlib_init_common_mutexes = 0x40001068;
memset = 0x40001074;
memcpy = 0x40001080;
memmove = 0x4000108c;
memcmp = 0x40001098;
strcpy = 0x400010a4;
strncpy = 0x400010b0;
strcmp = 0x400010bc;
strncmp = 0x400010c8;
strlen = 0x400010d4;
strstr = 0x400010e0;
bzero = 0x400010ec;
_isatty_r = 0x400010f8;
sbrk = 0x40001104;
isalnum = 0x40001110;
isalpha = 0x4000111c;
isascii = 0x40001128;
isblank = 0x40001134;
iscntrl = 0x40001140;
isdigit = 0x4000114c;
islower = 0x40001158;
isgraph = 0x40001164;
isprint = 0x40001170;
ispunct = 0x4000117c;
isspace = 0x40001188;
isupper = 0x40001194;
toupper = 0x400011a0;
tolower = 0x400011ac;
toascii = 0x400011b8;
memccpy = 0x400011c4;
memchr = 0x400011d0;
memrchr = 0x400011dc;
strcasecmp = 0x400011e8;
strcasestr = 0x400011f4;
strcat = 0x40001200;
strdup = 0x4000120c;
strchr = 0x40001218;
strcspn = 0x40001224;
strcoll = 0x40001230;
strlcat = 0x4000123c;
strlcpy = 0x40001248;
strlwr = 0x40001254;
strncasecmp = 0x40001260;
strncat = 0x4000126c;
strndup = 0x40001278;
strnlen = 0x40001284;
strrchr = 0x40001290;
strsep = 0x4000129c;
strspn = 0x400012a8;
strtok_r = 0x400012b4;
strupr = 0x400012c0;
longjmp = 0x400012cc;
setjmp = 0x400012d8;
abs = 0x400012e4;
div = 0x400012f0;
labs = 0x400012fc;
ldiv = 0x40001308;
qsort = 0x40001314;
rand_r = 0x40001320;
rand = 0x4000132c;
srand = 0x40001338;
utoa = 0x40001344;
itoa = 0x40001350;
atoi = 0x4000135c;
atol = 0x40001368;
strtol = 0x40001374;
strtoul = 0x40001380;
PROVIDE( fflush = 0x4000138c );
PROVIDE( _fflush_r = 0x40001398 );
PROVIDE( _fwalk = 0x400013a4 );
PROVIDE( _fwalk_reent = 0x400013b0 );
PROVIDE( __smakebuf_r = 0x400013bc );
PROVIDE( __swhatbuf_r = 0x400013c8 );
PROVIDE( __swbuf_r = 0x400013d4 );
__swbuf = 0x400013e0;
PROVIDE( __swsetup_r = 0x400013ec );
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x3fceffe0;
_global_impure_ptr = 0x3fceffdc;
