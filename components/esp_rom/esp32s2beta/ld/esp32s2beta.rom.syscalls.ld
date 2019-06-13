/* These ROM functions call respective entries in the syscall table.
   They are called by other ROM functions (mostly from newlib).
   We don't link to them directly, since in IDF there are actual
   implementations of these functions, with same names.

   I.e.:

   times (in ROM) -> _times_r (in ROM) -> syscall table entry _times_r -> _times_r (in IDF)

   Hence the following entries are provided only for reference
   and commented out.
 */  

/*

PROVIDE ( calloc = 0x40015520 );
PROVIDE ( _calloc_r = 0x40015234 );
PROVIDE ( close = 0x4000848c );
PROVIDE ( _close_r = 0x40015378 );
PROVIDE ( free = 0x400154f4 );
PROVIDE ( _free_r = 0x40015208 );
PROVIDE ( _fstat_r = 0x40015308 );
PROVIDE ( _getpid_r = 0x40015338 );
PROVIDE ( __getreent = 0x400154c8 );
PROVIDE ( _gettimeofday_r = 0x40015294 );
PROVIDE ( _kill_r = 0x4001534c );
PROVIDE ( _link_r = 0x400152d8 );
PROVIDE ( _lock_acquire = 0x40015450 );
PROVIDE ( _lock_acquire_recursive = 0x40015464 );
PROVIDE ( _lock_close = 0x40015428 );
PROVIDE ( _lock_close_recursive = 0x4001543c );
PROVIDE ( _lock_init = 0x40015400 );
PROVIDE ( _lock_init_recursive = 0x40015414 );
PROVIDE ( _lock_release = 0x400154a0 );
PROVIDE ( _lock_release_recursive = 0x400154b4 );
PROVIDE ( _lock_try_acquire = 0x40015478 );
PROVIDE ( _lock_try_acquire_recursive = 0x4001548c );
PROVIDE ( _lseek_r = 0x400153c8 );
PROVIDE ( malloc = 0x400154dc );
PROVIDE ( _malloc_r = 0x400151f0 );
PROVIDE ( open = 0x400084a0 );
PROVIDE ( _open_r = 0x40015390 );
PROVIDE ( _raise_r = 0x400152ac );
PROVIDE ( read = 0x400084f0 );
PROVIDE ( _read_r = 0x400153e4 );
PROVIDE ( realloc = 0x40015508 );
PROVIDE ( _realloc_r = 0x4001521c );
PROVIDE ( _rename_r = 0x40015264 );
PROVIDE ( sbrk = 0x40008508 );
PROVIDE ( _sbrk_r = 0x40015320 );
PROVIDE ( _stat_r = 0x400152f0 );
PROVIDE ( _system_r = 0x4001524c );
PROVIDE ( _times_r = 0x4001527c );
PROVIDE ( _unlink_r = 0x400152c0 );
PROVIDE ( write = 0x4000851c );
PROVIDE ( _write_r = 0x400153ac );
PROVIDE ( _isatty_r = 0x40007c7c );
PROVIDE ( _exit_r = 0x40015364 );

*/
