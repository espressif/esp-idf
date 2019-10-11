/* These are the locale-related newlib functions present in ESP32 ROM.
   ESP32 ROM contains newlib version 2.2.0, and these functions should not be
   used when compiling with newlib version 3, since locale implementation is
   different there.

   Unlike other ROM functions which are exported using PROVIDE, which declares
   weak symbols, newlib related functions are exported using assignment,
   which declares strong symbols. This is done so that ROM functions are always
   used instead of the ones provided by libc.a.
 */

__locale_charset = 0x40059540;
__locale_cjk_lang = 0x40059558;
localeconv = 0x4005957c;
_localeconv_r = 0x40059560;
__locale_mb_cur_max = 0x40059548;
__locale_msgcharset = 0x40059550;
setlocale = 0x40059568;
_setlocale_r = 0x4005950c;
