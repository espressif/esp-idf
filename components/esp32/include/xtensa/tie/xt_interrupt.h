/* Definitions for the xt_interrupt TIE package */

/*
 * Customer ID=11657; Build=0x5fe96; Copyright (c) 2004-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_interrupt_HEADER
#define _XTENSA_xt_interrupt_HEADER

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

extern void _TIE_xt_interrupt_WAITI(immediate s);
extern unsigned _TIE_xt_interrupt_RSR_INTERRUPT(void);
extern void _TIE_xt_interrupt_WSR_INTSET(unsigned art);
extern void _TIE_xt_interrupt_WSR_INTCLEAR(unsigned art);
extern unsigned _TIE_xt_interrupt_RSR_INTENABLE(void);
extern void _TIE_xt_interrupt_WSR_INTENABLE(unsigned art);
extern void _TIE_xt_interrupt_XSR_INTENABLE(unsigned art /*inout*/);
#define XT_WAITI _TIE_xt_interrupt_WAITI
#define XT_RSR_INTERRUPT _TIE_xt_interrupt_RSR_INTERRUPT
#define XT_WSR_INTSET _TIE_xt_interrupt_WSR_INTSET
#define XT_WSR_INTCLEAR _TIE_xt_interrupt_WSR_INTCLEAR
#define XT_RSR_INTENABLE _TIE_xt_interrupt_RSR_INTENABLE
#define XT_WSR_INTENABLE _TIE_xt_interrupt_WSR_INTENABLE
#define XT_XSR_INTENABLE _TIE_xt_interrupt_XSR_INTENABLE

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_xt_interrupt_HEADER */
