/* These are the .bss/.data symbols used by newlib functions present in ESP32 ROM.
   See also esp32.rom.newlib-funcs.ld for the list of general newlib functions,
   and esp32.rom.newlib-nano.ld for "nano" versions of printf/scanf family of functions.

   Unlike other ROM functions and data which are exported using PROVIDE, which declares
   weak symbols, newlib related functions are exported using assignment,
   which declares strong symbols. This is done so that ROM functions are always
   used instead of the ones provided by libc.a.
 */

_ctype_ = 0x3ff96354;
__ctype_ptr__ = 0x3ff96350;
environ = 0x3ffae0b4;
_global_impure_ptr = 0x3ffae0b0;
__mb_cur_max = 0x3ff96530;
__sf_fake_stderr = 0x3ff96458;
__sf_fake_stdin = 0x3ff96498;
__sf_fake_stdout = 0x3ff96478;
__wctomb = 0x3ff96540;
