/******************************************************************************
 *
 *  Copyright (C) 2014 The Android Open Source Project
 *  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/**********************************************************************************
  $Revision: #1 $
***********************************************************************************/

/** @file
@ingroup codec_internal
*/

/**@addgroup codec_internal*/
/**@{*/

/*
 * Performs an 8-point Type-II scaled DCT using the Arai-Agui-Nakajima
 * factorization. The scaling factors are folded into the windowing
 * constants. 29 adds and 5 16x32 multiplies per 8 samples.
 */
#include "common/bt_target.h"
#include "oi_codec_sbc_private.h"

#if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE)

#define AAN_C4_FIX (759250125)/* S1.30  759250125   0.707107*/

#define AAN_C6_FIX (410903207)/* S1.30  410903207   0.382683*/

#define AAN_Q0_FIX (581104888)/* S1.30  581104888   0.541196*/

#define AAN_Q1_FIX (1402911301)/* S1.30 1402911301   1.306563*/

/** Scales x by y bits to the right, adding a rounding factor.
 */
#ifndef SCALE
#define SCALE(x, y) (((x) + (1 <<((y)-1))) >> (y))
#endif

/**
 * Default C language implementation of a 32x32->32 multiply. This function may
 * be replaced by a platform-specific version for speed.
 *
 * @param u A signed 32-bit multiplicand
 * @param v A signed 32-bit multiplier

 * @return  A signed 32-bit value corresponding to the 32 most significant bits
 * of the 64-bit product of u and v.
 */
static INLINE OI_INT32 default_mul_32s_32s_hi(OI_INT32 u, OI_INT32 v)
{
    OI_UINT32 u0, v0;
    OI_INT32 u1, v1, w1, w2, t;

    u0 = u & 0xFFFF; u1 = u >> 16;
    v0 = v & 0xFFFF; v1 = v >> 16;
    t = u0 * v0;
    t = u1 * v0 + ((OI_UINT32)t >> 16);
    w1 = t & 0xFFFF;
    w2 = t >> 16;
    w1 = u0 * v1 + w1;
    return u1 * v1 + w2 + (w1 >> 16);
}

#define MUL_32S_32S_HI(_x, _y) default_mul_32s_32s_hi(_x, _y)


