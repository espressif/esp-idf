/* Definitions for the xt_sync TIE package */

/*
 * Customer ID=11657; Build=0x5fe96; Copyright (c) 2004-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_sync_HEADER
#define _XTENSA_xt_sync_HEADER

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

extern unsigned _TIE_xt_sync_RSR_SCOMPARE1(void);
extern void _TIE_xt_sync_WSR_SCOMPARE1(unsigned art);
extern void _TIE_xt_sync_XSR_SCOMPARE1(unsigned art /*inout*/);
extern unsigned _TIE_xt_sync_RSR_ATOMCTL(void);
extern void _TIE_xt_sync_WSR_ATOMCTL(unsigned art);
extern void _TIE_xt_sync_XSR_ATOMCTL(unsigned art /*inout*/);
extern unsigned _TIE_xt_sync_L32AI(const unsigned * p, immediate o);
extern void _TIE_xt_sync_S32RI(unsigned c, unsigned * p, immediate o);
extern void _TIE_xt_sync_S32C1I(unsigned c /*inout*/, const unsigned * p, immediate o);
#define XT_RSR_SCOMPARE1 _TIE_xt_sync_RSR_SCOMPARE1
#define XT_WSR_SCOMPARE1 _TIE_xt_sync_WSR_SCOMPARE1
#define XT_XSR_SCOMPARE1 _TIE_xt_sync_XSR_SCOMPARE1
#define XT_RSR_ATOMCTL _TIE_xt_sync_RSR_ATOMCTL
#define XT_WSR_ATOMCTL _TIE_xt_sync_WSR_ATOMCTL
#define XT_XSR_ATOMCTL _TIE_xt_sync_XSR_ATOMCTL
#define XT_L32AI _TIE_xt_sync_L32AI
#define XT_S32RI _TIE_xt_sync_S32RI
#define XT_S32C1I _TIE_xt_sync_S32C1I

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_xt_sync_HEADER */
