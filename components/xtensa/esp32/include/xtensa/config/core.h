/*
 * xtensa/config/core.h -- HAL definitions dependent on CORE configuration
 *
 *  This header file is sometimes referred to as the "compile-time HAL" or CHAL.
 *  It pulls definitions tailored for a specific Xtensa processor configuration.
 *
 *  Sources for binaries meant to be configuration-independent generally avoid
 *  including this file (they may use the configuration-specific HAL library).
 *  It is normal for the HAL library source itself to include this file.
 */

/*
 * Copyright (c) 2005-2014 Cadence Design Systems, Inc.
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


#ifndef XTENSA_CONFIG_CORE_H
#define XTENSA_CONFIG_CORE_H

/*  CONFIGURATION INDEPENDENT DEFINITIONS:  */
#ifdef __XTENSA__
#include <xtensa/hal.h>
#include <xtensa/xtensa-versions.h>
#else
#include "xtensa/hal.h"
#include "xtensa/xtensa-versions.h"
#endif

/*  CONFIGURATION SPECIFIC DEFINITIONS:  */
#ifdef __XTENSA__
#include <xtensa/config/core-isa.h>
#include <xtensa/config/core-matmap.h>
#include <xtensa/config/tie.h>
#else
#include "core-isa.h"
#include "core-matmap.h"
#include "tie.h"
#endif

#if defined (_ASMLANGUAGE) || defined (__ASSEMBLER__)
#ifdef __XTENSA__
#include <xtensa/config/tie-asm.h>
#else
#include "tie-asm.h"
#endif
#endif /*_ASMLANGUAGE or __ASSEMBLER__*/


/*----------------------------------------------------------------------
				GENERAL
  ----------------------------------------------------------------------*/

/*
 *  Separators for macros that expand into arrays.
 *  These can be predefined by files that #include this one,
 *  when different separators are required.
 */
/*  Element separator for macros that expand into 1-dimensional arrays:  */
#ifndef XCHAL_SEP
#define XCHAL_SEP			,
#endif
/*  Array separator for macros that expand into 2-dimensional arrays:  */
#ifndef XCHAL_SEP2
#define XCHAL_SEP2			},{
#endif



/*----------------------------------------------------------------------
				ISA
  ----------------------------------------------------------------------*/

#if XCHAL_HAVE_BE
# define XCHAL_HAVE_LE			0
# define XCHAL_MEMORY_ORDER		XTHAL_BIGENDIAN
#else
# define XCHAL_HAVE_LE			1
# define XCHAL_MEMORY_ORDER		XTHAL_LITTLEENDIAN
#endif



/*----------------------------------------------------------------------
				INTERRUPTS
  ----------------------------------------------------------------------*/

/*  Indexing macros:  */
#define _XCHAL_INTLEVEL_MASK(n)		XCHAL_INTLEVEL ## n ## _MASK
#define XCHAL_INTLEVEL_MASK(n)		_XCHAL_INTLEVEL_MASK(n)		/* n = 0 .. 15 */
#define _XCHAL_INTLEVEL_ANDBELOWMASK(n)	XCHAL_INTLEVEL ## n ## _ANDBELOW_MASK
#define XCHAL_INTLEVEL_ANDBELOW_MASK(n)	_XCHAL_INTLEVEL_ANDBELOWMASK(n)	/* n = 0 .. 15 */
#define _XCHAL_INTLEVEL_NUM(n)		XCHAL_INTLEVEL ## n ## _NUM
#define XCHAL_INTLEVEL_NUM(n)		_XCHAL_INTLEVEL_NUM(n)		/* n = 0 .. 15 */
#define _XCHAL_INT_LEVEL(n)		XCHAL_INT ## n ## _LEVEL
#define XCHAL_INT_LEVEL(n)		_XCHAL_INT_LEVEL(n)		/* n = 0 .. 31 */
#define _XCHAL_INT_TYPE(n)		XCHAL_INT ## n ## _TYPE
#define XCHAL_INT_TYPE(n)		_XCHAL_INT_TYPE(n)		/* n = 0 .. 31 */
#define _XCHAL_TIMER_INTERRUPT(n)	XCHAL_TIMER ## n ## _INTERRUPT
#define XCHAL_TIMER_INTERRUPT(n)	_XCHAL_TIMER_INTERRUPT(n)	/* n = 0 .. 3 */


#define XCHAL_HAVE_HIGHLEVEL_INTERRUPTS	XCHAL_HAVE_HIGHPRI_INTERRUPTS
#define XCHAL_NUM_LOWPRI_LEVELS		1			/* number of low-priority interrupt levels (always 1) */
#define XCHAL_FIRST_HIGHPRI_LEVEL	(XCHAL_NUM_LOWPRI_LEVELS+1)	/* level of first high-priority interrupt (always 2) */
/*  Note:  1 <= LOWPRI_LEVELS <= EXCM_LEVEL < DEBUGLEVEL <= NUM_INTLEVELS < NMILEVEL <= 15  */

/*  These values are constant for existing Xtensa processor implementations:  */
#define XCHAL_INTLEVEL0_MASK		0x00000000
#define XCHAL_INTLEVEL8_MASK		0x00000000
#define XCHAL_INTLEVEL9_MASK		0x00000000
#define XCHAL_INTLEVEL10_MASK		0x00000000
#define XCHAL_INTLEVEL11_MASK		0x00000000
#define XCHAL_INTLEVEL12_MASK		0x00000000
#define XCHAL_INTLEVEL13_MASK		0x00000000
#define XCHAL_INTLEVEL14_MASK		0x00000000
#define XCHAL_INTLEVEL15_MASK		0x00000000

/*  Array of masks of interrupts at each interrupt level:  */
#define XCHAL_INTLEVEL_MASKS		XCHAL_INTLEVEL0_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL1_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL2_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL3_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL4_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL5_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL6_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL7_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL8_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL9_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL10_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL11_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL12_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL13_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL14_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL15_MASK

/*  These values are constant for existing Xtensa processor implementations:  */
#define XCHAL_INTLEVEL0_ANDBELOW_MASK	0x00000000
#define XCHAL_INTLEVEL8_ANDBELOW_MASK	XCHAL_INTLEVEL7_ANDBELOW_MASK
#define XCHAL_INTLEVEL9_ANDBELOW_MASK	XCHAL_INTLEVEL7_ANDBELOW_MASK
#define XCHAL_INTLEVEL10_ANDBELOW_MASK	XCHAL_INTLEVEL7_ANDBELOW_MASK
#define XCHAL_INTLEVEL11_ANDBELOW_MASK	XCHAL_INTLEVEL7_ANDBELOW_MASK
#define XCHAL_INTLEVEL12_ANDBELOW_MASK	XCHAL_INTLEVEL7_ANDBELOW_MASK
#define XCHAL_INTLEVEL13_ANDBELOW_MASK	XCHAL_INTLEVEL7_ANDBELOW_MASK
#define XCHAL_INTLEVEL14_ANDBELOW_MASK	XCHAL_INTLEVEL7_ANDBELOW_MASK
#define XCHAL_INTLEVEL15_ANDBELOW_MASK	XCHAL_INTLEVEL7_ANDBELOW_MASK

/*  Mask of all low-priority interrupts:  */
#define XCHAL_LOWPRI_MASK		XCHAL_INTLEVEL1_ANDBELOW_MASK

/*  Mask of all interrupts masked by PS.EXCM (or CEXCM):  */
#define XCHAL_EXCM_MASK			XCHAL_INTLEVEL_ANDBELOW_MASK(XCHAL_EXCM_LEVEL)

/*  Array of masks of interrupts at each range 1..n of interrupt levels:  */
#define XCHAL_INTLEVEL_ANDBELOW_MASKS	XCHAL_INTLEVEL0_ANDBELOW_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL1_ANDBELOW_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL2_ANDBELOW_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL3_ANDBELOW_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL4_ANDBELOW_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL5_ANDBELOW_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL6_ANDBELOW_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL7_ANDBELOW_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL8_ANDBELOW_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL9_ANDBELOW_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL10_ANDBELOW_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL11_ANDBELOW_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL12_ANDBELOW_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL13_ANDBELOW_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL14_ANDBELOW_MASK \
			XCHAL_SEP	XCHAL_INTLEVEL15_ANDBELOW_MASK

#if 0 /*XCHAL_HAVE_NMI*/
/*  NMI "interrupt level" (for use with EXCSAVE_n, EPS_n, EPC_n, RFI n):  */
# define XCHAL_NMILEVEL		(XCHAL_NUM_INTLEVELS+1)
#endif

/*  Array of levels of each possible interrupt:  */
#define XCHAL_INT_LEVELS		XCHAL_INT0_LEVEL \
			XCHAL_SEP	XCHAL_INT1_LEVEL \
			XCHAL_SEP	XCHAL_INT2_LEVEL \
			XCHAL_SEP	XCHAL_INT3_LEVEL \
			XCHAL_SEP	XCHAL_INT4_LEVEL \
			XCHAL_SEP	XCHAL_INT5_LEVEL \
			XCHAL_SEP	XCHAL_INT6_LEVEL \
			XCHAL_SEP	XCHAL_INT7_LEVEL \
			XCHAL_SEP	XCHAL_INT8_LEVEL \
			XCHAL_SEP	XCHAL_INT9_LEVEL \
			XCHAL_SEP	XCHAL_INT10_LEVEL \
			XCHAL_SEP	XCHAL_INT11_LEVEL \
			XCHAL_SEP	XCHAL_INT12_LEVEL \
			XCHAL_SEP	XCHAL_INT13_LEVEL \
			XCHAL_SEP	XCHAL_INT14_LEVEL \
			XCHAL_SEP	XCHAL_INT15_LEVEL \
			XCHAL_SEP	XCHAL_INT16_LEVEL \
			XCHAL_SEP	XCHAL_INT17_LEVEL \
			XCHAL_SEP	XCHAL_INT18_LEVEL \
			XCHAL_SEP	XCHAL_INT19_LEVEL \
			XCHAL_SEP	XCHAL_INT20_LEVEL \
			XCHAL_SEP	XCHAL_INT21_LEVEL \
			XCHAL_SEP	XCHAL_INT22_LEVEL \
			XCHAL_SEP	XCHAL_INT23_LEVEL \
			XCHAL_SEP	XCHAL_INT24_LEVEL \
			XCHAL_SEP	XCHAL_INT25_LEVEL \
			XCHAL_SEP	XCHAL_INT26_LEVEL \
			XCHAL_SEP	XCHAL_INT27_LEVEL \
			XCHAL_SEP	XCHAL_INT28_LEVEL \
			XCHAL_SEP	XCHAL_INT29_LEVEL \
			XCHAL_SEP	XCHAL_INT30_LEVEL \
			XCHAL_SEP	XCHAL_INT31_LEVEL

