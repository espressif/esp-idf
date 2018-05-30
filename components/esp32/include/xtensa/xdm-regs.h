/* xdm-regs.h  -  Common register and related definitions for the XDM
                  (Xtensa Debug Module) */

/* Copyright (c) 2011-2012 Tensilica Inc.

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  */


#ifndef _XDM_REGS_H_
#define _XDM_REGS_H_

/*  NOTE:  This header file is included by C, assembler, and other sources.
    So any C-specific or asm-specific content must be appropriately #ifdef'd.  */


/*
 *  XDM registers can be accessed using APB, ERI, or JTAG (via NAR).
 *  Address offsets for APB and ERI are the same, and for JTAG
 *  is different (due to the limited 7-bit NAR addressing).
 *
 *  Here, we first provide the constants as APB / ERI address offsets.
 *  This is necessary for assembler code (which accesses XDM via ERI),
 *  because complex conversion macros between the two address maps
 *  don't work in the assembler.
 *  Conversion macros are used to convert these to/from JTAG (NAR),
 *  addresses, for software using JTAG.
 */
/* FIXME: maybe provide only MISC+CS registers here, and leave specific
   subsystem registers in separate headers? eg. for TRAX, PERF, OCD */

/*       XDM_....     		ERI addr  [NAR addr] Description...... */

/*  TRAX  */
#define  XDM_TRAX_ID		0x0000	/*[0x00] ID */
#define  XDM_TRAX_CONTROL 	0x0004	/*[0x01] Control */
#define  XDM_TRAX_STATUS	0x0008	/*[0x02] Status */
#define  XDM_TRAX_DATA		0x000C	/*[0x03] Data */
#define  XDM_TRAX_ADDRESS	0x0010	/*[0x04] Address */
#define  XDM_TRAX_TRIGGER	0x0014	/*[0x05] Stop PC */
#define  XDM_TRAX_MATCH		0x0018	/*[0x06] Stop PC Range */
#define  XDM_TRAX_DELAY		0x001C	/*[0x07] Post Stop Trigger Capture Size */
#define  XDM_TRAX_STARTADDR	0x0020	/*[0x08] Trace Memory Start */
#define  XDM_TRAX_ENDADDR	0x0024	/*[0x09] Trace Memory End */
#define  XDM_TRAX_DEBUGPC	0x003C	/*[0x0F] Debug PC */
#define  XDM_TRAX_P4CHANGE	0x0040	/*[0x10] X */
#define  XDM_TRAX_TIME0		0x0040	/*[0x10] First Time Register */
#define  XDM_TRAX_P4REV		0x0044	/*[0x11] X */
#define  XDM_TRAX_TIME1		0x0044	/*[0x11] Second Time Register */
#define  XDM_TRAX_P4DATE	0x0048	/*[0x12] X */
#define  XDM_TRAX_INTTIME_MAX	0x0048 /*[0x12] maximal Value of Timestamp IntTime */
#define  XDM_TRAX_P4TIME	0x004C	/*[0x13] X */
#define  XDM_TRAX_PDSTATUS	0x0050	/*[0x14] Sample of PDebugStatus */
#define  XDM_TRAX_PDDATA	0x0054	/*[0x15] Sample of PDebugData */
#define  XDM_TRAX_STOP_PC	0x0058	/*[0x16] X */
#define  XDM_TRAX_STOP_ICNT	0x005C	/*[0x16] X */
#define  XDM_TRAX_MSG_STATUS	0x0060	/*[0x17] X */
#define  XDM_TRAX_FSM_STATUS	0x0064	/*[0x18] X */
#define  XDM_TRAX_IB_STATUS	0x0068	/*[0x19] X */
#define  XDM_TRAX_STOPCNT	0x006C	/*[0x1A] X */

