/* Definitions for the xt_coprocessors TIE package */

/*
 * Customer ID=11657; Build=0x5fe96; Copyright (c) 2004-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_coprocessors_HEADER
#define _XTENSA_xt_coprocessors_HEADER

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

extern unsigned _TIE_xt_coprocessors_RSR_CPENABLE(void);
extern void _TIE_xt_coprocessors_WSR_CPENABLE(unsigned art);
extern void _TIE_xt_coprocessors_XSR_CPENABLE(unsigned art /*inout*/);
#define XT_RSR_CPENABLE _TIE_xt_coprocessors_RSR_CPENABLE
#define XT_WSR_CPENABLE _TIE_xt_coprocessors_WSR_CPENABLE
#define XT_XSR_CPENABLE _TIE_xt_coprocessors_XSR_CPENABLE

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_xt_coprocessors_HEADER */