/*  Array of types of each possible interrupt:  */
#define XCHAL_INT_TYPES			XCHAL_INT0_TYPE \
			XCHAL_SEP	XCHAL_INT1_TYPE \
			XCHAL_SEP	XCHAL_INT2_TYPE \
			XCHAL_SEP	XCHAL_INT3_TYPE \
			XCHAL_SEP	XCHAL_INT4_TYPE \
			XCHAL_SEP	XCHAL_INT5_TYPE \
			XCHAL_SEP	XCHAL_INT6_TYPE \
			XCHAL_SEP	XCHAL_INT7_TYPE \
			XCHAL_SEP	XCHAL_INT8_TYPE \
			XCHAL_SEP	XCHAL_INT9_TYPE \
			XCHAL_SEP	XCHAL_INT10_TYPE \
			XCHAL_SEP	XCHAL_INT11_TYPE \
			XCHAL_SEP	XCHAL_INT12_TYPE \
			XCHAL_SEP	XCHAL_INT13_TYPE \
			XCHAL_SEP	XCHAL_INT14_TYPE \
			XCHAL_SEP	XCHAL_INT15_TYPE \
			XCHAL_SEP	XCHAL_INT16_TYPE \
			XCHAL_SEP	XCHAL_INT17_TYPE \
			XCHAL_SEP	XCHAL_INT18_TYPE \
			XCHAL_SEP	XCHAL_INT19_TYPE \
			XCHAL_SEP	XCHAL_INT20_TYPE \
			XCHAL_SEP	XCHAL_INT21_TYPE \
			XCHAL_SEP	XCHAL_INT22_TYPE \
			XCHAL_SEP	XCHAL_INT23_TYPE \
			XCHAL_SEP	XCHAL_INT24_TYPE \
			XCHAL_SEP	XCHAL_INT25_TYPE \
			XCHAL_SEP	XCHAL_INT26_TYPE \
			XCHAL_SEP	XCHAL_INT27_TYPE \
			XCHAL_SEP	XCHAL_INT28_TYPE \
			XCHAL_SEP	XCHAL_INT29_TYPE \
			XCHAL_SEP	XCHAL_INT30_TYPE \
			XCHAL_SEP	XCHAL_INT31_TYPE

/*  Array of masks of interrupts for each type of interrupt:  */
#define XCHAL_INTTYPE_MASKS		XCHAL_INTTYPE_MASK_UNCONFIGURED	\
			XCHAL_SEP	XCHAL_INTTYPE_MASK_SOFTWARE	\
			XCHAL_SEP	XCHAL_INTTYPE_MASK_EXTERN_EDGE	\
			XCHAL_SEP	XCHAL_INTTYPE_MASK_EXTERN_LEVEL	\
			XCHAL_SEP	XCHAL_INTTYPE_MASK_TIMER	\
			XCHAL_SEP	XCHAL_INTTYPE_MASK_NMI		\
			XCHAL_SEP	XCHAL_INTTYPE_MASK_WRITE_ERROR

/*  Interrupts that can be cleared using the INTCLEAR special register:  */
#define XCHAL_INTCLEARABLE_MASK	(XCHAL_INTTYPE_MASK_SOFTWARE+XCHAL_INTTYPE_MASK_EXTERN_EDGE+XCHAL_INTTYPE_MASK_WRITE_ERROR)
/*  Interrupts that can be triggered using the INTSET special register:  */
#define XCHAL_INTSETTABLE_MASK	XCHAL_INTTYPE_MASK_SOFTWARE

/*  Array of interrupts assigned to each timer (CCOMPARE0 to CCOMPARE3):  */
#define XCHAL_TIMER_INTERRUPTS		XCHAL_TIMER0_INTERRUPT \
			XCHAL_SEP	XCHAL_TIMER1_INTERRUPT \
			XCHAL_SEP	XCHAL_TIMER2_INTERRUPT \
			XCHAL_SEP	XCHAL_TIMER3_INTERRUPT



/*  For backward compatibility and for the array macros, define macros for
 *  each unconfigured interrupt number (unfortunately, the value of
 *  XTHAL_INTTYPE_UNCONFIGURED is not zero):  */
#if XCHAL_NUM_INTERRUPTS == 0
# define XCHAL_INT0_LEVEL		0
# define XCHAL_INT0_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 1
# define XCHAL_INT1_LEVEL		0
# define XCHAL_INT1_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 2
# define XCHAL_INT2_LEVEL		0
# define XCHAL_INT2_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 3
# define XCHAL_INT3_LEVEL		0
# define XCHAL_INT3_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 4
# define XCHAL_INT4_LEVEL		0
# define XCHAL_INT4_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 5
# define XCHAL_INT5_LEVEL		0
# define XCHAL_INT5_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 6
# define XCHAL_INT6_LEVEL		0
# define XCHAL_INT6_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 7
# define XCHAL_INT7_LEVEL		0
# define XCHAL_INT7_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 8
# define XCHAL_INT8_LEVEL		0
# define XCHAL_INT8_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 9
# define XCHAL_INT9_LEVEL		0
# define XCHAL_INT9_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 10
# define XCHAL_INT10_LEVEL		0
# define XCHAL_INT10_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 11
# define XCHAL_INT11_LEVEL		0
# define XCHAL_INT11_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 12
# define XCHAL_INT12_LEVEL		0
# define XCHAL_INT12_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 13
# define XCHAL_INT13_LEVEL		0
# define XCHAL_INT13_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 14
# define XCHAL_INT14_LEVEL		0
# define XCHAL_INT14_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 15
# define XCHAL_INT15_LEVEL		0
# define XCHAL_INT15_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 16
# define XCHAL_INT16_LEVEL		0
# define XCHAL_INT16_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 17
# define XCHAL_INT17_LEVEL		0
# define XCHAL_INT17_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 18
# define XCHAL_INT18_LEVEL		0
# define XCHAL_INT18_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 19
# define XCHAL_INT19_LEVEL		0
# define XCHAL_INT19_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 20
# define XCHAL_INT20_LEVEL		0
# define XCHAL_INT20_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 21
# define XCHAL_INT21_LEVEL		0
# define XCHAL_INT21_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 22
# define XCHAL_INT22_LEVEL		0
# define XCHAL_INT22_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 23
# define XCHAL_INT23_LEVEL		0
# define XCHAL_INT23_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 24
# define XCHAL_INT24_LEVEL		0
# define XCHAL_INT24_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 25
# define XCHAL_INT25_LEVEL		0
# define XCHAL_INT25_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 26
# define XCHAL_INT26_LEVEL		0
# define XCHAL_INT26_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 27
# define XCHAL_INT27_LEVEL		0
# define XCHAL_INT27_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 28
# define XCHAL_INT28_LEVEL		0
# define XCHAL_INT28_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 29
# define XCHAL_INT29_LEVEL		0
# define XCHAL_INT29_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 30
# define XCHAL_INT30_LEVEL		0
# define XCHAL_INT30_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif
#if XCHAL_NUM_INTERRUPTS <= 31
# define XCHAL_INT31_LEVEL		0
# define XCHAL_INT31_TYPE		XTHAL_INTTYPE_UNCONFIGURED
#endif


/*
 *  Masks and levels corresponding to each *external* interrupt.
 */

