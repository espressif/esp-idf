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

/**
@file

The functions in this file relate to the allocation of available bits to
subbands within the SBC/eSBC frame, along with support functions for computing
frame length and bitrate.

@ingroup codec_internal
*/

/**
@addtogroup codec_internal
@{
*/

#include "common/bt_target.h"
#include "oi_utils.h"
#include <oi_codec_sbc_private.h>

#if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE)

OI_UINT32 OI_SBC_MaxBitpool(OI_CODEC_SBC_FRAME_INFO *frame)
{
    switch (frame->mode) {
    case SBC_MONO:
    case SBC_DUAL_CHANNEL:
        return 16 * frame->nrof_subbands;
    case SBC_STEREO:
    case SBC_JOINT_STEREO:
        return 32 * frame->nrof_subbands;
    }

    ERROR(("Invalid frame mode %d", frame->mode));
    OI_ASSERT(FALSE);
    return 0; /* Should never be reached */
}


PRIVATE OI_UINT16 internal_CalculateFramelen(OI_CODEC_SBC_FRAME_INFO *frame)
{
    OI_UINT16 nbits = frame->nrof_blocks * frame->bitpool;
    OI_UINT16 nrof_subbands = frame->nrof_subbands;
    OI_UINT16 result = nbits;

    if (frame->mode == SBC_JOINT_STEREO) {
        result += nrof_subbands + (8 * nrof_subbands);
    } else {
        if (frame->mode == SBC_DUAL_CHANNEL) {
            result += nbits;
        }
        if (frame->mode == SBC_MONO) {
            result += 4 * nrof_subbands;
        } else {
            result += 8 * nrof_subbands;
        }
    }
    return SBC_HEADER_LEN + (result + 7) / 8;
}


PRIVATE OI_UINT32 internal_CalculateBitrate(OI_CODEC_SBC_FRAME_INFO *frame)
{
    OI_UINT blocksbands;
    blocksbands = frame->nrof_subbands * frame->nrof_blocks;

    return DIVIDE(8 * internal_CalculateFramelen(frame) * frame->frequency, blocksbands);
}


INLINE OI_UINT16 OI_SBC_CalculateFrameAndHeaderlen(OI_CODEC_SBC_FRAME_INFO *frame, OI_UINT *headerLen_)
{
    OI_UINT headerLen = SBC_HEADER_LEN + frame->nrof_subbands * frame->nrof_channels / 2;

    if (frame->mode == SBC_JOINT_STEREO) {
        headerLen++;
    }

    *headerLen_ = headerLen;
    return internal_CalculateFramelen(frame);
}


#define MIN(x, y)  ((x) < (y) ? (x) : (y))


/*
 * Computes the bit need for each sample and as also returns a counts of bit needs that are greater
 * than one. This count is used in the first phase of bit allocation.
 *
 * We also compute a preferred bitpool value that this is the minimum bitpool needed to guarantee
 * lossless representation of the audio data. The preferred bitpool may be larger than the bits
 * actually required but the only input we have are the scale factors. For example, it takes 2 bits
 * to represent values in the range -1 .. +1 but the scale factor is 0. To guarantee lossless
 * representation we add 2 to each scale factor and sum them to come up with the preferred bitpool.
 * This is not ideal because 0 requires 0 bits but we currently have no way of knowing this.
 *
 * @param bitneed       Array to return bitneeds for each subband
 *
 * @param ch            Channel 0 or 1
 *
 * @param preferredBitpool  Returns the number of reserved bits
 *
 * @return              The SBC bit need
 *
 */
