/*  xtruntime-core-state.h  -  core state save area (used eg. by PSO) */
/* $Id: //depot/rel/Eaglenest/Xtensa/OS/include/xtensa/xtruntime-core-state.h#1 $ */

/*
 * Copyright (c) 2012-2013 Tensilica Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _XTOS_CORE_STATE_H_
#define _XTOS_CORE_STATE_H_

/*  Import STRUCT_xxx macros for defining structures:  */
#include <xtensa/xtruntime-frames.h>
#include <xtensa/config/core.h>
#include <xtensa/config/tie.h>

//#define XTOS_PSO_TEST	1		// uncommented for internal PSO testing only

#define CORE_STATE_SIGNATURE	0xB1C5AFED	// pattern that indicates state was saved


/*
 *  Save area for saving entire core state, such as across Power Shut-Off (PSO).
 */

STRUCT_BEGIN
STRUCT_FIELD (long,4,CS_SA_,signature)		// for checking whether state was saved
STRUCT_FIELD (long,4,CS_SA_,restore_label)
STRUCT_FIELD (long,4,CS_SA_,aftersave_label)
STRUCT_AFIELD(long,4,CS_SA_,areg,XCHAL_NUM_AREGS)
#if XCHAL_HAVE_WINDOWED
STRUCT_AFIELD(long,4,CS_SA_,caller_regs,16)	// save a max of 16 caller regs
STRUCT_FIELD (long,4,CS_SA_,caller_regs_saved)  // flag to show if caller regs saved
#endif
#if XCHAL_HAVE_PSO_CDM
STRUCT_FIELD (long,4,CS_SA_,pwrctl)
#endif
#if XCHAL_HAVE_WINDOWED
STRUCT_FIELD (long,4,CS_SA_,windowbase)
STRUCT_FIELD (long,4,CS_SA_,windowstart)
#endif
STRUCT_FIELD (long,4,CS_SA_,sar)
#if XCHAL_HAVE_EXCEPTIONS
STRUCT_FIELD (long,4,CS_SA_,epc1)
STRUCT_FIELD (long,4,CS_SA_,ps)
STRUCT_FIELD (long,4,CS_SA_,excsave1)
# ifdef XCHAL_DOUBLEEXC_VECTOR_VADDR
STRUCT_FIELD (long,4,CS_SA_,depc)
# endif
#endif
#if XCHAL_NUM_INTLEVELS + XCHAL_HAVE_NMI >= 2
STRUCT_AFIELD(long,4,CS_SA_,epc,    XCHAL_NUM_INTLEVELS + XCHAL_HAVE_NMI - 1)
STRUCT_AFIELD(long,4,CS_SA_,eps,    XCHAL_NUM_INTLEVELS + XCHAL_HAVE_NMI - 1)
STRUCT_AFIELD(long,4,CS_SA_,excsave,XCHAL_NUM_INTLEVELS + XCHAL_HAVE_NMI - 1)
#endif
#if XCHAL_HAVE_LOOPS
STRUCT_FIELD (long,4,CS_SA_,lcount)
STRUCT_FIELD (long,4,CS_SA_,lbeg)
STRUCT_FIELD (long,4,CS_SA_,lend)
#endif
#if XCHAL_HAVE_ABSOLUTE_LITERALS
STRUCT_FIELD (long,4,CS_SA_,litbase)
#endif
#if XCHAL_HAVE_VECBASE
STRUCT_FIELD (long,4,CS_SA_,vecbase)
#endif
#if XCHAL_HAVE_S32C1I && (XCHAL_HW_MIN_VERSION >= XTENSA_HWVERSION_RC_2009_0)	/* have ATOMCTL ? */
STRUCT_FIELD (long,4,CS_SA_,atomctl)
#endif
#if XCHAL_HAVE_PREFETCH
STRUCT_FIELD (long,4,CS_SA_,prefctl)
#endif
#if XCHAL_USE_MEMCTL
STRUCT_FIELD (long,4,CS_SA_,memctl)
#endif
#if XCHAL_HAVE_CCOUNT
STRUCT_FIELD (long,4,CS_SA_,ccount)
STRUCT_AFIELD(long,4,CS_SA_,ccompare, XCHAL_NUM_TIMERS)
#endif
#if XCHAL_HAVE_INTERRUPTS
STRUCT_FIELD (long,4,CS_SA_,intenable)
STRUCT_FIELD (long,4,CS_SA_,interrupt)
#endif
#if XCHAL_HAVE_DEBUG
STRUCT_FIELD (long,4,CS_SA_,icount)
STRUCT_FIELD (long,4,CS_SA_,icountlevel)
STRUCT_FIELD (long,4,CS_SA_,debugcause)
// DDR not saved
# if XCHAL_NUM_DBREAK
STRUCT_AFIELD(long,4,CS_SA_,dbreakc, XCHAL_NUM_DBREAK)
STRUCT_AFIELD(long,4,CS_SA_,dbreaka, XCHAL_NUM_DBREAK)
# endif
# if XCHAL_NUM_IBREAK
STRUCT_AFIELD(long,4,CS_SA_,ibreaka, XCHAL_NUM_IBREAK)
STRUCT_FIELD (long,4,CS_SA_,ibreakenable)
# endif
#endif
#if XCHAL_NUM_MISC_REGS
STRUCT_AFIELD(long,4,CS_SA_,misc,XCHAL_NUM_MISC_REGS)
#endif
#if XCHAL_HAVE_MEM_ECC_PARITY
STRUCT_FIELD (long,4,CS_SA_,mepc)
STRUCT_FIELD (long,4,CS_SA_,meps)
STRUCT_FIELD (long,4,CS_SA_,mesave)
STRUCT_FIELD (long,4,CS_SA_,mesr)
STRUCT_FIELD (long,4,CS_SA_,mecr)
STRUCT_FIELD (long,4,CS_SA_,mevaddr)
#endif

