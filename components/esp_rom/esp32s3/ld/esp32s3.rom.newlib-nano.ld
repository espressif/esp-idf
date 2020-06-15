/**
 * These are the printf/scanf related newlib functions present in ESP32S3 ROM.
 * These functions are compiled with newlib "nano" format option.
 * As such, they don't support 64-bit integer formats.
 * Floating point formats are supported by setting _printf_float and
 * _scanf_float entries in syscall table. This is done automatically by startup code.
 *
 * See also esp32s3.rom.newlib-data.ld for the list of .data/.bss symbols used by newlib functions.
 * See also esp32s3.rom.newlib-funcs.ld for the list of general newlib functions.
 *
 * Unlike other ROM functions which declare weak symbols using PROVIDE,
 * newlib related functions are exported using assignment, which declare strong symbols.
 *
 * Note: These ROM functions are always linked instead of the ones provided by libc.a.
 */
_printf_common = 0x40033114;
_printf_i = 0x40033214;
__sfputs_r = 0x40032d44;
fiprintf = 0x40032cdc;
_fiprintf_r = 0x40032cac;
__fp_lock_all = 0x40054cbc;
fprintf = 0x40032cdc;
_fprintf_r = 0x40032cac;
__sprint_r = 0x40032d90;
vfiprintf = 0x400330f4;
_vfiprintf_r = 0x40032df8;
vfprintf = 0x400330f4;
_vfprintf_r = 0x40032df8;