#ifdef DEBUG_DCT
PRIVATE void float_dct2_8(float *RESTRICT out, OI_INT32 const *RESTRICT in)
{
#define FIX(x,bits) (((int)floor(0.5f+((x)*((float)(1<<bits)))))/((float)(1<<bits)))
#define FLOAT_BUTTERFLY(x,y) x += y; y = x - (y*2); OI_ASSERT(VALID_INT32(x)); OI_ASSERT(VALID_INT32(y));
#define FLOAT_MULT_DCT(K, sample) (FIX(K,20) * sample)
#define FLOAT_SCALE(x, y) (((x) / (double)(1 << (y))))

    double L00, L01, L02, L03, L04, L05, L06, L07;
    double L25;

    double in0, in1, in2, in3;
    double in4, in5, in6, in7;

    in0 = FLOAT_SCALE(in[0], DCTII_8_SHIFT_IN); OI_ASSERT(VALID_INT32(in0));
    in1 = FLOAT_SCALE(in[1], DCTII_8_SHIFT_IN); OI_ASSERT(VALID_INT32(in1));
    in2 = FLOAT_SCALE(in[2], DCTII_8_SHIFT_IN); OI_ASSERT(VALID_INT32(in2));
    in3 = FLOAT_SCALE(in[3], DCTII_8_SHIFT_IN); OI_ASSERT(VALID_INT32(in3));
    in4 = FLOAT_SCALE(in[4], DCTII_8_SHIFT_IN); OI_ASSERT(VALID_INT32(in4));
    in5 = FLOAT_SCALE(in[5], DCTII_8_SHIFT_IN); OI_ASSERT(VALID_INT32(in5));
    in6 = FLOAT_SCALE(in[6], DCTII_8_SHIFT_IN); OI_ASSERT(VALID_INT32(in6));
    in7 = FLOAT_SCALE(in[7], DCTII_8_SHIFT_IN); OI_ASSERT(VALID_INT32(in7));

    L00 = (in0 + in7); OI_ASSERT(VALID_INT32(L00));
    L01 = (in1 + in6); OI_ASSERT(VALID_INT32(L01));
    L02 = (in2 + in5); OI_ASSERT(VALID_INT32(L02));
    L03 = (in3 + in4); OI_ASSERT(VALID_INT32(L03));

    L04 = (in3 - in4); OI_ASSERT(VALID_INT32(L04));
    L05 = (in2 - in5); OI_ASSERT(VALID_INT32(L05));
    L06 = (in1 - in6); OI_ASSERT(VALID_INT32(L06));
    L07 = (in0 - in7); OI_ASSERT(VALID_INT32(L07));

    FLOAT_BUTTERFLY(L00, L03);
    FLOAT_BUTTERFLY(L01, L02);

    L02 += L03; OI_ASSERT(VALID_INT32(L02));

    L02 = FLOAT_MULT_DCT(AAN_C4_FLOAT, L02); OI_ASSERT(VALID_INT32(L02));

    FLOAT_BUTTERFLY(L00, L01);

    out[0] = (float)FLOAT_SCALE(L00, DCTII_8_SHIFT_0); OI_ASSERT(VALID_INT16(out[0]));
    out[4] = (float)FLOAT_SCALE(L01, DCTII_8_SHIFT_4); OI_ASSERT(VALID_INT16(out[4]));

    FLOAT_BUTTERFLY(L03, L02);
    out[6] = (float)FLOAT_SCALE(L02, DCTII_8_SHIFT_6); OI_ASSERT(VALID_INT16(out[6]));
    out[2] = (float)FLOAT_SCALE(L03, DCTII_8_SHIFT_2); OI_ASSERT(VALID_INT16(out[2]));

    L04 += L05; OI_ASSERT(VALID_INT32(L04));
    L05 += L06; OI_ASSERT(VALID_INT32(L05));
    L06 += L07; OI_ASSERT(VALID_INT32(L06));

    L04 /= 2;
    L05 /= 2;
    L06 /= 2;
    L07 /= 2;

    L05 = FLOAT_MULT_DCT(AAN_C4_FLOAT, L05); OI_ASSERT(VALID_INT32(L05));

    L25 = L06 - L04; OI_ASSERT(VALID_INT32(L25));
    L25 = FLOAT_MULT_DCT(AAN_C6_FLOAT, L25); OI_ASSERT(VALID_INT32(L25));

    L04 = FLOAT_MULT_DCT(AAN_Q0_FLOAT, L04); OI_ASSERT(VALID_INT32(L04));
    L04 -= L25; OI_ASSERT(VALID_INT32(L04));

    L06 = FLOAT_MULT_DCT(AAN_Q1_FLOAT, L06); OI_ASSERT(VALID_INT32(L06));
    L06 -= L25; OI_ASSERT(VALID_INT32(L25));

    FLOAT_BUTTERFLY(L07, L05);

    FLOAT_BUTTERFLY(L05, L04);
    out[3] = (float)(FLOAT_SCALE(L04, DCTII_8_SHIFT_3 - 1)); OI_ASSERT(VALID_INT16(out[3]));
    out[5] = (float)(FLOAT_SCALE(L05, DCTII_8_SHIFT_5 - 1)); OI_ASSERT(VALID_INT16(out[5]));

    FLOAT_BUTTERFLY(L07, L06);
    out[7] = (float)(FLOAT_SCALE(L06, DCTII_8_SHIFT_7 - 1)); OI_ASSERT(VALID_INT16(out[7]));
    out[1] = (float)(FLOAT_SCALE(L07, DCTII_8_SHIFT_1 - 1)); OI_ASSERT(VALID_INT16(out[1]));
}
#undef BUTTERFLY
#endif


/*
 * This function calculates the AAN DCT. Its inputs are in S16.15 format, as
 * returned by OI_SBC_Dequant. In practice, abs(in[x]) < 52429.0 / 1.38
 * (1244918057 integer). The function it computes is an approximation to the array defined
 * by:
 *
 * diag(aan_s) * AAN= C2
 *
 *   or
 *
 * AAN = diag(1/aan_s) * C2
 *
 * where C2 is as it is defined in the comment at the head of this file, and
 *
 * aan_s[i] = aan_s = 1/(2*cos(i*pi/16)) with i = 1..7, aan_s[0] = 1;
 *
 * aan_s[i] = [ 1.000  0.510  0.541  0.601  0.707  0.900  1.307  2.563 ]
 *
 * The output ranges are shown as follows:
 *
 * Let Y[0..7] = AAN * X[0..7]
 *
 * Without loss of generality, assume the input vector X consists of elements
 * between -1 and 1. The maximum possible value of a given output element occurs
 * with some particular combination of input vector elements each of which is -1
 * or 1. Consider the computation of Y[i]. Y[i] = sum t=0..7 of AAN[t,i]*X[i]. Y is
 * maximized if the sign of X[i] matches the sign of AAN[t,i], ensuring a
 * positive contribution to the sum. Equivalently, one may simply sum
 * abs(AAN)[t,i] over t to get the maximum possible value of Y[i].
 *
 * This yields approximately [8.00  10.05   9.66   8.52   8.00   5.70   4.00   2.00]
 *
 * Given the maximum magnitude sensible input value of +/-37992, this yields the
 * following vector of maximum output magnitudes:
 *
 * [ 303936  381820  367003  323692  303936  216555  151968   75984 ]
 *
 * Ultimately, these values must fit into 16 bit signed integers, so they must
 * be scaled. A non-uniform scaling helps maximize the kept precision. The
 * relative number of extra bits of precision maintainable with respect to the
 * largest value is given here:
 *
 * [ 0  0  0  0  0  0  1  2 ]
 *
 */
