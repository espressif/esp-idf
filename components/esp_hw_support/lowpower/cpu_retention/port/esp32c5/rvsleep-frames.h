/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __RVSLEEP_FRAMES_H__
#define __RVSLEEP_FRAMES_H__

#include "sdkconfig.h"

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
 * -------------------------------------------------------------------------------
 *     RISC-V CORE CRITICAL REGISTER CONTEXT LAYOUT FOR SLEEP
 * -------------------------------------------------------------------------------
 */
STRUCT_BEGIN
    STRUCT_FIELD (long, 4, RV_SLP_CTX_MEPC,     mepc)       /* Machine Exception Program Counter */
    STRUCT_FIELD (long, 4, RV_SLP_CTX_RA,       ra)         /* Return address */
    STRUCT_FIELD (long, 4, RV_SLP_CTX_SP,       sp)         /* Stack pointer */
    STRUCT_FIELD (long, 4, RV_SLP_CTX_GP,       gp)         /* Global pointer */
    STRUCT_FIELD (long, 4, RV_SLP_CTX_TP,       tp)         /* Thread pointer */
    STRUCT_FIELD (long, 4, RV_SLP_CTX_T0,       t0)         /* Temporary/alternate link register */
    STRUCT_FIELD (long, 4, RV_SLP_CTX_T1,       t1)         /* t1-2: Temporaries */
    STRUCT_FIELD (long, 4, RV_SLP_CTX_T2,       t2)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_S0,       s0)         /* Saved register/frame pointer */
    STRUCT_FIELD (long, 4, RV_SLP_CTX_S1,       s1)         /* Saved register */
    STRUCT_FIELD (long, 4, RV_SLP_CTX_A0,       a0)         /* a0-1: Function arguments/return address */
    STRUCT_FIELD (long, 4, RV_SLP_CTX_A1,       a1)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_A2,       a2)         /* a2-7: Function arguments */
    STRUCT_FIELD (long, 4, RV_SLP_CTX_A3,       a3)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_A4,       a4)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_A5,       a5)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_A6,       a6)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_A7,       a7)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_S2,       s2)         /* s2-11: Saved registers */
    STRUCT_FIELD (long, 4, RV_SLP_CTX_S3,       s3)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_S4,       s4)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_S5,       s5)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_S6,       s6)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_S7,       s7)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_S8,       s8)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_S9,       s9)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_S10,      s10)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_S11,      s11)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_T3,       t3)         /* t3-6: Temporaries */
    STRUCT_FIELD (long, 4, RV_SLP_CTX_T4,       t4)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_T5,       t5)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_T6,       t6)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_MSTATUS,  mstatus)    /* Machine Status */
    STRUCT_FIELD (long, 4, RV_SLP_CTX_MTVEC,    mtvec)      /* Machine Trap-Vector Base Address */
    STRUCT_FIELD (long, 4, RV_SLP_CTX_MCAUSE,   mcause)     /* Machine Trap Cause */
    STRUCT_FIELD (long, 4, RV_SLP_CTX_MTVAL,    mtval)      /* Machine Trap Value */
    STRUCT_FIELD (long, 4, RV_SLP_CTX_MIE,      mie)        /* Machine intr enable */
    STRUCT_FIELD (long, 4, RV_SLP_CTX_MIP,      mip)        /* Machine intr pending */

    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMUFUNC,  pmufunc)    /* A field is used to identify whether it is going
                                                             * to sleep or has just been awakened. We use the
                                                             * lowest 2 bits as indication information, 3 means
                                                             * being awakened, 1 means going to sleep */
#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
    STRUCT_FIELD (long, 4, RV_SLP_CSF_CTX_CRC,  frame_crc)  /* Used to check RvCoreCriticalSleepFrame integrity */
#endif
STRUCT_END(RvCoreCriticalSleepFrame)

