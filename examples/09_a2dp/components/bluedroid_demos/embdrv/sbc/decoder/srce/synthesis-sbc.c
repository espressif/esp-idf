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

This file, along with synthesis-generated.c, contains the synthesis
filterbank routines. The operations performed correspond to the
operations described in A2DP Appendix B, Figure 12.3. Several
mathematical optimizations are performed, particularly for the
8-subband case.

One important optimization is to note that the "matrixing" operation
can be decomposed into the product of a type II discrete cosine kernel
and another, sparse matrix.

According to Fig 12.3, in the 8-subband case,
@code
    N[k][i] = cos((i+0.5)*(k+4)*pi/8), k = 0..15 and i = 0..7
@endcode

N can be factored as R * C2, where C2 is an 8-point type II discrete
cosine kernel given by
@code
    C2[k][i] = cos((i+0.5)*k*pi/8)), k = 0..7 and i = 0..7
@endcode

R turns out to be a sparse 16x8 matrix with the following non-zero
entries:
@code
    R[k][k+4]        =  1,   k = 0..3
    R[k][abs(12-k)]  = -1,   k = 5..15
@endcode

The spec describes computing V[0..15] as N * R.
@code
    V[0..15] = N * R = (R * C2) * R = R * (C2 * R)
@endcode

C2 * R corresponds to computing the discrete cosine transform of R, so
V[0..15] can be computed by taking the DCT of R followed by assignment
and selective negation of the DCT result into V.

        Although this was derived empirically using GNU Octave, it is
        formally demonstrated in, e.g., Liu, Chi-Min and Lee,
        Wen-Chieh. "A Unified Fast Algorithm for Cosine Modulated
        Filter Banks in Current Audio Coding Standards." Journal of
        the AES 47 (December 1999): 1061.

Given the shift operation performed prior to computing V[0..15], it is
clear that V[0..159] represents a rolling history of the 10 most
recent groups of blocks input to the synthesis operation. Interpreting
the matrix N in light of its factorization into C2 and R, R's
sparseness has implications for interpreting the values in V. In
particular, there is considerable redundancy in the values stored in
V. Furthermore, since R[4][0..7] are all zeros, one out of every 16
values in V will be zero regardless of the input data. Within each
block of 16 values in V, fully half of them are redundant or
irrelevant:

@code
    V[ 0] =  DCT[4]
    V[ 1] =  DCT[5]
    V[ 2] =  DCT[6]
    V[ 3] =  DCT[7]
    V[ 4] = 0
    V[ 5] = -DCT[7] = -V[3] (redundant)
    V[ 6] = -DCT[6] = -V[2] (redundant)
    V[ 7] = -DCT[5] = -V[1] (redundant)
    V[ 8] = -DCT[4] = -V[0] (redundant)
    V[ 9] = -DCT[3]
    V[10] = -DCT[2]
    V[11] = -DCT[1]
    V[12] = -DCT[0]
    V[13] = -DCT[1] = V[11] (redundant)
    V[14] = -DCT[2] = V[10] (redundant)
    V[15] = -DCT[3] = V[ 9] (redundant)
@endcode

Since the elements of V beyond 15 were originally computed the same
way during a previous run, what holds true for V[x] also holds true
for V[x+16]. Thus, so long as care is taken to maintain the mapping,
we need only actually store the unique values, which correspond to the
output of the DCT, in some cases inverted. In fact, instead of storing
V[0..159], we could store DCT[0..79] which would contain a history of
DCT results. More on this in a bit.

Going back to figure 12.3 in the spec, it should be clear that the
vector U need not actually be explicitly constructed, but that with
suitable indexing into V during the window operation, the same end can
be accomplished. In the same spirit of the pseudocode shown in the
figure, the following is the construction of W without using U:

@code
    for i=0 to 79 do
        W[i] = D[i]*VSIGN(i)*V[remap_V(i)] where remap_V(i) = 32*(int(i/16)) + (i % 16) + (i % 16 >= 8 ? 16 : 0)
                                             and VSIGN(i) maps i%16 into {1, 1, 1, 1, 0, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1 }
                                             These values correspond to the
                                             signs of the redundant values as
                                             shown in the explanation three
                                             paragraphs above.
@endcode

We saw above how V[4..8,13..15] (and by extension
V[(4..8,13..15)+16*n]) can be defined in terms of other elements
within the subblock of V. V[0..3,9..12] correspond to DCT elements.