#define XCHAL_EXTINT0_MASK		(1 << XCHAL_EXTINT0_NUM)
#define XCHAL_EXTINT0_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT0_NUM)
#define XCHAL_EXTINT1_MASK		(1 << XCHAL_EXTINT1_NUM)
#define XCHAL_EXTINT1_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT1_NUM)
#define XCHAL_EXTINT2_MASK		(1 << XCHAL_EXTINT2_NUM)
#define XCHAL_EXTINT2_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT2_NUM)
#define XCHAL_EXTINT3_MASK		(1 << XCHAL_EXTINT3_NUM)
#define XCHAL_EXTINT3_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT3_NUM)
#define XCHAL_EXTINT4_MASK		(1 << XCHAL_EXTINT4_NUM)
#define XCHAL_EXTINT4_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT4_NUM)
#define XCHAL_EXTINT5_MASK		(1 << XCHAL_EXTINT5_NUM)
#define XCHAL_EXTINT5_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT5_NUM)
#define XCHAL_EXTINT6_MASK		(1 << XCHAL_EXTINT6_NUM)
#define XCHAL_EXTINT6_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT6_NUM)
#define XCHAL_EXTINT7_MASK		(1 << XCHAL_EXTINT7_NUM)
#define XCHAL_EXTINT7_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT7_NUM)
#define XCHAL_EXTINT8_MASK		(1 << XCHAL_EXTINT8_NUM)
#define XCHAL_EXTINT8_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT8_NUM)
#define XCHAL_EXTINT9_MASK		(1 << XCHAL_EXTINT9_NUM)
#define XCHAL_EXTINT9_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT9_NUM)
#define XCHAL_EXTINT10_MASK		(1 << XCHAL_EXTINT10_NUM)
#define XCHAL_EXTINT10_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT10_NUM)
#define XCHAL_EXTINT11_MASK		(1 << XCHAL_EXTINT11_NUM)
#define XCHAL_EXTINT11_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT11_NUM)
#define XCHAL_EXTINT12_MASK		(1 << XCHAL_EXTINT12_NUM)
#define XCHAL_EXTINT12_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT12_NUM)
#define XCHAL_EXTINT13_MASK		(1 << XCHAL_EXTINT13_NUM)
#define XCHAL_EXTINT13_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT13_NUM)
#define XCHAL_EXTINT14_MASK		(1 << XCHAL_EXTINT14_NUM)
#define XCHAL_EXTINT14_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT14_NUM)
#define XCHAL_EXTINT15_MASK		(1 << XCHAL_EXTINT15_NUM)
#define XCHAL_EXTINT15_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT15_NUM)
#define XCHAL_EXTINT16_MASK		(1 << XCHAL_EXTINT16_NUM)
#define XCHAL_EXTINT16_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT16_NUM)
#define XCHAL_EXTINT17_MASK		(1 << XCHAL_EXTINT17_NUM)
#define XCHAL_EXTINT17_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT17_NUM)
#define XCHAL_EXTINT18_MASK		(1 << XCHAL_EXTINT18_NUM)
#define XCHAL_EXTINT18_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT18_NUM)
#define XCHAL_EXTINT19_MASK		(1 << XCHAL_EXTINT19_NUM)
#define XCHAL_EXTINT19_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT19_NUM)
#define XCHAL_EXTINT20_MASK		(1 << XCHAL_EXTINT20_NUM)
#define XCHAL_EXTINT20_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT20_NUM)
#define XCHAL_EXTINT21_MASK		(1 << XCHAL_EXTINT21_NUM)
#define XCHAL_EXTINT21_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT21_NUM)
#define XCHAL_EXTINT22_MASK		(1 << XCHAL_EXTINT22_NUM)
#define XCHAL_EXTINT22_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT22_NUM)
#define XCHAL_EXTINT23_MASK		(1 << XCHAL_EXTINT23_NUM)
#define XCHAL_EXTINT23_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT23_NUM)
#define XCHAL_EXTINT24_MASK		(1 << XCHAL_EXTINT24_NUM)
#define XCHAL_EXTINT24_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT24_NUM)
#define XCHAL_EXTINT25_MASK		(1 << XCHAL_EXTINT25_NUM)
#define XCHAL_EXTINT25_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT25_NUM)
#define XCHAL_EXTINT26_MASK		(1 << XCHAL_EXTINT26_NUM)
#define XCHAL_EXTINT26_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT26_NUM)
#define XCHAL_EXTINT27_MASK		(1 << XCHAL_EXTINT27_NUM)
#define XCHAL_EXTINT27_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT27_NUM)
#define XCHAL_EXTINT28_MASK		(1 << XCHAL_EXTINT28_NUM)
#define XCHAL_EXTINT28_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT28_NUM)
#define XCHAL_EXTINT29_MASK		(1 << XCHAL_EXTINT29_NUM)
#define XCHAL_EXTINT29_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT29_NUM)
#define XCHAL_EXTINT30_MASK		(1 << XCHAL_EXTINT30_NUM)
#define XCHAL_EXTINT30_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT30_NUM)
#define XCHAL_EXTINT31_MASK		(1 << XCHAL_EXTINT31_NUM)
#define XCHAL_EXTINT31_LEVEL		XCHAL_INT_LEVEL(XCHAL_EXTINT31_NUM)


/*----------------------------------------------------------------------
			EXCEPTIONS and VECTORS
  ----------------------------------------------------------------------*/

/*  For backward compatibility ONLY -- DO NOT USE (will be removed in future release):  */
#define XCHAL_HAVE_OLD_EXC_ARCH		XCHAL_HAVE_XEA1	/* (DEPRECATED) 1 if old exception architecture (XEA1), 0 otherwise (eg. XEA2) */
#define XCHAL_HAVE_EXCM			XCHAL_HAVE_XEA2	/* (DEPRECATED) 1 if PS.EXCM bit exists (currently equals XCHAL_HAVE_TLBS) */
#ifdef XCHAL_USER_VECTOR_VADDR
#define XCHAL_PROGRAMEXC_VECTOR_VADDR	XCHAL_USER_VECTOR_VADDR
#define XCHAL_USEREXC_VECTOR_VADDR	XCHAL_USER_VECTOR_VADDR
#endif
#ifdef XCHAL_USER_VECTOR_PADDR
# define XCHAL_PROGRAMEXC_VECTOR_PADDR	XCHAL_USER_VECTOR_PADDR
# define XCHAL_USEREXC_VECTOR_PADDR	XCHAL_USER_VECTOR_PADDR
#endif
#ifdef XCHAL_KERNEL_VECTOR_VADDR
# define XCHAL_STACKEDEXC_VECTOR_VADDR	XCHAL_KERNEL_VECTOR_VADDR
# define XCHAL_KERNELEXC_VECTOR_VADDR	XCHAL_KERNEL_VECTOR_VADDR
#endif
#ifdef XCHAL_KERNEL_VECTOR_PADDR
# define XCHAL_STACKEDEXC_VECTOR_PADDR	XCHAL_KERNEL_VECTOR_PADDR
# define XCHAL_KERNELEXC_VECTOR_PADDR	XCHAL_KERNEL_VECTOR_PADDR
#endif

#if 0
#if XCHAL_HAVE_DEBUG
# define XCHAL_DEBUG_VECTOR_VADDR	XCHAL_INTLEVEL_VECTOR_VADDR(XCHAL_DEBUGLEVEL)
/*  This one should only get defined if the corresponding intlevel paddr macro exists:  */
# define XCHAL_DEBUG_VECTOR_PADDR	XCHAL_INTLEVEL_VECTOR_PADDR(XCHAL_DEBUGLEVEL)
#endif
#endif

/*  Indexing macros:  */
#define _XCHAL_INTLEVEL_VECTOR_VADDR(n)		XCHAL_INTLEVEL ## n ## _VECTOR_VADDR
#define XCHAL_INTLEVEL_VECTOR_VADDR(n)		_XCHAL_INTLEVEL_VECTOR_VADDR(n)		/* n = 0 .. 15 */

/*
 *  General Exception Causes
 *  (values of EXCCAUSE special register set by general exceptions,
 *   which vector to the user, kernel, or double-exception vectors).
 *
 *  DEPRECATED.  Please use the equivalent EXCCAUSE_xxx macros
 *  defined in <xtensa/corebits.h>.  (Note that these have slightly
 *  different names, they don't just have the XCHAL_ prefix removed.)
 */
#define XCHAL_EXCCAUSE_ILLEGAL_INSTRUCTION		0	/* Illegal Instruction */
#define XCHAL_EXCCAUSE_SYSTEM_CALL			1	/* System Call */
#define XCHAL_EXCCAUSE_INSTRUCTION_FETCH_ERROR		2	/* Instruction Fetch Error */
#define XCHAL_EXCCAUSE_LOAD_STORE_ERROR			3	/* Load Store Error */
#define XCHAL_EXCCAUSE_LEVEL1_INTERRUPT			4	/* Level 1 Interrupt */
#define XCHAL_EXCCAUSE_ALLOCA				5	/* Stack Extension Assist */
#define XCHAL_EXCCAUSE_INTEGER_DIVIDE_BY_ZERO		6	/* Integer Divide by Zero */
#define XCHAL_EXCCAUSE_SPECULATION			7	/* Speculation */
#define XCHAL_EXCCAUSE_PRIVILEGED			8	/* Privileged Instruction */
#define XCHAL_EXCCAUSE_UNALIGNED			9	/* Unaligned Load Store */
/*10..15 reserved*/
#define XCHAL_EXCCAUSE_ITLB_MISS			16	/* ITlb Miss Exception */
#define XCHAL_EXCCAUSE_ITLB_MULTIHIT			17	/* ITlb Mutltihit Exception */
#define XCHAL_EXCCAUSE_ITLB_PRIVILEGE			18	/* ITlb Privilege Exception */
#define XCHAL_EXCCAUSE_ITLB_SIZE_RESTRICTION		19	/* ITlb Size Restriction Exception */
#define XCHAL_EXCCAUSE_FETCH_CACHE_ATTRIBUTE		20	/* Fetch Cache Attribute Exception */
/*21..23 reserved*/
#define XCHAL_EXCCAUSE_DTLB_MISS			24	/* DTlb Miss Exception */
#define XCHAL_EXCCAUSE_DTLB_MULTIHIT			25	/* DTlb Multihit Exception */
#define XCHAL_EXCCAUSE_DTLB_PRIVILEGE			26	/* DTlb Privilege Exception */
#define XCHAL_EXCCAUSE_DTLB_SIZE_RESTRICTION		27	/* DTlb Size Restriction Exception */
#define XCHAL_EXCCAUSE_LOAD_CACHE_ATTRIBUTE		28	/* Load Cache Attribute Exception */
#define XCHAL_EXCCAUSE_STORE_CACHE_ATTRIBUTE		29	/* Store Cache Attribute Exception */
/*30..31 reserved*/
#define XCHAL_EXCCAUSE_COPROCESSOR0_DISABLED		32	/* Coprocessor 0 disabled */
#define XCHAL_EXCCAUSE_COPROCESSOR1_DISABLED		33	/* Coprocessor 1 disabled */
#define XCHAL_EXCCAUSE_COPROCESSOR2_DISABLED		34	/* Coprocessor 2 disabled */
#define XCHAL_EXCCAUSE_COPROCESSOR3_DISABLED		35	/* Coprocessor 3 disabled */
#define XCHAL_EXCCAUSE_COPROCESSOR4_DISABLED		36	/* Coprocessor 4 disabled */
#define XCHAL_EXCCAUSE_COPROCESSOR5_DISABLED		37	/* Coprocessor 5 disabled */
#define XCHAL_EXCCAUSE_COPROCESSOR6_DISABLED		38	/* Coprocessor 6 disabled */
#define XCHAL_EXCCAUSE_COPROCESSOR7_DISABLED		39	/* Coprocessor 7 disabled */
/*40..63 reserved*/