/*  Performance Monitoring Counters  */
#define  XDM_PERF_PMG		0x1000	/*[0x20] perf. mon. global control register */
#define  XDM_PERF_INTPC		0x1010	/*[0x24] perf. mon. interrupt PC */
#define  XDM_PERF_PM0		0x1080	/*[0x28] perf. mon. counter 0 value */
#define  XDM_PERF_PM1		0x1084	/*[0x29] perf. mon. counter 1 value */
#define  XDM_PERF_PM2		0x1088	/*[0x2A] perf. mon. counter 2 value */
#define  XDM_PERF_PM3		0x108C	/*[0x2B] perf. mon. counter 3 value */
#define  XDM_PERF_PM4		0x1090	/*[0x2C] perf. mon. counter 4 value */
#define  XDM_PERF_PM5		0x1094	/*[0x2D] perf. mon. counter 5 value */
#define  XDM_PERF_PM6		0x1098	/*[0x2E] perf. mon. counter 6 value */
#define  XDM_PERF_PM7		0x109C	/*[0x2F] perf. mon. counter 7 value */
#define  XDM_PERF_PM(n)		(0x1080+((n)<<2)) /* perfmon cnt n=0..7 value */
#define  XDM_PERF_PMCTRL0	0x1100	/*[0x30] perf. mon. counter 0 control */
#define  XDM_PERF_PMCTRL1	0x1104	/*[0x31] perf. mon. counter 1 control */
#define  XDM_PERF_PMCTRL2	0x1108	/*[0x32] perf. mon. counter 2 control */
#define  XDM_PERF_PMCTRL3	0x110C	/*[0x33] perf. mon. counter 3 control */
#define  XDM_PERF_PMCTRL4	0x1110	/*[0x34] perf. mon. counter 4 control */
#define  XDM_PERF_PMCTRL5	0x1114	/*[0x35] perf. mon. counter 5 control */
#define  XDM_PERF_PMCTRL6	0x1118	/*[0x36] perf. mon. counter 6 control */
#define  XDM_PERF_PMCTRL7	0x111C	/*[0x37] perf. mon. counter 7 control */
#define  XDM_PERF_PMCTRL(n)	(0x1100+((n)<<2)) /* perfmon cnt n=0..7 control */
#define  XDM_PERF_PMSTAT0	0x1180	/*[0x38] perf. mon. counter 0 status */
#define  XDM_PERF_PMSTAT1	0x1184	/*[0x39] perf. mon. counter 1 status */
#define  XDM_PERF_PMSTAT2	0x1188	/*[0x3A] perf. mon. counter 2 status */
#define  XDM_PERF_PMSTAT3	0x118C	/*[0x3B] perf. mon. counter 3 status */
#define  XDM_PERF_PMSTAT4	0x1190	/*[0x3C] perf. mon. counter 4 status */
#define  XDM_PERF_PMSTAT5	0x1194	/*[0x3D] perf. mon. counter 5 status */
#define  XDM_PERF_PMSTAT6	0x1198	/*[0x3E] perf. mon. counter 6 status */
#define  XDM_PERF_PMSTAT7	0x119C	/*[0x3F] perf. mon. counter 7 status */
#define  XDM_PERF_PMSTAT(n)	(0x1180+((n)<<2)) /* perfmon cnt n=0..7 status */

/*  On-Chip-Debug (OCD)  */
#define  XDM_OCD_ID		0x2000	/*[0x40] ID register */
#define  XDM_OCD_DCR_CLR	0x2008	/*[0x42] Debug Control reg clear */
#define  XDM_OCD_DCR_SET	0x200C	/*[0x43] Debug Control reg set */
#define  XDM_OCD_DSR		0x2010	/*[0x44] Debug Status reg */
#define  XDM_OCD_DDR		0x2014	/*[0x45] Debug Data reg */
#define  XDM_OCD_DDREXEC	0x2018	/*[0x46] Debug Data reg + execute-DIR */
#define  XDM_OCD_DIR0EXEC	0x201C	/*[0x47] Debug Instruction reg, word 0 + execute-DIR */
#define  XDM_OCD_DIR0		0x2020	/*[0x48] Debug Instruction reg, word 1 */
#define  XDM_OCD_DIR1		0x2024	/*[0x49] Debug Instruction reg, word 2 */
#define  XDM_OCD_DIR2		0x2028	/*[0x4A] Debug Instruction reg, word 3 */
#define  XDM_OCD_DIR3		0x202C	/*[0x49] Debug Instruction reg, word 4 */
#define  XDM_OCD_DIR4		0x2030	/*[0x4C] Debug Instruction reg, word 5 */
#define  XDM_OCD_DIR5		0x2034	/*[0x4D] Debug Instruction reg, word 5 */
#define  XDM_OCD_DIR6		0x2038	/*[0x4E] Debug Instruction reg, word 6 */
#define  XDM_OCD_DIR7		0x203C	/*[0x4F] Debug Instruction reg, word 7 */