OI_UINT computeBitneed(OI_CODEC_SBC_COMMON_CONTEXT *common,
                       OI_UINT8 *bitneeds,
                       OI_UINT ch,
                       OI_UINT *preferredBitpool)
{
    static const OI_INT8 offset4[4][4] = {
        { -1, 0, 0, 0 },
        { -2, 0, 0, 1 },
        { -2, 0, 0, 1 },
        { -2, 0, 0, 1 }
    };

    static const OI_INT8 offset8[4][8] = {
        { -2, 0, 0, 0, 0, 0, 0, 1 },
        { -3, 0, 0, 0, 0, 0, 1, 2 },
        { -4, 0, 0, 0, 0, 0, 1, 2 },
        { -4, 0, 0, 0, 0, 0, 1, 2 }
    };

    const OI_UINT nrof_subbands = common->frameInfo.nrof_subbands;
    OI_UINT sb;
    OI_INT8 *scale_factor = &common->scale_factor[ch ? nrof_subbands : 0];
    OI_UINT bitcount = 0;
    OI_UINT8 maxBits = 0;
    OI_UINT8 prefBits = 0;

    if (common->frameInfo.alloc == SBC_SNR) {
        for (sb = 0; sb < nrof_subbands; sb++) {
            OI_INT bits = scale_factor[sb];
            if (bits > maxBits) {
                maxBits = bits;
            }
            if ((bitneeds[sb] = bits) > 1) {
                bitcount += bits;
            }
            prefBits += 2 + bits;
        }
    } else {
        const OI_INT8 *offset;
        if (nrof_subbands == 4) {
            offset = offset4[common->frameInfo.freqIndex];
        } else {
            offset = offset8[common->frameInfo.freqIndex];
        }
        for (sb = 0; sb < nrof_subbands; sb++) {
            OI_INT bits = scale_factor[sb];
            if (bits > maxBits) {
                maxBits = bits;
            }
            prefBits += 2 + bits;
            if (bits) {
                bits -= offset[sb];
                if (bits > 0) {
                    bits /= 2;
                }
                bits += 5;
            }
            if ((bitneeds[sb] = bits) > 1) {
                bitcount += bits;
            }
        }
    }
    common->maxBitneed = OI_MAX(maxBits, common->maxBitneed);
    *preferredBitpool += prefBits;
    return bitcount;
}


/*
 * Explanation of the adjustToFitBitpool inner loop.
 *
 * The inner loop computes the effect of adjusting the bit allocation up or
 * down. Allocations must be 0 or in the range 2..16. This is accomplished by
 * the following code:
 *
 *           for (s = bands - 1; s >= 0; --s) {
 *              OI_INT bits = bitadjust + bitneeds[s];
 *              bits = bits < 2 ? 0 : bits;
 *              bits = bits > 16 ? 16 : bits;
 *              count += bits;
 *          }
 *
 * This loop can be optimized to perform 4 operations at a time as follows:
 *
 * Adjustment is computed as a 7 bit signed value and added to the bitneed.
 *
 * Negative allocations are zeroed by masking. (n & 0x40) >> 6 puts the
 * sign bit into bit 0, adding this to 0x7F give us a mask of 0x80
 * for -ve values and 0x7F for +ve values.
 *
 * n &= 0x7F + (n & 0x40) >> 6)
 *
 * Allocations greater than 16 are truncated to 16. Adjusted allocations are in
 * the range 0..31 so we know that bit 4 indicates values >= 16. We use this bit
 * to create a mask that zeroes bits 0 .. 3 if bit 4 is set.
 *
 * n &= (15 + (n >> 4))
 *
 * Allocations of 1 are disallowed. Add and shift creates a mask that
 * eliminates the illegal value
 *
 * n &= ((n + 14) >> 4) | 0x1E
 *
 * These operations can be performed in 8 bits without overflowing so we can
 * operate on 4 values at once.
 */


/*
 * Encoder/Decoder
 *
 * Computes adjustment +/- of bitneeds to fill bitpool and returns overall
 * adjustment and excess bits.
 *
 * @param bitpool   The bitpool we have to work within
 *
 * @param bitneeds  An array of bit needs (more acturately allocation prioritities) for each
 *                  subband across all blocks in the SBC frame
 *
 * @param subbands  The number of subbands over which the adkustment is calculated. For mono and
 *                  dual mode this is 4 or 8, for stereo or joint stereo this is 8 or 16.
 *
 * @param bitcount  A starting point for the adjustment
 *
 * @param excess    Returns the excess bits after the adjustment
 *
 * @return   The adjustment.
 */
