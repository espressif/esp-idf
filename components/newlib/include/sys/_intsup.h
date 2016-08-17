/*
 * Copyright (c) 2004, 2005 by
 * Ralf Corsepius, Ulm/Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 *
 * Modified for xtensa arch & non-long int32_t, removes automatic setting of __have_long32.
 */

#ifndef _SYS__INTSUP_H
#define _SYS__INTSUP_H

#include <sys/features.h>

#define __STDINT_EXP(x) __##x##__

#define __have_longlong64 1

#endif /* _SYS__INTSUP_H */