/*  Miscellaneous Registers  */
#define  XDM_MISC_PWRCTL	0x3020	/*[0x58] Power and Reset Control */
#define  XDM_MISC_PWRSTAT	0x3024	/*[0x59] Power and Reset Status */
#define  XDM_MISC_ERISTAT	0x3028	/*[0x5A] ERI Transaction Status */
#define  XDM_MISC_DATETIME	0x3034	/*[0x5D] [INTERNAL] Timestamps of build */
#define  XDM_MISC_UBID		0x3038	/*[0x5E] [INTERNAL] Build Unique ID */
#define  XDM_MISC_CID		0x303C	/*[0x5F] [INTERNAL] Customer ID */

/*  CoreSight compatibility  */
#define  XDM_CS_ITCTRL		0x3F00	/*[0x60] InTegration Mode control reg */
#define  XDM_CS_CLAIMSET	0x3FA0	/*[0x68] Claim Tag Set reg */
#define  XDM_CS_CLAIMCLR	0x3FA4	/*[0x69] Claim Tag Clear reg */
#define  XDM_CS_LOCK_ACCESS	0x3FB0	/*[0x6B] Lock Access (writing 0xC5ACCE55 unlocks) */
#define  XDM_CS_LOCK_STATUS	0x3FB4	/*[0x6D] Lock Status */
#define  XDM_CS_AUTH_STATUS	0x3FB8	/*[0x6E] Authentication Status */
#define  XDM_CS_DEV_ID		0x3FC8	/*[0x72] Device ID */
#define  XDM_CS_DEV_TYPE	0x3FCC	/*[0x73] Device Type */
#define  XDM_CS_PER_ID4		0x3FD0	/*[0x74] Peripheral ID reg byte 4 */
#define  XDM_CS_PER_ID5		0x3FD4	/*[0x75] Peripheral ID reg byte 5 */
#define  XDM_CS_PER_ID6		0x3FD8	/*[0x76] Peripheral ID reg byte 6 */
#define  XDM_CS_PER_ID7		0x3FDC	/*[0x77] Peripheral ID reg byte 7 */
#define  XDM_CS_PER_ID0		0x3FE0	/*[0x78] Peripheral ID reg byte 0 */
#define  XDM_CS_PER_ID1		0x3FE4	/*[0x79] Peripheral ID reg byte 1 */
#define  XDM_CS_PER_ID2		0x3FE8	/*[0x7A] Peripheral ID reg byte 2 */
#define  XDM_CS_PER_ID3		0x3FEC	/*[0x7B] Peripheral ID reg byte 3 */
#define  XDM_CS_COMP_ID0	0x3FF0	/*[0x7C] Component ID reg byte 0 */
#define  XDM_CS_COMP_ID1	0x3FF4	/*[0x7D] Component ID reg byte 1 */
#define  XDM_CS_COMP_ID2	0x3FF8	/*[0x7E] Component ID reg byte 2 */
#define  XDM_CS_COMP_ID3	0x3FFC	/*[0x7F] Component ID reg byte 3 */

#define CS_PER_ID0	0x00000003
#define CS_PER_ID1	0x00000021
#define CS_PER_ID2	0x0000000f
#define CS_PER_ID3	0x00000000
#define CS_PER_ID4	0x00000024

#define CS_COMP_ID0	0x0000000d
#define CS_COMP_ID1	0x00000090
#define CS_COMP_ID2	0x00000005
#define CS_COMP_ID3	0x000000b1

#define CS_DEV_TYPE	0x00000015

#define XTENSA_IDCODE	0x120034e5    // FIXME (upper bits not spec. out but BE is !)
#define XTENSA_MFC_ID	(XTENSA_IDCODE & 0xFFF)
#define CS_DEV_ID	XTENSA_IDCODE

#define NXS_OCD_REG(val)  ((val >= 0x40) && (val <= 0x5F))
#define NXS_TRAX_REG(val)  val <= 0x3F

#define ERI_TRAX_REG(val)  ((val & 0xFFFF) < 0x1000)
#define ERI_OCD_REG(val)   ((val & 0xFFFF) >= 0x2000) && ((val & 0xFFFF) < 0x4000))

