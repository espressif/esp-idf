/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __RVRUNTIME_FRAMES_H__
#define __RVRUNTIME_FRAMES_H__

#include "soc/soc_caps.h"

/* Align a value up to nearest n-byte boundary, where n is a power of 2. */
#define ALIGNUP(n, val) (((val) + (n) - 1) & -(n))

#ifdef STRUCT_BEGIN
#undef STRUCT_BEGIN
#undef STRUCT_FIELD
#undef STRUCT_AFIELD
#undef STRUCT_END
#endif

#if defined(_ASMLANGUAGE) || defined(__ASSEMBLER__)
#ifdef __clang__
#define STRUCT_BEGIN                            .set RV_STRUCT_OFFSET, 0
#define STRUCT_FIELD(ctype,size,asname,name)    .set asname, RV_STRUCT_OFFSET; .set RV_STRUCT_OFFSET, asname + size
#define STRUCT_AFIELD(ctype,size,asname,name,n) .set asname, RV_STRUCT_OFFSET;\
                                                .set RV_STRUCT_OFFSET, asname + (size)*(n);
#define STRUCT_END(sname)                       .set sname##Size, RV_STRUCT_OFFSET;
#else // __clang__
#define STRUCT_BEGIN            .pushsection .text; .struct 0
#define STRUCT_FIELD(ctype,size,asname,name)    asname: .space  size
#define STRUCT_AFIELD(ctype,size,asname,name,n) asname: .space  (size)*(n)
#define STRUCT_END(sname)       sname##Size:; .popsection
#endif // __clang__
#else
#define STRUCT_BEGIN            typedef struct {
#define STRUCT_FIELD(ctype,size,asname,name)    ctype   name;
#define STRUCT_AFIELD(ctype,size,asname,name,n) ctype   name[n];
#define STRUCT_END(sname)       } sname;
#endif

/*
-------------------------------------------------------------------------------
   INTERRUPT/EXCEPTION STACK FRAME FOR A EXCEPTION OR NESTED INTERRUPT
-------------------------------------------------------------------------------
*/
STRUCT_BEGIN
STRUCT_FIELD (long, 4, RV_STK_MEPC,    mepc)       /* Machine Exception Program Counter */
STRUCT_FIELD (long, 4, RV_STK_RA,      ra)         /* Return address */
STRUCT_FIELD (long, 4, RV_STK_SP,      sp)         /* Stack pointer */
STRUCT_FIELD (long, 4, RV_STK_GP,      gp)         /* Global pointer */
STRUCT_FIELD (long, 4, RV_STK_TP,      tp)         /* Thread pointer */
STRUCT_FIELD (long, 4, RV_STK_T0,      t0)         /* Temporary/alternate link register */
STRUCT_FIELD (long, 4, RV_STK_T1,      t1)         /* t1-2: Temporaries */
STRUCT_FIELD (long, 4, RV_STK_T2,      t2)
STRUCT_FIELD (long, 4, RV_STK_S0,      s0)         /* Saved register/frame pointer */
STRUCT_FIELD (long, 4, RV_STK_S1,      s1)         /* Saved register */
STRUCT_FIELD (long, 4, RV_STK_A0,      a0)         /* a0-1: Function arguments/return address */
STRUCT_FIELD (long, 4, RV_STK_A1,      a1)
STRUCT_FIELD (long, 4, RV_STK_A2,      a2)         /* a2-7: Function arguments */
STRUCT_FIELD (long, 4, RV_STK_A3,      a3)
STRUCT_FIELD (long, 4, RV_STK_A4,      a4)
STRUCT_FIELD (long, 4, RV_STK_A5,      a5)
STRUCT_FIELD (long, 4, RV_STK_A6,      a6)
STRUCT_FIELD (long, 4, RV_STK_A7,      a7)
STRUCT_FIELD (long, 4, RV_STK_S2,      s2)         /* s2-11: Saved registers */
STRUCT_FIELD (long, 4, RV_STK_S3,      s3)
STRUCT_FIELD (long, 4, RV_STK_S4,      s4)
STRUCT_FIELD (long, 4, RV_STK_S5,      s5)
STRUCT_FIELD (long, 4, RV_STK_S6,      s6)
STRUCT_FIELD (long, 4, RV_STK_S7,      s7)
STRUCT_FIELD (long, 4, RV_STK_S8,      s8)
STRUCT_FIELD (long, 4, RV_STK_S9,      s9)
STRUCT_FIELD (long, 4, RV_STK_S10,     s10)
STRUCT_FIELD (long, 4, RV_STK_S11,     s11)
STRUCT_FIELD (long, 4, RV_STK_T3,      t3)         /* t3-6: Temporaries */
STRUCT_FIELD (long, 4, RV_STK_T4,      t4)
STRUCT_FIELD (long, 4, RV_STK_T5,      t5)
STRUCT_FIELD (long, 4, RV_STK_T6,      t6)
STRUCT_FIELD (long, 4, RV_STK_MSTATUS, mstatus)    /* Machine Status */
STRUCT_FIELD (long, 4, RV_STK_MTVEC,   mtvec)      /* Machine Trap-Vector Base Address */
STRUCT_FIELD (long, 4, RV_STK_MCAUSE,  mcause)     /* Machine Trap Cause */
STRUCT_FIELD (long, 4, RV_STK_MTVAL,   mtval)      /* Machine Trap Value */
STRUCT_FIELD (long, 4, RV_STK_MHARTID, mhartid)    /* Hardware Thread ID in machine mode */
STRUCT_END(RvExcFrame)

