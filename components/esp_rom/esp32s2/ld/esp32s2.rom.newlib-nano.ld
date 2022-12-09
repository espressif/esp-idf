/* These are the printf/scanf related newlib functions present in ESP32-S2 ROM.
   These functions are compiled with newlib "nano" format option.
   As such, they don's support 64-bit integer formats.
   Floating point formats are supported by setting _printf_float and
   _scanf_float entries in syscall table. This is done automatically
   by startup code.
   See also esp32s2.rom.newlib-data.ld for the list of .data/.bss symbols
   used by newlib functions, and esp32s2.rom.newlib-funcs.ld for the list
   of general newlib functions.

   Unlike other ROM functions which are exported using PROVIDE, which declares
   weak symbols, newlib related functions are exported using assignment,
   which declares strong symbols. This is done so that ROM functions are always
   used instead of the ones provided by libc.a.
 */

fiprintf = 0x40000a3c;
_fiprintf_r = 0x40000a18;
__fp_lock_all = 0x4001a638;
fprintf = 0x40000a3c;
_fprintf_r = 0x40000a18;
__sprint_r = 0x40000aec;
vfiprintf = 0x40000e40;
_vfiprintf_r = 0x40000b58;
vfprintf = 0x40000e40;
_vfprintf_r = 0x40000b58;
