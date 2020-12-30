/**
 * These are the .bss/.data symbols used by newlib functions present in ESP32-S2 ROM.
 * See also esp32s2.rom.newlib-funcs.ld for the list of general newlib functions.
 *
 * Unlike other ROM functions and data which are exported using PROVIDE, which declare weak symbols,
 * newlib related functions are exported using assignment, which declare strong symbols.
 * This is done so that ROM functions are always used instead of the ones provided by libc.a.
 */

_ctype_ = 0x3ffac76c;
_global_impure_ptr = 0x3ffffd8c;
_PathLocale = 0x3ffffd80;
__sf_fake_stderr = 0x3ffaf08c;
__sf_fake_stdin = 0x3ffaf0cc;
__sf_fake_stdout = 0x3ffaf0ac;
__sfp_recursive_mutex = 0x3ffffd88;
__sinit_recursive_mutex = 0x3ffffd84;