@code
    for i=0 to 79 do
        W[i] = D[i]*DSIGN(i)*DCT[remap_DCT(i)]
@endcode

The DCT is calculated using the Arai-Agui-Nakajima factorization,
which saves some computation by producing output that needs to be
multiplied by scaling factors before being used.

@code
    for i=0 to 79 do
        W[i] = D[i]*SCALE[i%8]*AAN_DCT[remap_DCT(i)]
@endcode

D can be premultiplied with the DCT scaling factors to yield

@code
    for i=0 to 79 do
        W[i] = DSCALED[i]*AAN_DCT[remap_DCT(i)] where DSCALED[i] = D[i]*SCALE[i%8]
@endcode

The output samples X[0..7] are defined as sums of W:

@code
        X[j] = sum{i=0..9}(W[j+8*i])
@endcode

@ingroup codec_internal
*/

/**
@addtogroup codec_internal
@{
*/

#include "oi_codec_sbc_private.h"

const OI_INT32 dec_window_4[21] = {
           0,        /* +0.00000000E+00 */
          97,        /* +5.36548976E-04 */
         270,        /* +1.49188357E-03 */
         495,        /* +2.73370904E-03 */
         694,        /* +3.83720193E-03 */
         704,        /* +3.89205149E-03 */
         338,        /* +1.86581691E-03 */
        -554,        /* -3.06012286E-03 */
        1974,        /* +1.09137620E-02 */
        3697,        /* +2.04385087E-02 */
        5224,        /* +2.88757392E-02 */
        5824,        /* +3.21939290E-02 */
        4681,        /* +2.58767811E-02 */
        1109,        /* +6.13245186E-03 */
       -5214,        /* -2.88217274E-02 */
      -14047,        /* -7.76463494E-02 */
       24529,        /* +1.35593274E-01 */
       35274,        /* +1.94987841E-01 */
       44618,        /* +2.46636662E-01 */
       50984,        /* +2.81828203E-01 */
       53243,        /* +2.94315332E-01 */
};

#define DCTII_4_K06_FIX ( 11585)/* S1.14      11585   0.707107*/

#define DCTII_4_K08_FIX ( 21407)/* S1.14      21407   1.306563*/

#define DCTII_4_K09_FIX (-15137)/* S1.14     -15137  -0.923880*/

#define DCTII_4_K10_FIX ( -8867)/* S1.14      -8867  -0.541196*/

/** Scales x by y bits to the right, adding a rounding factor.
 */
#ifndef SCALE
#define SCALE(x, y) (((x) + (1 <<((y)-1))) >> (y))
#endif

#ifndef CLIP_INT16
#define CLIP_INT16(x) do { if (x > OI_INT16_MAX) { x = OI_INT16_MAX; } else if (x < OI_INT16_MIN) { x = OI_INT16_MIN; } } while (0)
#endif

/**
 * Default C language implementation of a 16x32->32 multiply. This function may
 * be replaced by a platform-specific version for speed.
 *
 * @param u A signed 16-bit multiplicand
 * @param v A signed 32-bit multiplier

 * @return  A signed 32-bit value corresponding to the 32 most significant bits
 * of the 48-bit product of u and v.
 */
INLINE OI_INT32 default_mul_16s_32s_hi(OI_INT16 u, OI_INT32 v)
{
    OI_UINT16 v0;
    OI_INT16 v1;

    OI_INT32 w,x;

    v0 = (OI_UINT16)(v & 0xffff);
    v1 = (OI_INT16) (v >> 16);

    w = v1 * u;
    x = u * v0;

    return w + (x >> 16);
}

#define MUL_16S_32S_HI(_x, _y) default_mul_16s_32s_hi(_x, _y)

#define LONG_MULT_DCT(K, sample) (MUL_16S_32S_HI(K, sample)<<2)

PRIVATE void SynthWindow80_generated(OI_INT16 *pcm, SBC_BUFFER_T const * RESTRICT buffer, OI_UINT strideShift);
PRIVATE void SynthWindow112_generated(OI_INT16 *pcm, SBC_BUFFER_T const * RESTRICT buffer, OI_UINT strideShift);
PRIVATE void dct2_8(SBC_BUFFER_T * RESTRICT out, OI_INT32 const * RESTRICT x);

typedef void (*SYNTH_FRAME)(OI_CODEC_SBC_DECODER_CONTEXT *context, OI_INT16 *pcm, OI_UINT blkstart, OI_UINT blkcount);

