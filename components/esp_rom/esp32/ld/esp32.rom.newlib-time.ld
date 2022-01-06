/* These are the newlib functions and the .bss/.data symbols necessary for these functions present in ESP32 ROM.
   They should not be used when you need to solve the Y2K38 problem.
   Because these functions were compiled with 32-bit width for the time_t structure.
 */

asctime = 0x40059588;
asctime_r = 0x40000ec8;
ctime = 0x400595b0;
ctime_r = 0x400595c4;
__gettzinfo = 0x40001fcc;
__get_current_time_locale = 0x40001834;
gmtime = 0x40059848;
gmtime_r = 0x40059868;
localtime = 0x400595dc;
localtime_r = 0x400595fc;
mktime = 0x4005a5e8;
strftime = 0x40059ab4;
time = 0x40001844;
__time_load_locale = 0x4000183c;
tzset = 0x40001a1c;
_tzset_r = 0x40001a28;
__tzcalc_limits = 0x400018a0;
__tz_lock = 0x40001a04;
__tz_unlock = 0x40001a10;
/* The .bss/.data symbols necessary for these functions */
_timezone = 0x3ffae0a0;
_tzname = 0x3ffae030;
_daylight = 0x3ffae0a4;
__month_lengths = 0x3ff9609c;

/* These functions don't use time_t, but use other structures which include time_t.
 * For example, 'struct stat' contains time_t.
 */
_isatty_r = 0x40000ea0;
__sfvwrite_r = 0x4005893c;
__smakebuf_r = 0x40059108;
__srefill_r = 0x400593d4;
__swsetup_r = 0x40058cc8;
