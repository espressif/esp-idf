/*
 * SPDX-FileCopyrightText: 1994-2009 Red Hat, Inc.
 *
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD AND Apache-2.0
 *
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 */
#include <_ansi.h>
#include <limits.h>
/*
   Taken from glibc:
   Add the compiler optimization to inhibit loop transformation to library
   calls.  This is used to avoid recursive calls in memset and memmove
   default implementations.
*/
# define __inhibit_loop_to_libcall \
  __attribute__ ((__optimize__ ("-fno-tree-loop-distribute-patterns")))

/* Nonzero if X is not aligned on a "long" boundary.
 * This macro is used to skip a few bytes to find an aligned pointer.
 * It's better to keep it as is even if _HAVE_HW_MISALIGNED_ACCESS is enabled,
 * to avoid small performance penalties (if they are not zero).  */
#define UNALIGNED_X(X) ((long)X & (sizeof (long) - 1))

#define _HAVE_HW_MISALIGNED_ACCESS (__riscv_misaligned_fast || __riscv_misaligned_slow)

#if _HAVE_HW_MISALIGNED_ACCESS
/* Hardware performs unaligned operations with little
 * to no penalty compared to byte-to-byte copy.  */
#define UNALIGNED_X_Y(X, Y) (0)
#else /* _HAVE_HW_MISALIGNED_ACCESS */
/* Nonzero if either X or Y is not aligned on a "long" boundary.  */
#define UNALIGNED_X_Y(X, Y) \
  (((long)X & (sizeof (long) - 1)) | ((long)Y & (sizeof (long) - 1)))
#endif /* _HAVE_HW_MISALIGNED_ACCESS */

/* How many bytes are copied each iteration of the word copy loop.  */
#define LITTLE_BLOCK_SIZE (sizeof (long))

/* How many bytes are copied each iteration of the 4X unrolled loop.  */
#define BIG_BLOCK_SIZE (sizeof (long) << 2)

/* Threshold for punting to the little block byte copier.  */
#define TOO_SMALL_LITTLE_BLOCK(LEN)  ((LEN) < LITTLE_BLOCK_SIZE)

/* Threshold for punting to the big block byte copier.  */
#define TOO_SMALL_BIG_BLOCK(LEN)  ((LEN) < BIG_BLOCK_SIZE)

/* Macros for detecting endchar.  */
#if LONG_MAX == 2147483647L
#define DETECT_NULL(X) (((X) - 0x01010101) & ~(X) & 0x80808080)
#else
#if LONG_MAX == 9223372036854775807L
/* Nonzero if X (a long int) contains a NULL byte.  */
#define DETECT_NULL(X) (((X) - 0x0101010101010101) & ~(X) & 0x8080808080808080)
#else
#error long int is not a 32bit or 64bit type.
#endif
#endif

/* Returns nonzero if (long)X contains the byte used to fill (long)MASK.  */
#define DETECT_CHAR(X, MASK) (DETECT_NULL(X ^ MASK))
