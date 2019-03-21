/*
	Address table for printf/scanf family of functions in ESP32 ROM.
	These functions are compiled with newlib "nano" format option.
	As such, they don's support 64-bit integer formats.
	Floating point formats are supported by setting _printf_float and
	_scanf_float entries in syscall table. This is done automatically
	by startup code.

	Generated for ROM with MD5sum:
	ab8282ae908fe9e7a63fb2a4ac2df013  eagle.pro.rom.out
*/

PROVIDE ( asiprintf = 0x40056d9c );
PROVIDE ( _asiprintf_r = 0x40056d4c );
PROVIDE ( asniprintf = 0x40056cd8 );
PROVIDE ( _asniprintf_r = 0x40056c64 );
PROVIDE ( asnprintf = 0x40056cd8 );
PROVIDE ( _asnprintf_r = 0x40056c64 );
PROVIDE ( asprintf = 0x40056d9c );
PROVIDE ( _asprintf_r = 0x40056d4c );
PROVIDE ( fiprintf = 0x40056efc );
PROVIDE ( _fiprintf_r = 0x40056ed8 );
PROVIDE ( fiscanf = 0x40058884 );
PROVIDE ( _fiscanf_r = 0x400588b4 );
PROVIDE ( fprintf = 0x40056efc );
PROVIDE ( _fprintf_r = 0x40056ed8 );
PROVIDE ( iprintf = 0x40056978 );
PROVIDE ( _iprintf_r = 0x40056944 );
PROVIDE ( printf = 0x40056978 );
PROVIDE ( _printf_common = 0x40057338 );
PROVIDE ( _printf_float = 0x4000befc );
PROVIDE ( _printf_i = 0x40057404 );
PROVIDE ( _printf_r = 0x40056944 );
PROVIDE ( siprintf = 0x40056c08 );
PROVIDE ( _siprintf_r = 0x40056bbc );
PROVIDE ( sniprintf = 0x40056b4c );
PROVIDE ( _sniprintf_r = 0x40056ae4 );
PROVIDE ( snprintf = 0x40056b4c );
PROVIDE ( _snprintf_r = 0x40056ae4 );
PROVIDE ( sprintf = 0x40056c08 );
PROVIDE ( _sprintf_r = 0x40056bbc );
PROVIDE ( __sprint_r = 0x400577e4 );
PROVIDE ( _svfiprintf_r = 0x40057100 );
PROVIDE ( __svfiscanf_r = 0x40057b08 );
PROVIDE ( _svfprintf_r = 0x40057100 );
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
PROVIDE ( vfiprintf = 0x40057ae8 );
PROVIDE ( _vfiprintf_r = 0x40057850 );
PROVIDE ( vfiscanf = 0x40057eb8 );
PROVIDE ( _vfiscanf_r = 0x40057f24 );
PROVIDE ( vfprintf = 0x40057ae8 );
PROVIDE ( _vfprintf_r = 0x40057850 );
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
PROVIDE ( _scanf_chars = 0x40058384 );
PROVIDE ( _scanf_float = 0x4000bf18 );
PROVIDE ( _scanf_i = 0x4005845c );
PROVIDE ( _scanf_r = 0x4005879c );
PROVIDE ( siscanf = 0x400587d0 );
PROVIDE ( _siscanf_r = 0x40058830 );
PROVIDE ( sscanf = 0x400587d0 );
PROVIDE ( _sscanf_r = 0x40058830 );
PROVIDE ( __ssvfiscanf_r = 0x4005802c );
PROVIDE ( __ssvfscanf_r = 0x4005802c );