/*  We put this ahead of TLB and other TIE state,
    to keep it within S32I/L32I offset range.  */
#if XCHAL_HAVE_CP
STRUCT_FIELD (long,4,CS_SA_,cpenable)
#endif

/*  TLB state  */
#if XCHAL_HAVE_MIMIC_CACHEATTR || XCHAL_HAVE_XLT_CACHEATTR
STRUCT_AFIELD(long,4,CS_SA_,tlbs,8*2)
#endif
#if XCHAL_HAVE_PTP_MMU
/*  Compute number of auto-refill (ARF) entries as max of I and D,
    to simplify TLB save logic.  On the unusual configs with
    ITLB ARF != DTLB ARF entries, we'll just end up
    saving/restoring some extra entries redundantly.  */
#  if XCHAL_DTLB_ARF_ENTRIES_LOG2 + XCHAL_ITLB_ARF_ENTRIES_LOG2 > 4
#   define ARF_ENTRIES	8
#  else
#   define ARF_ENTRIES	4
#  endif
STRUCT_FIELD (long,4,CS_SA_,ptevaddr)
STRUCT_FIELD (long,4,CS_SA_,rasid)
STRUCT_FIELD (long,4,CS_SA_,dtlbcfg)
STRUCT_FIELD (long,4,CS_SA_,itlbcfg)
/*** WARNING:  past this point, field offsets may be larger than S32I/L32I range ***/
STRUCT_AFIELD(long,4,CS_SA_,tlbs,((4*ARF_ENTRIES+4)*2+3)*2)
# if XCHAL_HAVE_SPANNING_WAY	/* MMU v3 */
STRUCT_AFIELD(long,4,CS_SA_,tlbs_ways56,(4+8)*2*2)
# endif
#endif

/*  TIE state  */
/*  NOTE: NCP area is aligned to XCHAL_TOTAL_SA_ALIGN not XCHAL_NCP_SA_ALIGN,
    because the offsets of all subsequent coprocessor save areas are relative
    to the NCP save area.  */