/*
 *  Miscellaneous special register fields.
 *
 *  For each special register, and each field within each register:
 *	XCHAL_<regname>_VALIDMASK is the set of bits defined in the register.
 *	XCHAL_<regname>_<field>_BITS is the number of bits in the field.
 *	XCHAL_<regname>_<field>_NUM is 2^bits, the number of possible values
 *			of the field.
 *	XCHAL_<regname>_<field>_SHIFT is the position of the field within
 *			the register, starting from the least significant bit.
 *
 *  DEPRECATED.  Please use the equivalent macros defined in
 *  <xtensa/corebits.h>.  (Note that these have different names.)
 */

/*  DBREAKC (special register number 160):  */
#define XCHAL_DBREAKC_VALIDMASK		0xC000003F
#define XCHAL_DBREAKC_MASK_BITS 	6
#define XCHAL_DBREAKC_MASK_NUM  	64
#define XCHAL_DBREAKC_MASK_SHIFT	0
#define XCHAL_DBREAKC_MASK_MASK 	0x0000003F
#define XCHAL_DBREAKC_LOADBREAK_BITS 	1
#define XCHAL_DBREAKC_LOADBREAK_NUM  	2
#define XCHAL_DBREAKC_LOADBREAK_SHIFT	30
#define XCHAL_DBREAKC_LOADBREAK_MASK 	0x40000000
#define XCHAL_DBREAKC_STOREBREAK_BITS 	1
#define XCHAL_DBREAKC_STOREBREAK_NUM  	2
#define XCHAL_DBREAKC_STOREBREAK_SHIFT	31
#define XCHAL_DBREAKC_STOREBREAK_MASK 	0x80000000
/*  PS (special register number 230):  */
#define XCHAL_PS_VALIDMASK		0x00070F3F
#define XCHAL_PS_INTLEVEL_BITS 		4
#define XCHAL_PS_INTLEVEL_NUM  		16
#define XCHAL_PS_INTLEVEL_SHIFT		0
#define XCHAL_PS_INTLEVEL_MASK 		0x0000000F
#define XCHAL_PS_EXCM_BITS 		1
#define XCHAL_PS_EXCM_NUM  		2
#define XCHAL_PS_EXCM_SHIFT		4
#define XCHAL_PS_EXCM_MASK 		0x00000010
#define XCHAL_PS_UM_BITS 		1
#define XCHAL_PS_UM_NUM  		2
#define XCHAL_PS_UM_SHIFT		5
#define XCHAL_PS_UM_MASK 		0x00000020
#define XCHAL_PS_RING_BITS 		2
#define XCHAL_PS_RING_NUM  		4
#define XCHAL_PS_RING_SHIFT		6
#define XCHAL_PS_RING_MASK 		0x000000C0
#define XCHAL_PS_OWB_BITS 		4
#define XCHAL_PS_OWB_NUM  		16
#define XCHAL_PS_OWB_SHIFT		8
#define XCHAL_PS_OWB_MASK 		0x00000F00
#define XCHAL_PS_CALLINC_BITS 		2
#define XCHAL_PS_CALLINC_NUM  		4
#define XCHAL_PS_CALLINC_SHIFT		16
#define XCHAL_PS_CALLINC_MASK 		0x00030000
#define XCHAL_PS_WOE_BITS 		1
#define XCHAL_PS_WOE_NUM  		2
#define XCHAL_PS_WOE_SHIFT		18
#define XCHAL_PS_WOE_MASK 		0x00040000
/*  EXCCAUSE (special register number 232):  */
#define XCHAL_EXCCAUSE_VALIDMASK	0x0000003F
#define XCHAL_EXCCAUSE_BITS 		6
#define XCHAL_EXCCAUSE_NUM  		64
#define XCHAL_EXCCAUSE_SHIFT		0
#define XCHAL_EXCCAUSE_MASK 		0x0000003F
/*  DEBUGCAUSE (special register number 233):  */
#define XCHAL_DEBUGCAUSE_VALIDMASK	0x0000003F
#define XCHAL_DEBUGCAUSE_ICOUNT_BITS 	1
#define XCHAL_DEBUGCAUSE_ICOUNT_NUM  	2
#define XCHAL_DEBUGCAUSE_ICOUNT_SHIFT	0
#define XCHAL_DEBUGCAUSE_ICOUNT_MASK 	0x00000001
#define XCHAL_DEBUGCAUSE_IBREAK_BITS 	1
#define XCHAL_DEBUGCAUSE_IBREAK_NUM  	2
#define XCHAL_DEBUGCAUSE_IBREAK_SHIFT	1
#define XCHAL_DEBUGCAUSE_IBREAK_MASK 	0x00000002
#define XCHAL_DEBUGCAUSE_DBREAK_BITS 	1
#define XCHAL_DEBUGCAUSE_DBREAK_NUM  	2
#define XCHAL_DEBUGCAUSE_DBREAK_SHIFT	2
#define XCHAL_DEBUGCAUSE_DBREAK_MASK 	0x00000004
#define XCHAL_DEBUGCAUSE_BREAK_BITS 	1
#define XCHAL_DEBUGCAUSE_BREAK_NUM  	2
#define XCHAL_DEBUGCAUSE_BREAK_SHIFT	3
#define XCHAL_DEBUGCAUSE_BREAK_MASK 	0x00000008
#define XCHAL_DEBUGCAUSE_BREAKN_BITS 	1
#define XCHAL_DEBUGCAUSE_BREAKN_NUM  	2
#define XCHAL_DEBUGCAUSE_BREAKN_SHIFT	4
#define XCHAL_DEBUGCAUSE_BREAKN_MASK 	0x00000010
#define XCHAL_DEBUGCAUSE_DEBUGINT_BITS 	1
#define XCHAL_DEBUGCAUSE_DEBUGINT_NUM  	2
#define XCHAL_DEBUGCAUSE_DEBUGINT_SHIFT	5
#define XCHAL_DEBUGCAUSE_DEBUGINT_MASK 	0x00000020




/*----------------------------------------------------------------------
				TIMERS
  ----------------------------------------------------------------------*/

/*#define XCHAL_HAVE_TIMERS		XCHAL_HAVE_CCOUNT*/



/*----------------------------------------------------------------------
			INTERNAL I/D RAM/ROMs and XLMI
  ----------------------------------------------------------------------*/

#define  XCHAL_NUM_IROM		XCHAL_NUM_INSTROM	/* (DEPRECATED) */
#define  XCHAL_NUM_IRAM		XCHAL_NUM_INSTRAM	/* (DEPRECATED) */
#define  XCHAL_NUM_DROM		XCHAL_NUM_DATAROM	/* (DEPRECATED) */
#define  XCHAL_NUM_DRAM		XCHAL_NUM_DATARAM	/* (DEPRECATED) */

#define XCHAL_IROM0_VADDR	XCHAL_INSTROM0_VADDR	/* (DEPRECATED) */
#define XCHAL_IROM0_PADDR	XCHAL_INSTROM0_PADDR	/* (DEPRECATED) */
#define XCHAL_IROM0_SIZE	XCHAL_INSTROM0_SIZE	/* (DEPRECATED) */
#define XCHAL_IROM1_VADDR	XCHAL_INSTROM1_VADDR	/* (DEPRECATED) */
#define XCHAL_IROM1_PADDR	XCHAL_INSTROM1_PADDR	/* (DEPRECATED) */
#define XCHAL_IROM1_SIZE	XCHAL_INSTROM1_SIZE	/* (DEPRECATED) */
#define XCHAL_IRAM0_VADDR	XCHAL_INSTRAM0_VADDR	/* (DEPRECATED) */
#define XCHAL_IRAM0_PADDR	XCHAL_INSTRAM0_PADDR	/* (DEPRECATED) */
#define XCHAL_IRAM0_SIZE	XCHAL_INSTRAM0_SIZE	/* (DEPRECATED) */
#define XCHAL_IRAM1_VADDR	XCHAL_INSTRAM1_VADDR	/* (DEPRECATED) */
#define XCHAL_IRAM1_PADDR	XCHAL_INSTRAM1_PADDR	/* (DEPRECATED) */
#define XCHAL_IRAM1_SIZE	XCHAL_INSTRAM1_SIZE	/* (DEPRECATED) */
#define XCHAL_DROM0_VADDR	XCHAL_DATAROM0_VADDR	/* (DEPRECATED) */
#define XCHAL_DROM0_PADDR	XCHAL_DATAROM0_PADDR	/* (DEPRECATED) */
#define XCHAL_DROM0_SIZE	XCHAL_DATAROM0_SIZE	/* (DEPRECATED) */
#define XCHAL_DROM1_VADDR	XCHAL_DATAROM1_VADDR	/* (DEPRECATED) */
#define XCHAL_DROM1_PADDR	XCHAL_DATAROM1_PADDR	/* (DEPRECATED) */
#define XCHAL_DROM1_SIZE	XCHAL_DATAROM1_SIZE	/* (DEPRECATED) */
#define XCHAL_DRAM0_VADDR	XCHAL_DATARAM0_VADDR	/* (DEPRECATED) */
#define XCHAL_DRAM0_PADDR	XCHAL_DATARAM0_PADDR	/* (DEPRECATED) */
#define XCHAL_DRAM0_SIZE	XCHAL_DATARAM0_SIZE	/* (DEPRECATED) */
#define XCHAL_DRAM1_VADDR	XCHAL_DATARAM1_VADDR	/* (DEPRECATED) */
#define XCHAL_DRAM1_PADDR	XCHAL_DATARAM1_PADDR	/* (DEPRECATED) */
#define XCHAL_DRAM1_SIZE	XCHAL_DATARAM1_SIZE	/* (DEPRECATED) */



