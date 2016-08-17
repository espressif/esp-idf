/* Definitions for the xt_FP TIE package */

/*
 * Customer ID=11657; Build=0x5fe96; Copyright (c) 2004-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_FP_HEADER
#define _XTENSA_xt_FP_HEADER

#ifdef __XTENSA__
#ifdef __XCC__

#include <xtensa/tie/xt_core.h>
#include <xtensa/tie/xt_booleans.h>
typedef float _TIE_xt_FP_xtfloat;
typedef _TIE_xt_FP_xtfloat xtfloat;

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

extern unsigned _TIE_xt_FP_RUR_FCR(void);
extern void _TIE_xt_FP_WUR_FCR(unsigned v);
extern unsigned _TIE_xt_FP_RUR_FSR(void);
extern void _TIE_xt_FP_WUR_FSR(unsigned v);
extern xtfloat _TIE_xt_FP_xtfloat_loadi(const xtfloat * p, immediate imm8x4);
extern void _TIE_xt_FP_xtfloat_storei(xtfloat t, xtfloat * p, immediate imm8x4);
extern void _TIE_xt_FP_xtfloat_loadip(xtfloat t /*out*/, const xtfloat * p /*inout*/, immediate imm8x4);
extern void _TIE_xt_FP_xtfloat_storeip(xtfloat t, xtfloat * p /*inout*/, immediate imm8x4);
extern xtfloat _TIE_xt_FP_xtfloat_loadx(const xtfloat * p, int imm8x4);
extern void _TIE_xt_FP_xtfloat_storex(xtfloat t, xtfloat * p, int imm8x4);
extern void _TIE_xt_FP_xtfloat_loadxp(xtfloat t /*out*/, const xtfloat * p /*inout*/, int imm8x4);
extern void _TIE_xt_FP_xtfloat_storexp(xtfloat t, xtfloat * p /*inout*/, int imm8x4);
extern xtfloat _TIE_xt_FP_xtfloat_move(xtfloat r);
extern int _TIE_xt_FP_ROUND_S(xtfloat s, immediate t);
extern int _TIE_xt_FP_TRUNC_S(xtfloat s, immediate t);
extern unsigned _TIE_xt_FP_UTRUNC_S(xtfloat s, immediate t);
extern int _TIE_xt_FP_FLOOR_S(xtfloat s, immediate t);
extern int _TIE_xt_FP_CEIL_S(xtfloat s, immediate t);
extern xtfloat _TIE_xt_FP_LSI(const xtfloat * p, immediate imm8x4);
extern void _TIE_xt_FP_SSI(xtfloat t, xtfloat * p, immediate imm8x4);
extern void _TIE_xt_FP_LSIP(xtfloat t /*out*/, const xtfloat * p /*inout*/, immediate imm8x4);
extern void _TIE_xt_FP_SSIP(xtfloat t, xtfloat * p /*inout*/, immediate imm8x4);
extern xtfloat _TIE_xt_FP_LSX(const xtfloat * p, int imm8x4);
extern void _TIE_xt_FP_SSX(xtfloat t, xtfloat * p, int imm8x4);
extern void _TIE_xt_FP_LSXP(xtfloat t /*out*/, const xtfloat * p /*inout*/, int imm8x4);
extern void _TIE_xt_FP_SSXP(xtfloat t, xtfloat * p /*inout*/, int imm8x4);
extern xtfloat _TIE_xt_FP_ABS_S(xtfloat s);
extern xtfloat _TIE_xt_FP_NEG_S(xtfloat s);
extern xtfloat _TIE_xt_FP_MOV_S(xtfloat s);
extern void _TIE_xt_FP_MOVEQZ_S(xtfloat r /*inout*/, xtfloat s, int t);
extern void _TIE_xt_FP_MOVNEZ_S(xtfloat r /*inout*/, xtfloat s, int t);
extern void _TIE_xt_FP_MOVLTZ_S(xtfloat r /*inout*/, xtfloat s, int t);
extern void _TIE_xt_FP_MOVGEZ_S(xtfloat r /*inout*/, xtfloat s, int t);
extern void _TIE_xt_FP_MOVF_S(xtfloat r /*inout*/, xtfloat s, xtbool t);
extern void _TIE_xt_FP_MOVT_S(xtfloat r /*inout*/, xtfloat s, xtbool t);
extern unsigned _TIE_xt_FP_RFR(xtfloat s);
extern xtfloat _TIE_xt_FP_WFR(unsigned s);
extern xtfloat _TIE_xt_FP_FLOAT_S(int s, immediate t);
extern xtfloat _TIE_xt_FP_UFLOAT_S(unsigned s, immediate t);
extern xtbool _TIE_xt_FP_OEQ_S(xtfloat s, xtfloat t);
extern xtbool _TIE_xt_FP_OLE_S(xtfloat s, xtfloat t);
extern xtbool _TIE_xt_FP_OLT_S(xtfloat s, xtfloat t);
extern xtbool _TIE_xt_FP_UEQ_S(xtfloat s, xtfloat t);
extern xtbool _TIE_xt_FP_ULE_S(xtfloat s, xtfloat t);
extern xtbool _TIE_xt_FP_ULT_S(xtfloat s, xtfloat t);
extern xtbool _TIE_xt_FP_UN_S(xtfloat s, xtfloat t);
extern xtfloat _TIE_xt_FP_ADD_S(xtfloat s, xtfloat t);
extern xtfloat _TIE_xt_FP_SUB_S(xtfloat s, xtfloat t);
extern xtfloat _TIE_xt_FP_MUL_S(xtfloat s, xtfloat t);
extern void _TIE_xt_FP_MADD_S(xtfloat r /*inout*/, xtfloat s, xtfloat t);
extern void _TIE_xt_FP_MSUB_S(xtfloat r /*inout*/, xtfloat s, xtfloat t);
extern xtfloat _TIE_xt_FP_RECIP0_S(xtfloat s);
extern xtfloat _TIE_xt_FP_DIV0_S(xtfloat s);
extern xtfloat _TIE_xt_FP_NEXP01_S(xtfloat s);
extern xtfloat _TIE_xt_FP_CONST_S(immediate s);
extern void _TIE_xt_FP_MKDADJ_S(xtfloat r /*inout*/, xtfloat s);
extern xtfloat _TIE_xt_FP_MKSADJ_S(xtfloat s);
extern void _TIE_xt_FP_ADDEXPM_S(xtfloat r /*inout*/, xtfloat s);
extern void _TIE_xt_FP_ADDEXP_S(xtfloat r /*inout*/, xtfloat s);
extern void _TIE_xt_FP_DIVN_S(xtfloat r /*inout*/, xtfloat s, xtfloat t);
extern xtfloat _TIE_xt_FP_RSQRT0_S(xtfloat s);
extern xtfloat _TIE_xt_FP_SQRT0_S(xtfloat s);
extern void _TIE_xt_FP_MADDN_S(xtfloat r /*inout*/, xtfloat s, xtfloat t);
extern xtfloat _TIE_xt_FP_DIV_S(xtfloat s, xtfloat t);
extern xtfloat _TIE_xt_FP_SQRT_S(xtfloat s);
extern xtfloat _TIE_xt_FP_RECIP_S(xtfloat s);
extern xtfloat _TIE_xt_FP_RSQRT_S(xtfloat s);
extern xtfloat _TIE_xt_FP_FSQRT_S(xtfloat s);
#define XT_RUR_FCR _TIE_xt_FP_RUR_FCR
#define RFCR _TIE_xt_FP_RUR_FCR
#define RUR232 _TIE_xt_FP_RUR_FCR
#define XT_WUR_FCR _TIE_xt_FP_WUR_FCR
#define WFCR _TIE_xt_FP_WUR_FCR
#define WUR232 _TIE_xt_FP_WUR_FCR
#define XT_RUR_FSR _TIE_xt_FP_RUR_FSR
#define RFSR _TIE_xt_FP_RUR_FSR
#define RUR233 _TIE_xt_FP_RUR_FSR
#define XT_WUR_FSR _TIE_xt_FP_WUR_FSR
#define WFSR _TIE_xt_FP_WUR_FSR
#define WUR233 _TIE_xt_FP_WUR_FSR
#define XT_xtfloat_loadi _TIE_xt_FP_xtfloat_loadi
#define XT_xtfloat_storei _TIE_xt_FP_xtfloat_storei
#define XT_xtfloat_loadip _TIE_xt_FP_xtfloat_loadip
#define XT_xtfloat_storeip _TIE_xt_FP_xtfloat_storeip
#define XT_xtfloat_loadx _TIE_xt_FP_xtfloat_loadx
#define XT_xtfloat_storex _TIE_xt_FP_xtfloat_storex
#define XT_xtfloat_loadxp _TIE_xt_FP_xtfloat_loadxp
#define XT_xtfloat_storexp _TIE_xt_FP_xtfloat_storexp
#define XT_xtfloat_move _TIE_xt_FP_xtfloat_move
#define XT_ROUND_S _TIE_xt_FP_ROUND_S
#define XT_TRUNC_S _TIE_xt_FP_TRUNC_S
#define XT_UTRUNC_S _TIE_xt_FP_UTRUNC_S
#define XT_FLOOR_S _TIE_xt_FP_FLOOR_S
#define XT_CEIL_S _TIE_xt_FP_CEIL_S
#define XT_LSI _TIE_xt_FP_LSI
#define XT_SSI _TIE_xt_FP_SSI
#define XT_LSIP _TIE_xt_FP_LSIP
#define XT_SSIP _TIE_xt_FP_SSIP
#define XT_LSX _TIE_xt_FP_LSX
#define XT_SSX _TIE_xt_FP_SSX
#define XT_LSXP _TIE_xt_FP_LSXP
#define XT_SSXP _TIE_xt_FP_SSXP
#define XT_ABS_S _TIE_xt_FP_ABS_S
#define XT_NEG_S _TIE_xt_FP_NEG_S
#define XT_MOV_S _TIE_xt_FP_MOV_S
#define XT_MOVEQZ_S _TIE_xt_FP_MOVEQZ_S
#define XT_MOVNEZ_S _TIE_xt_FP_MOVNEZ_S
#define XT_MOVLTZ_S _TIE_xt_FP_MOVLTZ_S
#define XT_MOVGEZ_S _TIE_xt_FP_MOVGEZ_S
#define XT_MOVF_S _TIE_xt_FP_MOVF_S
#define XT_MOVT_S _TIE_xt_FP_MOVT_S
#define XT_RFR _TIE_xt_FP_RFR
#define XT_WFR _TIE_xt_FP_WFR
#define XT_FLOAT_S _TIE_xt_FP_FLOAT_S
#define XT_UFLOAT_S _TIE_xt_FP_UFLOAT_S
#define XT_OEQ_S _TIE_xt_FP_OEQ_S
#define XT_OLE_S _TIE_xt_FP_OLE_S
#define XT_OLT_S _TIE_xt_FP_OLT_S
#define XT_UEQ_S _TIE_xt_FP_UEQ_S
#define XT_ULE_S _TIE_xt_FP_ULE_S
#define XT_ULT_S _TIE_xt_FP_ULT_S
#define XT_UN_S _TIE_xt_FP_UN_S
#define XT_ADD_S _TIE_xt_FP_ADD_S
#define XT_SUB_S _TIE_xt_FP_SUB_S
#define XT_MUL_S _TIE_xt_FP_MUL_S
#define XT_MADD_S _TIE_xt_FP_MADD_S
#define XT_MSUB_S _TIE_xt_FP_MSUB_S
#define XT_RECIP0_S _TIE_xt_FP_RECIP0_S
#define XT_DIV0_S _TIE_xt_FP_DIV0_S
#define XT_NEXP01_S _TIE_xt_FP_NEXP01_S
#define XT_CONST_S _TIE_xt_FP_CONST_S
#define XT_MKDADJ_S _TIE_xt_FP_MKDADJ_S
#define XT_MKSADJ_S _TIE_xt_FP_MKSADJ_S
#define XT_ADDEXPM_S _TIE_xt_FP_ADDEXPM_S
#define XT_ADDEXP_S _TIE_xt_FP_ADDEXP_S
#define XT_DIVN_S _TIE_xt_FP_DIVN_S
#define XT_RSQRT0_S _TIE_xt_FP_RSQRT0_S
#define XT_SQRT0_S _TIE_xt_FP_SQRT0_S
#define XT_MADDN_S _TIE_xt_FP_MADDN_S
#define XT_DIV_S _TIE_xt_FP_DIV_S
#define XT_SQRT_S _TIE_xt_FP_SQRT_S
#define XT_RECIP_S _TIE_xt_FP_RECIP_S
#define XT_RSQRT_S _TIE_xt_FP_RSQRT_S
#define XT_FSQRT_S _TIE_xt_FP_FSQRT_S

#ifndef RUR
#define RUR(NUM) RUR##NUM()
#endif

#ifndef WUR
#define WUR(VAL, NUM) WUR##NUM(VAL)
#endif

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_xt_FP_HEADER */