#ifndef COPY_BACKWARD_32BIT_ALIGNED_72_HALFWORDS
#define COPY_BACKWARD_32BIT_ALIGNED_72_HALFWORDS(dest, src) do { shift_buffer(dest, src, 72); } while (0)
#endif

#ifndef DCT2_8
#define DCT2_8(dst, src) dct2_8(dst, src)
#endif

#ifndef SYNTH80
#define SYNTH80 SynthWindow80_generated
#endif

#ifndef SYNTH112
#define SYNTH112 SynthWindow112_generated
#endif

PRIVATE void OI_SBC_SynthFrame_80(OI_CODEC_SBC_DECODER_CONTEXT *context, OI_INT16 *pcm, OI_UINT blkstart, OI_UINT blkcount)
{
    OI_UINT blk;
    OI_UINT ch;
    OI_UINT nrof_channels = context->common.frameInfo.nrof_channels;
    OI_UINT pcmStrideShift = context->common.pcmStride == 1 ? 0 : 1;
    OI_UINT offset = context->common.filterBufferOffset;
    OI_INT32 *s = context->common.subdata + 8 * nrof_channels * blkstart;
    OI_UINT blkstop = blkstart + blkcount;

    for (blk = blkstart; blk < blkstop; blk++) {
        if (offset == 0) {
            COPY_BACKWARD_32BIT_ALIGNED_72_HALFWORDS(context->common.filterBuffer[0] + context->common.filterBufferLen - 72, context->common.filterBuffer[0]);
            if (nrof_channels == 2) {
                COPY_BACKWARD_32BIT_ALIGNED_72_HALFWORDS(context->common.filterBuffer[1] + context->common.filterBufferLen - 72, context->common.filterBuffer[1]);
            }
            offset = context->common.filterBufferLen - 80;
        } else {
            offset -= 1*8;
        }

        for (ch = 0; ch < nrof_channels; ch++) {
            DCT2_8(context->common.filterBuffer[ch] + offset, s);
            SYNTH80(pcm + ch, context->common.filterBuffer[ch] + offset, pcmStrideShift);
            s += 8;
        }
        pcm += (8 << pcmStrideShift);
    }
    context->common.filterBufferOffset = offset;
}

PRIVATE void OI_SBC_SynthFrame_4SB(OI_CODEC_SBC_DECODER_CONTEXT *context, OI_INT16 *pcm, OI_UINT blkstart, OI_UINT blkcount)
{
    OI_UINT blk;
    OI_UINT ch;
    OI_UINT nrof_channels = context->common.frameInfo.nrof_channels;
    OI_UINT pcmStrideShift = context->common.pcmStride == 1 ? 0 : 1;
    OI_UINT offset = context->common.filterBufferOffset;
    OI_INT32 *s = context->common.subdata + 8 * nrof_channels * blkstart;
    OI_UINT blkstop = blkstart + blkcount;

    for (blk = blkstart; blk < blkstop; blk++) {
        if (offset == 0) {
            COPY_BACKWARD_32BIT_ALIGNED_72_HALFWORDS(context->common.filterBuffer[0] + context->common.filterBufferLen - 72,context->common.filterBuffer[0]);
            if (nrof_channels == 2) {
                COPY_BACKWARD_32BIT_ALIGNED_72_HALFWORDS(context->common.filterBuffer[1] + context->common.filterBufferLen - 72,context->common.filterBuffer[1]);
            }
            offset =context->common.filterBufferLen - 80;
        } else {
            offset -= 8;
        }
        for (ch = 0; ch < nrof_channels; ch++) {
            cosineModulateSynth4(context->common.filterBuffer[ch] + offset, s);
            SynthWindow40_int32_int32_symmetry_with_sum(pcm + ch,
                                                        context->common.filterBuffer[ch] + offset,
                                                        pcmStrideShift);
            s += 4;
        }
        pcm += (4 << pcmStrideShift);
    }
    context->common.filterBufferOffset = offset;
}

#ifdef SBC_ENHANCED

