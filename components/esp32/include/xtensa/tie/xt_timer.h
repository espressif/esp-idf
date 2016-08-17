/* Definitions for the xt_timer TIE package */

/*
 * Customer ID=11657; Build=0x5fe96; Copyright (c) 2004-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_timer_HEADER
#define _XTENSA_xt_timer_HEADER

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

extern unsigned _TIE_xt_timer_RSR_CCOUNT(void);
extern void _TIE_xt_timer_WSR_CCOUNT(unsigned art);
extern void _TIE_xt_timer_XSR_CCOUNT(unsigned art /*inout*/);
extern unsigned _TIE_xt_timer_RSR_CCOMPARE0(void);
extern void _TIE_xt_timer_WSR_CCOMPARE0(unsigned art);
extern void _TIE_xt_timer_XSR_CCOMPARE0(unsigned art /*inout*/);
extern unsigned _TIE_xt_timer_RSR_CCOMPARE1(void);
extern void _TIE_xt_timer_WSR_CCOMPARE1(unsigned art);
extern void _TIE_xt_timer_XSR_CCOMPARE1(unsigned art /*inout*/);
extern unsigned _TIE_xt_timer_RSR_CCOMPARE2(void);
extern void _TIE_xt_timer_WSR_CCOMPARE2(unsigned art);
extern void _TIE_xt_timer_XSR_CCOMPARE2(unsigned art /*inout*/);
#define XT_RSR_CCOUNT _TIE_xt_timer_RSR_CCOUNT
#define XT_WSR_CCOUNT _TIE_xt_timer_WSR_CCOUNT
#define XT_XSR_CCOUNT _TIE_xt_timer_XSR_CCOUNT
#define XT_RSR_CCOMPARE0 _TIE_xt_timer_RSR_CCOMPARE0
#define XT_WSR_CCOMPARE0 _TIE_xt_timer_WSR_CCOMPARE0
#define XT_XSR_CCOMPARE0 _TIE_xt_timer_XSR_CCOMPARE0
#define XT_RSR_CCOMPARE1 _TIE_xt_timer_RSR_CCOMPARE1
#define XT_WSR_CCOMPARE1 _TIE_xt_timer_WSR_CCOMPARE1
#define XT_XSR_CCOMPARE1 _TIE_xt_timer_XSR_CCOMPARE1
#define XT_RSR_CCOMPARE2 _TIE_xt_timer_RSR_CCOMPARE2
#define XT_WSR_CCOMPARE2 _TIE_xt_timer_WSR_CCOMPARE2
#define XT_XSR_CCOMPARE2 _TIE_xt_timer_XSR_CCOMPARE2

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_xt_timer_HEADER */
