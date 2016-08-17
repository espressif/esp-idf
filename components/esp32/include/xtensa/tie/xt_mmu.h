/* Definitions for the xt_mmu TIE package */

/*
 * Customer ID=11657; Build=0x5fe96; Copyright (c) 2004-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_mmu_HEADER
#define _XTENSA_xt_mmu_HEADER

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

extern void _TIE_xt_mmu_IDTLB(unsigned ars);
extern unsigned _TIE_xt_mmu_RDTLB1(unsigned ars);
extern unsigned _TIE_xt_mmu_RDTLB0(unsigned ars);
extern unsigned _TIE_xt_mmu_PDTLB(unsigned ars);
extern void _TIE_xt_mmu_WDTLB(unsigned art, unsigned ars);
extern void _TIE_xt_mmu_IITLB(unsigned ars);
extern unsigned _TIE_xt_mmu_RITLB1(unsigned ars);
extern unsigned _TIE_xt_mmu_RITLB0(unsigned ars);
extern unsigned _TIE_xt_mmu_PITLB(unsigned ars);
extern void _TIE_xt_mmu_WITLB(unsigned art, unsigned ars);
#define XT_IDTLB _TIE_xt_mmu_IDTLB
#define XT_RDTLB1 _TIE_xt_mmu_RDTLB1
#define XT_RDTLB0 _TIE_xt_mmu_RDTLB0
#define XT_PDTLB _TIE_xt_mmu_PDTLB
#define XT_WDTLB _TIE_xt_mmu_WDTLB
#define XT_IITLB _TIE_xt_mmu_IITLB
#define XT_RITLB1 _TIE_xt_mmu_RITLB1
#define XT_RITLB0 _TIE_xt_mmu_RITLB0
#define XT_PITLB _TIE_xt_mmu_PITLB
#define XT_WITLB _TIE_xt_mmu_WITLB

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_xt_mmu_HEADER */