PRIVATE void dct2_8(SBC_BUFFER_T *RESTRICT out, OI_INT32 const *RESTRICT in)
{
#define BUTTERFLY(x,y) x += y; y = x - (y<<1);
#define FIX_MULT_DCT(K, x) (MUL_32S_32S_HI(K,x)<<2)

    OI_INT32 L00, L01, L02, L03, L04, L05, L06, L07;
    OI_INT32 L25;

    OI_INT32 in0, in1, in2, in3;
    OI_INT32 in4, in5, in6, in7;

#if DCTII_8_SHIFT_IN != 0
    in0 = SCALE(in[0], DCTII_8_SHIFT_IN);
    in1 = SCALE(in[1], DCTII_8_SHIFT_IN);
    in2 = SCALE(in[2], DCTII_8_SHIFT_IN);
    in3 = SCALE(in[3], DCTII_8_SHIFT_IN);
    in4 = SCALE(in[4], DCTII_8_SHIFT_IN);
    in5 = SCALE(in[5], DCTII_8_SHIFT_IN);
    in6 = SCALE(in[6], DCTII_8_SHIFT_IN);
    in7 = SCALE(in[7], DCTII_8_SHIFT_IN);
#else
    in0 = in[0];
    in1 = in[1];
    in2 = in[2];
    in3 = in[3];
    in4 = in[4];
    in5 = in[5];
    in6 = in[6];
    in7 = in[7];
#endif

    L00 = in0 + in7;
    L01 = in1 + in6;
    L02 = in2 + in5;
    L03 = in3 + in4;

    L04 = in3 - in4;
    L05 = in2 - in5;
    L06 = in1 - in6;
    L07 = in0 - in7;

    BUTTERFLY(L00, L03);
    BUTTERFLY(L01, L02);

    L02 += L03;

    L02 = FIX_MULT_DCT(AAN_C4_FIX, L02);

    BUTTERFLY(L00, L01);

    out[0] = (OI_INT16)SCALE(L00, DCTII_8_SHIFT_0);
    out[4] = (OI_INT16)SCALE(L01, DCTII_8_SHIFT_4);

    BUTTERFLY(L03, L02);
    out[6] = (OI_INT16)SCALE(L02, DCTII_8_SHIFT_6);
    out[2] = (OI_INT16)SCALE(L03, DCTII_8_SHIFT_2);

    L04 += L05;
    L05 += L06;
    L06 += L07;

    L04 /= 2;
    L05 /= 2;
    L06 /= 2;
    L07 /= 2;

    L05 = FIX_MULT_DCT(AAN_C4_FIX, L05);

    L25 = L06 - L04;
    L25 = FIX_MULT_DCT(AAN_C6_FIX, L25);

    L04 = FIX_MULT_DCT(AAN_Q0_FIX, L04);
    L04 -= L25;

    L06 = FIX_MULT_DCT(AAN_Q1_FIX, L06);
    L06 -= L25;

    BUTTERFLY(L07, L05);

    BUTTERFLY(L05, L04);
    out[3] = (OI_INT16)SCALE(L04, DCTII_8_SHIFT_3 - 1);
    out[5] = (OI_INT16)SCALE(L05, DCTII_8_SHIFT_5 - 1);

    BUTTERFLY(L07, L06);
    out[7] = (OI_INT16)SCALE(L06, DCTII_8_SHIFT_7 - 1);
    out[1] = (OI_INT16)SCALE(L07, DCTII_8_SHIFT_1 - 1);
#undef BUTTERFLY

#ifdef DEBUG_DCT
    {
        float float_out[8];
        float_dct2_8(float_out, in);
    }
#endif
}

/**@}*/
#endif /* #if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE) */
