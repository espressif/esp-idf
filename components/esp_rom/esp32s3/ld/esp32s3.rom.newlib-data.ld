/**
 * These are the .bss/.data symbols used by newlib functions present in ESP32S3 ROM.
 * See also esp32s3.rom.newlib-funcs.ld for the list of general newlib functions.
 *
 * Unlike other ROM functions which declare weak symbols using PROVIDE,
 * newlib related functions are exported using assignment, which declare strong symbols.
 *
 * Note: These ROM data are always linked instead of the ones provided by libc.a.
 */
_ctype_ = 0x3ff0732c;
__default_global_locale = 0x3ff071c0;
_global_impure_ptr = 0x3fcefcdc;
__global_locale_ptr = 0x3fcefccc;
_PathLocale = 0x3fcefcd0;
__sf_fake_stderr = 0x3ff0c524;
__sf_fake_stdin = 0x3ff0c564;
__sf_fake_stdout = 0x3ff0c544;
__sinit_recursive_mutex = 0x3fcefcd4;
__sfp_recursive_mutex = 0x3fcefcd8;
