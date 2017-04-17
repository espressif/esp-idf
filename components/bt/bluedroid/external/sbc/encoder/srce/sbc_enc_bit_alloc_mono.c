/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
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

/******************************************************************************
 *
 *  This file contains the code for bit allocation algorithm. It calculates
 *  the number of bits required for the encoded stream of data.
 *
 ******************************************************************************/

/*Includes*/
#include "bt_target.h"
#include "sbc_encoder.h"
#include "sbc_enc_func_declare.h"

#if (defined(SBC_ENC_INCLUDED) && SBC_ENC_INCLUDED == TRUE)

/*global arrays*/
const SINT16 sbc_enc_as16Offset4[4][4] = {  { -1, 0, 0, 0}, { -2, 0, 0, 1},
    { -2, 0, 0, 1}, { -2, 0, 0, 1}
};
const SINT16 sbc_enc_as16Offset8[4][8] = {  { -2, 0, 0, 0, 0, 0, 0, 1},
    { -3, 0, 0, 0, 0, 0, 1, 2},
    { -4, 0, 0, 0, 0, 0, 1, 2},
    { -4, 0, 0, 0, 0, 0, 1, 2}
};

/****************************************************************************
* BitAlloc - Calculates the required number of bits for the given scale factor
* and the number of subbands.
*
* RETURNS : N/A
*/