#if SOC_CPU_COPROC_NUM > 0

#if SOC_CPU_HAS_FPU
/**
 * @brief Floating-Point Unit save area
 */
STRUCT_BEGIN
STRUCT_FIELD (long, 4, RV_FPU_FT0,  ft0)    /* ft0-ft7: Floating Point temporaries */
STRUCT_FIELD (long, 4, RV_FPU_FT1,  ft1)
STRUCT_FIELD (long, 4, RV_FPU_FT2,  ft2)
STRUCT_FIELD (long, 4, RV_FPU_FT3,  ft3)
STRUCT_FIELD (long, 4, RV_FPU_FT4,  ft4)
STRUCT_FIELD (long, 4, RV_FPU_FT5,  ft5)
STRUCT_FIELD (long, 4, RV_FPU_FT6,  ft6)
STRUCT_FIELD (long, 4, RV_FPU_FT7,  ft7)
STRUCT_FIELD (long, 4, RV_FPU_FS0,  fs0)    /* fs0-fs1: Floating Point saved registers */
STRUCT_FIELD (long, 4, RV_FPU_FS1,  fs1)
STRUCT_FIELD (long, 4, RV_FPU_FA0,  fa0)    /* fa0-fa1: Floating Point arguments/return values */
STRUCT_FIELD (long, 4, RV_FPU_FA1,  fa1)
STRUCT_FIELD (long, 4, RV_FPU_FA2,  fa2)    /* fa2-fa7: Floating Point arguments */
STRUCT_FIELD (long, 4, RV_FPU_FA3,  fa3)
STRUCT_FIELD (long, 4, RV_FPU_FA4,  fa4)
STRUCT_FIELD (long, 4, RV_FPU_FA5,  fa5)
STRUCT_FIELD (long, 4, RV_FPU_FA6,  fa6)
STRUCT_FIELD (long, 4, RV_FPU_FA7,  fa7)
STRUCT_FIELD (long, 4, RV_FPU_FS2,  fs2)    /* fs2-fs11: Floating Point saved registers */
STRUCT_FIELD (long, 4, RV_FPU_FS3,  fs3)
STRUCT_FIELD (long, 4, RV_FPU_FS4,  fs4)
STRUCT_FIELD (long, 4, RV_FPU_FS5,  fs5)
STRUCT_FIELD (long, 4, RV_FPU_FS6,  fs6)
STRUCT_FIELD (long, 4, RV_FPU_FS7,  fs7)
STRUCT_FIELD (long, 4, RV_FPU_FS8,  fs8)
STRUCT_FIELD (long, 4, RV_FPU_FS9,  fs9)
STRUCT_FIELD (long, 4, RV_FPU_FS10, fs10)
STRUCT_FIELD (long, 4, RV_FPU_FS11, fs11)
STRUCT_FIELD (long, 4, RV_FPU_FT8,  ft8)    /* ft8-ft11: Floating Point temporary registers */
STRUCT_FIELD (long, 4, RV_FPU_FT9,  ft9)
STRUCT_FIELD (long, 4, RV_FPU_FT10, ft10)
STRUCT_FIELD (long, 4, RV_FPU_FT11, ft11)
STRUCT_FIELD (long, 4, RV_FPU_FCSR, fcsr)   /* fcsr special register */
STRUCT_END(RvFPUSaveArea)

/* Floating-Point Unit coprocessor is now considered coprocessor 0 */
#define FPU_COPROC_IDX  0
/* PIE/AIA coprocessor is coprocessor 1 */
#define PIE_COPROC_IDX  1

/* Define the size of each coprocessor save area */
#if defined(_ASMLANGUAGE) || defined(__ASSEMBLER__)
#define RV_COPROC0_SIZE RvFPUSaveAreaSize
#define RV_COPROC1_SIZE 0   // PIE/AIA coprocessor area
#else
#define RV_COPROC0_SIZE sizeof(RvFPUSaveArea)
#define RV_COPROC1_SIZE 0   // PIE/AIA coprocessor area
#endif /* defined(_ASMLANGUAGE) || defined(__ASSEMBLER__) */

#endif /* SOC_CPU_HAS_FPU */

/**
 * @brief Coprocessors save area, containing each coprocessor save area
 */
STRUCT_BEGIN
/* Enable bitmap: BIT(i) represents coprocessor i, 1 is used, 0 else */
STRUCT_FIELD  (long, 4, RV_COPROC_ENABLE, sa_enable)
/* Address of the original lowest stack address, convenient when the stack needs to re-initialized */
STRUCT_FIELD  (void*, 4, RV_COPROC_TCB_STACK, sa_tcbstack)
/* Address of the pool of memory used to allocate coprocessors save areas */
STRUCT_FIELD  (long, 4, RV_COPROC_ALLOCATOR, sa_allocator)
/* Pointer to the coprocessors save areas */
STRUCT_AFIELD (void*, 4, RV_COPROC_SA, sa_coprocs, SOC_CPU_COPROC_NUM)
STRUCT_END(RvCoprocSaveArea)


#endif /* SOC_CPU_COPROC_NUM > 0 */


#if defined(_ASMLANGUAGE) || defined(__ASSEMBLER__)
#define RV_STK_SZ1     RvExcFrameSize
#else
#define RV_STK_SZ1     sizeof(RvExcFrame)
#endif

/*
 * Exception stack frame size, after align up to 16 bytes boundary
 */
#define RV_STK_FRMSZ    (ALIGNUP(0x10, RV_STK_SZ1))

#endif /* #ifndef __RVRUNTIME_FRAMES_H__ */
