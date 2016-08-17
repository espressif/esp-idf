/* Definitions for the xt_MAC16 TIE package */

/*
 * Customer ID=11657; Build=0x5fe96; Copyright (c) 2004-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_MAC16_HEADER
#define _XTENSA_xt_MAC16_HEADER

#ifdef __XTENSA__
#ifdef __XCC__

#include <xtensa/tie/xt_core.h>
#include <xtensa/tie/xt_mul.h>

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

extern void _TIE_xt_MAC16_UMUL_AA_HH(unsigned ars, unsigned art);
extern void _TIE_xt_MAC16_UMUL_AA_LH(unsigned ars, unsigned art);
extern void _TIE_xt_MAC16_UMUL_AA_HL(unsigned ars, unsigned art);
extern void _TIE_xt_MAC16_UMUL_AA_LL(unsigned ars, unsigned art);
extern void _TIE_xt_MAC16_MUL_AA_HH(unsigned ars, unsigned art);
extern void _TIE_xt_MAC16_MUL_AA_LH(unsigned ars, unsigned art);
extern void _TIE_xt_MAC16_MUL_AA_HL(unsigned ars, unsigned art);
extern void _TIE_xt_MAC16_MUL_AA_LL(unsigned ars, unsigned art);
extern void _TIE_xt_MAC16_MUL_AD_HH(unsigned ars, immediate my);
extern void _TIE_xt_MAC16_MUL_AD_LH(unsigned ars, immediate my);
extern void _TIE_xt_MAC16_MUL_AD_HL(unsigned ars, immediate my);
extern void _TIE_xt_MAC16_MUL_AD_LL(unsigned ars, immediate my);
extern void _TIE_xt_MAC16_MUL_DA_HH(immediate mx, unsigned art);
extern void _TIE_xt_MAC16_MUL_DA_LH(immediate mx, unsigned art);
extern void _TIE_xt_MAC16_MUL_DA_HL(immediate mx, unsigned art);
extern void _TIE_xt_MAC16_MUL_DA_LL(immediate mx, unsigned art);
extern void _TIE_xt_MAC16_MUL_DD_HH(immediate mx, immediate my);
extern void _TIE_xt_MAC16_MUL_DD_LH(immediate mx, immediate my);
extern void _TIE_xt_MAC16_MUL_DD_HL(immediate mx, immediate my);
extern void _TIE_xt_MAC16_MUL_DD_LL(immediate mx, immediate my);
extern void _TIE_xt_MAC16_MULS_AA_HH(unsigned ars, unsigned art);
extern void _TIE_xt_MAC16_MULS_AA_LH(unsigned ars, unsigned art);
extern void _TIE_xt_MAC16_MULS_AA_HL(unsigned ars, unsigned art);
extern void _TIE_xt_MAC16_MULS_AA_LL(unsigned ars, unsigned art);
extern void _TIE_xt_MAC16_MULA_AA_HH(unsigned ars, unsigned art);
extern void _TIE_xt_MAC16_MULA_AA_LH(unsigned ars, unsigned art);
extern void _TIE_xt_MAC16_MULA_AA_HL(unsigned ars, unsigned art);
extern void _TIE_xt_MAC16_MULA_AA_LL(unsigned ars, unsigned art);
extern void _TIE_xt_MAC16_MULS_AD_HH(unsigned ars, immediate my);
extern void _TIE_xt_MAC16_MULS_AD_LH(unsigned ars, immediate my);
extern void _TIE_xt_MAC16_MULS_AD_HL(unsigned ars, immediate my);
extern void _TIE_xt_MAC16_MULS_AD_LL(unsigned ars, immediate my);
extern void _TIE_xt_MAC16_MULA_AD_HH(unsigned ars, immediate my);
extern void _TIE_xt_MAC16_MULA_AD_LH(unsigned ars, immediate my);
extern void _TIE_xt_MAC16_MULA_AD_HL(unsigned ars, immediate my);
extern void _TIE_xt_MAC16_MULA_AD_LL(unsigned ars, immediate my);
extern void _TIE_xt_MAC16_MULS_DA_HH(immediate mx, unsigned art);
extern void _TIE_xt_MAC16_MULS_DA_LH(immediate mx, unsigned art);
extern void _TIE_xt_MAC16_MULS_DA_HL(immediate mx, unsigned art);
extern void _TIE_xt_MAC16_MULS_DA_LL(immediate mx, unsigned art);
extern void _TIE_xt_MAC16_MULA_DA_HH(immediate mx, unsigned art);
extern void _TIE_xt_MAC16_MULA_DA_LH(immediate mx, unsigned art);
extern void _TIE_xt_MAC16_MULA_DA_HL(immediate mx, unsigned art);
extern void _TIE_xt_MAC16_MULA_DA_LL(immediate mx, unsigned art);
extern void _TIE_xt_MAC16_MULS_DD_HH(immediate mx, immediate my);
extern void _TIE_xt_MAC16_MULS_DD_LH(immediate mx, immediate my);
extern void _TIE_xt_MAC16_MULS_DD_HL(immediate mx, immediate my);
extern void _TIE_xt_MAC16_MULS_DD_LL(immediate mx, immediate my);
extern void _TIE_xt_MAC16_MULA_DD_HH(immediate mx, immediate my);
extern void _TIE_xt_MAC16_MULA_DD_LH(immediate mx, immediate my);
extern void _TIE_xt_MAC16_MULA_DD_HL(immediate mx, immediate my);
extern void _TIE_xt_MAC16_MULA_DD_LL(immediate mx, immediate my);
extern unsigned _TIE_xt_MAC16_RSR_M0(void);
extern void _TIE_xt_MAC16_WSR_M0(unsigned art);
extern void _TIE_xt_MAC16_XSR_M0(unsigned art /*inout*/);
extern unsigned _TIE_xt_MAC16_RSR_M1(void);
extern void _TIE_xt_MAC16_WSR_M1(unsigned art);
extern void _TIE_xt_MAC16_XSR_M1(unsigned art /*inout*/);
extern unsigned _TIE_xt_MAC16_RSR_M2(void);
extern void _TIE_xt_MAC16_WSR_M2(unsigned art);
extern void _TIE_xt_MAC16_XSR_M2(unsigned art /*inout*/);
extern unsigned _TIE_xt_MAC16_RSR_M3(void);
extern void _TIE_xt_MAC16_WSR_M3(unsigned art);
extern void _TIE_xt_MAC16_XSR_M3(unsigned art /*inout*/);
extern unsigned _TIE_xt_MAC16_RSR_ACCLO(void);
extern void _TIE_xt_MAC16_WSR_ACCLO(unsigned art);
extern void _TIE_xt_MAC16_XSR_ACCLO(unsigned art /*inout*/);
extern unsigned _TIE_xt_MAC16_RSR_ACCHI(void);
extern void _TIE_xt_MAC16_WSR_ACCHI(unsigned art);
extern void _TIE_xt_MAC16_XSR_ACCHI(unsigned art /*inout*/);
extern void _TIE_xt_MAC16_MULA_DA_LL_LDDEC(immediate w, const short * s /*inout*/, immediate x, int t);
extern void _TIE_xt_MAC16_MULA_DA_LL_LDINC(immediate w, const short * s /*inout*/, immediate x, int t);
extern void _TIE_xt_MAC16_MULA_DA_HL_LDDEC(immediate w, const short * s /*inout*/, immediate x, int t);
extern void _TIE_xt_MAC16_MULA_DA_HL_LDINC(immediate w, const short * s /*inout*/, immediate x, int t);
extern void _TIE_xt_MAC16_MULA_DA_LH_LDDEC(immediate w, const short * s /*inout*/, immediate x, int t);
extern void _TIE_xt_MAC16_MULA_DA_LH_LDINC(immediate w, const short * s /*inout*/, immediate x, int t);
extern void _TIE_xt_MAC16_MULA_DA_HH_LDDEC(immediate w, const short * s /*inout*/, immediate x, int t);
extern void _TIE_xt_MAC16_MULA_DA_HH_LDINC(immediate w, const short * s /*inout*/, immediate x, int t);
extern void _TIE_xt_MAC16_MULA_DD_LL_LDDEC(immediate w, const short * s /*inout*/, immediate x, immediate y);
extern void _TIE_xt_MAC16_MULA_DD_LL_LDINC(immediate w, const short * s /*inout*/, immediate x, immediate y);
extern void _TIE_xt_MAC16_MULA_DD_HL_LDDEC(immediate w, const short * s /*inout*/, immediate x, immediate y);
extern void _TIE_xt_MAC16_MULA_DD_HL_LDINC(immediate w, const short * s /*inout*/, immediate x, immediate y);
extern void _TIE_xt_MAC16_MULA_DD_LH_LDDEC(immediate w, const short * s /*inout*/, immediate x, immediate y);
extern void _TIE_xt_MAC16_MULA_DD_LH_LDINC(immediate w, const short * s /*inout*/, immediate x, immediate y);
extern void _TIE_xt_MAC16_MULA_DD_HH_LDDEC(immediate w, const short * s /*inout*/, immediate x, immediate y);
extern void _TIE_xt_MAC16_MULA_DD_HH_LDINC(immediate w, const short * s /*inout*/, immediate x, immediate y);
extern void _TIE_xt_MAC16_LDDEC(immediate w, const short * p /*inout*/);
extern void _TIE_xt_MAC16_ULDDEC(immediate w, const unsigned short * p /*inout*/);
extern void _TIE_xt_MAC16_SLDDEC(immediate w, const short * p /*inout*/);
extern void _TIE_xt_MAC16_LDINC(immediate w, const short * p /*inout*/);
extern void _TIE_xt_MAC16_ULDINC(immediate w, const unsigned short * p /*inout*/);
extern void _TIE_xt_MAC16_SLDINC(immediate w, const short * p /*inout*/);
extern int _TIE_xt_MAC16_RSR16(void);
extern void _TIE_xt_MAC16_WSR16(int t);
extern void _TIE_xt_MAC16_XSR16(int t /*inout*/);
extern int _TIE_xt_MAC16_RSR17(void);
extern void _TIE_xt_MAC16_WSR17(int t);
extern void _TIE_xt_MAC16_XSR17(int t /*inout*/);
#define XT_UMUL_AA_HH _TIE_xt_MAC16_UMUL_AA_HH
#define XT_UMUL_AA_LH _TIE_xt_MAC16_UMUL_AA_LH
#define XT_UMUL_AA_HL _TIE_xt_MAC16_UMUL_AA_HL
#define XT_UMUL_AA_LL _TIE_xt_MAC16_UMUL_AA_LL
#define XT_MUL_AA_HH _TIE_xt_MAC16_MUL_AA_HH
#define XT_MUL_AA_LH _TIE_xt_MAC16_MUL_AA_LH
#define XT_MUL_AA_HL _TIE_xt_MAC16_MUL_AA_HL
#define XT_MUL_AA_LL _TIE_xt_MAC16_MUL_AA_LL
#define XT_MUL_AD_HH _TIE_xt_MAC16_MUL_AD_HH
#define XT_MUL_AD_LH _TIE_xt_MAC16_MUL_AD_LH
#define XT_MUL_AD_HL _TIE_xt_MAC16_MUL_AD_HL
#define XT_MUL_AD_LL _TIE_xt_MAC16_MUL_AD_LL
#define XT_MUL_DA_HH _TIE_xt_MAC16_MUL_DA_HH
#define XT_MUL_DA_LH _TIE_xt_MAC16_MUL_DA_LH
#define XT_MUL_DA_HL _TIE_xt_MAC16_MUL_DA_HL
#define XT_MUL_DA_LL _TIE_xt_MAC16_MUL_DA_LL
#define XT_MUL_DD_HH _TIE_xt_MAC16_MUL_DD_HH
#define XT_MUL_DD_LH _TIE_xt_MAC16_MUL_DD_LH
#define XT_MUL_DD_HL _TIE_xt_MAC16_MUL_DD_HL
#define XT_MUL_DD_LL _TIE_xt_MAC16_MUL_DD_LL
#define XT_MULS_AA_HH _TIE_xt_MAC16_MULS_AA_HH
#define XT_MULS_AA_LH _TIE_xt_MAC16_MULS_AA_LH
#define XT_MULS_AA_HL _TIE_xt_MAC16_MULS_AA_HL
#define XT_MULS_AA_LL _TIE_xt_MAC16_MULS_AA_LL
#define XT_MULA_AA_HH _TIE_xt_MAC16_MULA_AA_HH
#define XT_MULA_AA_LH _TIE_xt_MAC16_MULA_AA_LH
#define XT_MULA_AA_HL _TIE_xt_MAC16_MULA_AA_HL
#define XT_MULA_AA_LL _TIE_xt_MAC16_MULA_AA_LL
#define XT_MULS_AD_HH _TIE_xt_MAC16_MULS_AD_HH
#define XT_MULS_AD_LH _TIE_xt_MAC16_MULS_AD_LH
#define XT_MULS_AD_HL _TIE_xt_MAC16_MULS_AD_HL
#define XT_MULS_AD_LL _TIE_xt_MAC16_MULS_AD_LL
#define XT_MULA_AD_HH _TIE_xt_MAC16_MULA_AD_HH
#define XT_MULA_AD_LH _TIE_xt_MAC16_MULA_AD_LH
#define XT_MULA_AD_HL _TIE_xt_MAC16_MULA_AD_HL
#define XT_MULA_AD_LL _TIE_xt_MAC16_MULA_AD_LL
#define XT_MULS_DA_HH _TIE_xt_MAC16_MULS_DA_HH
#define XT_MULS_DA_LH _TIE_xt_MAC16_MULS_DA_LH
#define XT_MULS_DA_HL _TIE_xt_MAC16_MULS_DA_HL
#define XT_MULS_DA_LL _TIE_xt_MAC16_MULS_DA_LL
#define XT_MULA_DA_HH _TIE_xt_MAC16_MULA_DA_HH
#define XT_MULA_DA_LH _TIE_xt_MAC16_MULA_DA_LH
#define XT_MULA_DA_HL _TIE_xt_MAC16_MULA_DA_HL
#define XT_MULA_DA_LL _TIE_xt_MAC16_MULA_DA_LL
#define XT_MULS_DD_HH _TIE_xt_MAC16_MULS_DD_HH
#define XT_MULS_DD_LH _TIE_xt_MAC16_MULS_DD_LH
#define XT_MULS_DD_HL _TIE_xt_MAC16_MULS_DD_HL
#define XT_MULS_DD_LL _TIE_xt_MAC16_MULS_DD_LL
#define XT_MULA_DD_HH _TIE_xt_MAC16_MULA_DD_HH
#define XT_MULA_DD_LH _TIE_xt_MAC16_MULA_DD_LH
#define XT_MULA_DD_HL _TIE_xt_MAC16_MULA_DD_HL
#define XT_MULA_DD_LL _TIE_xt_MAC16_MULA_DD_LL
#define XT_RSR_M0 _TIE_xt_MAC16_RSR_M0
#define XT_WSR_M0 _TIE_xt_MAC16_WSR_M0
#define XT_XSR_M0 _TIE_xt_MAC16_XSR_M0
#define XT_RSR_M1 _TIE_xt_MAC16_RSR_M1
#define XT_WSR_M1 _TIE_xt_MAC16_WSR_M1
#define XT_XSR_M1 _TIE_xt_MAC16_XSR_M1
#define XT_RSR_M2 _TIE_xt_MAC16_RSR_M2
#define XT_WSR_M2 _TIE_xt_MAC16_WSR_M2
#define XT_XSR_M2 _TIE_xt_MAC16_XSR_M2
#define XT_RSR_M3 _TIE_xt_MAC16_RSR_M3
#define XT_WSR_M3 _TIE_xt_MAC16_WSR_M3
#define XT_XSR_M3 _TIE_xt_MAC16_XSR_M3
#define XT_RSR_ACCLO _TIE_xt_MAC16_RSR_ACCLO
#define XT_WSR_ACCLO _TIE_xt_MAC16_WSR_ACCLO
#define XT_XSR_ACCLO _TIE_xt_MAC16_XSR_ACCLO
#define XT_RSR_ACCHI _TIE_xt_MAC16_RSR_ACCHI
#define XT_WSR_ACCHI _TIE_xt_MAC16_WSR_ACCHI
#define XT_XSR_ACCHI _TIE_xt_MAC16_XSR_ACCHI
#define XT_MULA_DA_LL_LDDEC _TIE_xt_MAC16_MULA_DA_LL_LDDEC
#define XT_MULA_DA_LL_LDINC _TIE_xt_MAC16_MULA_DA_LL_LDINC
#define XT_MULA_DA_HL_LDDEC _TIE_xt_MAC16_MULA_DA_HL_LDDEC
#define XT_MULA_DA_HL_LDINC _TIE_xt_MAC16_MULA_DA_HL_LDINC
#define XT_MULA_DA_LH_LDDEC _TIE_xt_MAC16_MULA_DA_LH_LDDEC
#define XT_MULA_DA_LH_LDINC _TIE_xt_MAC16_MULA_DA_LH_LDINC
#define XT_MULA_DA_HH_LDDEC _TIE_xt_MAC16_MULA_DA_HH_LDDEC
#define XT_MULA_DA_HH_LDINC _TIE_xt_MAC16_MULA_DA_HH_LDINC
#define XT_MULA_DD_LL_LDDEC _TIE_xt_MAC16_MULA_DD_LL_LDDEC
#define XT_MULA_DD_LL_LDINC _TIE_xt_MAC16_MULA_DD_LL_LDINC
#define XT_MULA_DD_HL_LDDEC _TIE_xt_MAC16_MULA_DD_HL_LDDEC
#define XT_MULA_DD_HL_LDINC _TIE_xt_MAC16_MULA_DD_HL_LDINC
#define XT_MULA_DD_LH_LDDEC _TIE_xt_MAC16_MULA_DD_LH_LDDEC
#define XT_MULA_DD_LH_LDINC _TIE_xt_MAC16_MULA_DD_LH_LDINC
#define XT_MULA_DD_HH_LDDEC _TIE_xt_MAC16_MULA_DD_HH_LDDEC
#define XT_MULA_DD_HH_LDINC _TIE_xt_MAC16_MULA_DD_HH_LDINC
#define XT_LDDEC _TIE_xt_MAC16_LDDEC
#define XT_ULDDEC _TIE_xt_MAC16_ULDDEC
#define XT_SLDDEC _TIE_xt_MAC16_SLDDEC
#define XT_LDINC _TIE_xt_MAC16_LDINC
#define XT_ULDINC _TIE_xt_MAC16_ULDINC
#define XT_SLDINC _TIE_xt_MAC16_SLDINC
#define XT_RSR16 _TIE_xt_MAC16_RSR16
#define XT_WSR16 _TIE_xt_MAC16_WSR16
#define XT_XSR16 _TIE_xt_MAC16_XSR16
#define XT_RSR17 _TIE_xt_MAC16_RSR17
#define XT_WSR17 _TIE_xt_MAC16_WSR17
#define XT_XSR17 _TIE_xt_MAC16_XSR17

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_xt_MAC16_HEADER */
