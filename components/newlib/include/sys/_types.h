/* ANSI C namespace clean utility typedefs */

/* This file defines various typedefs needed by the system calls that support
   the C library.  Basically, they're just the POSIX versions with an '_'
   prepended.  This file lives in the `sys' directory so targets can provide
   their own if desired (or they can put target dependant conditionals here).
*/

#ifndef	_SYS__TYPES_H
#define _SYS__TYPES_H

#include <machine/_types.h>
#include <sys/lock.h>

#ifndef __off_t_defined
typedef long _off_t;
#endif

#ifndef __dev_t_defined
typedef short __dev_t;
#endif

#ifndef __uid_t_defined
typedef unsigned short __uid_t;
#endif
#ifndef __gid_t_defined
typedef unsigned short __gid_t;
#endif

#ifndef __off64_t_defined
__extension__ typedef long long _off64_t;
#endif

/*
 * We need fpos_t for the following, but it doesn't have a leading "_",
 * so we use _fpos_t instead.
 */
#ifndef __fpos_t_defined
typedef long _fpos_t;		/* XXX must match off_t in <sys/types.h> */
				/* (and must be `long' for now) */
#endif

#ifdef __LARGE64_FILES
#ifndef __fpos64_t_defined
typedef _off64_t _fpos64_t;
#endif
#endif

#ifndef __ssize_t_defined
#ifdef __SIZE_TYPE__
/* If __SIZE_TYPE__ is defined (gcc) we define ssize_t based on size_t.
   We simply change "unsigned" to "signed" for this single definition
   to make sure ssize_t and size_t only differ by their signedness. */
#define unsigned signed
typedef __SIZE_TYPE__ _ssize_t;
#undef unsigned
#else
#if defined(__INT_MAX__) && __INT_MAX__ == 2147483647
typedef int _ssize_t;
#else
typedef long _ssize_t;
#endif
#endif
#endif

#define __need_wint_t
#include <stddef.h>

#ifndef __mbstate_t_defined
/* Conversion state information.  */
typedef struct
{
  int __count;
  union
  {
    wint_t __wch;
    unsigned char __wchb[4];
  } __value;		/* Value so far.  */
} _mbstate_t;
#endif

#ifndef __flock_t_defined
typedef _LOCK_RECURSIVE_T _flock_t;
#endif

#ifndef __iconv_t_defined
/* Iconv descriptor type */
typedef void *_iconv_t;
#endif

#endif	/* _SYS__TYPES_H */