/*  Convert above 14-bit ERI/APB address/offset to 7-bit NAR address:  */
#define _XDM_ERI_TO_NAR(a)	( ((a)&0x3F80)==0x0000 ? (((a)>>2) & 0x1F) \
				: ((a)&0x3E00)==0x1000 ? (0x20 | (((a)>>2) & 7) | (((a)>>4) & 0x18)) \
				: ((a)&0x3FC0)==0x2000 ? (0x40 | (((a)>>2) & 0xF)) \
				: ((a)&0x3FE0)==0x3020 ? (0x50 | (((a)>>2) & 0xF)) \
				: ((a)&0x3FFC)==0x3F00 ? 0x60 \
				: ((a)&0x3F80)==0x3F80 ? (0x60 | (((a)>>2) & 0x1F)) \
				: -1 )

#define XDM_ERI_TO_NAR(a)	_XDM_ERI_TO_NAR(a & 0xFFFF)

/* Convert 7-bit NAR address back to ERI/APB address/offset:  */
#define _XDM_NAR_TO_APB(a)	((a) <= 0x1f                ? ((a) << 2)                   \
				:(a) >= 0x20 && (a) <= 0x3F ? (0x1000 | (((a)&  7) << 2) | (((a)&0x18)<<4)) \
				:(a) >= 0x40 && (a) <= 0x4F ? (0x2000 | (((a)&0xF) << 2))  \
				:(a) >= 0x58 && (a) <= 0x5F ? (0x3000 | (((a)&0xF) << 2))  \
				:(a) == 0x60                ? (0x3F00)                     \
				:(a) >= 0x68 && (a) <= 0x7F ? (0x3F80 | (((a)&0x1F) << 2)) \
				: -1)

#define XDM_NAR_TO_APB(a)	_XDM_NAR_TO_APB((a & 0xFFFF))
#define XDM_NAR_TO_ERI(a)	_XDM_NAR_TO_APB((a & 0xFFFF)) | 0x000000

/* Convert APB to ERI address */
#define XDM_APB_TO_ERI(a)	((a) | (0x100000))
#define XDM_ERI_TO_APB(a)	((a) & (0x0FFFFF))

/***********  Bit definitions within some of the above registers  ***********/
#define OCD_ID_LSDDRP			0x01000000
#define OCD_ID_LSDDRP_SHIFT			24
#define OCD_ID_ENDIANESS		0x00000001
#define OCD_ID_ENDIANESS_SHIFT			 0
#define OCD_ID_PSO			0x0000000C
#define OCD_ID_PSO_SHIFT			 2
#define OCD_ID_TRACEPORT		0x00000080
#define OCD_ID_TRACEPORT_SHIFT			 7

/*  Power Status register.  NOTE:  different bit positions in JTAG vs. ERI/APB !!  */
/*  ERI/APB:  */
#define PWRSTAT_CORE_DOMAIN_ON		0x00000001	/* set if core is powered on */
#define PWRSTAT_CORE_DOMAIN_ON_SHIFT		0
#define PWRSTAT_WAKEUP_RESET		0x00000002	/* [ERI only] 0=cold start, 1=PSO wakeup */
#define PWRSTAT_WAKEUP_RESET_SHIFT		1
#define PWRSTAT_CACHES_LOST_POWER	0x00000004	/* [ERI only] set if caches (/localmems?) lost power */
							/* FIXME: does this include local memories? */
#define PWRSTAT_CACHES_LOST_POWER_SHIFT		2
#define PWRSTAT_CORE_STILL_NEEDED	0x00000010	/* set if others keeping core awake */
#define PWRSTAT_CORE_STILL_NEEDED_SHIFT		4
#define PWRSTAT_MEM_DOMAIN_ON		0x00000100	/* set if memory domain is powered on */
#define PWRSTAT_MEM_DOMAIN_ON_SHIFT		8
#define PWRSTAT_DEBUG_DOMAIN_ON		0x00001000	/* set if debug domain is powered on */
#define PWRSTAT_DEBUG_DOMAIN_ON_SHIFT		12
#define PWRSTAT_ALL_ON			PWRSTAT_CORE_DOMAIN_ON | PWRSTAT_MEM_DOMAIN_ON | PWRSTAT_DEBUG_DOMAIN_ON
#define PWRSTAT_CORE_WAS_RESET		0x00010000	/* [APB only] set if core got reset */
#define PWRSTAT_CORE_WAS_RESET_SHIFT		16
#define PWRSTAT_DEBUG_WAS_RESET		0x10000000	/* set if debug module got reset */
#define PWRSTAT_DEBUG_WAS_RESET_SHIFT		28
/*  JTAG:  */
#define J_PWRSTAT_CORE_DOMAIN_ON	0x01		/* set if core is powered on */
#define J_PWRSTAT_MEM_DOMAIN_ON		0x02		/* set if memory domain is powered on */
#define J_PWRSTAT_DEBUG_DOMAIN_ON	0x04		/* set if debug domain is powered on */
#define J_PWRSTAT_ALL_ON		J_PWRSTAT_CORE_DOMAIN_ON | J_PWRSTAT_MEM_DOMAIN_ON | J_PWRSTAT_DEBUG_DOMAIN_ON
#define J_PWRSTAT_CORE_STILL_NEEDED	0x08		/* set if others keeping core awake */
#define J_PWRSTAT_CORE_WAS_RESET	0x10		/* set if core got reset */
#define J_PWRSTAT_DEBUG_WAS_RESET	0x40		/* set if debug module got reset */

