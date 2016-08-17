/* Definitions for the xt_core TIE package */

/*
 * Customer ID=11657; Build=0x5fe96; Copyright (c) 2004-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_core_HEADER
#define _XTENSA_xt_core_HEADER

#ifdef __XTENSA__
#ifdef __XCC__


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

extern void _TIE_xt_core_ILL(void);
extern void _TIE_xt_core_NOP(void);
extern void _TIE_xt_core_SIMCALL(void);
extern void _TIE_xt_core_MEMW(void);
extern void _TIE_xt_core_EXTW(void);
extern void _TIE_xt_core_ISYNC(void);
extern void _TIE_xt_core_DSYNC(void);
extern void _TIE_xt_core_ESYNC(void);
extern void _TIE_xt_core_RSYNC(void);
extern unsigned _TIE_xt_core_RSR_LBEG(void);
extern void _TIE_xt_core_WSR_LBEG(unsigned art);
extern void _TIE_xt_core_XSR_LBEG(unsigned art /*inout*/);
extern unsigned _TIE_xt_core_RSR_CONFIGID0(void);
extern void _TIE_xt_core_WSR_CONFIGID0(unsigned art);
extern unsigned _TIE_xt_core_RSR_CONFIGID1(void);
extern unsigned _TIE_xt_core_RUR_THREADPTR(void);
extern void _TIE_xt_core_WUR_THREADPTR(unsigned v);
extern unsigned _TIE_xt_core_uint32_loadi(const unsigned * p, immediate o);
extern void _TIE_xt_core_uint32_storei(unsigned c, unsigned * p, immediate o);
extern unsigned _TIE_xt_core_uint32_move(unsigned b);
extern int _TIE_xt_core_ADDI(int s, immediate i);
extern int _TIE_xt_core_OR(int s, int t);
extern int _TIE_xt_core_L32I(const int * p, immediate i);
extern void _TIE_xt_core_S32I(int r, int * p, immediate i);
extern void _TIE_xt_core_S32NB(int r, int * p, immediate i);
extern unsigned char _TIE_xt_core_L8UI(const unsigned char * p, immediate i);
extern void _TIE_xt_core_S8I(signed char r, signed char * p, immediate i);
extern unsigned short _TIE_xt_core_L16UI(const unsigned short * p, immediate i);
extern short _TIE_xt_core_L16SI(const short * p, immediate i);
extern void _TIE_xt_core_S16I(short r, short * p, immediate i);
extern int _TIE_xt_core_ADDMI(int s, immediate i);
extern int _TIE_xt_core_ADD(int s, int t);
extern int _TIE_xt_core_ADDX2(int s, int t);
extern int _TIE_xt_core_ADDX4(int s, int t);
extern int _TIE_xt_core_ADDX8(int s, int t);
extern int _TIE_xt_core_SUB(int s, int t);
extern int _TIE_xt_core_SUBX2(int s, int t);
extern int _TIE_xt_core_SUBX4(int s, int t);
extern int _TIE_xt_core_SUBX8(int s, int t);
extern int _TIE_xt_core_AND(int s, int t);
extern int _TIE_xt_core_XOR(int s, int t);
extern unsigned _TIE_xt_core_EXTUI(unsigned t, immediate i, immediate o);
extern int _TIE_xt_core_MOVI(immediate i);
extern void _TIE_xt_core_MOVEQZ(int r /*inout*/, int s, int t);
extern void _TIE_xt_core_MOVNEZ(int r /*inout*/, int s, int t);
extern void _TIE_xt_core_MOVLTZ(int r /*inout*/, int s, int t);
extern void _TIE_xt_core_MOVGEZ(int r /*inout*/, int s, int t);
extern int _TIE_xt_core_NEG(int t);
extern int _TIE_xt_core_ABS(int t);
extern void _TIE_xt_core_SSR(int s);
extern void _TIE_xt_core_SSL(int s);
extern void _TIE_xt_core_SSA8L(int s);
extern void _TIE_xt_core_SSA8B(int s);
extern void _TIE_xt_core_SSAI(immediate i);
extern int _TIE_xt_core_SLL(int s);
extern int _TIE_xt_core_SRC(int s, int t);
extern unsigned _TIE_xt_core_SRL(unsigned t);
extern int _TIE_xt_core_SRA(int t);
extern int _TIE_xt_core_SLLI(int s, immediate i);
extern int _TIE_xt_core_SRAI(int t, immediate i);
extern unsigned _TIE_xt_core_SRLI(unsigned t, immediate i);
extern int _TIE_xt_core_SSAI_SRC(int src1, int src2, immediate amount);
extern int _TIE_xt_core_SSR_SRC(int src1, int src2, int amount);
extern int _TIE_xt_core_WSR_SAR_SRC(int src1, int src2, int amount);
extern int _TIE_xt_core_SSR_SRA(int src, int amount);
extern unsigned _TIE_xt_core_SSR_SRL(unsigned src, int amount);
extern int _TIE_xt_core_SSL_SLL(int src, int amount);
extern int _TIE_xt_core_RSIL(immediate t);
extern int _TIE_xt_core_RSR_LEND(void);
extern void _TIE_xt_core_WSR_LEND(int t);
extern void _TIE_xt_core_XSR_LEND(int t /*inout*/);
extern int _TIE_xt_core_RSR_LCOUNT(void);
extern void _TIE_xt_core_WSR_LCOUNT(int t);
extern void _TIE_xt_core_XSR_LCOUNT(int t /*inout*/);
extern unsigned _TIE_xt_core_RSR_SAR(void);
extern void _TIE_xt_core_WSR_SAR(unsigned t);
extern void _TIE_xt_core_XSR_SAR(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_MEMCTL(void);
extern void _TIE_xt_core_WSR_MEMCTL(unsigned t);
extern void _TIE_xt_core_XSR_MEMCTL(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_LITBASE(void);
extern void _TIE_xt_core_WSR_LITBASE(unsigned t);
extern void _TIE_xt_core_XSR_LITBASE(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_PS(void);
extern void _TIE_xt_core_WSR_PS(unsigned t);
extern void _TIE_xt_core_XSR_PS(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EPC1(void);
extern void _TIE_xt_core_WSR_EPC1(unsigned t);
extern void _TIE_xt_core_XSR_EPC1(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EXCSAVE1(void);
extern void _TIE_xt_core_WSR_EXCSAVE1(unsigned t);
extern void _TIE_xt_core_XSR_EXCSAVE1(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EPC2(void);
extern void _TIE_xt_core_WSR_EPC2(unsigned t);
extern void _TIE_xt_core_XSR_EPC2(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EXCSAVE2(void);
extern void _TIE_xt_core_WSR_EXCSAVE2(unsigned t);
extern void _TIE_xt_core_XSR_EXCSAVE2(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EPC3(void);
extern void _TIE_xt_core_WSR_EPC3(unsigned t);
extern void _TIE_xt_core_XSR_EPC3(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EXCSAVE3(void);
extern void _TIE_xt_core_WSR_EXCSAVE3(unsigned t);
extern void _TIE_xt_core_XSR_EXCSAVE3(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EPC4(void);
extern void _TIE_xt_core_WSR_EPC4(unsigned t);
extern void _TIE_xt_core_XSR_EPC4(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EXCSAVE4(void);
extern void _TIE_xt_core_WSR_EXCSAVE4(unsigned t);
extern void _TIE_xt_core_XSR_EXCSAVE4(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EPC5(void);
extern void _TIE_xt_core_WSR_EPC5(unsigned t);
extern void _TIE_xt_core_XSR_EPC5(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EXCSAVE5(void);
extern void _TIE_xt_core_WSR_EXCSAVE5(unsigned t);
extern void _TIE_xt_core_XSR_EXCSAVE5(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EPC6(void);
extern void _TIE_xt_core_WSR_EPC6(unsigned t);
extern void _TIE_xt_core_XSR_EPC6(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EXCSAVE6(void);
extern void _TIE_xt_core_WSR_EXCSAVE6(unsigned t);
extern void _TIE_xt_core_XSR_EXCSAVE6(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EPC7(void);
extern void _TIE_xt_core_WSR_EPC7(unsigned t);
extern void _TIE_xt_core_XSR_EPC7(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EXCSAVE7(void);
extern void _TIE_xt_core_WSR_EXCSAVE7(unsigned t);
extern void _TIE_xt_core_XSR_EXCSAVE7(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_VECBASE(void);
extern void _TIE_xt_core_WSR_VECBASE(unsigned t);
extern void _TIE_xt_core_XSR_VECBASE(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EPS2(void);
extern void _TIE_xt_core_WSR_EPS2(unsigned t);
extern void _TIE_xt_core_XSR_EPS2(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EPS3(void);
extern void _TIE_xt_core_WSR_EPS3(unsigned t);
extern void _TIE_xt_core_XSR_EPS3(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EPS4(void);
extern void _TIE_xt_core_WSR_EPS4(unsigned t);
extern void _TIE_xt_core_XSR_EPS4(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EPS5(void);
extern void _TIE_xt_core_WSR_EPS5(unsigned t);
extern void _TIE_xt_core_XSR_EPS5(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EPS6(void);
extern void _TIE_xt_core_WSR_EPS6(unsigned t);
extern void _TIE_xt_core_XSR_EPS6(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EPS7(void);
extern void _TIE_xt_core_WSR_EPS7(unsigned t);
extern void _TIE_xt_core_XSR_EPS7(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EXCCAUSE(void);
extern void _TIE_xt_core_WSR_EXCCAUSE(unsigned t);
extern void _TIE_xt_core_XSR_EXCCAUSE(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_EXCVADDR(void);
extern void _TIE_xt_core_WSR_EXCVADDR(unsigned t);
extern void _TIE_xt_core_XSR_EXCVADDR(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_DEPC(void);
extern void _TIE_xt_core_WSR_DEPC(unsigned t);
extern void _TIE_xt_core_XSR_DEPC(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_MISC0(void);
extern void _TIE_xt_core_WSR_MISC0(unsigned t);
extern void _TIE_xt_core_XSR_MISC0(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_MISC1(void);
extern void _TIE_xt_core_WSR_MISC1(unsigned t);
extern void _TIE_xt_core_XSR_MISC1(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_MISC2(void);
extern void _TIE_xt_core_WSR_MISC2(unsigned t);
extern void _TIE_xt_core_XSR_MISC2(unsigned t /*inout*/);
extern unsigned _TIE_xt_core_RSR_MISC3(void);
extern void _TIE_xt_core_WSR_MISC3(unsigned t);
extern void _TIE_xt_core_XSR_MISC3(unsigned t /*inout*/);
extern int _TIE_xt_core_RSR_PRID(void);
#define XT_ILL _TIE_xt_core_ILL
#define XT_NOP _TIE_xt_core_NOP
#define XT_SIMCALL _TIE_xt_core_SIMCALL
#define XT_MEMW _TIE_xt_core_MEMW
#define XT_EXTW _TIE_xt_core_EXTW
#define XT_ISYNC _TIE_xt_core_ISYNC
#define XT_DSYNC _TIE_xt_core_DSYNC
#define XT_ESYNC _TIE_xt_core_ESYNC
#define XT_RSYNC _TIE_xt_core_RSYNC
#define XT_RSR_LBEG _TIE_xt_core_RSR_LBEG
#define XT_WSR_LBEG _TIE_xt_core_WSR_LBEG
#define XT_XSR_LBEG _TIE_xt_core_XSR_LBEG
#define XT_RSR_CONFIGID0 _TIE_xt_core_RSR_CONFIGID0
#define XT_WSR_CONFIGID0 _TIE_xt_core_WSR_CONFIGID0
#define XT_RSR_CONFIGID1 _TIE_xt_core_RSR_CONFIGID1
#define XT_RUR_THREADPTR _TIE_xt_core_RUR_THREADPTR
#define RTHREADPTR _TIE_xt_core_RUR_THREADPTR
#define RUR231 _TIE_xt_core_RUR_THREADPTR
#define XT_WUR_THREADPTR _TIE_xt_core_WUR_THREADPTR
#define WTHREADPTR _TIE_xt_core_WUR_THREADPTR
#define WUR231 _TIE_xt_core_WUR_THREADPTR
#define XT_uint32_loadi _TIE_xt_core_uint32_loadi
#define XT_uint32_storei _TIE_xt_core_uint32_storei
#define XT_uint32_move _TIE_xt_core_uint32_move
#define XT_ADDI _TIE_xt_core_ADDI
#define XT_OR _TIE_xt_core_OR
#define XT_L32I _TIE_xt_core_L32I
#define XT_S32I _TIE_xt_core_S32I
#define XT_S32NB _TIE_xt_core_S32NB
#define XT_L8UI _TIE_xt_core_L8UI
#define XT_S8I _TIE_xt_core_S8I
#define XT_L16UI _TIE_xt_core_L16UI
#define XT_L16SI _TIE_xt_core_L16SI
#define XT_S16I _TIE_xt_core_S16I
#define XT_ADDMI _TIE_xt_core_ADDMI
#define XT_ADD _TIE_xt_core_ADD
#define XT_ADDX2 _TIE_xt_core_ADDX2
#define XT_ADDX4 _TIE_xt_core_ADDX4
#define XT_ADDX8 _TIE_xt_core_ADDX8
#define XT_SUB _TIE_xt_core_SUB
#define XT_SUBX2 _TIE_xt_core_SUBX2
#define XT_SUBX4 _TIE_xt_core_SUBX4
#define XT_SUBX8 _TIE_xt_core_SUBX8
#define XT_AND _TIE_xt_core_AND
#define XT_XOR _TIE_xt_core_XOR
#define XT_EXTUI _TIE_xt_core_EXTUI
#define XT_MOVI _TIE_xt_core_MOVI
#define XT_MOVEQZ _TIE_xt_core_MOVEQZ
#define XT_MOVNEZ _TIE_xt_core_MOVNEZ
#define XT_MOVLTZ _TIE_xt_core_MOVLTZ
#define XT_MOVGEZ _TIE_xt_core_MOVGEZ
#define XT_NEG _TIE_xt_core_NEG
#define XT_ABS _TIE_xt_core_ABS
#define XT_SSR _TIE_xt_core_SSR
#define XT_SSL _TIE_xt_core_SSL
#define XT_SSA8L _TIE_xt_core_SSA8L
#define XT_SSA8B _TIE_xt_core_SSA8B
#define XT_SSAI _TIE_xt_core_SSAI
#define XT_SLL _TIE_xt_core_SLL
#define XT_SRC _TIE_xt_core_SRC
#define XT_SRL _TIE_xt_core_SRL
#define XT_SRA _TIE_xt_core_SRA
#define XT_SLLI _TIE_xt_core_SLLI
#define XT_SRAI _TIE_xt_core_SRAI
#define XT_SRLI _TIE_xt_core_SRLI
#define XT_SSAI_SRC _TIE_xt_core_SSAI_SRC
#define XT_SSR_SRC _TIE_xt_core_SSR_SRC
#define XT_WSR_SAR_SRC _TIE_xt_core_WSR_SAR_SRC
#define XT_SSR_SRA _TIE_xt_core_SSR_SRA
#define XT_SSR_SRL _TIE_xt_core_SSR_SRL
#define XT_SSL_SLL _TIE_xt_core_SSL_SLL
#define XT_RSIL _TIE_xt_core_RSIL
#define XT_RSR_LEND _TIE_xt_core_RSR_LEND
#define XT_WSR_LEND _TIE_xt_core_WSR_LEND
#define XT_XSR_LEND _TIE_xt_core_XSR_LEND
#define XT_RSR_LCOUNT _TIE_xt_core_RSR_LCOUNT
#define XT_WSR_LCOUNT _TIE_xt_core_WSR_LCOUNT
#define XT_XSR_LCOUNT _TIE_xt_core_XSR_LCOUNT
#define XT_RSR_SAR _TIE_xt_core_RSR_SAR
#define XT_WSR_SAR _TIE_xt_core_WSR_SAR
#define XT_XSR_SAR _TIE_xt_core_XSR_SAR
#define XT_RSR_MEMCTL _TIE_xt_core_RSR_MEMCTL
#define XT_WSR_MEMCTL _TIE_xt_core_WSR_MEMCTL
#define XT_XSR_MEMCTL _TIE_xt_core_XSR_MEMCTL
#define XT_RSR_LITBASE _TIE_xt_core_RSR_LITBASE
#define XT_WSR_LITBASE _TIE_xt_core_WSR_LITBASE
#define XT_XSR_LITBASE _TIE_xt_core_XSR_LITBASE
#define XT_RSR_PS _TIE_xt_core_RSR_PS
#define XT_WSR_PS _TIE_xt_core_WSR_PS
#define XT_XSR_PS _TIE_xt_core_XSR_PS
#define XT_RSR_EPC1 _TIE_xt_core_RSR_EPC1
#define XT_WSR_EPC1 _TIE_xt_core_WSR_EPC1
#define XT_XSR_EPC1 _TIE_xt_core_XSR_EPC1
#define XT_RSR_EXCSAVE1 _TIE_xt_core_RSR_EXCSAVE1
#define XT_WSR_EXCSAVE1 _TIE_xt_core_WSR_EXCSAVE1
#define XT_XSR_EXCSAVE1 _TIE_xt_core_XSR_EXCSAVE1
#define XT_RSR_EPC2 _TIE_xt_core_RSR_EPC2
#define XT_WSR_EPC2 _TIE_xt_core_WSR_EPC2
#define XT_XSR_EPC2 _TIE_xt_core_XSR_EPC2
#define XT_RSR_EXCSAVE2 _TIE_xt_core_RSR_EXCSAVE2
#define XT_WSR_EXCSAVE2 _TIE_xt_core_WSR_EXCSAVE2
#define XT_XSR_EXCSAVE2 _TIE_xt_core_XSR_EXCSAVE2
#define XT_RSR_EPC3 _TIE_xt_core_RSR_EPC3
#define XT_WSR_EPC3 _TIE_xt_core_WSR_EPC3
#define XT_XSR_EPC3 _TIE_xt_core_XSR_EPC3
#define XT_RSR_EXCSAVE3 _TIE_xt_core_RSR_EXCSAVE3
#define XT_WSR_EXCSAVE3 _TIE_xt_core_WSR_EXCSAVE3
#define XT_XSR_EXCSAVE3 _TIE_xt_core_XSR_EXCSAVE3
#define XT_RSR_EPC4 _TIE_xt_core_RSR_EPC4
#define XT_WSR_EPC4 _TIE_xt_core_WSR_EPC4
#define XT_XSR_EPC4 _TIE_xt_core_XSR_EPC4
#define XT_RSR_EXCSAVE4 _TIE_xt_core_RSR_EXCSAVE4
#define XT_WSR_EXCSAVE4 _TIE_xt_core_WSR_EXCSAVE4
#define XT_XSR_EXCSAVE4 _TIE_xt_core_XSR_EXCSAVE4
#define XT_RSR_EPC5 _TIE_xt_core_RSR_EPC5
#define XT_WSR_EPC5 _TIE_xt_core_WSR_EPC5
#define XT_XSR_EPC5 _TIE_xt_core_XSR_EPC5
#define XT_RSR_EXCSAVE5 _TIE_xt_core_RSR_EXCSAVE5
#define XT_WSR_EXCSAVE5 _TIE_xt_core_WSR_EXCSAVE5
#define XT_XSR_EXCSAVE5 _TIE_xt_core_XSR_EXCSAVE5
#define XT_RSR_EPC6 _TIE_xt_core_RSR_EPC6
#define XT_WSR_EPC6 _TIE_xt_core_WSR_EPC6
#define XT_XSR_EPC6 _TIE_xt_core_XSR_EPC6
#define XT_RSR_EXCSAVE6 _TIE_xt_core_RSR_EXCSAVE6
#define XT_WSR_EXCSAVE6 _TIE_xt_core_WSR_EXCSAVE6
#define XT_XSR_EXCSAVE6 _TIE_xt_core_XSR_EXCSAVE6
#define XT_RSR_EPC7 _TIE_xt_core_RSR_EPC7
#define XT_WSR_EPC7 _TIE_xt_core_WSR_EPC7
#define XT_XSR_EPC7 _TIE_xt_core_XSR_EPC7
#define XT_RSR_EXCSAVE7 _TIE_xt_core_RSR_EXCSAVE7
#define XT_WSR_EXCSAVE7 _TIE_xt_core_WSR_EXCSAVE7
#define XT_XSR_EXCSAVE7 _TIE_xt_core_XSR_EXCSAVE7
#define XT_RSR_VECBASE _TIE_xt_core_RSR_VECBASE
#define XT_WSR_VECBASE _TIE_xt_core_WSR_VECBASE
#define XT_XSR_VECBASE _TIE_xt_core_XSR_VECBASE
#define XT_RSR_EPS2 _TIE_xt_core_RSR_EPS2
#define XT_WSR_EPS2 _TIE_xt_core_WSR_EPS2
#define XT_XSR_EPS2 _TIE_xt_core_XSR_EPS2
#define XT_RSR_EPS3 _TIE_xt_core_RSR_EPS3
#define XT_WSR_EPS3 _TIE_xt_core_WSR_EPS3
#define XT_XSR_EPS3 _TIE_xt_core_XSR_EPS3
#define XT_RSR_EPS4 _TIE_xt_core_RSR_EPS4
#define XT_WSR_EPS4 _TIE_xt_core_WSR_EPS4
#define XT_XSR_EPS4 _TIE_xt_core_XSR_EPS4
#define XT_RSR_EPS5 _TIE_xt_core_RSR_EPS5
#define XT_WSR_EPS5 _TIE_xt_core_WSR_EPS5
#define XT_XSR_EPS5 _TIE_xt_core_XSR_EPS5
#define XT_RSR_EPS6 _TIE_xt_core_RSR_EPS6
#define XT_WSR_EPS6 _TIE_xt_core_WSR_EPS6
#define XT_XSR_EPS6 _TIE_xt_core_XSR_EPS6
#define XT_RSR_EPS7 _TIE_xt_core_RSR_EPS7
#define XT_WSR_EPS7 _TIE_xt_core_WSR_EPS7
#define XT_XSR_EPS7 _TIE_xt_core_XSR_EPS7
#define XT_RSR_EXCCAUSE _TIE_xt_core_RSR_EXCCAUSE
#define XT_WSR_EXCCAUSE _TIE_xt_core_WSR_EXCCAUSE
#define XT_XSR_EXCCAUSE _TIE_xt_core_XSR_EXCCAUSE
#define XT_RSR_EXCVADDR _TIE_xt_core_RSR_EXCVADDR
#define XT_WSR_EXCVADDR _TIE_xt_core_WSR_EXCVADDR
#define XT_XSR_EXCVADDR _TIE_xt_core_XSR_EXCVADDR
#define XT_RSR_DEPC _TIE_xt_core_RSR_DEPC
#define XT_WSR_DEPC _TIE_xt_core_WSR_DEPC
#define XT_XSR_DEPC _TIE_xt_core_XSR_DEPC
#define XT_RSR_MISC0 _TIE_xt_core_RSR_MISC0
#define XT_WSR_MISC0 _TIE_xt_core_WSR_MISC0
#define XT_XSR_MISC0 _TIE_xt_core_XSR_MISC0
#define XT_RSR_MISC1 _TIE_xt_core_RSR_MISC1
#define XT_WSR_MISC1 _TIE_xt_core_WSR_MISC1
#define XT_XSR_MISC1 _TIE_xt_core_XSR_MISC1
#define XT_RSR_MISC2 _TIE_xt_core_RSR_MISC2
#define XT_WSR_MISC2 _TIE_xt_core_WSR_MISC2
#define XT_XSR_MISC2 _TIE_xt_core_XSR_MISC2
#define XT_RSR_MISC3 _TIE_xt_core_RSR_MISC3
#define XT_WSR_MISC3 _TIE_xt_core_WSR_MISC3
#define XT_XSR_MISC3 _TIE_xt_core_XSR_MISC3
#define XT_RSR_PRID _TIE_xt_core_RSR_PRID

#ifndef RUR
#define RUR(NUM) RUR##NUM()
#endif

#ifndef WUR
#define WUR(VAL, NUM) WUR##NUM(VAL)
#endif

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_xt_core_HEADER */
