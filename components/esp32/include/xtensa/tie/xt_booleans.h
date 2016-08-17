/* Definitions for the xt_booleans TIE package */

/*
 * Customer ID=11657; Build=0x5fe96; Copyright (c) 2004-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_booleans_HEADER
#define _XTENSA_xt_booleans_HEADER

#ifdef __XTENSA__
#ifdef __XCC__

#include <xtensa/tie/xt_core.h>
typedef _TIE_xtbool xtbool;
typedef _TIE_xtbool2 xtbool2;
typedef _TIE_xtbool4 xtbool4;
typedef _TIE_xtbool8 xtbool8;
typedef _TIE_xtbool16 xtbool16;

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

extern xtbool _TIE_xt_booleans_XORB(xtbool bs, xtbool bt);
extern xtbool _TIE_xt_booleans_ORBC(xtbool bs, xtbool bt);
extern xtbool _TIE_xt_booleans_ORB(xtbool bs, xtbool bt);
extern xtbool _TIE_xt_booleans_ANDBC(xtbool bs, xtbool bt);
extern xtbool _TIE_xt_booleans_ANDB(xtbool bs, xtbool bt);
extern xtbool _TIE_xt_booleans_ALL4(xtbool4 bs4);
extern xtbool _TIE_xt_booleans_ANY4(xtbool4 bs4);
extern xtbool _TIE_xt_booleans_ALL8(xtbool8 bs8);
extern xtbool _TIE_xt_booleans_ANY8(xtbool8 bs8);
extern void _TIE_xt_booleans_MOVT(unsigned arr /*inout*/, unsigned ars, xtbool bt);
extern void _TIE_xt_booleans_MOVF(unsigned arr /*inout*/, unsigned ars, xtbool bt);
#define XT_XORB _TIE_xt_booleans_XORB
#define XT_ORBC _TIE_xt_booleans_ORBC
#define XT_ORB _TIE_xt_booleans_ORB
#define XT_ANDBC _TIE_xt_booleans_ANDBC
#define XT_ANDB _TIE_xt_booleans_ANDB
#define XT_ALL4 _TIE_xt_booleans_ALL4
#define XT_ANY4 _TIE_xt_booleans_ANY4
#define XT_ALL8 _TIE_xt_booleans_ALL8
#define XT_ANY8 _TIE_xt_booleans_ANY8
#define XT_MOVT _TIE_xt_booleans_MOVT
#define XT_MOVF _TIE_xt_booleans_MOVF

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_xt_booleans_HEADER */
