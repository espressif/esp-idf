/*
 * Xtensa Special Register symbolic names
 */

/* SPDX-FileCopyrightText: 2005-2011 Tensilica Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 */

/* $Id: //depot/rel/Foxhill/dot.9/Xtensa/OS/include/xtensa/specreg.h#1 $ */

/*
 * Copyright (c) 2005-2011 Tensilica Inc.
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

#ifndef XTENSA_XT_SPECREG_H
#define XTENSA_XT_SPECREG_H

/*  Special registers:  */
#define XT_REG_LBEG		0
#define XT_REG_LEND		1
#define XT_REG_LCOUNT		2
#define XT_REG_SAR		3
#define XT_REG_BR		4
#define XT_REG_LITBASE		5
#define XT_REG_SCOMPARE1	12
#define XT_REG_ACCLO		16
#define XT_REG_ACCHI		17
#define XT_REG_MR_0		32
#define XT_REG_MR_1		33
#define XT_REG_MR_2		34
#define XT_REG_MR_3		35
#define XT_REG_PREFCTL		40
#define XT_REG_WINDOWBASE	72
#define XT_REG_WINDOWSTART	73
#define XT_REG_PTEVADDR	83
#define XT_REG_RASID		90
#define XT_REG_ITLBCFG		91
#define XT_REG_DTLBCFG		92
#define XT_REG_IBREAKENABLE	96
#define XT_REG_MEMCTL		97
#define XT_REG_CACHEATTR	98	/* until T1050, XEA1 */
#define XT_REG_CACHEADRDIS	98	/* LX7+ */
#define XT_REG_ATOMCTL		99
#define XT_REG_DDR		104
#define XT_REG_MECR		110
#define XT_REG_IBREAKA_0	128
#define XT_REG_IBREAKA_1	129
#define XT_REG_DBREAKA_0	144
#define XT_REG_DBREAKA_1	145
#define XT_REG_DBREAKC_0	160
#define XT_REG_DBREAKC_1	161
#define XT_REG_CONFIGID0	176
#define XT_REG_EPC_1		177
#define XT_REG_EPC_2		178
#define XT_REG_EPC_3		179
#define XT_REG_EPC_4		180
#define XT_REG_EPC_5		181
#define XT_REG_EPC_6		182
#define XT_REG_EPC_7		183
#define XT_REG_DEPC		192
#define XT_REG_EPS_2		194
#define XT_REG_EPS_3		195
#define XT_REG_EPS_4		196
#define XT_REG_EPS_5		197
#define XT_REG_EPS_6		198
#define XT_REG_EPS_7		199
#define XT_REG_CONFIGID1	208
#define XT_REG_EXCSAVE_1	209
#define XT_REG_EXCSAVE_2	210
#define XT_REG_EXCSAVE_3	211
#define XT_REG_EXCSAVE_4	212
#define XT_REG_EXCSAVE_5	213
#define XT_REG_EXCSAVE_6	214
#define XT_REG_EXCSAVE_7	215
#define XT_REG_CPENABLE	224
#define XT_REG_INTERRUPT	226
#define XT_REG_INTREAD		INTERRUPT	/* alternate name for backward compatibility */
#define XT_REG_INTSET		INTERRUPT	/* alternate name for backward compatibility */
#define XT_REG_INTCLEAR	227
#define XT_REG_INTENABLE	228
#define XT_REG_PS		230
#define XT_REG_VECBASE		231
#define XT_REG_EXCCAUSE	232
#define XT_REG_DEBUGCAUSE	233
#define XT_REG_CCOUNT		234
#define XT_REG_PRID		235
#define XT_REG_ICOUNT		236
#define XT_REG_ICOUNTLEVEL	237
#define XT_REG_EXCVADDR	238
#define XT_REG_CCOMPARE_0	240
#define XT_REG_CCOMPARE_1	241
#define XT_REG_CCOMPARE_2	242
#define XT_REG_MISC_REG_0	244
#define XT_REG_MISC_REG_1	245
#define XT_REG_MISC_REG_2	246
#define XT_REG_MISC_REG_3	247

/*  Special cases (bases of special register series):  */
#define XT_REG_MR		32
#define XT_REG_IBREAKA		128
#define XT_REG_DBREAKA		144
#define XT_REG_DBREAKC		160
#define XT_REG_EPC		176
#define XT_REG_EPS		192
#define XT_REG_EXCSAVE		208
#define XT_REG_CCOMPARE	240
#define XT_REG_MISC_REG	244

/*  Tensilica-defined user registers:  */
#if 0
/*#define ...	 21..24 */	/* (545CK) */
/*#define ...	140..143 */	/* (545CK) */
#define XT_REG_EXPSTATE	230	/* Diamond */
#define XT_REG_THREADPTR	231	/* threadptr option */
#define XT_REG_FCR		232	/* FPU */
#define XT_REG_FSR		233	/* FPU */
#define XT_REG_AE_OVF_SAR	240	/* HiFi2 */
#define XT_REG_AE_BITHEAD	241	/* HiFi2 */
#define XT_REG_AE_TS_FTS_BU_BP	242	/* HiFi2 */
#define XT_REG_AE_SD_NO	243	/* HiFi2 */
#define XT_REG_VSAR		240	/* VectraLX */
#define XT_REG_ROUND_LO	242	/* VectraLX */
#define XT_REG_ROUND_HI	243	/* VectraLX */
#define XT_REG_CBEGIN		246	/* VectraLX */
#define XT_REG_CEND		247	/* VectraLX */
#endif

#endif /* XTENSA_XT_SPECREG_H */