STRUCT_AFIELD_A(char,1,XCHAL_TOTAL_SA_ALIGN,CS_SA_,ncp,XCHAL_NCP_SA_SIZE)
#if XCHAL_HAVE_CP
STRUCT_AFIELD_A(char,1,XCHAL_CP0_SA_ALIGN,CS_SA_,cp0,XCHAL_CP0_SA_SIZE)
STRUCT_AFIELD_A(char,1,XCHAL_CP1_SA_ALIGN,CS_SA_,cp1,XCHAL_CP1_SA_SIZE)
STRUCT_AFIELD_A(char,1,XCHAL_CP2_SA_ALIGN,CS_SA_,cp2,XCHAL_CP2_SA_SIZE)
STRUCT_AFIELD_A(char,1,XCHAL_CP3_SA_ALIGN,CS_SA_,cp3,XCHAL_CP3_SA_SIZE)
STRUCT_AFIELD_A(char,1,XCHAL_CP4_SA_ALIGN,CS_SA_,cp4,XCHAL_CP4_SA_SIZE)
STRUCT_AFIELD_A(char,1,XCHAL_CP5_SA_ALIGN,CS_SA_,cp5,XCHAL_CP5_SA_SIZE)
STRUCT_AFIELD_A(char,1,XCHAL_CP6_SA_ALIGN,CS_SA_,cp6,XCHAL_CP6_SA_SIZE)
STRUCT_AFIELD_A(char,1,XCHAL_CP7_SA_ALIGN,CS_SA_,cp7,XCHAL_CP7_SA_SIZE)
//STRUCT_AFIELD_A(char,1,XCHAL_CP8_SA_ALIGN,CS_SA_,cp8,XCHAL_CP8_SA_SIZE)
//STRUCT_AFIELD_A(char,1,XCHAL_CP9_SA_ALIGN,CS_SA_,cp9,XCHAL_CP9_SA_SIZE)
//STRUCT_AFIELD_A(char,1,XCHAL_CP10_SA_ALIGN,CS_SA_,cp10,XCHAL_CP10_SA_SIZE)
//STRUCT_AFIELD_A(char,1,XCHAL_CP11_SA_ALIGN,CS_SA_,cp11,XCHAL_CP11_SA_SIZE)
//STRUCT_AFIELD_A(char,1,XCHAL_CP12_SA_ALIGN,CS_SA_,cp12,XCHAL_CP12_SA_SIZE)
//STRUCT_AFIELD_A(char,1,XCHAL_CP13_SA_ALIGN,CS_SA_,cp13,XCHAL_CP13_SA_SIZE)
//STRUCT_AFIELD_A(char,1,XCHAL_CP14_SA_ALIGN,CS_SA_,cp14,XCHAL_CP14_SA_SIZE)
//STRUCT_AFIELD_A(char,1,XCHAL_CP15_SA_ALIGN,CS_SA_,cp15,XCHAL_CP15_SA_SIZE)
#endif

STRUCT_END(XtosCoreState)



//  These are part of non-coprocessor state (ncp):
#if XCHAL_HAVE_MAC16
//STRUCT_FIELD (long,4,CS_SA_,acclo)
//STRUCT_FIELD (long,4,CS_SA_,acchi)
//STRUCT_AFIELD(long,4,CS_SA_,mr, 4)
#endif
#if XCHAL_HAVE_THREADPTR
//STRUCT_FIELD (long,4,CS_SA_,threadptr)
#endif
#if XCHAL_HAVE_S32C1I
//STRUCT_FIELD (long,4,CS_SA_,scompare1)
#endif
#if XCHAL_HAVE_BOOLEANS
//STRUCT_FIELD (long,4,CS_SA_,br)
#endif

//  Not saved:
//	EXCCAUSE ??
//	DEBUGCAUSE ??
//	EXCVADDR ??
//	DDR
//	INTERRUPT
//	... locked cache lines ...

#endif /* _XTOS_CORE_STATE_H_ */