/*  Power Control register.  NOTE:  different bit positions in JTAG vs. ERI/APB !!  */
/*  ERI/APB:  */
#define PWRCTL_CORE_SHUTOFF		0x00000001	/* [ERI only] core wants to shut off on WAITI */
#define PWRCTL_CORE_SHUTOFF_SHIFT		0
#define PWRCTL_CORE_WAKEUP		0x00000001	/* [APB only] set to force core to stay powered on */
#define PWRCTL_CORE_WAKEUP_SHIFT		0
#define PWRCTL_MEM_WAKEUP		0x00000100	/* set to force memory domain to stay powered on */
#define PWRCTL_MEM_WAKEUP_SHIFT			8
#define PWRCTL_DEBUG_WAKEUP		0x00001000	/* set to force debug domain to stay powered on */
#define PWRCTL_DEBUG_WAKEUP_SHIFT		12
#define PWRCTL_ALL_ON			PWRCTL_CORE_WAKEUP | PWRCTL_MEM_WAKEUP | PWRCTL_DEBUG_WAKEUP
#define PWRCTL_CORE_RESET		0x00010000	/* [APB only] set to assert core reset */
#define PWRCTL_CORE_RESET_SHIFT			16
#define PWRCTL_DEBUG_RESET		0x10000000	/* set to assert debug module reset */
#define PWRCTL_DEBUG_RESET_SHIFT		28
/*  JTAG:  */
#define J_PWRCTL_CORE_WAKEUP		0x01		/* set to force core to stay powered on */
#define J_PWRCTL_MEM_WAKEUP		0x02		/* set to force memory domain to stay powered on */
#define J_PWRCTL_DEBUG_WAKEUP		0x04		/* set to force debug domain to stay powered on */
#define J_DEBUG_USE			0x80		/*  */
#define J_PWRCTL_ALL_ON			J_DEBUG_USE | J_PWRCTL_CORE_WAKEUP | J_PWRCTL_MEM_WAKEUP | J_PWRCTL_DEBUG_WAKEUP
#define J_PWRCTL_DEBUG_ON		J_DEBUG_USE | J_PWRCTL_DEBUG_WAKEUP
#define J_PWRCTL_CORE_RESET		0x10		/* set to assert core reset */
#define J_PWRCTL_DEBUG_RESET		0x40		/* set to assert debug module reset */

#define J_PWRCTL_WRITE_MASK		0xFF
#define J_PWRSTAT_WRITE_MASK		0xFF

#define PWRCTL_WRITE_MASK		~0
#define PWRSTAT_WRITE_MASK		~0

/************  The following are only relevant for JTAG, so perhaps belong in OCD only  **************/

/*  XDM 5-bit JTAG Instruction Register (IR) values:  */
#define XDM_IR_PWRCTL		0x08	/* select 8-bit Power/Reset Control (PRC) */
#define XDM_IR_PWRSTAT		0x09	/* select 8-bit Power/Reset Status (PRS) */
#define XDM_IR_NAR_SEL		0x1c	/* select altern. 8-bit NAR / 32-bit NDR (Nexus-style) */
#define XDM_IR_NDR_SEL		0x1d	/* select altern. 32-bit NDR / 8-bit NAR
					   (FIXME - functionality not yet in HW) */
#define XDM_IR_IDCODE		0x1e	/* select 32-bit JTAG IDCODE */
#define XDM_IR_BYPASS		0x1f	/* select 1-bit bypass */

#define XDM_IR_WIDTH		5	/* width of IR for Xtensa TAP */

/*  NAR register bits:  */
#define XDM_NAR_WRITE		0x01
#define XDM_NAR_ADDR_MASK	0xFE
#define XDM_NAR_ADDR_SHIFT	1