OI_INT adjustToFitBitpool(const OI_UINT bitpool,
                          OI_UINT32 *bitneeds,
                          const OI_UINT subbands,
                          OI_UINT bitcount,
                          OI_UINT *excess)
{
    OI_INT maxBitadjust = 0;
    OI_INT bitadjust = (bitcount > bitpool) ? -8 : 8;
    OI_INT chop = 8;

    /*
     * This is essentially a binary search for the optimal adjustment value.
     */
    while ((bitcount != bitpool) && chop) {
        OI_UINT32 total = 0;
        OI_UINT count;
        OI_UINT32 adjust4;
        OI_INT i;

        adjust4 = bitadjust & 0x7F;
        adjust4 |= (adjust4 << 8);
        adjust4 |= (adjust4 << 16);

        for (i = (subbands / 4 - 1); i >= 0; --i) {
            OI_UINT32 mask;
            OI_UINT32 n = bitneeds[i] + adjust4;
            mask = 0x7F7F7F7F + ((n & 0x40404040) >> 6);
            n &= mask;
            mask = 0x0F0F0F0F + ((n & 0x10101010) >> 4);
            n &= mask;
            mask = (((n + 0x0E0E0E0E) >> 4) | 0x1E1E1E1E);
            n &= mask;
            total += n;
        }

        count = (total & 0xFFFF) + (total >> 16);
        count = (count & 0xFF) + (count >> 8);

        chop >>= 1;
        if (count > bitpool) {
            bitadjust -= chop;
        } else {
            maxBitadjust = bitadjust;
            bitcount = count;
            bitadjust += chop;
        }
    }

    *excess = bitpool - bitcount;

    return maxBitadjust;
}


/*
 * The bit allocator trys to avoid single bit allocations except as a last resort. So in the case
 * where a bitneed of 1 was passed over during the adsjustment phase 2 bits are now allocated.
 */
INLINE OI_INT allocAdjustedBits(OI_UINT8 *dest,
                                OI_INT bits,
                                OI_INT excess)
{
    if (bits < 16) {
        if (bits > 1) {
            if (excess) {
                ++bits;
                --excess;
            }
        } else if ((bits == 1) && (excess > 1)) {
            bits = 2;
            excess -= 2;
        } else {
            bits  = 0;
        }
    } else {
        bits = 16;
    }
    *dest = (OI_UINT8)bits;
    return excess;
}


/*
 * Excess bits not allocated by allocaAdjustedBits are allocated round-robin.
 */
INLINE OI_INT allocExcessBits(OI_UINT8 *dest,
                              OI_INT excess)
{
    if (*dest < 16) {
        *dest += 1;
        return excess - 1;
    } else {
        return excess;
    }
}

void oneChannelBitAllocation(OI_CODEC_SBC_COMMON_CONTEXT *common,
                             BITNEED_UNION1 *bitneeds,
                             OI_UINT ch,
                             OI_UINT bitcount)
{
    const OI_UINT8 nrof_subbands = common->frameInfo.nrof_subbands;
    OI_UINT excess;
    OI_UINT sb;
    OI_INT bitadjust;
    OI_UINT8 RESTRICT *allocBits;


    {
        OI_UINT ex;
        bitadjust = adjustToFitBitpool(common->frameInfo.bitpool, bitneeds->uint32, nrof_subbands, bitcount, &ex);
        /* We want the compiler to put excess into a register */
        excess = ex;
    }

    /*
     * Allocate adjusted bits
     */
    allocBits = &common->bits.uint8[ch ? nrof_subbands : 0];

    sb = 0;
    while (sb < nrof_subbands) {
        excess = allocAdjustedBits(&allocBits[sb], bitneeds->uint8[sb] + bitadjust, excess);
        ++sb;
    }
    sb = 0;
    while (excess) {
        excess = allocExcessBits(&allocBits[sb], excess);
        ++sb;
    }
}


void monoBitAllocation(OI_CODEC_SBC_COMMON_CONTEXT *common)
{
    BITNEED_UNION1 bitneeds;
    OI_UINT bitcount;
    OI_UINT bitpoolPreference = 0;

    bitcount = computeBitneed(common, bitneeds.uint8, 0, &bitpoolPreference);

    oneChannelBitAllocation(common, &bitneeds, 0, bitcount);
}

/**
@}
*/

#endif /* #if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE) */
