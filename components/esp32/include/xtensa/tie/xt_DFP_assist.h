/* Definitions for the xt_DFP_assist TIE package */

/*
 * Customer ID=11657; Build=0x5fe96; Copyright (c) 2004-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_DFP_assist_HEADER
#define _XTENSA_xt_DFP_assist_HEADER

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

extern void _TIE_xt_DFP_assist_F64ITER(unsigned arr /*inout*/, unsigned ars, unsigned art, immediate oper, immediate noshift);
extern unsigned _TIE_xt_DFP_assist_F64RND(unsigned ars, unsigned art, immediate mode);
extern void _TIE_xt_DFP_assist_F64ADDC(unsigned art /*inout*/, unsigned ars, immediate immZ, immediate immC);
extern void _TIE_xt_DFP_assist_F64SUBC(unsigned art /*inout*/, unsigned ars, immediate immZ, immediate immC);
extern unsigned _TIE_xt_DFP_assist_F64SIG(unsigned ars);
extern unsigned _TIE_xt_DFP_assist_F64CMPL(unsigned ars, unsigned art);
extern unsigned _TIE_xt_DFP_assist_F64CMPH(unsigned ars, unsigned art, immediate oper);
extern unsigned _TIE_xt_DFP_assist_F64NORM(unsigned ars, unsigned art, immediate mode);
extern unsigned _TIE_xt_DFP_assist_F64SEXP(unsigned ars, unsigned art);
extern unsigned _TIE_xt_DFP_assist_RF64R(immediate hilo);
extern void _TIE_xt_DFP_assist_WF64R(unsigned ars, unsigned art, immediate hilo);
extern unsigned _TIE_xt_DFP_assist_RUR_F64R_LO(void);
extern unsigned _TIE_xt_DFP_assist_RUR_F64R_HI(void);
extern void _TIE_xt_DFP_assist_WUR_F64R_LO(unsigned art);
extern void _TIE_xt_DFP_assist_WUR_F64R_HI(unsigned art);
extern unsigned _TIE_xt_DFP_assist_RUR_F64S(void);
extern void _TIE_xt_DFP_assist_WUR_F64S(unsigned art);
#define F64ITER _TIE_xt_DFP_assist_F64ITER
#define F64RND _TIE_xt_DFP_assist_F64RND
#define F64ADDC _TIE_xt_DFP_assist_F64ADDC
#define F64SUBC _TIE_xt_DFP_assist_F64SUBC
#define F64SIG _TIE_xt_DFP_assist_F64SIG
#define F64CMPL _TIE_xt_DFP_assist_F64CMPL
#define F64CMPH _TIE_xt_DFP_assist_F64CMPH
#define F64NORM _TIE_xt_DFP_assist_F64NORM
#define F64SEXP _TIE_xt_DFP_assist_F64SEXP
#define RF64R _TIE_xt_DFP_assist_RF64R
#define WF64R _TIE_xt_DFP_assist_WF64R
#define RUR_F64R_LO _TIE_xt_DFP_assist_RUR_F64R_LO
#define RF64R_LO _TIE_xt_DFP_assist_RUR_F64R_LO
#define RUR234 _TIE_xt_DFP_assist_RUR_F64R_LO
#define RUR_F64R_HI _TIE_xt_DFP_assist_RUR_F64R_HI
#define RF64R_HI _TIE_xt_DFP_assist_RUR_F64R_HI
#define RUR235 _TIE_xt_DFP_assist_RUR_F64R_HI
#define WUR_F64R_LO _TIE_xt_DFP_assist_WUR_F64R_LO
#define WF64R_LO _TIE_xt_DFP_assist_WUR_F64R_LO
#define WUR234 _TIE_xt_DFP_assist_WUR_F64R_LO
#define WUR_F64R_HI _TIE_xt_DFP_assist_WUR_F64R_HI
#define WF64R_HI _TIE_xt_DFP_assist_WUR_F64R_HI
#define WUR235 _TIE_xt_DFP_assist_WUR_F64R_HI
#define RUR_F64S _TIE_xt_DFP_assist_RUR_F64S
#define RF64S _TIE_xt_DFP_assist_RUR_F64S
#define RUR236 _TIE_xt_DFP_assist_RUR_F64S
#define WUR_F64S _TIE_xt_DFP_assist_WUR_F64S
#define WF64S _TIE_xt_DFP_assist_WUR_F64S
#define WUR236 _TIE_xt_DFP_assist_WUR_F64S

#ifndef RUR
#define RUR(NUM) RUR##NUM()
#endif

#ifndef WUR
#define WUR(VAL, NUM) WUR##NUM(VAL)
#endif

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_xt_DFP_assist_HEADER */