#define XDM_NAR_BUSY		0x02
#define XDM_NAR_ERROR		0x01

#define NEXUS_DIR_READ	0x00
#define	NEXUS_DIR_WRITE	0x01

/************  Define DCR register bits  **************/

#define DCR_ENABLEOCD		0x0000001
#define DCR_ENABLEOCD_SHIFT		0
#define DCR_DEBUG_INT		0x0000002
#define DCR_DEBUG_INT_SHIFT		1
#define DCR_DEBUG_OVERRIDE	0x0000004
#define DCR_DEBUG_OVERRIDE_SHIFT	2
#define DCR_DEBUG_INTERCEPT	0x0000008
#define DCR_DEBUG_INTERCEPT_SHIFT	3
#define DCR_MASK_NMI		0x0000020
#define DCR_MASK_NMI_SHIFT		5
#define DCR_STEP_ENABLE		0x0000040
#define DCR_STEP_ENABLE_SHIFT		6
#define DCR_BREAK_IN_EN		0x0010000
#define DCR_BREAK_IN_EN_SHIFT		16
#define DCR_BREAK_OUT_EN	0x0020000
#define DCR_BREAK_OUT_EN_SHIFT		17
#define DCR_DEBUG_INT_EN	0x0040000
#define DCR_DEBUG_INT_EN_SHIFT		18
#define DCR_DBG_SW_ACTIVE	0x0100000
#define DCR_DBG_SW_ACTIVE_SHIFT		20
#define DCR_STALL_IN_EN		0x0200000
#define DCR_STALL_IN_EN_SHIFT		21
#define DCR_DEBUG_OUT_EN	0x0400000
#define DCR_DEBUG_OUT_EN_SHIFT		22
#define DCR_BREAK_OUT_ITO	0x1000000
#define DCR_STALL_OUT_ITO	0x2000000
#define DCR_STALL_OUT_ITO_SHIFT		25

/************  Define DSR register bits  **************/

#define DOSR_EXECDONE_ER	0x01
#define DOSR_EXECDONE_SHIFT	0
#define DOSR_EXCEPTION_ER	0x02
#define DOSR_EXCEPTION_SHIFT	1
#define DOSR_BUSY		0x04
#define DOSR_BUSY_SHIFT		2
#define DOSR_OVERRUN		0x08
#define DOSR_OVERRUN_SHIFT	3
#define DOSR_INOCDMODE_ER	0x10
#define DOSR_INOCDMODE_SHIFT	4
#define DOSR_CORE_WROTE_DDR_ER		0x400
#define DOSR_CORE_WROTE_DDR_SHIFT	10
#define DOSR_CORE_READ_DDR_ER		0x800
#define DOSR_CORE_READ_DDR_SHIFT	11
#define DOSR_HOST_WROTE_DDR_ER		0x4000
#define DOSR_HOST_WROTE_DDR_SHIFT	14
#define DOSR_HOST_READ_DDR_ER		0x8000
#define DOSR_HOST_READ_DDR_SHIFT	15

#define DOSR_DEBUG_PEND_BIN		0x10000
#define DOSR_DEBUG_PEND_HOST		0x20000
#define DOSR_DEBUG_PEND_TRAX		0x40000
#define DOSR_DEBUG_BIN		        0x100000
#define DOSR_DEBUG_HOST			0x200000
#define DOSR_DEBUG_TRAX			0x400000
#define DOSR_DEBUG_PEND_BIN_SHIFT	16
#define DOSR_DEBUG_PEND_HOST_SHIFT	17
#define DOSR_DEBUG_PEND_TRAX_SHIFT	18
#define DOSR_DEBUG_BREAKIN              0x0100000
#define DOSR_DEBUG_BREAKIN_SHIFT        20
#define DOSR_DEBUG_HOST_SHIFT		21
#define DOSR_DEBUG_TRAX_SHIFT		22

#define DOSR_DEBUG_STALL		0x1000000
#define DOSR_DEBUG_STALL_SHIFT		24

#define DOSR_CORE_ON			0x40000000
#define DOSR_CORE_ON_SHIFT		30
#define DOSR_DEBUG_ON			0x80000000
#define DOSR_DEBUG_ON_SHIFT		31

/********** Performance monitor registers bits **********/

#define PERF_PMG_ENABLE                 0x00000001 /* global enable bit */
#define PERF_PMG_ENABLE_SHIFT           0