void sbc_enc_bit_alloc_mono(SBC_ENC_PARAMS *pstrCodecParams)
{
    SINT32 s32MaxBitNeed;   /*to store the max bits needed per sb*/
    SINT32 s32BitCount;     /*the used number of bits*/
    SINT32 s32SliceCount;   /*to store hwo many slices can be put in bitpool*/
    SINT32 s32BitSlice;     /*number of bitslices in bitpool*/
    SINT32 s32Sb;           /*counter for sub-band*/
    SINT32 s32Ch;           /*counter for channel*/
    SINT16 *ps16BitNeed;    /*temp memory to store required number of bits*/
    SINT32 s32Loudness;     /*used in Loudness calculation*/
    SINT16 *ps16GenBufPtr;
    SINT16 *ps16GenArrPtr;
    SINT16 *ps16GenTabPtr;
    SINT32  s32NumOfSubBands = pstrCodecParams->s16NumOfSubBands;

    ps16BitNeed = pstrCodecParams->s16ScartchMemForBitAlloc;

    for (s32Ch = 0; s32Ch < pstrCodecParams->s16NumOfChannels; s32Ch++) {
        ps16GenBufPtr = ps16BitNeed + s32Ch * s32NumOfSubBands;
        ps16GenArrPtr = pstrCodecParams->as16Bits + s32Ch * SBC_MAX_NUM_OF_SUBBANDS;

        /* bitneed values are derived from scale factor */
        if (pstrCodecParams->s16AllocationMethod == SBC_SNR) {
            ps16BitNeed = pstrCodecParams->as16ScaleFactor;
            ps16GenBufPtr = ps16BitNeed + s32Ch * s32NumOfSubBands;
        } else {
            ps16GenBufPtr = ps16BitNeed + s32Ch * s32NumOfSubBands;
            if (s32NumOfSubBands == 4) {
                ps16GenTabPtr = (SINT16 *)
                                sbc_enc_as16Offset4[pstrCodecParams->s16SamplingFreq];
            } else {
                ps16GenTabPtr = (SINT16 *)
                                sbc_enc_as16Offset8[pstrCodecParams->s16SamplingFreq];
            }
            for (s32Sb = 0; s32Sb < s32NumOfSubBands; s32Sb++) {
                if (pstrCodecParams->as16ScaleFactor[s32Ch * s32NumOfSubBands + s32Sb] == 0) {
                    *(ps16GenBufPtr) = -5;
                } else {
                    s32Loudness =
                        (SINT32)(pstrCodecParams->as16ScaleFactor[s32Ch * s32NumOfSubBands + s32Sb]
                                 - *ps16GenTabPtr);
                    if (s32Loudness > 0) {
                        *(ps16GenBufPtr) = (SINT16)(s32Loudness >> 1);
                    } else {
                        *(ps16GenBufPtr) = (SINT16)s32Loudness;
                    }
                }
                ps16GenBufPtr++;
                ps16GenTabPtr++;
            }

        }

        /* max bitneed index is searched*/
        s32MaxBitNeed = 0;
        ps16GenBufPtr = ps16BitNeed + s32Ch * s32NumOfSubBands;
        for (s32Sb = 0; s32Sb < s32NumOfSubBands; s32Sb++) {
            if ( *(ps16GenBufPtr) > s32MaxBitNeed) {
                s32MaxBitNeed = *(ps16GenBufPtr);
            }

            ps16GenBufPtr++;
        }
        ps16GenBufPtr = ps16BitNeed + s32Ch * s32NumOfSubBands;
        /*iterative process to find hwo many bitslices fit into the bitpool*/
        s32BitSlice = s32MaxBitNeed + 1;
        s32BitCount = pstrCodecParams->s16BitPool;
        s32SliceCount = 0;
        do {
            s32BitSlice --;
            s32BitCount -= s32SliceCount;
            s32SliceCount = 0;

            for (s32Sb = 0; s32Sb < s32NumOfSubBands; s32Sb++) {
                if ( (((*ps16GenBufPtr - s32BitSlice) < 16) && (*ps16GenBufPtr - s32BitSlice) >= 1)) {
                    if ((*ps16GenBufPtr - s32BitSlice) == 1) {
                        s32SliceCount += 2;
                    } else {
                        s32SliceCount++;
                    }
                }
                ps16GenBufPtr++;

            }/*end of for*/
            ps16GenBufPtr = ps16BitNeed + s32Ch * s32NumOfSubBands;
        } while (s32BitCount - s32SliceCount > 0);

        if (s32BitCount == 0) {
            s32BitCount -= s32SliceCount;
            s32BitSlice --;
        }

        /*Bits are distributed until the last bitslice is reached*/
        ps16GenArrPtr = pstrCodecParams->as16Bits + s32Ch * s32NumOfSubBands;
        ps16GenBufPtr = ps16BitNeed + s32Ch * s32NumOfSubBands;
        for (s32Sb = 0; s32Sb < s32NumOfSubBands; s32Sb++) {
            if (*(ps16GenBufPtr) < s32BitSlice + 2) {
                *(ps16GenArrPtr) = 0;
            } else
                *(ps16GenArrPtr) = ((*(ps16GenBufPtr) - s32BitSlice) < 16) ?
                                   (SINT16)(*(ps16GenBufPtr) - s32BitSlice) : 16;

            ps16GenBufPtr++;
            ps16GenArrPtr++;
        }
        ps16GenArrPtr = pstrCodecParams->as16Bits + s32Ch * s32NumOfSubBands;
        ps16GenBufPtr = ps16BitNeed + s32Ch * s32NumOfSubBands;
        /*the remaining bits are allocated starting at subband 0*/
        s32Sb = 0;
        while ( (s32BitCount > 0) && (s32Sb < s32NumOfSubBands) ) {
            if ( (*(ps16GenArrPtr) >= 2) && (*(ps16GenArrPtr) < 16) ) {
                (*(ps16GenArrPtr))++;
                s32BitCount--;
            } else if ( (*(ps16GenBufPtr) == s32BitSlice + 1) &&
                        (s32BitCount > 1) ) {
                *(ps16GenArrPtr) = 2;
                s32BitCount -= 2;
            }
            s32Sb++;
            ps16GenArrPtr++;
            ps16GenBufPtr++;
        }
        ps16GenArrPtr = pstrCodecParams->as16Bits + s32Ch * s32NumOfSubBands;


        s32Sb = 0;
        while ( (s32BitCount > 0) && (s32Sb < s32NumOfSubBands) ) {
            if ( *(ps16GenArrPtr) < 16) {
                (*(ps16GenArrPtr))++;
                s32BitCount--;
            }
            s32Sb++;
            ps16GenArrPtr++;
        }
    }
}
/*End of BitAlloc() function*/

#endif /* #if (defined(SBC_ENC_INCLUDED) && SBC_ENC_INCLUDED == TRUE) */
