/* ROM function interface esp32h2.rom.newlib.ld for esp32h2
 *
 *
 * Generated from ./interface-esp32h2.yml md5sum 47e064f8d2b991d372a72a89ab7d47d3
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib
 ***************************************/

/* Functions */
esp_rom_newlib_init_common_mutexes = 0x4000031c;
memset = 0x40000320;
memcpy = 0x40000324;
memmove = 0x40000328;
memcmp = 0x4000032c;
strcpy = 0x40000330;
strncpy = 0x40000334;
strcmp = 0x40000338;
strncmp = 0x4000033c;
strlen = 0x40000340;
strstr = 0x40000344;
bzero = 0x40000348;
_isatty_r = 0x4000034c;
sbrk = 0x40000350;
isalnum = 0x40000354;
isalpha = 0x40000358;
isascii = 0x4000035c;
isblank = 0x40000360;
iscntrl = 0x40000364;
isdigit = 0x40000368;
islower = 0x4000036c;
isgraph = 0x40000370;
isprint = 0x40000374;
ispunct = 0x40000378;
isspace = 0x4000037c;
isupper = 0x40000380;
toupper = 0x40000384;
tolower = 0x40000388;
toascii = 0x4000038c;
memccpy = 0x40000390;
memchr = 0x40000394;
memrchr = 0x40000398;
strcasecmp = 0x4000039c;
strcasestr = 0x400003a0;
strcat = 0x400003a4;
strdup = 0x400003a8;
strchr = 0x400003ac;
strcspn = 0x400003b0;
strcoll = 0x400003b4;
strlcat = 0x400003b8;
strlcpy = 0x400003bc;
strlwr = 0x400003c0;
strncasecmp = 0x400003c4;
strncat = 0x400003c8;
strndup = 0x400003cc;
strnlen = 0x400003d0;
strrchr = 0x400003d4;
strsep = 0x400003d8;
strspn = 0x400003dc;
strtok_r = 0x400003e0;
strupr = 0x400003e4;
longjmp = 0x400003e8;
setjmp = 0x400003ec;
abs = 0x400003f0;
div = 0x400003f4;
labs = 0x400003f8;
ldiv = 0x400003fc;
qsort = 0x40000400;
rand_r = 0x40000404;
rand = 0x40000408;
srand = 0x4000040c;
utoa = 0x40000410;
itoa = 0x40000414;
atoi = 0x40000418;
atol = 0x4000041c;
strtol = 0x40000420;
strtoul = 0x40000424;
PROVIDE( fflush = 0x40000428 );
PROVIDE( _fflush_r = 0x4000042c );
PROVIDE( _fwalk = 0x40000430 );
PROVIDE( _fwalk_reent = 0x40000434 );
PROVIDE( __smakebuf_r = 0x40000438 );
PROVIDE( __swhatbuf_r = 0x4000043c );
PROVIDE( __swbuf_r = 0x40000440 );
__swbuf = 0x40000444;
PROVIDE( __swsetup_r = 0x40000448 );
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x3fcdffdc;
_global_impure_ptr = 0x3fcdffd8;