#define PERF_PMCTRL_INT_ENABLE          0x00000001 /* assert interrupt on overflow */
#define PERF_PMCTRL_INT_ENABLE_SHIFT    0
#define PERF_PMCTRL_KRNLCNT             0x00000008 /* ignore TRACELEVEL */
#define PERF_PMCTRL_KRNLCNT_SHIFT       3
#define PERF_PMCTRL_TRACELEVEL          0x000000F0 /* count when CINTLEVEL <= TRACELEVEL */
#define PERF_PMCTRL_TRACELEVEL_SHIFT    4
#define PERF_PMCTRL_SELECT              0x00001F00 /* events group selector */
#define PERF_PMCTRL_SELECT_SHIFT        8
#define PERF_PMCTRL_MASK                0xFFFF0000 /* events mask */
#define PERF_PMCTRL_MASK_SHIFT          16

#define PERF_PMSTAT_OVERFLOW            0x00000001 /* counter overflowed */
#define PERF_PMSTAT_OVERFLOW_SHIFT      0
#define PERF_PMSTAT_INT                 0x00000010 /* interrupt asserted */
#define PERF_PMSTAT_INT_SHIFT           4

#if defined (USE_XDM_REGNAME) || defined (USE_DAP_REGNAME)
/*  Describes XDM register:  */
typedef struct {
    int    reg;
    char*  name;
} regdef_t;

/*char* regname(regdef_t* list, int regno)
{
  unsigned i;
  for(i = 0 ; i < (sizeof(list) / sizeof(regdef_t)); i++){
    if(list[i].reg == regno)
      return list[i].name;
  }
  return "???";
}*/

/*
 *  Returns the name of the specified XDM register number,
 *  or simply "???" if the register number is not recognized.
 *  FIXME - requires -1 as the last entry - change to compare the name to ???
 *  or even better, make the code above to work.
 */
static char * regname(regdef_t* list, int reg)
{
  int i = 0;
  while (list[i].reg != -1){
    if (list[i].reg == reg)
      break;
    i++;
  }
  return list[i].name;
}

