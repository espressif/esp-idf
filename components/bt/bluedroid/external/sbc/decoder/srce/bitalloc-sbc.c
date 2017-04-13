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
#include "bt_target.h"
#include <oi_codec_sbc_private.h>

#if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE)

static void dualBitAllocation(OI_CODEC_SBC_COMMON_CONTEXT *common)
{
    OI_UINT bitcountL;
    OI_UINT bitcountR;
    OI_UINT bitpoolPreferenceL = 0;
    OI_UINT bitpoolPreferenceR = 0;
    BITNEED_UNION1 bitneedsL;
    BITNEED_UNION1 bitneedsR;

    bitcountL = computeBitneed(common, bitneedsL.uint8, 0, &bitpoolPreferenceL);
    bitcountR = computeBitneed(common, bitneedsR.uint8, 1, &bitpoolPreferenceR);

    oneChannelBitAllocation(common, &bitneedsL, 0, bitcountL);
    oneChannelBitAllocation(common, &bitneedsR, 1, bitcountR);
}

static void stereoBitAllocation(OI_CODEC_SBC_COMMON_CONTEXT *common)
{
    const OI_UINT nrof_subbands = common->frameInfo.nrof_subbands;
    BITNEED_UNION2 bitneeds;
    OI_UINT excess;
    OI_INT bitadjust;
    OI_UINT bitcount;
    OI_UINT sbL;
    OI_UINT sbR;
    OI_UINT bitpoolPreference = 0;

    bitcount = computeBitneed(common, &bitneeds.uint8[0], 0, &bitpoolPreference);
    bitcount += computeBitneed(common, &bitneeds.uint8[nrof_subbands], 1, &bitpoolPreference);

    {
        OI_UINT ex;
        bitadjust = adjustToFitBitpool(common->frameInfo.bitpool, bitneeds.uint32, 2 * nrof_subbands, bitcount, &ex);
        /* We want the compiler to put excess into a register */
        excess = ex;
    }
    sbL = 0;
    sbR = nrof_subbands;
    while (sbL < nrof_subbands) {
        excess = allocAdjustedBits(&common->bits.uint8[sbL], bitneeds.uint8[sbL] + bitadjust, excess);
        ++sbL;
        excess = allocAdjustedBits(&common->bits.uint8[sbR], bitneeds.uint8[sbR] + bitadjust, excess);
        ++sbR;
    }
    sbL = 0;
    sbR = nrof_subbands;
    while (excess) {
        excess = allocExcessBits(&common->bits.uint8[sbL], excess);
        ++sbL;
        if (!excess) {
            break;
        }
        excess = allocExcessBits(&common->bits.uint8[sbR], excess);
        ++sbR;
    }

}

static const BIT_ALLOC balloc[] = {
    monoBitAllocation,    /* SBC_MONO */
    dualBitAllocation,    /* SBC_DUAL_CHANNEL */
    stereoBitAllocation,  /* SBC_STEREO */
    stereoBitAllocation   /* SBC_JOINT_STEREO */
};


PRIVATE void OI_SBC_ComputeBitAllocation(OI_CODEC_SBC_COMMON_CONTEXT *common)
{
    OI_ASSERT(common->frameInfo.bitpool <= OI_SBC_MaxBitpool(&common->frameInfo));
    OI_ASSERT(common->frameInfo.mode < OI_ARRAYSIZE(balloc));

    /*
     * Using an array of function pointers prevents the compiler from creating a suboptimal
     * monolithic inlined bit allocation function.
     */
    balloc[common->frameInfo.mode](common);
}

OI_UINT32 OI_CODEC_SBC_CalculateBitrate(OI_CODEC_SBC_FRAME_INFO *frame)
{
    return internal_CalculateBitrate(frame);
}

/*
 * Return the current maximum bitneed and clear it.
 */
OI_UINT8 OI_CODEC_SBC_GetMaxBitneed(OI_CODEC_SBC_COMMON_CONTEXT *common)
{
    OI_UINT8 max = common->maxBitneed;

    common->maxBitneed = 0;
    return max;
}

/*
 * Calculates the bitpool size for a given frame length
 */
OI_UINT16 OI_CODEC_SBC_CalculateBitpool(OI_CODEC_SBC_FRAME_INFO *frame,
                                        OI_UINT16 frameLen)
{
    OI_UINT16 nrof_subbands = frame->nrof_subbands;
    OI_UINT16 nrof_blocks = frame->nrof_blocks;
    OI_UINT16 hdr;
    OI_UINT16 bits;

    if (frame->mode == SBC_JOINT_STEREO) {
        hdr = 9 * nrof_subbands;
    } else {
        if (frame->mode == SBC_MONO) {
            hdr = 4 * nrof_subbands;
        } else {
            hdr = 8 * nrof_subbands;
        }
        if (frame->mode == SBC_DUAL_CHANNEL) {
            nrof_blocks *= 2;
        }
    }
    bits = 8 * (frameLen - SBC_HEADER_LEN) - hdr;
    return DIVIDE(bits, nrof_blocks);
}

OI_UINT16 OI_CODEC_SBC_CalculatePcmBytes(OI_CODEC_SBC_COMMON_CONTEXT *common)
{
    return sizeof(OI_INT16) * common->pcmStride * common->frameInfo.nrof_subbands * common->frameInfo.nrof_blocks;
}


OI_UINT16 OI_CODEC_SBC_CalculateFramelen(OI_CODEC_SBC_FRAME_INFO *frame)
{
    return internal_CalculateFramelen(frame);
}

/**@}*/
#endif /* #if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE) */
