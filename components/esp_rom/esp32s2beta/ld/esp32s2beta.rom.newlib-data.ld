/* These are the .bss/.data symbols used by newlib functions present in ESP32S2beta ROM.
   See also esp32s2beta.rom.newlib-funcs.ld for the list of general newlib functions,
   and esp32s2beta.rom.newlib-stdio.ld for stdio related newlib functions.

   Unlike other ROM functions and data which are exported using PROVIDE, which declares
   weak symbols, newlib related functions are exported using assignment,
   which declares strong symbols. This is done so that ROM functions are always
   used instead of the ones provided by libc.a.
 */

_ctype_ = 0x3ffab96c;
__default_global_locale = 0x3ffab800;
_global_impure_ptr = 0x3fffc04c;
__global_locale_ptr = 0x3fffc030;
__locale_ctype_ptr = 0x40002008;
__locale_ctype_ptr_l = 0x40002000;
__locale_mb_cur_max = 0x40001fe8;
__packed = 0x3fffc530;
_PathLocale = 0x3fffc040;
