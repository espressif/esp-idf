/* ROM function interface esp32c3.rom.newlib.ld for esp32c3
 *
 *
 * Generated from ./interface-esp32c3.yml md5sum 93b28a9e1fe42d212018eb4336849208
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group newlib
 ***************************************/

/* Functions */
esp_rom_newlib_init_common_mutexes = 0x40000350;
memset = 0x40000354;
memcpy = 0x40000358;
memmove = 0x4000035c;
memcmp = 0x40000360;
strcpy = 0x40000364;
strncpy = 0x40000368;
strcmp = 0x4000036c;
strncmp = 0x40000370;
strlen = 0x40000374;
strstr = 0x40000378;
bzero = 0x4000037c;
sbrk = 0x40000384;
isalnum = 0x40000388;
isalpha = 0x4000038c;
isascii = 0x40000390;
isblank = 0x40000394;
iscntrl = 0x40000398;
isdigit = 0x4000039c;
islower = 0x400003a0;
isgraph = 0x400003a4;
isprint = 0x400003a8;
ispunct = 0x400003ac;
isspace = 0x400003b0;
isupper = 0x400003b4;
toupper = 0x400003b8;
tolower = 0x400003bc;
toascii = 0x400003c0;
memccpy = 0x400003c4;
memchr = 0x400003c8;
memrchr = 0x400003cc;
strcasecmp = 0x400003d0;
strcasestr = 0x400003d4;
strcat = 0x400003d8;
strdup = 0x400003dc;
strchr = 0x400003e0;
strcspn = 0x400003e4;
strcoll = 0x400003e8;
strlcat = 0x400003ec;
strlcpy = 0x400003f0;
strlwr = 0x400003f4;
strncasecmp = 0x400003f8;
strncat = 0x400003fc;
strndup = 0x40000400;
strnlen = 0x40000404;
strrchr = 0x40000408;
strsep = 0x4000040c;
strspn = 0x40000410;
strtok_r = 0x40000414;
strupr = 0x40000418;
longjmp = 0x4000041c;
setjmp = 0x40000420;
abs = 0x40000424;
div = 0x40000428;
labs = 0x4000042c;
ldiv = 0x40000430;
qsort = 0x40000434;
rand_r = 0x40000438;
rand = 0x4000043c;
srand = 0x40000440;
utoa = 0x40000444;
itoa = 0x40000448;
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
/* Data (.data, .bss, .rodata) */
syscall_table_ptr = 0x3fcdffe0;
_global_impure_ptr = 0x3fcdffdc;