#if defined (USE_XDM_REGNAME)
regdef_t xdm_reglist[] =
{
  {XDM_TRAX_ID		  ,"TRAX_ID"      },
  {XDM_TRAX_CONTROL	  ,"CONTROL"      },
  {XDM_TRAX_STATUS	  ,"STATUS"       },
  {XDM_TRAX_DATA          ,"DATA"         },
  {XDM_TRAX_ADDRESS	  ,"ADDRESS"      },
  {XDM_TRAX_TRIGGER	  ,"TRIGGER PC"   },
  {XDM_TRAX_MATCH         ,"PC MATCH"     },
  {XDM_TRAX_DELAY	  ,"DELAY CNT."   },
  {XDM_TRAX_STARTADDR	  ,"START ADDRESS"},
  {XDM_TRAX_ENDADDR	  ,"END ADDRESS"  },
  {XDM_TRAX_DEBUGPC	  ,"DEBUG PC"  },
  {XDM_TRAX_P4CHANGE	  ,"P4 CHANGE"    },
  {XDM_TRAX_P4REV         ,"P4 REV."      },
  {XDM_TRAX_P4DATE	  ,"P4 DATE"      },
  {XDM_TRAX_P4TIME	  ,"P4 TIME"      },
  {XDM_TRAX_PDSTATUS	  ,"PD STATUS"    },
  {XDM_TRAX_PDDATA	  ,"PD DATA"      },
  {XDM_TRAX_STOP_PC	  ,"STOP PC"      },
  {XDM_TRAX_STOP_ICNT	  ,"STOP ICNT"    },
  {XDM_TRAX_MSG_STATUS	  ,"MSG STAT."    },
  {XDM_TRAX_FSM_STATUS	  ,"FSM STAT."    },
  {XDM_TRAX_IB_STATUS	  ,"IB STAT."     },

  {XDM_OCD_ID         ,"OCD_ID"  },
  {XDM_OCD_DCR_CLR    ,"DCR_CLR" },
  {XDM_OCD_DCR_SET    ,"DCR_SET" },
  {XDM_OCD_DSR        ,"DOSR"    },
  {XDM_OCD_DDR        ,"DDR"     },
  {XDM_OCD_DDREXEC    ,"DDREXEC" },
  {XDM_OCD_DIR0EXEC   ,"DIR0EXEC"},
  {XDM_OCD_DIR0       ,"DIR0"    },
  {XDM_OCD_DIR1       ,"DIR1"    },
  {XDM_OCD_DIR2       ,"DIR2"    },
  {XDM_OCD_DIR3       ,"DIR3"    },
  {XDM_OCD_DIR4       ,"DIR4"    },
  {XDM_OCD_DIR5       ,"DIR5"    },
  {XDM_OCD_DIR6       ,"DIR6"    },
  {XDM_OCD_DIR7       ,"DIR7"    },

  {XDM_PERF_PMG       ,"PMG"    },
  {XDM_PERF_INTPC     ,"INTPC"  },
  {XDM_PERF_PM0       ,"PM0 "   },
  {XDM_PERF_PM1       ,"PM1 "   },
  {XDM_PERF_PM2       ,"PM2 "   },
  {XDM_PERF_PM3       ,"PM3 "   },
  {XDM_PERF_PM4       ,"PM4 "   },
  {XDM_PERF_PM5       ,"PM5 "   },
  {XDM_PERF_PM6       ,"PM6 "   },
  {XDM_PERF_PM7       ,"PM7 "   },
  {XDM_PERF_PMCTRL0   ,"PMCTRL0"},
  {XDM_PERF_PMCTRL1   ,"PMCTRL1"},
  {XDM_PERF_PMCTRL2   ,"PMCTRL2"},
  {XDM_PERF_PMCTRL3   ,"PMCTRL3"},
  {XDM_PERF_PMCTRL4   ,"PMCTRL4"},
  {XDM_PERF_PMCTRL5   ,"PMCTRL5"},
  {XDM_PERF_PMCTRL6   ,"PMCTRL6"},
  {XDM_PERF_PMCTRL7   ,"PMCTRL7"},
  {XDM_PERF_PMSTAT0   ,"PMSTAT0"},
  {XDM_PERF_PMSTAT1   ,"PMSTAT1"},
  {XDM_PERF_PMSTAT2   ,"PMSTAT2"},
  {XDM_PERF_PMSTAT3   ,"PMSTAT3"},
  {XDM_PERF_PMSTAT4   ,"PMSTAT4"},
  {XDM_PERF_PMSTAT5   ,"PMSTAT5"},
  {XDM_PERF_PMSTAT6   ,"PMSTAT6"},
  {XDM_PERF_PMSTAT7   ,"PMSTAT7"},

  {XDM_MISC_PWRCTL    ,"PWRCTL"  },
  {XDM_MISC_PWRSTAT   ,"PWRSTAT" },
  {XDM_MISC_ERISTAT   ,"ERISTAT" },
  {XDM_MISC_DATETIME  ,"DATETIME"},
  {XDM_MISC_UBID      ,"UBID"    },
  {XDM_MISC_CID       ,"CID"     },

  {XDM_CS_ITCTRL      ,"ITCTRL"     },
  {XDM_CS_CLAIMSET    ,"CLAIMSET"   },
  {XDM_CS_CLAIMCLR    ,"CLAIMCLR"   },
  {XDM_CS_LOCK_ACCESS ,"LOCK_ACCESS"},
  {XDM_CS_LOCK_STATUS ,"LOCK_STATUS"},
  {XDM_CS_AUTH_STATUS ,"AUTH_STATUS"},
  {XDM_CS_DEV_ID      ,"DEV_ID"     },
  {XDM_CS_DEV_TYPE    ,"DEV_TYPE"   },
  {XDM_CS_PER_ID4     ,"PER_ID4"    },
  {XDM_CS_PER_ID5     ,"PER_ID5"    },
  {XDM_CS_PER_ID6     ,"PER_ID6"    },
  {XDM_CS_PER_ID7     ,"PER_ID7"    },
  {XDM_CS_PER_ID0     ,"PER_ID0"    },
  {XDM_CS_PER_ID1     ,"PER_ID1"    },
  {XDM_CS_PER_ID2     ,"PER_ID2"    },
  {XDM_CS_PER_ID3     ,"PER_ID3"    },
  {XDM_CS_COMP_ID0    ,"COMP_ID0"   },
  {XDM_CS_COMP_ID1    ,"COMP_ID1"   },
  {XDM_CS_COMP_ID2    ,"COMP_ID2"   },
  {XDM_CS_COMP_ID3    ,"COMP_ID3"   },
  {-1                 ,"???"        },
};
#endif

#endif

#endif /* _XDM_REGS_H_ */