#if defined(_ASMLANGUAGE) || defined(__ASSEMBLER__)
#define RV_SLEEP_CTX_SZ1    RvCoreCriticalSleepFrameSize
#else
#define RV_SLEEP_CTX_SZ1    sizeof(RvCoreCriticalSleepFrame)
#endif

/*
 * Sleep stack frame size, after align up to 16 bytes boundary
 */
#define RV_SLEEP_CTX_FRMSZ  (ALIGNUP(0x10, RV_SLEEP_CTX_SZ1))

/*
 * -------------------------------------------------------------------------------
 *     RISC-V CORE NON-CRITICAL REGISTER CONTEXT LAYOUT FOR SLEEP
 * -------------------------------------------------------------------------------
 */
STRUCT_BEGIN
    STRUCT_FIELD (long, 4, RV_SLP_CTX_MSCRATCH,         mscratch)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_MISA,             misa)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_TSELECT,          tselect)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_TDATA1,           tdata1)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_TDATA2,           tdata2)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_TCONTROL,         tcontrol)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPADDR0,         pmpaddr0)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPADDR1,         pmpaddr1)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPADDR2,         pmpaddr2)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPADDR3,         pmpaddr3)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPADDR4,         pmpaddr4)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPADDR5,         pmpaddr5)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPADDR6,         pmpaddr6)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPADDR7,         pmpaddr7)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPADDR8,         pmpaddr8)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPADDR9,         pmpaddr9)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPADDR10,        pmpaddr10)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPADDR11,        pmpaddr11)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPADDR12,        pmpaddr12)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPADDR13,        pmpaddr13)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPADDR14,        pmpaddr14)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPADDR15,        pmpaddr15)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPCFG0,          pmpcfg0)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPCFG1,          pmpcfg1)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPCFG2,          pmpcfg2)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMPCFG3,          pmpcfg3)

    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMAADDR0,         pmaaddr0)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMAADDR1,         pmaaddr1)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMAADDR2,         pmaaddr2)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMAADDR3,         pmaaddr3)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMAADDR4,         pmaaddr4)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMAADDR5,         pmaaddr5)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMAADDR6,         pmaaddr6)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMAADDR7,         pmaaddr7)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMAADDR8,         pmaaddr8)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMAADDR9,         pmaaddr9)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMAADDR10,        pmaaddr10)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMAADDR11,        pmaaddr11)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMAADDR12,        pmaaddr12)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMAADDR13,        pmaaddr13)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMAADDR14,        pmaaddr14)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMAADDR15,        pmaaddr15)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMACFG0,          pmacfg0)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMACFG1,          pmacfg1)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMACFG2,          pmacfg2)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMACFG3,          pmacfg3)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMACFG4,          pmacfg4)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMACFG5,          pmacfg5)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMACFG6,          pmacfg6)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMACFG7,          pmacfg7)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMACFG8,          pmacfg8)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMACFG9,          pmacfg9)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMACFG10,         pmacfg10)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMACFG11,         pmacfg11)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMACFG12,         pmacfg12)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMACFG13,         pmacfg13)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMACFG14,         pmacfg14)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_PMACFG15,         pmacfg15)

    STRUCT_FIELD (long, 4, RV_SLP_CTX_MCYCLE,           mcycle)

    STRUCT_FIELD (long, 4, RV_SLP_CTX_MTVT,             mtvt)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_MNXTI,            mnxti)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_MINTSTATUS,       mintstatus)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_MXSTATUS,         mxstatus)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_MHCR,             mhcr)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_MHINT,            mhint)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_MEXSTATUS,        mexstatus)
    STRUCT_FIELD (long, 4, RV_SLP_CTX_JVT,              jvt)
#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
    STRUCT_FIELD (long, 4, RV_SLP_NCSF_CTX_CRC,         frame_crc)        /* Used to check RvCoreNonCriticalSleepFrame integrity */
#endif
STRUCT_END(RvCoreNonCriticalSleepFrame)

#endif /* #ifndef __RVSLEEP_FRAMES_H__ */