PRIVATE void OI_SBC_SynthFrame_Enhanced(OI_CODEC_SBC_DECODER_CONTEXT *context, OI_INT16 *pcm, OI_UINT blkstart, OI_UINT blkcount)
{
    OI_UINT blk;
    OI_UINT ch;
    OI_UINT nrof_channels = context->common.frameInfo.nrof_channels;
    OI_UINT pcmStrideShift = context->common.pcmStride == 1 ? 0 : 1;
    OI_UINT offset = context->common.filterBufferOffset;
    OI_INT32 *s = context->common.subdata + 8 * nrof_channels * blkstart;
    OI_UINT blkstop = blkstart + blkcount;

    for (blk = blkstart; blk < blkstop; blk++) {
        if (offset == 0) {
            COPY_BACKWARD_32BIT_ALIGNED_104_HALFWORDS(context->common.filterBuffer[0] +context->common.filterBufferLen - 104, context->common.filterBuffer[0]);
            if (nrof_channels == 2) {
                COPY_BACKWARD_32BIT_ALIGNED_104_HALFWORDS(context->common.filterBuffer[1] + context->common.filterBufferLen - 104, context->common.filterBuffer[1]);
            }
            offset = context->common.filterBufferLen - 112;
        } else {
            offset -= 8;
        }
        for (ch = 0; ch < nrof_channels; ++ch) {
            DCT2_8(context->common.filterBuffer[ch] + offset, s);
            SYNTH112(pcm + ch, context->common.filterBuffer[ch] + offset, pcmStrideShift);
            s += 8;
        }
        pcm += (8 << pcmStrideShift);
    }
    context->common.filterBufferOffset = offset;
}

static const SYNTH_FRAME SynthFrameEnhanced[] = {
    NULL,                       /* invalid */
    OI_SBC_SynthFrame_Enhanced, /* mono */
    OI_SBC_SynthFrame_Enhanced  /* stereo */
};

#endif

static const SYNTH_FRAME SynthFrame8SB[] = {
    NULL,             /* invalid */
    OI_SBC_SynthFrame_80, /* mono */
    OI_SBC_SynthFrame_80  /* stereo */
};


static const SYNTH_FRAME SynthFrame4SB[] = {
    NULL,                  /* invalid */
    OI_SBC_SynthFrame_4SB, /* mono */
    OI_SBC_SynthFrame_4SB  /* stereo */
};

PRIVATE void OI_SBC_SynthFrame(OI_CODEC_SBC_DECODER_CONTEXT *context, OI_INT16 *pcm, OI_UINT start_block, OI_UINT nrof_blocks)
{
    OI_UINT nrof_subbands = context->common.frameInfo.nrof_subbands;
    OI_UINT nrof_channels = context->common.frameInfo.nrof_channels;

    OI_ASSERT(nrof_subbands == 4 || nrof_subbands == 8);
    if (nrof_subbands == 4) {
        SynthFrame4SB[nrof_channels](context, pcm, start_block, nrof_blocks);
#ifdef SBC_ENHANCED
    } else if (context->common.frameInfo.enhanced) {
        SynthFrameEnhanced[nrof_channels](context, pcm, start_block, nrof_blocks);
#endif /* SBC_ENHANCED */
        } else {
        SynthFrame8SB[nrof_channels](context, pcm, start_block, nrof_blocks);
    }
}