/*----------------------------------------------------------------------
				CACHE
  ----------------------------------------------------------------------*/


/*  Default PREFCTL value to enable prefetch.  */
#if XCHAL_HW_MIN_VERSION < XTENSA_HWVERSION_RE_2012_0
#define XCHAL_CACHE_PREFCTL_DEFAULT	0x00044	/* enabled, not aggressive */
#elif XCHAL_HW_MIN_VERSION < XTENSA_HWVERSION_RF_2014_0
#define XCHAL_CACHE_PREFCTL_DEFAULT	0x01044	/* + enable prefetch to L1 */
#elif XCHAL_PREFETCH_ENTRIES >= 16
#define XCHAL_CACHE_PREFCTL_DEFAULT	0x81044	/* 12 entries for block ops */
#elif XCHAL_PREFETCH_ENTRIES >= 8
#define XCHAL_CACHE_PREFCTL_DEFAULT	0x51044	/* 5 entries for block ops */
#else
#define XCHAL_CACHE_PREFCTL_DEFAULT	0x01044	/* 0 entries for block ops */
#endif


/*  Max for both I-cache and D-cache (used for general alignment):  */
#if XCHAL_ICACHE_LINESIZE > XCHAL_DCACHE_LINESIZE
# define XCHAL_CACHE_LINEWIDTH_MAX	XCHAL_ICACHE_LINEWIDTH
# define XCHAL_CACHE_LINESIZE_MAX	XCHAL_ICACHE_LINESIZE
#else
# define XCHAL_CACHE_LINEWIDTH_MAX	XCHAL_DCACHE_LINEWIDTH
# define XCHAL_CACHE_LINESIZE_MAX	XCHAL_DCACHE_LINESIZE
#endif

#define XCHAL_ICACHE_SETSIZE		(1<<XCHAL_ICACHE_SETWIDTH)
#define XCHAL_DCACHE_SETSIZE		(1<<XCHAL_DCACHE_SETWIDTH)
/*  Max for both I and D caches (used for cache-coherency page alignment):  */
#if XCHAL_ICACHE_SETWIDTH > XCHAL_DCACHE_SETWIDTH
# define XCHAL_CACHE_SETWIDTH_MAX	XCHAL_ICACHE_SETWIDTH
# define XCHAL_CACHE_SETSIZE_MAX	XCHAL_ICACHE_SETSIZE
#else
# define XCHAL_CACHE_SETWIDTH_MAX	XCHAL_DCACHE_SETWIDTH
# define XCHAL_CACHE_SETSIZE_MAX	XCHAL_DCACHE_SETSIZE
#endif

/*  Instruction cache tag bits:  */
#define XCHAL_ICACHE_TAG_V_SHIFT	0
#define XCHAL_ICACHE_TAG_V		0x1	/* valid bit */
#if XCHAL_ICACHE_WAYS > 1
# define XCHAL_ICACHE_TAG_F_SHIFT	1
# define XCHAL_ICACHE_TAG_F		0x2	/* fill (LRU) bit */
#else
# define XCHAL_ICACHE_TAG_F_SHIFT	0
# define XCHAL_ICACHE_TAG_F		0	/* no fill (LRU) bit */
#endif
#if XCHAL_ICACHE_LINE_LOCKABLE
# define XCHAL_ICACHE_TAG_L_SHIFT	(XCHAL_ICACHE_TAG_F_SHIFT+1)
# define XCHAL_ICACHE_TAG_L		(1 << XCHAL_ICACHE_TAG_L_SHIFT)	/* lock bit */
#else
# define XCHAL_ICACHE_TAG_L_SHIFT	XCHAL_ICACHE_TAG_F_SHIFT
# define XCHAL_ICACHE_TAG_L		0	/* no lock bit */
#endif
/*  Data cache tag bits:  */
#define XCHAL_DCACHE_TAG_V_SHIFT	0
#define XCHAL_DCACHE_TAG_V		0x1	/* valid bit */
#if XCHAL_DCACHE_WAYS > 1
# define XCHAL_DCACHE_TAG_F_SHIFT	1
# define XCHAL_DCACHE_TAG_F		0x2	/* fill (LRU) bit */
#else
# define XCHAL_DCACHE_TAG_F_SHIFT	0
# define XCHAL_DCACHE_TAG_F		0	/* no fill (LRU) bit */
#endif
#if XCHAL_DCACHE_IS_WRITEBACK
# define XCHAL_DCACHE_TAG_D_SHIFT	(XCHAL_DCACHE_TAG_F_SHIFT+1)
# define XCHAL_DCACHE_TAG_D		(1 << XCHAL_DCACHE_TAG_D_SHIFT)	/* dirty bit */
#else
# define XCHAL_DCACHE_TAG_D_SHIFT	XCHAL_DCACHE_TAG_F_SHIFT
# define XCHAL_DCACHE_TAG_D		0	/* no dirty bit */
#endif
#if XCHAL_DCACHE_LINE_LOCKABLE
# define XCHAL_DCACHE_TAG_L_SHIFT	(XCHAL_DCACHE_TAG_D_SHIFT+1)
# define XCHAL_DCACHE_TAG_L		(1 << XCHAL_DCACHE_TAG_L_SHIFT)	/* lock bit */
#else
# define XCHAL_DCACHE_TAG_L_SHIFT	XCHAL_DCACHE_TAG_D_SHIFT
# define XCHAL_DCACHE_TAG_L		0	/* no lock bit */
#endif

/*  Whether MEMCTL register has anything useful  */
#define XCHAL_USE_MEMCTL		(((XCHAL_LOOP_BUFFER_SIZE > 0)	||      \
					XCHAL_DCACHE_IS_COHERENT	||      \
					XCHAL_HAVE_ICACHE_DYN_WAYS	||      \
					XCHAL_HAVE_DCACHE_DYN_WAYS)	&&      \
					(XCHAL_HW_MIN_VERSION >= XTENSA_HWVERSION_RE_2012_0))

/*  Default MEMCTL values:  */
#if XCHAL_HAVE_ICACHE_DYN_WAYS || XCHAL_HAVE_DCACHE_DYN_WAYS
/* NOTE: constant defined this way to allow movi instead of l32r in reset code. */
#define XCHAL_CACHE_MEMCTL_DEFAULT	0xFFFFFF00	/* Init all possible ways */
#else
#define XCHAL_CACHE_MEMCTL_DEFAULT	0x00000000	/* Nothing to do */
#endif

#if XCHAL_DCACHE_IS_COHERENT
#define _MEMCTL_SNOOP_EN		0x02		/* Enable snoop */
#else
#define _MEMCTL_SNOOP_EN		0x00		/* Don't enable snoop */
#endif

#if (XCHAL_LOOP_BUFFER_SIZE == 0) || XCHAL_ERRATUM_453
#define _MEMCTL_L0IBUF_EN		0x00		/* No loop buffer or don't enable */
#else
#define _MEMCTL_L0IBUF_EN		0x01		/* Enable loop buffer */
#endif

#define XCHAL_SNOOP_LB_MEMCTL_DEFAULT	(_MEMCTL_SNOOP_EN | _MEMCTL_L0IBUF_EN)


/*----------------------------------------------------------------------
				MMU
  ----------------------------------------------------------------------*/

/*  See <xtensa/config/core-matmap.h> for more details.  */

/*  Has different semantic in open source headers (where it means HAVE_PTP_MMU),
    so comment out starting with RB-2008.3 release; later, might get
    get reintroduced as a synonym for XCHAL_HAVE_PTP_MMU instead:  */
/*#define XCHAL_HAVE_MMU		XCHAL_HAVE_TLBS*/	/* (DEPRECATED; use XCHAL_HAVE_TLBS instead) */

/*  Indexing macros:  */
#define _XCHAL_ITLB_SET(n,_what)	XCHAL_ITLB_SET ## n ## _what
#define XCHAL_ITLB_SET(n,what)		_XCHAL_ITLB_SET(n, _ ## what )
#define _XCHAL_ITLB_SET_E(n,i,_what)	XCHAL_ITLB_SET ## n ## _E ## i ## _what
#define XCHAL_ITLB_SET_E(n,i,what)	_XCHAL_ITLB_SET_E(n,i, _ ## what )
#define _XCHAL_DTLB_SET(n,_what)	XCHAL_DTLB_SET ## n ## _what
#define XCHAL_DTLB_SET(n,what)		_XCHAL_DTLB_SET(n, _ ## what )
#define _XCHAL_DTLB_SET_E(n,i,_what)	XCHAL_DTLB_SET ## n ## _E ## i ## _what
#define XCHAL_DTLB_SET_E(n,i,what)	_XCHAL_DTLB_SET_E(n,i, _ ## what )
/*
 *  Example use:  XCHAL_ITLB_SET(XCHAL_ITLB_ARF_SET0,ENTRIES)
 *	to get the value of XCHAL_ITLB_SET<n>_ENTRIES where <n> is the first auto-refill set.
 */

/*  Number of entries per autorefill way:  */
#define XCHAL_ITLB_ARF_ENTRIES		(1<<XCHAL_ITLB_ARF_ENTRIES_LOG2)
#define XCHAL_DTLB_ARF_ENTRIES		(1<<XCHAL_DTLB_ARF_ENTRIES_LOG2)

/*
 *  Determine whether we have a full MMU (with Page Table and Protection)
 *  usable for an MMU-based OS:
 */
