/* These are the printf/scanf related newlib functions present in ESP32 ROM.
   These functions are compiled with newlib "nano" format option.
   As such, they don's support 64-bit integer formats.
   Floating point formats are supported by setting _printf_float and
   _scanf_float entries in syscall table. This is done automatically
   by startup code.
   These functions should not be used when compiling with PSRAM cache workaround enabled.
   See also esp32.rom.newlib-data.ld for the list of .data/.bss symbols
   used by newlib functions, and esp32.rom.newlib-funcs.ld for the list
   of general newlib functions.

   Unlike other ROM functions which are exported using PROVIDE, which declares
   weak symbols, newlib related functions are exported using assignment,
   which declares strong symbols. This is done so that ROM functions are always
   used instead of the ones provided by libc.a.
 */

asiprintf = 0x40056d9c;
_asiprintf_r = 0x40056d4c;
asniprintf = 0x40056cd8;
_asniprintf_r = 0x40056c64;
asnprintf = 0x40056cd8;
_asnprintf_r = 0x40056c64;
asprintf = 0x40056d9c;
_asprintf_r = 0x40056d4c;
fiprintf = 0x40056efc;
_fiprintf_r = 0x40056ed8;
fiscanf = 0x40058884;
_fiscanf_r = 0x400588b4;
fprintf = 0x40056efc;
_fprintf_r = 0x40056ed8;
fscanf = 0x40058884;
_fscanf_r = 0x400588b4;
iprintf = 0x40056978;
_iprintf_r = 0x40056944;
iscanf = 0x40058760;
_iscanf_r = 0x4005879c;
printf = 0x40056978;
_printf_common = 0x40057338;
_printf_i = 0x40057404;
_printf_r = 0x40056944;
scanf = 0x40058760;
_scanf_chars = 0x40058384;
_scanf_i = 0x4005845c;
_scanf_r = 0x4005879c;
__sfputs_r = 0x40057790;
siprintf = 0x40056c08;
_siprintf_r = 0x40056bbc;
siscanf = 0x400587d0;
_siscanf_r = 0x40058830;
sniprintf = 0x40056b4c;
_sniprintf_r = 0x40056ae4;
snprintf = 0x40056b4c;
_snprintf_r = 0x40056ae4;
sprintf = 0x40056c08;
_sprintf_r = 0x40056bbc;
__sprint_r = 0x400577e4;
sscanf = 0x400587d0;
_sscanf_r = 0x40058830;
__ssprint_r = 0x40056ff8;
__ssputs_r = 0x40056f2c;
__ssrefill_r = 0x40057fec;
__ssvfiscanf_r = 0x4005802c;
__ssvfscanf_r = 0x4005802c;
_sungetc_r = 0x40057f6c;
_svfiprintf_r = 0x40057100;
__svfiscanf_r = 0x40057b08;
_svfprintf_r = 0x40057100;
__svfscanf = 0x40057f04;
__svfscanf_r = 0x40057b08;
vasiprintf = 0x40056eb8;
_vasiprintf_r = 0x40056e80;
vasniprintf = 0x40056e58;
_vasniprintf_r = 0x40056df8;
vasnprintf = 0x40056e58;
_vasnprintf_r = 0x40056df8;
vasprintf = 0x40056eb8;
_vasprintf_r = 0x40056e80;
vfiprintf = 0x40057ae8;
_vfiprintf_r = 0x40057850;
vfiscanf = 0x40057eb8;
_vfiscanf_r = 0x40057f24;
vfprintf = 0x40057ae8;
_vfprintf_r = 0x40057850;
vfscanf = 0x40057eb8;
_vfscanf_r = 0x40057f24;
viprintf = 0x400569b4;
_viprintf_r = 0x400569e4;
viscanf = 0x40058698;
_viscanf_r = 0x400586c8;
vprintf = 0x400569b4;
_vprintf_r = 0x400569e4;
vscanf = 0x40058698;
_vscanf_r = 0x400586c8;
vsiprintf = 0x40056ac4;
_vsiprintf_r = 0x40056a90;
vsiscanf = 0x40058740;
_vsiscanf_r = 0x400586f8;
vsniprintf = 0x40056a68;
_vsniprintf_r = 0x40056a14;
vsnprintf = 0x40056a68;
_vsnprintf_r = 0x40056a14;
vsprintf = 0x40056ac4;
_vsprintf_r = 0x40056a90;
vsscanf = 0x40058740;
_vsscanf_r = 0x400586f8;

/* _print_float and _scanf_float functions in ROM are stubs which call
   real implementations in IDF through the syscall table.
   As such, don't include these ROM symbols.

_printf_float = 0x4000befc;
_scanf_float = 0x4000bf18;

*/
