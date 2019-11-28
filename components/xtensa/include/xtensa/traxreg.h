/* TRAX register definitions

   Copyright (c) 2006-2012 Tensilica Inc.

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

#ifndef _TRAX_REGISTERS_H_
#define _TRAX_REGISTERS_H_

#define SHOW	1
#define HIDE	0

#define RO	0
#define RW	1

/*  TRAX Register Numbers (from possible range of 0..127)  */
#if 0
#define TRAXREG_ID		0
#define TRAXREG_CONTROL		1
#define TRAXREG_STATUS		2
#define TRAXREG_DATA		3
#define TRAXREG_ADDRESS		4
#define TRAXREG_TRIGGER		5
#define TRAXREG_MATCH		6
#define TRAXREG_DELAY		7
#define TRAXREG_STARTADDR	8
#define TRAXREG_ENDADDR		9
/*  Internal use only (unpublished):  */
#define TRAXREG_P4CHANGE	16
#define TRAXREG_P4REV		17
#define TRAXREG_P4DATE		18
#define TRAXREG_P4TIME		19
#define TRAXREG_PDSTATUS	20
#define TRAXREG_PDDATA		21
#define TRAXREG_STOP_PC		22
#define TRAXREG_STOP_ICNT	23
#define TRAXREG_MSG_STATUS	24
#define TRAXREG_FSM_STATUS	25
#define TRAXREG_IB_STATUS	26
#define TRAXREG_MAX		27
#define TRAXREG_ITCTRL		96
#endif
/* The registers above match the NAR addresses. So, their values are used for NAR access */

/*  TRAX Register Fields  */

/*  TRAX ID register fields:  */
#define TRAX_ID_PRODNO          0xf0000000	/* product number (0=TRAX) */
#define TRAX_ID_PRODOPT         0x0f000000	/* product options */
#define TRAX_ID_MIW64		0x08000000	/* opt: instruction width */
#define TRAX_ID_AMTRAX		0x04000000	/* opt: collection of options, internal (VER_2_0 or later)*/
#define TRAX_ID_MAJVER(id)	(((id) >> 20) & 0x0f)
#define TRAX_ID_MINVER(id)	(((id) >> 17) & 0x07)
#define TRAX_ID_VER(id)		((TRAX_ID_MAJVER(id)<<4)|TRAX_ID_MINVER(id))
#define TRAX_ID_STDCFG		0x00010000	/* standard config */
#define TRAX_ID_CFGID		0x0000ffff	/* TRAX configuration ID */
#define TRAX_ID_MEMSHARED	0x00001000	/* Memshared option in TRAX */
#define TRAX_ID_FROM_VER(ver)	((((ver) & 0xf0) << 16) | (((ver) & 0x7) << 17))
/*  Other TRAX ID register macros:  */
/*  TRAX versions of interest (TRAX_ID_VER(), ie. MAJVER*16 + MINVER):  */
#define TRAX_VER_1_0		0x10		/* RA */
#define TRAX_VER_1_1		0x11		/* RB thru RC-2010.1 */
#define TRAX_VER_2_0		0x20		/* RC-2010.2, RD-2010.0, RD-2011.1 */
#define TRAX_VER_2_1		0x21		/* RC-2011.3 / RD-2011.2 and later */
#define TRAX_VER_3_0		0x30		/* RE-2012.0 */
#define	TRAX_VER_3_1		0x31		/* RE-2012.1 */
#define TRAX_VER_HUAWEI_3	TRAX_VER_3_0	/* For Huawei, PRs: 25223, 25224, 24880 */


/*  TRAX version 1.0 requires a couple software workarounds:  */
#define TRAX_ID_1_0_ERRATUM(id)	(TRAX_ID_VER(id) == TRAX_VER_1_0)
/*  TRAX version 2.0 requires software workaround for PR 22161:  */
#define TRAX_ID_MEMSZ_ERRATUM(id)	(TRAX_ID_VER(id) == TRAX_VER_2_0)