#if 0
#if XCHAL_HAVE_TLBS && !XCHAL_HAVE_SPANNING_WAY && XCHAL_ITLB_ARF_WAYS > 0 && XCHAL_DTLB_ARF_WAYS > 0 && XCHAL_MMU_RINGS >= 2
# define XCHAL_HAVE_PTP_MMU		1	/* have full MMU (with page table [autorefill] and protection) */
#else
# define XCHAL_HAVE_PTP_MMU		0	/* don't have full MMU */
#endif
#endif

/*
 *  For full MMUs, report kernel RAM segment and kernel I/O segment static page mappings:
 */
#if XCHAL_HAVE_PTP_MMU && !XCHAL_HAVE_SPANNING_WAY
#define XCHAL_KSEG_CACHED_VADDR		0xD0000000	/* virt.addr of kernel RAM cached static map */
#define XCHAL_KSEG_CACHED_PADDR		0x00000000	/* phys.addr of kseg_cached */
#define XCHAL_KSEG_CACHED_SIZE		0x08000000	/* size in bytes of kseg_cached (assumed power of 2!!!) */
#define XCHAL_KSEG_BYPASS_VADDR		0xD8000000	/* virt.addr of kernel RAM bypass (uncached) static map */
#define XCHAL_KSEG_BYPASS_PADDR		0x00000000	/* phys.addr of kseg_bypass */
#define XCHAL_KSEG_BYPASS_SIZE		0x08000000	/* size in bytes of kseg_bypass (assumed power of 2!!!) */

#define XCHAL_KIO_CACHED_VADDR		0xE0000000	/* virt.addr of kernel I/O cached static map */
#define XCHAL_KIO_CACHED_PADDR		0xF0000000	/* phys.addr of kio_cached */
#define XCHAL_KIO_CACHED_SIZE		0x10000000	/* size in bytes of kio_cached (assumed power of 2!!!) */
#define XCHAL_KIO_BYPASS_VADDR		0xF0000000	/* virt.addr of kernel I/O bypass (uncached) static map */
#define XCHAL_KIO_BYPASS_PADDR		0xF0000000	/* phys.addr of kio_bypass */
#define XCHAL_KIO_BYPASS_SIZE		0x10000000	/* size in bytes of kio_bypass (assumed power of 2!!!) */

#define XCHAL_SEG_MAPPABLE_VADDR	0x00000000	/* start of largest non-static-mapped virtual addr area */
#define XCHAL_SEG_MAPPABLE_SIZE		0xD0000000	/* size in bytes of  "  */
/* define XCHAL_SEG_MAPPABLE2_xxx if more areas present, sorted in order of descending size.  */
#endif


/*----------------------------------------------------------------------
				MISC
  ----------------------------------------------------------------------*/

/*  Data alignment required if used for instructions:  */
#if XCHAL_INST_FETCH_WIDTH > XCHAL_DATA_WIDTH
# define XCHAL_ALIGN_MAX		XCHAL_INST_FETCH_WIDTH
#else
# define XCHAL_ALIGN_MAX		XCHAL_DATA_WIDTH
#endif

/*
 *  Names kept for backward compatibility.
 *  (Here "RELEASE" is now a misnomer; these are product *versions*, not the releases
 *   under which they are released.  In the T10##.# era there was no distinction.)
 */
#define XCHAL_HW_RELEASE_MAJOR		XCHAL_HW_VERSION_MAJOR
#define XCHAL_HW_RELEASE_MINOR		XCHAL_HW_VERSION_MINOR
#define XCHAL_HW_RELEASE_NAME		XCHAL_HW_VERSION_NAME




/*----------------------------------------------------------------------
			COPROCESSORS and EXTRA STATE
  ----------------------------------------------------------------------*/

#define XCHAL_EXTRA_SA_SIZE		XCHAL_NCP_SA_SIZE
#define XCHAL_EXTRA_SA_ALIGN		XCHAL_NCP_SA_ALIGN
#define XCHAL_CPEXTRA_SA_SIZE		XCHAL_TOTAL_SA_SIZE
#define XCHAL_CPEXTRA_SA_ALIGN		XCHAL_TOTAL_SA_ALIGN

#if defined (_ASMLANGUAGE) || defined (__ASSEMBLER__)

	/*  Invoked at start of save area load/store sequence macro to setup macro
	 *  internal offsets.  Not usually invoked directly.
	 *	continue	0 for 1st sequence, 1 for subsequent consecutive ones.
	 *	totofs		offset from original ptr to next load/store location.
	 */
	.macro	xchal_sa_start	continue totofs
	.ifeq \continue
	 .set	.Lxchal_pofs_, 0	/* offset from original ptr to current \ptr */
	 .set	.Lxchal_ofs_, 0		/* offset from current \ptr to next load/store location */
	.endif
	.if \totofs + 1			/* if totofs specified (not -1) */
	 .set	.Lxchal_ofs_, \totofs - .Lxchal_pofs_	/* specific offset from original ptr */
	.endif
	.endm

	/*  Align portion of save area and bring ptr in range if necessary.
	 *  Used by save area load/store sequences.  Not usually invoked directly.
	 *  Allows combining multiple (sub-)sequences arbitrarily.
	 *	ptr		pointer to save area (may be off, see .Lxchal_pofs_)
	 *	minofs,maxofs	range of offset from cur ptr to next load/store loc;
	 *			minofs <= 0 <= maxofs  (0 must always be valid offset)
	 *			range must be within +/- 30kB or so.
	 *	ofsalign	alignment granularity of minofs .. maxofs (pow of 2)
	 *			(restriction on offset from ptr to next load/store loc)
	 *	totalign	align from orig ptr to next load/store loc (pow of 2)
	 */
	.macro	xchal_sa_align	ptr minofs maxofs ofsalign totalign
	/*  First align where we start accessing the next register
	 *  per \totalign relative to original ptr (i.e. start of the save area):
	 */
	.set	.Lxchal_ofs_, ((.Lxchal_pofs_ + .Lxchal_ofs_ + \totalign - 1) & -\totalign) - .Lxchal_pofs_
	/*  If necessary, adjust \ptr to bring .Lxchal_ofs_ in acceptable range:  */
	.if (((\maxofs) - .Lxchal_ofs_) & 0xC0000000) | ((.Lxchal_ofs_ - (\minofs)) & 0xC0000000) | (.Lxchal_ofs_ & (\ofsalign-1))
	 .set	.Ligmask, 0xFFFFFFFF	/* TODO: optimize to addmi, per aligns and .Lxchal_ofs_ */
	 addi	\ptr, \ptr, (.Lxchal_ofs_ & .Ligmask)
	 .set	.Lxchal_pofs_, .Lxchal_pofs_ + (.Lxchal_ofs_ & .Ligmask)
	 .set	.Lxchal_ofs_, (.Lxchal_ofs_ & ~.Ligmask)
	.endif
	.endm
	/*
	 *  We could optimize for addi to expand to only addmi instead of
	 *  "addmi;addi", where possible.  Here's a partial example how:
	 * .set	.Lmaxmask, -(\ofsalign) & -(\totalign)
	 * .if (((\maxofs) + ~.Lmaxmask + 1) & 0xFFFFFF00) && ((.Lxchal_ofs_ & ~.Lmaxmask) == 0)
	 *  .set	.Ligmask, 0xFFFFFF00
	 * .elif ... ditto for negative ofs range ...
	 *  .set .Ligmask, 0xFFFFFF00
	 *  .set ... adjust per offset ...
	 * .else
	 *  .set .Ligmask, 0xFFFFFFFF
	 * .endif
	 */

	/*  Invoke this after xchal_XXX_{load,store} macros to restore \ptr.  */
	.macro	xchal_sa_ptr_restore	ptr
	.if .Lxchal_pofs_
	 addi	\ptr, \ptr, - .Lxchal_pofs_
	 .set	.Lxchal_ofs_, .Lxchal_ofs_ + .Lxchal_pofs_
	 .set	.Lxchal_pofs_, 0
	.endif
	.endm

	/*
	 *  Use as eg:
	 *	xchal_atmps_store a1, SOMEOFS, XCHAL_SA_NUM_ATMPS, a4, a5
	 *	xchal_ncp_load a2, a0,a3,a4,a5
	 *	xchal_atmps_load  a1, SOMEOFS, XCHAL_SA_NUM_ATMPS, a4, a5
	 *
	 *  Specify only the ARs you *haven't* saved/restored already, up to 4.
	 *  They *must* be the *last* ARs (in same order) specified to save area
	 *  load/store sequences.  In the example above, a0 and a3 were already
	 *  saved/restored and unused (thus available) but a4 and a5 were not.
	 */
#define xchal_atmps_store	xchal_atmps_loadstore s32i,
#define xchal_atmps_load	xchal_atmps_loadstore l32i,
	.macro	xchal_atmps_loadstore	inst ptr offset nreq aa=0 ab=0 ac=0 ad=0
	.set	.Lnsaved_, 0
	.irp	reg,\aa,\ab,\ac,\ad
	 .ifeq 0x\reg ; .set .Lnsaved_,.Lnsaved_+1 ; .endif
	.endr
	.set	.Laofs_, 0
	.irp	reg,\aa,\ab,\ac,\ad
	 .ifgt (\nreq)-.Lnsaved_
	  \inst	\reg, \ptr, .Laofs_+\offset
	  .set	.Laofs_,.Laofs_+4
	  .set	.Lnsaved_,.Lnsaved_+1
	 .endif
	.endr
	.endm

