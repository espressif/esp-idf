/* Definitions for the xt_misc TIE package */

/*
 * Customer ID=11657; Build=0x5fe96; Copyright (c) 2004-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_misc_HEADER
#define _XTENSA_xt_misc_HEADER

#ifdef __XTENSA__
#ifdef __XCC__

#include <xtensa/tie/xt_core.h>

/*
 * The following prototypes describe intrinsic functions
 * corresponding to TIE instructions.  Some TIE instructions
 * may produce multiple results (designated as "out" operands
 * in the iclass section) or may have operands used as both
 * inputs and outputs (designated as "inout").  However, the C
 * and C++ languages do not provide syntax that can express
 * the in/out/inout constraints of TIE intrinsics.
 * Nevertheless, the compiler understands these constraints
 * and will check that the intrinsic functions are used
 * correctly.  To improve the readability of these prototypes,
 * the "out" and "inout" parameters are marked accordingly
 * with comments.
 */

extern int _TIE_xt_misc_CLAMPS(int s, immediate i);
extern int _TIE_xt_misc_MIN(int s, int t);
extern int _TIE_xt_misc_MAX(int s, int t);
extern unsigned _TIE_xt_misc_MINU(unsigned s, unsigned t);
extern unsigned _TIE_xt_misc_MAXU(unsigned s, unsigned t);
extern int _TIE_xt_misc_NSA(int s);
extern unsigned _TIE_xt_misc_NSAU(unsigned s);
extern int _TIE_xt_misc_SEXT(int s, immediate i);
#define XT_CLAMPS _TIE_xt_misc_CLAMPS
#define XT_MIN _TIE_xt_misc_MIN
#define XT_MAX _TIE_xt_misc_MAX
#define XT_MINU _TIE_xt_misc_MINU
#define XT_MAXU _TIE_xt_misc_MAXU
#define XT_NSA _TIE_xt_misc_NSA
#define XT_NSAU _TIE_xt_misc_NSAU
#define XT_SEXT _TIE_xt_misc_SEXT

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_xt_misc_HEADER */