/*  TRAX Control register fields:  */
#define TRAX_CONTROL_TREN	0x00000001
#define TRAX_CONTROL_TRSTP	0x00000002
#define TRAX_CONTROL_PCMEN	0x00000004
#define TRAX_CONTROL_PTIEN	0x00000010
#define TRAX_CONTROL_CTIEN	0x00000020
#define TRAX_CONTROL_TMEN	0x00000080	/* 2.0+ */
#define TRAX_CONTROL_CNTU	0x00000200
#define TRAX_CONTROL_BIEN	0x00000400
#define TRAX_CONTROL_BOEN	0x00000800
#define TRAX_CONTROL_TSEN	0x00000800
#define TRAX_CONTROL_SMPER	0x00007000
#define TRAX_CONTROL_SMPER_SHIFT	12
#define TRAX_CONTROL_PTOWT	0x00010000
#define TRAX_CONTROL_CTOWT	0x00020000
#define TRAX_CONTROL_PTOWS	0x00100000
#define TRAX_CONTROL_CTOWS	0x00200000
#define TRAX_CONTROL_ATID	0x7F000000	/* 2.0+, amtrax */
#define TRAX_CONTROL_ATID_SHIFT		24
#define TRAX_CONTROL_ATEN	0x80000000	/* 2.0+, amtrax */

#define TRAX_CONTROL_PTOWS_ER	0x00020000	/* For 3.0 */
#define TRAX_CONTROL_CTOWT_ER	0x00100000	/* For 3.0 */

#define TRAX_CONTROL_ITCTO	0x00400000	/* For 3.0 */
#define TRAX_CONTROL_ITCTIA	0x00800000	/* For 3.0 */
#define TRAX_CONTROL_ITATV	0x01000000	/* For 3.0 */


/*  TRAX Status register fields:  */
#define TRAX_STATUS_TRACT	0x00000001
#define TRAX_STATUS_TRIG	0x00000002
#define TRAX_STATUS_PCMTG	0x00000004
#define TRAX_STATUS_BUSY	0x00000008  /* ER ??? */
#define TRAX_STATUS_PTITG	0x00000010
#define TRAX_STATUS_CTITG	0x00000020
#define TRAX_STATUS_MEMSZ	0x00001F00
#define TRAX_STATUS_MEMSZ_SHIFT		8
#define TRAX_STATUS_PTO		0x00010000
#define TRAX_STATUS_CTO		0x00020000

#define TRAX_STATUS_ITCTOA	0x00400000	/* For 3.0 */
#define TRAX_STATUS_ITCTI	0x00800000	/* For 3.0 */
#define TRAX_STATUS_ITATR	0x01000000	/* For 3.0 */


/*  TRAX Address register fields:  */
#define TRAX_ADDRESS_TWSAT	0x80000000
#define TRAX_ADDRESS_TWSAT_SHIFT	31
#define TRAX_ADDRESS_TOTALMASK	0x00FFFFFF
// !!! VUakiVU. added for new TRAX:
#define TRAX_ADDRESS_WRAPCNT	0x7FE00000	/* version ???... */
#define TRAX_ADDRESS_WRAP_SHIFT		21

/*  TRAX PCMatch register fields:  */
#define TRAX_PCMATCH_PCML	0x0000001F
#define TRAX_PCMATCH_PCML_SHIFT		0
#define TRAX_PCMATCH_PCMS	0x80000000

/*  Compute trace ram buffer size (in bytes) from status register:  */
#define TRAX_MEM_SIZE(status)	(1L << (((status) & TRAX_STATUS_MEMSZ) >> TRAX_STATUS_MEMSZ_SHIFT))

#if 0
/*  Describes a field within a register:  */
typedef struct {
    const char*	name;
// unsigned	width;
// unsigned	shift;
    char	width;
    char	shift;
    char	visible;		/* 0 = internal use only, 1 = shown */
    char	reserved;
} trax_regfield_t;
#endif

/*  Describes a TRAX register:  */
typedef struct {
    const char*	name;
    unsigned	id;
    char	width;
    char	visible;
    char	writable;
    char	reserved;
    //const trax_regfield_t * fieldset;
} trax_regdef_t;


extern const trax_regdef_t	trax_reglist[];
extern const signed int 	trax_readable_regs[];
extern const signed int 	trax_unamed_header_regs[];

#ifdef  __cplusplus
extern "C" {
#endif

/*  Prototypes:  */
extern int		trax_find_reg(char * regname, char **errmsg);
extern const char *	trax_regname(int regno);

#ifdef  __cplusplus
}
#endif

#endif /* _TRAX_REGISTERS_H_ */

