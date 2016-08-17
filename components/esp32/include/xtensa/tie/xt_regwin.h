/* Definitions for the xt_regwin TIE package */

/*
 * Customer ID=11657; Build=0x5fe96; Copyright (c) 2004-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_regwin_HEADER
#define _XTENSA_xt_regwin_HEADER

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

extern void _TIE_xt_regwin_ENTRY(unsigned ars /*inout*/, immediate uimm12x8);
extern void _TIE_xt_regwin_ROTW(immediate simm4);
extern int _TIE_xt_regwin_MOVSP(int s);
extern int _TIE_xt_regwin_L32E(const int * s, immediate o);
extern void _TIE_xt_regwin_S32E(int t, int * s, immediate o);
extern unsigned _TIE_xt_regwin_RSR_WINDOWBASE(void);
extern void _TIE_xt_regwin_WSR_WINDOWBASE(unsigned t);
extern void _TIE_xt_regwin_XSR_WINDOWBASE(unsigned t /*inout*/);
extern unsigned _TIE_xt_regwin_RSR_WINDOWSTART(void);
extern void _TIE_xt_regwin_WSR_WINDOWSTART(unsigned t);
extern void _TIE_xt_regwin_XSR_WINDOWSTART(unsigned t /*inout*/);
#define XT_ENTRY _TIE_xt_regwin_ENTRY
#define XT_ROTW _TIE_xt_regwin_ROTW
#define XT_MOVSP _TIE_xt_regwin_MOVSP
#define XT_L32E _TIE_xt_regwin_L32E
#define XT_S32E _TIE_xt_regwin_S32E
#define XT_RSR_WINDOWBASE _TIE_xt_regwin_RSR_WINDOWBASE
#define XT_WSR_WINDOWBASE _TIE_xt_regwin_WSR_WINDOWBASE
#define XT_XSR_WINDOWBASE _TIE_xt_regwin_XSR_WINDOWBASE
#define XT_RSR_WINDOWSTART _TIE_xt_regwin_RSR_WINDOWSTART
#define XT_WSR_WINDOWSTART _TIE_xt_regwin_WSR_WINDOWSTART
#define XT_XSR_WINDOWSTART _TIE_xt_regwin_XSR_WINDOWSTART

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_xt_regwin_HEADER */
