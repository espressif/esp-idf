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
This file drives SBC decoding.

@ingroup codec_internal
*/

/**
@addtogroup codec_internal
@{
*/

#include "oi_codec_sbc_private.h"
#include "oi_bitstream.h"
#include <stdio.h>

OI_CHAR * const OI_Codec_Copyright = "Copyright 2002-2007 Open Interface North America, Inc. All rights reserved";

INLINE OI_STATUS internal_DecoderReset(OI_CODEC_SBC_DECODER_CONTEXT *context,
                                       OI_UINT32 *decoderData,
                                       OI_UINT32 decoderDataBytes,
                                       OI_BYTE maxChannels,
                                       OI_BYTE pcmStride,
                                       OI_BOOL enhanced)
{
    OI_UINT i;
    OI_STATUS status;

    for (i = 0; i < sizeof(*context); i++) {
        ((char *)context)[i] = 0;
    }

#ifdef SBC_ENHANCED
    context->enhancedEnabled = enhanced ? TRUE : FALSE;
#else
    context->enhancedEnabled = FALSE;
    if (enhanced){
        return OI_STATUS_INVALID_PARAMETERS;
    }
#endif

    status = OI_CODEC_SBC_Alloc(&context->common, decoderData, decoderDataBytes, maxChannels, pcmStride);

    if (!OI_SUCCESS(status)) {
        return status;
    }

    context->common.codecInfo = OI_Codec_Copyright;
    context->common.maxBitneed = 0;
    context->limitFrameFormat = FALSE;
    OI_SBC_ExpandFrameFields(&context->common.frameInfo);

    /*PLATFORM_DECODER_RESET(context);*/

    return OI_OK;
}




/**
 * Read the SBC header up to but not including the joint stereo mask.  The syncword has already been
 * examined, and the enhanced mode flag set, by FindSyncword.
 */
INLINE void OI_SBC_ReadHeader(OI_CODEC_SBC_COMMON_CONTEXT *common, const OI_BYTE *data)
{
    OI_CODEC_SBC_FRAME_INFO *frame = &common->frameInfo;
    OI_UINT8 d1;


    OI_ASSERT(data[0] == OI_SBC_SYNCWORD || data[0] == OI_SBC_ENHANCED_SYNCWORD);

    /* Avoid filling out all these strucutures if we already remember the values
     * from last time. Just in case we get a stream corresponding to data[1] ==
     * 0, DecoderReset is responsible for ensuring the lookup table entries have
     * already been populated
     */
    d1 = data[1];
    if (d1 != frame->cachedInfo) {

        frame->freqIndex = (d1 & (BIT7 | BIT6)) >> 6;
        frame->frequency = freq_values[frame->freqIndex];

        frame->blocks = (d1 & (BIT5 | BIT4)) >> 4;
        frame->nrof_blocks = block_values[frame->blocks];

        frame->mode = (d1 & (BIT3 | BIT2)) >> 2;
        frame->nrof_channels = channel_values[frame->mode];

        frame->alloc = (d1 & BIT1) >> 1;

        frame->subbands = (d1 & BIT0);
        frame->nrof_subbands = band_values[frame->subbands];

        frame->cachedInfo = d1;
    }
    /*
     * For decode, the bit allocator needs to know the bitpool value
     */
    frame->bitpool = data[2];
    frame->crc = data[3];
}


#define LOW(x)  ((x)& 0xf)
#define HIGH(x) ((x) >> 4)

/*
 * Read scalefactor values and prepare the bitstream for OI_SBC_ReadSamples
 */
PRIVATE void OI_SBC_ReadScalefactors(OI_CODEC_SBC_COMMON_CONTEXT *common,
                             const OI_BYTE *b,
                             OI_BITSTREAM *bs)
{
    OI_UINT i = common->frameInfo.nrof_subbands * common->frameInfo.nrof_channels;
    OI_INT8 *scale_factor = common->scale_factor;
    OI_UINT f;

    if (common->frameInfo.nrof_subbands == 8 || common->frameInfo.mode != SBC_JOINT_STEREO) {
        if (common->frameInfo.mode == SBC_JOINT_STEREO) {
            common->frameInfo.join = *b++;
        } else {
            common->frameInfo.join = 0;
        }
        i /= 2;
        do {
            *scale_factor++ = HIGH(f = *b++);
            *scale_factor++ = LOW(f);
        } while (--i);
        /*
         * In this case we know that the scale factors end on a byte boundary so all we need to do
         * is initialize the bitstream.
         */
        OI_BITSTREAM_ReadInit(bs, b);
    } else {
        OI_ASSERT(common->frameInfo.nrof_subbands == 4 && common->frameInfo.mode == SBC_JOINT_STEREO);
        common->frameInfo.join = HIGH(f = *b++);
        i = (i - 1) / 2;
        do {
            *scale_factor++ = LOW(f);
            *scale_factor++ = HIGH(f = *b++);
        } while (--i);
        *scale_factor++ = LOW(f);
        /*
         * In 4-subband joint stereo mode, the joint stereo information ends on a half-byte
         * boundary, so it's necessary to use the bitstream abstraction to read it, since
         * OI_SBC_ReadSamples will need to pick up in mid-byte.
         */
        OI_BITSTREAM_ReadInit(bs, b);
        *scale_factor++ = OI_BITSTREAM_ReadUINT4Aligned(bs);
    }
}

/** Read quantized subband samples from the input bitstream and expand them. */
PRIVATE void OI_SBC_ReadSamples(OI_CODEC_SBC_DECODER_CONTEXT *context, OI_BITSTREAM *global_bs)
{
    OI_CODEC_SBC_COMMON_CONTEXT *common = &context->common;
    OI_UINT nrof_blocks = common->frameInfo.nrof_blocks;
    OI_INT32 * RESTRICT s = common->subdata;
    OI_UINT8 *ptr = global_bs->ptr.w;
    OI_UINT32 value = global_bs->value;
    OI_UINT bitPtr = global_bs->bitPtr;

    const OI_UINT iter_count = common->frameInfo.nrof_channels * common->frameInfo.nrof_subbands / 4;
    do {
        OI_UINT i;
        for (i = 0; i < iter_count; ++i) {
            OI_UINT32 sf_by4 = ((OI_UINT32*)common->scale_factor)[i];
            OI_UINT32 bits_by4 = common->bits.uint32[i];
            OI_UINT n;
            for (n = 0; n < 4; ++n) {
                OI_INT32 dequant;
                OI_UINT bits;
                OI_INT sf;

                if (OI_CPU_BYTE_ORDER == OI_LITTLE_ENDIAN_BYTE_ORDER) {
                    bits = bits_by4 & 0xFF;
                    bits_by4 >>= 8;
                    sf = sf_by4 & 0xFF;
                    sf_by4 >>= 8;
                } else {
                    bits = (bits_by4 >> 24) & 0xFF;
                    bits_by4 <<= 8;
                    sf = (sf_by4 >> 24) & 0xFF;
                    sf_by4 <<= 8;
                }
                if (bits) {
                    OI_UINT32 raw;
                    OI_BITSTREAM_READUINT(raw, bits, ptr, value, bitPtr);
                    dequant = OI_SBC_Dequant(raw, sf, bits);
                } else {
                    dequant = 0;
                }
                *s++ = dequant;
            }
        }
    } while (--nrof_blocks);
}



/**
@}
*/
