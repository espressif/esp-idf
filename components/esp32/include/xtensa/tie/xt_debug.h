/* Definitions for the xt_debug TIE package */

/*
 * Customer ID=11657; Build=0x5fe96; Copyright (c) 2004-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_debug_HEADER
#define _XTENSA_xt_debug_HEADER

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

extern void _TIE_xt_debug_BREAK(immediate imms, immediate immt);
extern void _TIE_xt_debug_BREAK_N(immediate imms);
extern unsigned _TIE_xt_debug_RSR_DBREAKA0(void);
extern void _TIE_xt_debug_WSR_DBREAKA0(unsigned art);
extern void _TIE_xt_debug_XSR_DBREAKA0(unsigned art /*inout*/);
extern unsigned _TIE_xt_debug_RSR_DBREAKC0(void);
extern void _TIE_xt_debug_WSR_DBREAKC0(unsigned art);
extern void _TIE_xt_debug_XSR_DBREAKC0(unsigned art /*inout*/);
extern unsigned _TIE_xt_debug_RSR_DBREAKA1(void);
extern void _TIE_xt_debug_WSR_DBREAKA1(unsigned art);
extern void _TIE_xt_debug_XSR_DBREAKA1(unsigned art /*inout*/);
extern unsigned _TIE_xt_debug_RSR_DBREAKC1(void);
extern void _TIE_xt_debug_WSR_DBREAKC1(unsigned art);
extern void _TIE_xt_debug_XSR_DBREAKC1(unsigned art /*inout*/);
extern unsigned _TIE_xt_debug_RSR_IBREAKA0(void);
extern void _TIE_xt_debug_WSR_IBREAKA0(unsigned art);
extern void _TIE_xt_debug_XSR_IBREAKA0(unsigned art /*inout*/);
extern unsigned _TIE_xt_debug_RSR_IBREAKA1(void);
extern void _TIE_xt_debug_WSR_IBREAKA1(unsigned art);
extern void _TIE_xt_debug_XSR_IBREAKA1(unsigned art /*inout*/);
extern unsigned _TIE_xt_debug_RSR_IBREAKENABLE(void);
extern void _TIE_xt_debug_WSR_IBREAKENABLE(unsigned art);
extern void _TIE_xt_debug_XSR_IBREAKENABLE(unsigned art /*inout*/);
extern unsigned _TIE_xt_debug_RSR_DEBUGCAUSE(void);
extern void _TIE_xt_debug_WSR_DEBUGCAUSE(unsigned art);
extern void _TIE_xt_debug_XSR_DEBUGCAUSE(unsigned art /*inout*/);
extern unsigned _TIE_xt_debug_RSR_ICOUNT(void);
extern void _TIE_xt_debug_WSR_ICOUNT(unsigned art);
extern void _TIE_xt_debug_XSR_ICOUNT(unsigned art /*inout*/);
extern unsigned _TIE_xt_debug_RSR_ICOUNTLEVEL(void);
extern void _TIE_xt_debug_WSR_ICOUNTLEVEL(unsigned art);
extern void _TIE_xt_debug_XSR_ICOUNTLEVEL(unsigned art /*inout*/);
extern unsigned _TIE_xt_debug_RSR_DDR(void);
extern void _TIE_xt_debug_WSR_DDR(unsigned art);
extern void _TIE_xt_debug_XSR_DDR(unsigned art /*inout*/);
extern void _TIE_xt_debug_LDDR32_P(const void * ars /*inout*/);
extern void _TIE_xt_debug_SDDR32_P(void * ars /*inout*/);
#define XT_BREAK _TIE_xt_debug_BREAK
#define XT_BREAK_N _TIE_xt_debug_BREAK_N
#define XT_RSR_DBREAKA0 _TIE_xt_debug_RSR_DBREAKA0
#define XT_WSR_DBREAKA0 _TIE_xt_debug_WSR_DBREAKA0
#define XT_XSR_DBREAKA0 _TIE_xt_debug_XSR_DBREAKA0
#define XT_RSR_DBREAKC0 _TIE_xt_debug_RSR_DBREAKC0
#define XT_WSR_DBREAKC0 _TIE_xt_debug_WSR_DBREAKC0
#define XT_XSR_DBREAKC0 _TIE_xt_debug_XSR_DBREAKC0
#define XT_RSR_DBREAKA1 _TIE_xt_debug_RSR_DBREAKA1
#define XT_WSR_DBREAKA1 _TIE_xt_debug_WSR_DBREAKA1
#define XT_XSR_DBREAKA1 _TIE_xt_debug_XSR_DBREAKA1
#define XT_RSR_DBREAKC1 _TIE_xt_debug_RSR_DBREAKC1
#define XT_WSR_DBREAKC1 _TIE_xt_debug_WSR_DBREAKC1
#define XT_XSR_DBREAKC1 _TIE_xt_debug_XSR_DBREAKC1
#define XT_RSR_IBREAKA0 _TIE_xt_debug_RSR_IBREAKA0
#define XT_WSR_IBREAKA0 _TIE_xt_debug_WSR_IBREAKA0
#define XT_XSR_IBREAKA0 _TIE_xt_debug_XSR_IBREAKA0
#define XT_RSR_IBREAKA1 _TIE_xt_debug_RSR_IBREAKA1
#define XT_WSR_IBREAKA1 _TIE_xt_debug_WSR_IBREAKA1
#define XT_XSR_IBREAKA1 _TIE_xt_debug_XSR_IBREAKA1
#define XT_RSR_IBREAKENABLE _TIE_xt_debug_RSR_IBREAKENABLE
#define XT_WSR_IBREAKENABLE _TIE_xt_debug_WSR_IBREAKENABLE
#define XT_XSR_IBREAKENABLE _TIE_xt_debug_XSR_IBREAKENABLE
#define XT_RSR_DEBUGCAUSE _TIE_xt_debug_RSR_DEBUGCAUSE
#define XT_WSR_DEBUGCAUSE _TIE_xt_debug_WSR_DEBUGCAUSE
#define XT_XSR_DEBUGCAUSE _TIE_xt_debug_XSR_DEBUGCAUSE
#define XT_RSR_ICOUNT _TIE_xt_debug_RSR_ICOUNT
#define XT_WSR_ICOUNT _TIE_xt_debug_WSR_ICOUNT
#define XT_XSR_ICOUNT _TIE_xt_debug_XSR_ICOUNT
#define XT_RSR_ICOUNTLEVEL _TIE_xt_debug_RSR_ICOUNTLEVEL
#define XT_WSR_ICOUNTLEVEL _TIE_xt_debug_WSR_ICOUNTLEVEL
#define XT_XSR_ICOUNTLEVEL _TIE_xt_debug_XSR_ICOUNTLEVEL
#define XT_RSR_DDR _TIE_xt_debug_RSR_DDR
#define XT_WSR_DDR _TIE_xt_debug_WSR_DDR
#define XT_XSR_DDR _TIE_xt_debug_XSR_DDR
#define XT_LDDR32_P _TIE_xt_debug_LDDR32_P
#define XT_SDDR32_P _TIE_xt_debug_SDDR32_P

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_xt_debug_HEADER */
