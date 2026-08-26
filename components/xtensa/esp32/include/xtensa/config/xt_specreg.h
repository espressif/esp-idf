/*
 * Xtensa Special Register symbolic names
 */

/* SPDX-FileCopyrightText: 1998-2002 Tensilica Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 */

/* $Id: //depot/rel/Eaglenest/Xtensa/SWConfig/hal/specreg.h.tpp#1 $ */

/* Customer ID=11657; Build=0x5fe96; Copyright (c) 1998-2002 Tensilica Inc.

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

#ifndef XTENSA_XT_SPECREG_H
#define XTENSA_XT_SPECREG_H

/*  Include these special register bitfield definitions, for historical reasons:  */
#include <xtensa/corebits.h>

/*  Special registers:  */
#define XT_REG_LBEG		0
#define XT_REG_LEND		1
#define XT_REG_LCOUNT		2
#define XT_REG_SAR		3
#define XT_REG_BR		4
#define XT_REG_SCOMPARE1	12
#define XT_REG_ACCLO		16
#define XT_REG_ACCHI		17
#define XT_REG_MR_0		32
#define XT_REG_MR_1		33
#define XT_REG_MR_2		34
#define XT_REG_MR_3		35
#define XT_REG_WINDOWBASE	72
#define XT_REG_WINDOWSTART	73
#define XT_REG_IBREAKENABLE	96
#define XT_REG_MEMCTL		97
#define XT_REG_ATOMCTL		99
#define XT_REG_DDR		104
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

/*  Special names for read-only and write-only interrupt registers:  */
#define XT_REG_INTREAD		226
#define XT_REG_INTSET		226
#define XT_REG_INTCLEAR	227

#endif /* XTENSA_XT_SPECREG_H */