/*#define xchal_ncp_load_a2	xchal_ncp_load	a2,a3,a4,a5,a6*/
/*#define xchal_ncp_store_a2	xchal_ncp_store	a2,a3,a4,a5,a6*/
#define xchal_extratie_load		xchal_ncptie_load
#define xchal_extratie_store		xchal_ncptie_store
#define xchal_extratie_load_a2		xchal_ncptie_load  a2,a3,a4,a5,a6
#define xchal_extratie_store_a2		xchal_ncptie_store a2,a3,a4,a5,a6
#define xchal_extra_load		xchal_ncp_load
#define xchal_extra_store		xchal_ncp_store
#define xchal_extra_load_a2		xchal_ncp_load  a2,a3,a4,a5,a6
#define xchal_extra_store_a2		xchal_ncp_store a2,a3,a4,a5,a6
#define xchal_extra_load_funcbody	xchal_ncp_load  a2,a3,a4,a5,a6
#define xchal_extra_store_funcbody	xchal_ncp_store a2,a3,a4,a5,a6
#define xchal_cp0_store_a2		xchal_cp0_store  a2,a3,a4,a5,a6
#define xchal_cp0_load_a2		xchal_cp0_load   a2,a3,a4,a5,a6
#define xchal_cp1_store_a2		xchal_cp1_store  a2,a3,a4,a5,a6
#define xchal_cp1_load_a2		xchal_cp1_load   a2,a3,a4,a5,a6
#define xchal_cp2_store_a2		xchal_cp2_store  a2,a3,a4,a5,a6
#define xchal_cp2_load_a2		xchal_cp2_load   a2,a3,a4,a5,a6
#define xchal_cp3_store_a2		xchal_cp3_store  a2,a3,a4,a5,a6
#define xchal_cp3_load_a2		xchal_cp3_load   a2,a3,a4,a5,a6
#define xchal_cp4_store_a2		xchal_cp4_store  a2,a3,a4,a5,a6
#define xchal_cp4_load_a2		xchal_cp4_load   a2,a3,a4,a5,a6
#define xchal_cp5_store_a2		xchal_cp5_store  a2,a3,a4,a5,a6
#define xchal_cp5_load_a2		xchal_cp5_load   a2,a3,a4,a5,a6
#define xchal_cp6_store_a2		xchal_cp6_store  a2,a3,a4,a5,a6
#define xchal_cp6_load_a2		xchal_cp6_load   a2,a3,a4,a5,a6
#define xchal_cp7_store_a2		xchal_cp7_store  a2,a3,a4,a5,a6
#define xchal_cp7_load_a2		xchal_cp7_load   a2,a3,a4,a5,a6

/*  Empty placeholder macros for undefined coprocessors:  */
#if (XCHAL_CP_MASK & ~XCHAL_CP_PORT_MASK) == 0
# if XCHAL_CP0_SA_SIZE == 0
	.macro xchal_cp0_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp0_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
# endif
# if XCHAL_CP1_SA_SIZE == 0
	.macro xchal_cp1_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp1_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
# endif
# if XCHAL_CP2_SA_SIZE == 0
	.macro xchal_cp2_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp2_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
# endif
# if XCHAL_CP3_SA_SIZE == 0
	.macro xchal_cp3_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp3_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
# endif
# if XCHAL_CP4_SA_SIZE == 0
	.macro xchal_cp4_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp4_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
# endif
# if XCHAL_CP5_SA_SIZE == 0
	.macro xchal_cp5_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp5_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
# endif
# if XCHAL_CP6_SA_SIZE == 0
	.macro xchal_cp6_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp6_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
# endif
# if XCHAL_CP7_SA_SIZE == 0
	.macro xchal_cp7_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp7_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
# endif
#endif

	/********************
	 *  Macros to create functions that save and restore the state of *any* TIE
	 *  coprocessor (by dynamic index).
	 */

	/*
	 *  Macro that expands to the body of a function
	 *  that stores the selected coprocessor's state (registers etc).
	 *	Entry:	a2 = ptr to save area in which to save cp state
	 *		a3 = coprocessor number
	 *	Exit:	any register a2-a15 (?) may have been clobbered.
	 */
	.macro	xchal_cpi_store_funcbody
#if (XCHAL_CP_MASK & ~XCHAL_CP_PORT_MASK)
# if XCHAL_CP0_SA_SIZE
	bnez	a3, 99f
	xchal_cp0_store_a2
	j	90f
99:
# endif
# if XCHAL_CP1_SA_SIZE
	bnei	a3, 1, 99f
	xchal_cp1_store_a2
	j	90f
99:
# endif
# if XCHAL_CP2_SA_SIZE
	bnei	a3, 2, 99f
	xchal_cp2_store_a2
	j	90f
99:
# endif
# if XCHAL_CP3_SA_SIZE
	bnei	a3, 3, 99f
	xchal_cp3_store_a2
	j	90f
99:
# endif
# if XCHAL_CP4_SA_SIZE
	bnei	a3, 4, 99f
	xchal_cp4_store_a2
	j	90f
99:
# endif
# if XCHAL_CP5_SA_SIZE
	bnei	a3, 5, 99f
	xchal_cp5_store_a2
	j	90f
99:
# endif
# if XCHAL_CP6_SA_SIZE
	bnei	a3, 6, 99f
	xchal_cp6_store_a2
	j	90f
99:
# endif
# if XCHAL_CP7_SA_SIZE
	bnei	a3, 7, 99f
	xchal_cp7_store_a2
	j	90f
99:
# endif
90:
#endif
	.endm

	/*
	 *  Macro that expands to the body of a function
	 *  that loads the selected coprocessor's state (registers etc).
	 *	Entry:	a2 = ptr to save area from which to restore cp state
	 *		a3 = coprocessor number
	 *	Exit:	any register a2-a15 (?) may have been clobbered.
	 */
	.macro	xchal_cpi_load_funcbody
#if (XCHAL_CP_MASK & ~XCHAL_CP_PORT_MASK)
# if XCHAL_CP0_SA_SIZE
	bnez	a3, 99f
	xchal_cp0_load_a2
	j	90f
99:
# endif
# if XCHAL_CP1_SA_SIZE
	bnei	a3, 1, 99f
	xchal_cp1_load_a2
	j	90f
99:
# endif
# if XCHAL_CP2_SA_SIZE
	bnei	a3, 2, 99f
	xchal_cp2_load_a2
	j	90f
99:
# endif
# if XCHAL_CP3_SA_SIZE
	bnei	a3, 3, 99f
	xchal_cp3_load_a2
	j	90f
99:
# endif
# if XCHAL_CP4_SA_SIZE
	bnei	a3, 4, 99f
	xchal_cp4_load_a2
	j	90f
99:
# endif
# if XCHAL_CP5_SA_SIZE
	bnei	a3, 5, 99f
	xchal_cp5_load_a2
	j	90f
99:
# endif
# if XCHAL_CP6_SA_SIZE
	bnei	a3, 6, 99f
	xchal_cp6_load_a2
	j	90f
99:
# endif
# if XCHAL_CP7_SA_SIZE
	bnei	a3, 7, 99f
	xchal_cp7_load_a2
	j	90f
99:
# endif
90:
#endif
	.endm

#endif /*_ASMLANGUAGE or __ASSEMBLER__*/


/*  Other default macros for undefined coprocessors:  */
#ifndef XCHAL_CP0_NAME
# define XCHAL_CP0_NAME				0
# define XCHAL_CP0_SA_CONTENTS_LIBDB_NUM	0
# define XCHAL_CP0_SA_CONTENTS_LIBDB		/* empty */
#endif
#ifndef XCHAL_CP1_NAME
# define XCHAL_CP1_NAME				0
# define XCHAL_CP1_SA_CONTENTS_LIBDB_NUM	0
# define XCHAL_CP1_SA_CONTENTS_LIBDB		/* empty */
#endif
#ifndef XCHAL_CP2_NAME
# define XCHAL_CP2_NAME				0
# define XCHAL_CP2_SA_CONTENTS_LIBDB_NUM	0
# define XCHAL_CP2_SA_CONTENTS_LIBDB		/* empty */
#endif
#ifndef XCHAL_CP3_NAME
# define XCHAL_CP3_NAME				0
# define XCHAL_CP3_SA_CONTENTS_LIBDB_NUM	0
# define XCHAL_CP3_SA_CONTENTS_LIBDB		/* empty */
#endif
#ifndef XCHAL_CP4_NAME
# define XCHAL_CP4_NAME				0
# define XCHAL_CP4_SA_CONTENTS_LIBDB_NUM	0
# define XCHAL_CP4_SA_CONTENTS_LIBDB		/* empty */
#endif
#ifndef XCHAL_CP5_NAME
# define XCHAL_CP5_NAME				0
# define XCHAL_CP5_SA_CONTENTS_LIBDB_NUM	0
# define XCHAL_CP5_SA_CONTENTS_LIBDB		/* empty */
#endif
#ifndef XCHAL_CP6_NAME
# define XCHAL_CP6_NAME				0
# define XCHAL_CP6_SA_CONTENTS_LIBDB_NUM	0
# define XCHAL_CP6_SA_CONTENTS_LIBDB		/* empty */
#endif
#ifndef XCHAL_CP7_NAME
# define XCHAL_CP7_NAME				0
# define XCHAL_CP7_SA_CONTENTS_LIBDB_NUM	0
# define XCHAL_CP7_SA_CONTENTS_LIBDB		/* empty */
#endif

#if XCHAL_CP_MASK == 0
/*  Filler info for unassigned coprocessors, to simplify arrays etc:  */
#define XCHAL_CP0_SA_SIZE               0
#define XCHAL_CP0_SA_ALIGN              1
#define XCHAL_CP1_SA_SIZE               0
#define XCHAL_CP1_SA_ALIGN              1
#define XCHAL_CP2_SA_SIZE               0
#define XCHAL_CP2_SA_ALIGN              1
#define XCHAL_CP3_SA_SIZE               0
#define XCHAL_CP3_SA_ALIGN              1
#define XCHAL_CP4_SA_SIZE               0
#define XCHAL_CP4_SA_ALIGN              1
#define XCHAL_CP5_SA_SIZE               0
#define XCHAL_CP5_SA_ALIGN              1
#define XCHAL_CP6_SA_SIZE               0
#define XCHAL_CP6_SA_ALIGN              1
#define XCHAL_CP7_SA_SIZE               0
#define XCHAL_CP7_SA_ALIGN              1
#endif


