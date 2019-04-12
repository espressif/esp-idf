/*
	Address table for printf/scanf family of functions in ESP32C ROM.
	These functions are compiled with newlib "nano" format option.
	As such, they don's support 64-bit integer formats.
	Floating point formats are supported by setting _printf_float and
	_scanf_float entries in syscall table. This is done automatically
	by startup code.

	Generated for ROM with MD5sum:
	b8cb1311c14d3fd5d32ed57d176e99d9  eagle.pro.rom.out
*/

/*
 fucntions that support in ESP32, but not in ESP32C
PROVIDE ( asiprintf = 0x40056d9c );
PROVIDE ( _asiprintf_r = 0x40056d4c );
PROVIDE ( asniprintf = 0x40056cd8 );
PROVIDE ( _asniprintf_r = 0x40056c64 );
PROVIDE ( asnprintf = 0x40056cd8 );
PROVIDE ( _asnprintf_r = 0x40056c64 );
PROVIDE ( asprintf = 0x40056d9c );
PROVIDE ( _asprintf_r = 0x40056d4c );
PROVIDE ( fiscanf = 0x40058884 );
PROVIDE ( _fiscanf_r = 0x400588b4 );
PROVIDE ( iprintf = 0x40056978 );
PROVIDE ( _iprintf_r = 0x40056944 );
PROVIDE ( printf = 0x40056978 );
PROVIDE ( _printf_r = 0x40056944 );
PROVIDE ( __svfiscanf_r = 0x40057b08 );
PROVIDE ( __svfscanf = 0x40057f04 );
PROVIDE ( __svfscanf_r = 0x40057b08 );
PROVIDE ( vasiprintf = 0x40056eb8 );
PROVIDE ( _vasiprintf_r = 0x40056e80 );
PROVIDE ( vasniprintf = 0x40056e58 );
PROVIDE ( _vasniprintf_r = 0x40056df8 );
PROVIDE ( vasnprintf = 0x40056e58 );
PROVIDE ( _vasnprintf_r = 0x40056df8 );
PROVIDE ( vasprintf = 0x40056eb8 );
PROVIDE ( _vasprintf_r = 0x40056e80 );
PROVIDE ( vfiscanf = 0x40057eb8 );
PROVIDE ( _vfiscanf_r = 0x40057f24 );
PROVIDE ( vfscanf = 0x40057eb8 );
PROVIDE ( _vfscanf_r = 0x40057f24 );
PROVIDE ( viprintf = 0x400569b4 );
PROVIDE ( _viprintf_r = 0x400569e4 );
PROVIDE ( viscanf = 0x40058698 );
PROVIDE ( _viscanf_r = 0x400586c8 );
PROVIDE ( vprintf = 0x400569b4 );
PROVIDE ( _vprintf_r = 0x400569e4 );
PROVIDE ( vscanf = 0x40058698 );
PROVIDE ( _vscanf_r = 0x400586c8 );
PROVIDE ( vsiprintf = 0x40056ac4 );
PROVIDE ( _vsiprintf_r = 0x40056a90 );
PROVIDE ( vsiscanf = 0x40058740 );
PROVIDE ( _vsiscanf_r = 0x400586f8 );
PROVIDE ( vsniprintf = 0x40056a68 );
PROVIDE ( _vsniprintf_r = 0x40056a14 );
PROVIDE ( vsnprintf = 0x40056a68 );
PROVIDE ( _vsnprintf_r = 0x40056a14 );
PROVIDE ( vsprintf = 0x40056ac4 );
PROVIDE ( _vsprintf_r = 0x40056a90 );
PROVIDE ( vsscanf = 0x40058740 );
PROVIDE ( _vsscanf_r = 0x400586f8 );
PROVIDE ( fscanf = 0x40058884 );
PROVIDE ( _fscanf_r = 0x400588b4 );
PROVIDE ( iscanf = 0x40058760 );
PROVIDE ( _iscanf_r = 0x4005879c );
PROVIDE ( scanf = 0x40058760 );
PROVIDE ( _scanf_r = 0x4005879c );
*/
PROVIDE ( fiprintf = 0x400220d4 );
PROVIDE ( _fiprintf_r = 0x400220b0 );
PROVIDE ( fprintf = 0x400220d4 );
PROVIDE ( _fprintf_r = 0x400220b0 );
PROVIDE ( _printf_common = 0x40022510 );
PROVIDE ( _printf_float = 0x4000dc40 );
PROVIDE ( _printf_i = 0x400225dc );
PROVIDE ( siprintf = 0x40022054 );
PROVIDE ( _siprintf_r = 0x40022008 );
PROVIDE ( sniprintf = 0x40021f98 );
PROVIDE ( _sniprintf_r = 0x40021f30 );
PROVIDE ( snprintf = 0x40021f98 );
PROVIDE ( _snprintf_r = 0x40021f30 );
PROVIDE ( sprintf = 0x40022054 );
PROVIDE ( _sprintf_r = 0x40022008 );
PROVIDE ( __sprint_r = 0x400230e8 );
PROVIDE ( _svfiprintf_r = 0x400222d8 );
PROVIDE ( _svfprintf_r = 0x400222d8 );
PROVIDE ( vfiprintf = 0x400233ec );
PROVIDE ( _vfiprintf_r = 0x40023154 );
PROVIDE ( vfprintf = 0x400233ec );
PROVIDE ( _vfprintf_r = 0x40023154 );
PROVIDE ( _scanf_chars = 0x40022d4c );
PROVIDE ( _scanf_float = 0x4000dc5c );
PROVIDE ( _scanf_i = 0x40022e24 );
PROVIDE ( siscanf = 0x4002340c );
PROVIDE ( _siscanf_r = 0x4002346c );
PROVIDE ( sscanf = 0x4002340c );
PROVIDE ( _sscanf_r = 0x4002346c );
PROVIDE ( __ssvfiscanf_r = 0x400229f4 );
PROVIDE ( __ssvfscanf_r = 0x400229f4 );
