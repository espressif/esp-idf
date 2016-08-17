#ifndef __SYS_CONFIG_H__
#define __SYS_CONFIG_H__

#include <machine/ieeefp.h>  /* floating point macros */
#include <sys/features.h>	/* POSIX defs */

#ifndef __EXPORT
#define __EXPORT
#endif

#ifndef __IMPORT
#define __IMPORT
#endif

/* Define return type of read/write routines.  In POSIX, the return type
   for read()/write() is "ssize_t" but legacy newlib code has been using
   "int" for some time.  If not specified, "int" is defaulted.  */
#ifndef _READ_WRITE_RETURN_TYPE
#define _READ_WRITE_RETURN_TYPE int
#endif
/* Define `count' parameter of read/write routines.  In POSIX, the `count'
   parameter is "size_t" but legacy newlib code has been using "int" for some
   time.  If not specified, "int" is defaulted.  */
#ifndef _READ_WRITE_BUFSIZE_TYPE
#define _READ_WRITE_BUFSIZE_TYPE int
#endif

#endif /* __SYS_CONFIG_H__ */