void SynthWindow40_int32_int32_symmetry_with_sum(OI_INT16 *pcm, SBC_BUFFER_T buffer[80], OI_UINT strideShift)
{
    OI_INT32 pa;
    OI_INT32 pb;

    /* These values should be zero, since out[2] of the 4-band cosine modulation
     * is always zero. */
    OI_ASSERT(buffer[ 2] == 0);
    OI_ASSERT(buffer[10] == 0);
    OI_ASSERT(buffer[18] == 0);
    OI_ASSERT(buffer[26] == 0);
    OI_ASSERT(buffer[34] == 0);
    OI_ASSERT(buffer[42] == 0);
    OI_ASSERT(buffer[50] == 0);
    OI_ASSERT(buffer[58] == 0);
    OI_ASSERT(buffer[66] == 0);
    OI_ASSERT(buffer[74] == 0);


    pa  = dec_window_4[ 4] * (buffer[12] + buffer[76]);
    pa += dec_window_4[ 8] * (buffer[16] - buffer[64]);
    pa += dec_window_4[12] * (buffer[28] + buffer[60]);
    pa += dec_window_4[16] * (buffer[32] - buffer[48]);
    pa += dec_window_4[20] *  buffer[44];
    pa = SCALE(-pa, 15);
    CLIP_INT16(pa);
    pcm[0 << strideShift] = (OI_INT16)pa;


    pa  = dec_window_4[ 1] * buffer[ 1]; pb  = dec_window_4[ 1] * buffer[79];
    pb += dec_window_4[ 3] * buffer[ 3]; pa += dec_window_4[ 3] * buffer[77];
    pa += dec_window_4[ 5] * buffer[13]; pb += dec_window_4[ 5] * buffer[67];
    pb += dec_window_4[ 7] * buffer[15]; pa += dec_window_4[ 7] * buffer[65];
    pa += dec_window_4[ 9] * buffer[17]; pb += dec_window_4[ 9] * buffer[63];
    pb += dec_window_4[11] * buffer[19]; pa += dec_window_4[11] * buffer[61];
    pa += dec_window_4[13] * buffer[29]; pb += dec_window_4[13] * buffer[51];
    pb += dec_window_4[15] * buffer[31]; pa += dec_window_4[15] * buffer[49];
    pa += dec_window_4[17] * buffer[33]; pb += dec_window_4[17] * buffer[47];
    pb += dec_window_4[19] * buffer[35]; pa += dec_window_4[19] * buffer[45];
    pa = SCALE(-pa, 15);
    CLIP_INT16(pa);
    pcm[1 << strideShift] = (OI_INT16)(pa);
    pb = SCALE(-pb, 15);
    CLIP_INT16(pb);
    pcm[3 << strideShift] = (OI_INT16)(pb);


    pa  = dec_window_4[2] * (/*buffer[ 2] + */ buffer[78]);  /* buffer[ 2] is always zero */
    pa += dec_window_4[6] * (buffer[14] /* + buffer[66]*/);  /* buffer[66] is always zero */
    pa += dec_window_4[10] * (/*buffer[18] + */ buffer[62]);  /* buffer[18] is always zero */
    pa += dec_window_4[14] * (buffer[30] /* + buffer[50]*/);  /* buffer[50] is always zero */
    pa += dec_window_4[18] * (/*buffer[34] + */ buffer[46]);  /* buffer[34] is always zero */
    pa = SCALE(-pa, 15);
    CLIP_INT16(pa);
    pcm[2 << strideShift] = (OI_INT16)(pa);
}


/**
  This routine implements the cosine modulation matrix for 4-subband
  synthesis. This is called "matrixing" in the SBC specification. This
  matrix, M4,  can be factored into an 8-point Type II Discrete Cosine
  Transform, DCTII_4 and a matrix S4, given here:

  @code
        __               __
       |   0   0   1   0   |
       |   0   0   0   1   |
       |   0   0   0   0   |
       |   0   0   0  -1   |
  S4 = |   0   0  -1   0   |
       |   0  -1   0   0   |
       |  -1   0   0   0   |
       |__ 0  -1   0   0 __|

  M4 * in = S4 * (DCTII_4 * in)
  @endcode

  (DCTII_4 * in) is computed using a Fast Cosine Transform. The algorithm
  here is based on an implementation computed by the SPIRAL computer
  algebra system, manually converted to fixed-point arithmetic. S4 can be
  implemented using only assignment and negation.
  */
PRIVATE void cosineModulateSynth4(SBC_BUFFER_T * RESTRICT out, OI_INT32 const * RESTRICT in)
{
    OI_INT32 f0, f1, f2, f3, f4, f7, f8, f9, f10;
    OI_INT32 y0, y1, y2, y3;

    f0 = (in[0] - in[3]);
    f1 = (in[0] + in[3]);
    f2 = (in[1] - in[2]);
    f3 = (in[1] + in[2]);

    f4 = f1 - f3;

    y0 = -SCALE(f1 + f3, DCT_SHIFT);
    y2 = -SCALE(LONG_MULT_DCT(DCTII_4_K06_FIX, f4), DCT_SHIFT);
    f7 = f0 + f2;
    f8 = LONG_MULT_DCT(DCTII_4_K08_FIX, f0);
    f9 = LONG_MULT_DCT(DCTII_4_K09_FIX, f7);
    f10 = LONG_MULT_DCT(DCTII_4_K10_FIX, f2);
    y3 = -SCALE(f8 + f9, DCT_SHIFT);
    y1 = -SCALE(f10 - f9, DCT_SHIFT);

    out[0] = (OI_INT16)-y2;
    out[1] = (OI_INT16)-y3;
    out[2] = (OI_INT16)0;
    out[3] = (OI_INT16)y3;
    out[4] = (OI_INT16)y2;
    out[5] = (OI_INT16)y1;
    out[6] = (OI_INT16)y0;
    out[7] = (OI_INT16)y1;
}



/**
@}
*/
