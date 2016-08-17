/* Definitions for the xt_ioports TIE package */

/*
 * Customer ID=11657; Build=0x5fe96; Copyright (c) 2004-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_ioports_HEADER
#define _XTENSA_xt_ioports_HEADER

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

extern unsigned _TIE_xt_ioports_READ_IMPWIRE(void);
extern void _TIE_xt_ioports_SETB_EXPSTATE(immediate bitindex);
extern void _TIE_xt_ioports_CLRB_EXPSTATE(immediate bitindex);
extern void _TIE_xt_ioports_WRMSK_EXPSTATE(unsigned art, unsigned ars);
extern unsigned _TIE_xt_ioports_RUR_EXPSTATE(void);
extern void _TIE_xt_ioports_WUR_EXPSTATE(unsigned v);
#define READ_IMPWIRE _TIE_xt_ioports_READ_IMPWIRE
#define SETB_EXPSTATE _TIE_xt_ioports_SETB_EXPSTATE
#define CLRB_EXPSTATE _TIE_xt_ioports_CLRB_EXPSTATE
#define WRMSK_EXPSTATE _TIE_xt_ioports_WRMSK_EXPSTATE
#define RUR_EXPSTATE _TIE_xt_ioports_RUR_EXPSTATE
#define REXPSTATE _TIE_xt_ioports_RUR_EXPSTATE
#define RUR230 _TIE_xt_ioports_RUR_EXPSTATE
#define WUR_EXPSTATE _TIE_xt_ioports_WUR_EXPSTATE
#define WEXPSTATE _TIE_xt_ioports_WUR_EXPSTATE
#define WUR230 _TIE_xt_ioports_WUR_EXPSTATE

#ifndef RUR
#define RUR(NUM) RUR##NUM()
#endif

#ifndef WUR
#define WUR(VAL, NUM) WUR##NUM(VAL)
#endif

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_xt_ioports_HEADER */