/*  Indexing macros:  */
#define _XCHAL_CP_SA_SIZE(n)		XCHAL_CP ## n ## _SA_SIZE
#define XCHAL_CP_SA_SIZE(n)		_XCHAL_CP_SA_SIZE(n)	/* n = 0 .. 7 */
#define _XCHAL_CP_SA_ALIGN(n)		XCHAL_CP ## n ## _SA_ALIGN
#define XCHAL_CP_SA_ALIGN(n)		_XCHAL_CP_SA_ALIGN(n)	/* n = 0 .. 7 */

#define XCHAL_CPEXTRA_SA_SIZE_TOR2      XCHAL_CPEXTRA_SA_SIZE	/* Tor2Beta only - do not use */

/*  Link-time HAL global variables that report coprocessor numbers by name
    (names are case-preserved from the original TIE):  */
#if !defined(_ASMLANGUAGE) && !defined(_NOCLANGUAGE) && !defined(__ASSEMBLER__)
# define _XCJOIN(a,b)	a ## b
# define XCJOIN(a,b)	_XCJOIN(a,b)
# ifdef XCHAL_CP0_NAME
extern const unsigned char XCJOIN(Xthal_cp_id_,XCHAL_CP0_IDENT);
extern const unsigned int  XCJOIN(Xthal_cp_mask_,XCHAL_CP0_IDENT);
# endif
# ifdef XCHAL_CP1_NAME
extern const unsigned char XCJOIN(Xthal_cp_id_,XCHAL_CP1_IDENT);
extern const unsigned int  XCJOIN(Xthal_cp_mask_,XCHAL_CP1_IDENT);
# endif
# ifdef XCHAL_CP2_NAME
extern const unsigned char XCJOIN(Xthal_cp_id_,XCHAL_CP2_IDENT);
extern const unsigned int  XCJOIN(Xthal_cp_mask_,XCHAL_CP2_IDENT);
# endif
# ifdef XCHAL_CP3_NAME
extern const unsigned char XCJOIN(Xthal_cp_id_,XCHAL_CP3_IDENT);
extern const unsigned int  XCJOIN(Xthal_cp_mask_,XCHAL_CP3_IDENT);
# endif
# ifdef XCHAL_CP4_NAME
extern const unsigned char XCJOIN(Xthal_cp_id_,XCHAL_CP4_IDENT);
extern const unsigned int  XCJOIN(Xthal_cp_mask_,XCHAL_CP4_IDENT);
# endif
# ifdef XCHAL_CP5_NAME
extern const unsigned char XCJOIN(Xthal_cp_id_,XCHAL_CP5_IDENT);
extern const unsigned int  XCJOIN(Xthal_cp_mask_,XCHAL_CP5_IDENT);
# endif
# ifdef XCHAL_CP6_NAME
extern const unsigned char XCJOIN(Xthal_cp_id_,XCHAL_CP6_IDENT);
extern const unsigned int  XCJOIN(Xthal_cp_mask_,XCHAL_CP6_IDENT);
# endif
# ifdef XCHAL_CP7_NAME
extern const unsigned char XCJOIN(Xthal_cp_id_,XCHAL_CP7_IDENT);
extern const unsigned int  XCJOIN(Xthal_cp_mask_,XCHAL_CP7_IDENT);
# endif
#endif




/*----------------------------------------------------------------------
				DERIVED
  ----------------------------------------------------------------------*/

#if XCHAL_HAVE_BE
#define XCHAL_INST_ILLN			0xD60F		/* 2-byte illegal instruction, msb-first */
#define XCHAL_INST_ILLN_BYTE0		0xD6		/* 2-byte illegal instruction, 1st byte */
#define XCHAL_INST_ILLN_BYTE1		0x0F		/* 2-byte illegal instruction, 2nd byte */
#else
#define XCHAL_INST_ILLN			0xF06D		/* 2-byte illegal instruction, lsb-first */
#define XCHAL_INST_ILLN_BYTE0		0x6D		/* 2-byte illegal instruction, 1st byte */
#define XCHAL_INST_ILLN_BYTE1		0xF0		/* 2-byte illegal instruction, 2nd byte */
#endif
/*  Belongs in xtensa/hal.h:  */
#define XTHAL_INST_ILL			0x000000	/* 3-byte illegal instruction */


/*
 *  Because information as to exactly which hardware version is targeted
 *  by a given software build is not always available, compile-time HAL
 *  Hardware-Release "_AT" macros are fuzzy (return 0, 1, or XCHAL_MAYBE):
 *  (Here "RELEASE" is now a misnomer; these are product *versions*, not the releases
 *   under which they are released.  In the T10##.# era there was no distinction.)
 */
#if XCHAL_HW_CONFIGID_RELIABLE
# define XCHAL_HW_RELEASE_AT_OR_BELOW(major,minor)	(XTHAL_REL_LE( XCHAL_HW_VERSION_MAJOR,XCHAL_HW_VERSION_MINOR, major,minor ) ? 1 : 0)
# define XCHAL_HW_RELEASE_AT_OR_ABOVE(major,minor)	(XTHAL_REL_GE( XCHAL_HW_VERSION_MAJOR,XCHAL_HW_VERSION_MINOR, major,minor ) ? 1 : 0)
# define XCHAL_HW_RELEASE_AT(major,minor)		(XTHAL_REL_EQ( XCHAL_HW_VERSION_MAJOR,XCHAL_HW_VERSION_MINOR, major,minor ) ? 1 : 0)
# define XCHAL_HW_RELEASE_MAJOR_AT(major)		((XCHAL_HW_VERSION_MAJOR == (major)) ? 1 : 0)
#else
# define XCHAL_HW_RELEASE_AT_OR_BELOW(major,minor)	( ((major) < 1040 && XCHAL_HAVE_XEA2) ? 0 \
							: ((major) > 1050 && XCHAL_HAVE_XEA1) ? 1 \
							: XTHAL_MAYBE )
# define XCHAL_HW_RELEASE_AT_OR_ABOVE(major,minor)	( ((major) >= 2000 && XCHAL_HAVE_XEA1) ? 0 \
							: (XTHAL_REL_LE(major,minor, 1040,0) && XCHAL_HAVE_XEA2) ? 1 \
							: XTHAL_MAYBE )
# define XCHAL_HW_RELEASE_AT(major,minor)		( (((major) < 1040 && XCHAL_HAVE_XEA2) || \
							   ((major) >= 2000 && XCHAL_HAVE_XEA1)) ? 0 : XTHAL_MAYBE)
# define XCHAL_HW_RELEASE_MAJOR_AT(major)		XCHAL_HW_RELEASE_AT(major,0)
#endif

/*
 *  Specific errata:
 */

/*
 *  Erratum T1020.H13, T1030.H7, T1040.H10, T1050.H4 (fixed in T1040.3 and T1050.1;
 *  relevant only in XEA1, kernel-vector mode, level-one interrupts and overflows enabled):
 */
#define XCHAL_MAYHAVE_ERRATUM_XEA1KWIN	(XCHAL_HAVE_XEA1 && \
					 (XCHAL_HW_RELEASE_AT_OR_BELOW(1040,2) != 0 \
					  || XCHAL_HW_RELEASE_AT(1050,0)))
/*
 *  Erratum 453 present in RE-2013.2 up to RF-2014.0, fixed in RF-2014.1.
 *  Applies to specific set of configuration options.
 *  Part of the workaround is to add ISYNC at certain points in the code.
 *  The workaround gated by this macro can be disabled if not needed, e.g. if
 *  zero-overhead loop buffer will be disabled, by defining _NO_ERRATUM_453.
 */
#if (	XCHAL_HW_MAX_VERSION >= XTENSA_HWVERSION_RE_2013_2 && \
	XCHAL_HW_MIN_VERSION <= XTENSA_HWVERSION_RF_2014_0 && \
	XCHAL_ICACHE_SIZE != 0    && XCHAL_HAVE_PIF /*covers also AXI/AHB*/ && \
	XCHAL_HAVE_LOOPS          && XCHAL_LOOP_BUFFER_SIZE != 0 && \
	XCHAL_CLOCK_GATING_GLOBAL && !defined(_NO_ERRATUM_453) )
#define XCHAL_ERRATUM_453	1
#else
#define XCHAL_ERRATUM_453	0
#endif

/*
 *  Erratum 497 present in RE-2012.2 up to RG/RF-2015.2
 *  Applies to specific set of configuration options.
 *  Workaround is to add MEMWs after at most 8 cache WB instructions
 */
#if ( ((XCHAL_HW_MAX_VERSION >= XTENSA_HWVERSION_RE_2012_0 &&    \
        XCHAL_HW_MIN_VERSION <= XTENSA_HWVERSION_RF_2015_2) ||   \
       (XCHAL_HW_MAX_VERSION >= XTENSA_HWVERSION_RG_2015_0 &&    \
        XCHAL_HW_MIN_VERSION <= XTENSA_HWVERSION_RG_2015_2)     \
      ) && \
      XCHAL_DCACHE_IS_WRITEBACK && \
      XCHAL_HAVE_AXI && \
      XCHAL_HAVE_PIF_WR_RESP && \
      XCHAL_HAVE_PIF_REQ_ATTR &&  !defined(_NO_ERRATUM_497) \
    )
#define XCHAL_ERRATUM_497	1
#else
#define XCHAL_ERRATUM_497	0
#endif

/*
 * Erratum 572 (releases TBD, but present in ESP32)
 * Disable zero-overhead loop buffer to prevent rare illegal instruction
 * exceptions while executing zero-overhead loops.
 */
#if ( XCHAL_HAVE_LOOPS && XCHAL_LOOP_BUFFER_SIZE != 0 )
#define XCHAL_ERRATUM_572   1
#else
#define XCHAL_ERRATUM_572   0
#endif

#endif /*XTENSA_CONFIG_CORE_H*/

