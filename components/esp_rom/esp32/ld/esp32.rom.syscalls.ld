/* These ROM functions call respective entries in the syscall table.
   They are called by other ROM functions (mostly from newlib).
   We don't link to them directly, since in IDF there are actual
   implementations of these functions, with same names.

   I.e.:

   times (in ROM) -> _times_r (in ROM) -> syscall table entry _times_r -> _times_r (in IDF)

   Hence the following entries are provided only for reference
   and commented out.
 */  

/*   <--- the following lines are commented out

PROVIDE ( calloc = 0x4000bee4 );
PROVIDE ( free = 0x4000beb8 );
PROVIDE ( _free_r = 0x4000bbcc );
PROVIDE ( _getpid_r = 0x4000bcfc );
PROVIDE ( __getreent = 0x4000be8c );
PROVIDE ( _gettimeofday_r = 0x4000bc58 );
PROVIDE ( _kill_r = 0x4000bd10 );
PROVIDE ( _lock_acquire = 0x4000be14 );
PROVIDE ( _lock_acquire_recursive = 0x4000be28 );
PROVIDE ( _lock_close = 0x4000bdec );
PROVIDE ( _lock_close_recursive = 0x4000be00 );
PROVIDE ( _lock_init = 0x4000bdc4 );
PROVIDE ( _lock_init_recursive = 0x4000bdd8 );
PROVIDE ( _lock_release = 0x4000be64 );
PROVIDE ( _lock_release_recursive = 0x4000be78 );
PROVIDE ( _lock_try_acquire = 0x4000be3c );
PROVIDE ( _lock_try_acquire_recursive = 0x4000be50 );
PROVIDE ( malloc = 0x4000bea0 );
PROVIDE ( _malloc_r = 0x4000bbb4 );
PROVIDE ( _raise_r = 0x4000bc70 );
PROVIDE ( realloc = 0x4000becc );
PROVIDE ( _realloc_r = 0x4000bbe0 );
PROVIDE ( _sbrk_r = 0x4000bce4 );
PROVIDE ( _system_r = 0x4000bc10 );
PROVIDE ( _times_r = 0x4000bc40 );
PROVIDE ( _close_r = 0x4000bd3c );
PROVIDE ( _exit_r = 0x4000bd28 );
PROVIDE ( _fstat_r = 0x4000bccc );
PROVIDE ( _link_r = 0x4000bc9c );
PROVIDE ( _lseek_r = 0x4000bd8c );
PROVIDE ( _open_r = 0x4000bd54 );
PROVIDE ( _read_r = 0x4000bda8 );
PROVIDE ( _rename_r = 0x4000bc28 );
PROVIDE ( _unlink_r = 0x4000bc84 );
PROVIDE ( _write_r = 0x4000bd70 );

    ---> end commented out block
*/


/* These are the non-reentrant versions of syscalls present in the ROM.
   They call the reentrant versions, passing the pointer returned by __getreent
   as the first argument.
 */

close = 0x40001778;
open = 0x4000178c;
read = 0x400017dc;
sbrk = 0x400017f4;
times = 0x40001808;
write = 0x4000181c;
