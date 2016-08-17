/* Definitions for the xt_density TIE package */

/*
 * Customer ID=11657; Build=0x5fe96; Copyright (c) 2004-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_density_HEADER
#define _XTENSA_xt_density_HEADER

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

extern void _TIE_xt_density_ILL_N(void);
extern void _TIE_xt_density_NOP_N(void);
extern int _TIE_xt_density_L32I_N(const int * p, immediate i);
extern void _TIE_xt_density_S32I_N(int t, int * p, immediate i);
extern int _TIE_xt_density_ADD_N(int s, int t);
extern int _TIE_xt_density_ADDI_N(int s, immediate i);
extern int _TIE_xt_density_MOV_N(int s);
extern int _TIE_xt_density_MOVI_N(immediate i);
#define XT_ILL_N _TIE_xt_density_ILL_N
#define XT_NOP_N _TIE_xt_density_NOP_N
#define XT_L32I_N _TIE_xt_density_L32I_N
#define XT_S32I_N _TIE_xt_density_S32I_N
#define XT_ADD_N _TIE_xt_density_ADD_N
#define XT_ADDI_N _TIE_xt_density_ADDI_N
#define XT_MOV_N _TIE_xt_density_MOV_N
#define XT_MOVI_N _TIE_xt_density_MOVI_N

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_xt_density_HEADER */
