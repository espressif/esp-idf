/* These are the newlib functions present in ESP32S2beta ROM.
   See also esp32s2beta.rom.newlib-data.ld for the list of .data/.bss symbols
   used by these functions, and esp32s2beta.rom.newlib-stdio.ld for stdio related
   functions.

   Unlike other ROM functions which are exported using PROVIDE, which declares
   weak symbols, newlib related functions are exported using assignment,
   which declares strong symbols. This is done so that ROM functions are always
   used instead of the ones provided by libc.a.
 */

abs = 0x4000073c;
__ascii_mbtowc = 0x40007de0;
__ascii_wctomb = 0x40001cac;
__assert = 0x4001565c;
__assert_func = 0x40015634;
atoi = 0x40000adc;
_atoi_r = 0x40000aec;
atol = 0x40000b04;
_atol_r = 0x40000b14;
bzero = 0x40007ca4;
_cvt = 0x4000c6d8;
div = 0x40000744;
isalnum = 0x40007cb4;
isalpha = 0x40007cc4;
isascii = 0x40015d18;
isblank = 0x40007cd4;
iscntrl = 0x40007cf4;
isdigit = 0x40007d0c;
isgraph = 0x40007d44;
islower = 0x40007d24;
isprint = 0x40007d5c;
ispunct = 0x40007d70;
isspace = 0x40007d88;
isupper = 0x40007da0;
itoa = 0x40000acc;
__itoa = 0x40000a90;
labs = 0x4000076c;
ldiv = 0x40000774;
longjmp = 0x400006c8;
_mbtowc_r = 0x40007dbc;
memccpy = 0x40015d2c;
memchr = 0x40015d50;
memcmp = 0x40015d6c;
memcpy = 0x40015dd4;
memmove = 0x40015edc;
memrchr = 0x40015f18;
memset = 0x40015f68;
pthread_setcancelstate = 0x400151d4;
qsort = 0x40000818;
rand = 0x40007e54;
rand_r = 0x40007ed0;
setjmp = 0x40000664;
srand = 0x40007e00;
strcasecmp = 0x40007f14;
strcasestr = 0x40007f58;
strcat = 0x40015fbc;
strchr = 0x40015fdc;
strcmp = 0x40007fc0;
strcoll = 0x400080c4;
strcpy = 0x400080d8;
strcspn = 0x40015ff8;
strdup = 0x40008160;
_strdup_r = 0x40008174;
strlcat = 0x40008194;
strlcpy = 0x40016024;
strlen = 0x400081e4;
strlwr = 0x40008244;
strncasecmp = 0x40008270;
strncat = 0x40016060;
strncmp = 0x40016090;
strncpy = 0x400082fc;
strndup = 0x400083c4;
_strndup_r = 0x400083d8;
strnlen = 0x400160c8;
strrchr = 0x4000841c;
strsep = 0x40008448;
strspn = 0x400160e8;
strstr = 0x40016114;
__strtok_r = 0x40016144;
strtok_r = 0x400161a8;
strtol = 0x40000c70;
strtol_l = 0x40000c58;
_strtol_r = 0x40000c34;
strtoul = 0x40000dcc;
strtoul_l = 0x40000db4;
_strtoul_r = 0x40000d90;
strupr = 0x40008460;
toascii = 0x400161bc;
tolower = 0x40008534;
toupper = 0x40008550;
utoa = 0x40000654;
__utoa = 0x400005f0;
wcrtomb = 0x400016d0;
_wcrtomb_r = 0x4000167c;
_wctomb_r = 0x40001c88;
