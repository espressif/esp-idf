/******************************************************************************
 *
 *  Copyright (C) 2014 The Android Open Source Project
 *  Copyright 2006 Open Interface North America, Inc. All rights reserved.
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

/**@addtogroup codec_internal */
/**@{*/

#include "common/bt_target.h"
#include "oi_codec_sbc_private.h"
#include "oi_bitstream.h"

#if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE)

#define SPECIALIZE_READ_SAMPLES_JOINT

/**
 * Scans through a buffer looking for a codec syncword. If the decoder has been
 * set for enhanced operation using OI_CODEC_SBC_DecoderReset(), it will search
 * for both a standard and an enhanced syncword.
 */
PRIVATE OI_STATUS FindSyncword(OI_CODEC_SBC_DECODER_CONTEXT *context,
                               const OI_BYTE **frameData,
                               OI_UINT32 *frameBytes)
{
#ifdef SBC_ENHANCED
    OI_BYTE search1 = OI_SBC_SYNCWORD;
    OI_BYTE search2 = OI_SBC_ENHANCED_SYNCWORD;
#endif // SBC_ENHANCED

    if (*frameBytes == 0) {
        return OI_CODEC_SBC_NOT_ENOUGH_HEADER_DATA;
    }

#ifdef SBC_ENHANCED
    if (context->limitFrameFormat && context->enhancedEnabled) {
        /* If the context is restricted, only search for specified SYNCWORD */
        search1 = search2;
    } else if (context->enhancedEnabled == FALSE) {
        /* If enhanced is not enabled, only search for classic SBC SYNCWORD*/
        search2 = search1;
    }
    while (*frameBytes && (**frameData != search1) && (**frameData != search2)) {
        (*frameBytes)--;
        (*frameData)++;
    }
    if (*frameBytes) {
        /* Syncword found, *frameData points to it, and *frameBytes correctly
         * reflects the number of bytes available to read, including the
         * syncword. */
        context->common.frameInfo.enhanced = (**frameData == OI_SBC_ENHANCED_SYNCWORD);
        return OI_OK;
    } else {
        /* No syncword was found anywhere in the provided input data.
         * *frameData points past the end of the original input, and
         * *frameBytes is 0. */
        return OI_CODEC_SBC_NO_SYNCWORD;
    }
#else  // SBC_ENHANCED
    while (*frameBytes
        && (!(context->sbc_mode == OI_SBC_MODE_STD && **frameData == OI_SBC_SYNCWORD))
        && (!(context->sbc_mode == OI_SBC_MODE_MSBC && **frameData == OI_mSBC_SYNCWORD))) {
        (*frameBytes)--;
        (*frameData)++;
    }
    if (*frameBytes) {
        /* Syncword found, *frameData points to it, and *frameBytes correctly
         * reflects the number of bytes available to read, including the
         * syncword. */
        context->common.frameInfo.enhanced = FALSE;
        return OI_OK;
    } else {
        /* No syncword was found anywhere in the provided input data.
         * *frameData points past the end of the original input, and
         * *frameBytes is 0. */
        return OI_CODEC_SBC_NO_SYNCWORD;
    }
#endif // SBC_ENHANCED
}

static OI_STATUS DecodeBody(OI_CODEC_SBC_DECODER_CONTEXT *context,
                            const OI_BYTE *bodyData,
                            OI_INT16 *pcmData,
                            OI_UINT32 *pcmBytes,
                            OI_BOOL allowPartial)
{
    OI_BITSTREAM bs;
    OI_UINT frameSamples = context->common.frameInfo.nrof_blocks * context->common.frameInfo.nrof_subbands;
    OI_UINT decode_block_count;

    /*
     * Based on the header data, make sure that there is enough room to write the output samples.
     */
    if (*pcmBytes < (sizeof(OI_INT16) * frameSamples * context->common.pcmStride) && !allowPartial) {
        /* If we're not allowing partial decodes, we need room for the entire
         * codec frame */
        TRACE(("-OI_CODEC_SBC_Decode: OI_CODEC_SBC_NOT_ENOUGH_AUDIO_DATA"));
        return OI_CODEC_SBC_NOT_ENOUGH_AUDIO_DATA;
    } else if (*pcmBytes < sizeof (OI_INT16) * context->common.frameInfo.nrof_subbands * context->common.pcmStride) {
        /* Even if we're allowing partials, we can still only decode on a frame
         * boundary */
        return OI_CODEC_SBC_NOT_ENOUGH_AUDIO_DATA;
    }

    if (context->bufferedBlocks == 0) {
        TRACE(("Reading scalefactors"));
        OI_SBC_ReadScalefactors(&context->common, bodyData, &bs);

        TRACE(("Computing bit allocation"));
        OI_SBC_ComputeBitAllocation(&context->common);

        TRACE(("Reading samples"));
        if (context->common.frameInfo.mode == SBC_JOINT_STEREO) {
            OI_SBC_ReadSamplesJoint(context, &bs);
        } else {
            OI_SBC_ReadSamples(context, &bs);
        }

        context->bufferedBlocks = context->common.frameInfo.nrof_blocks;
    }

    if (allowPartial) {
        decode_block_count = *pcmBytes / sizeof(OI_INT16) / context->common.pcmStride / context->common.frameInfo.nrof_subbands;

        if (decode_block_count > context->bufferedBlocks) {
            decode_block_count = context->bufferedBlocks;
        }

    } else {
        decode_block_count = context->common.frameInfo.nrof_blocks;
    }

    TRACE(("Synthesizing frame"));
    {
        OI_UINT start_block = context->common.frameInfo.nrof_blocks - context->bufferedBlocks;
        OI_SBC_SynthFrame(context, pcmData, start_block, decode_block_count);
    }

    OI_ASSERT(context->bufferedBlocks >= decode_block_count);
    context->bufferedBlocks -= decode_block_count;

    frameSamples = decode_block_count * context->common.frameInfo.nrof_subbands;

    /*
     * When decoding mono into a stride-2 array, copy pcm data to second channel
     */
    if (context->common.frameInfo.nrof_channels == 1 && context->common.pcmStride == 2) {
        OI_UINT i;
        for (i = 0; i < frameSamples; ++i) {
            pcmData[2 * i + 1] = pcmData[2 * i];
        }
    }

    /*
     * Return number of pcm bytes generated by the decode operation.
     */
    *pcmBytes = frameSamples * sizeof(OI_INT16) * context->common.pcmStride;
    if (context->bufferedBlocks > 0) {
        return OI_CODEC_SBC_PARTIAL_DECODE;
    } else {
        return OI_OK;
    }
}

PRIVATE OI_STATUS internal_DecodeRaw(OI_CODEC_SBC_DECODER_CONTEXT *context,
                                     OI_UINT8 bitpool,
                                     const OI_BYTE **frameData,
                                     OI_UINT32 *frameBytes,
                                     OI_INT16 *pcmData,
                                     OI_UINT32 *pcmBytes)
{
    OI_STATUS status;
    OI_UINT bodyLen;

    TRACE(("+OI_CODEC_SBC_DecodeRaw"));

    if (context->bufferedBlocks == 0) {
        /*
         * The bitallocator needs to know the bitpool value.
         */
        context->common.frameInfo.bitpool = bitpool;
        /*
         * Compute the frame length and check we have enough frame data to proceed
         */
        bodyLen = OI_CODEC_SBC_CalculateFramelen(&context->common.frameInfo) - SBC_HEADER_LEN;
        if (*frameBytes < bodyLen) {
            TRACE(("-OI_CODEC_SBC_Decode: OI_CODEC_SBC_NOT_ENOUGH_BODY_DATA"));
            return OI_CODEC_SBC_NOT_ENOUGH_BODY_DATA;
        }
    } else {
        bodyLen = 0;
    }
    /*
     * Decode the SBC data. Pass TRUE to DecodeBody to allow partial decoding of
     * tones.
     */
    status = DecodeBody(context, *frameData, pcmData, pcmBytes, TRUE);
    if (OI_SUCCESS(status) || status == OI_CODEC_SBC_PARTIAL_DECODE) {
        *frameData += bodyLen;
        *frameBytes -= bodyLen;
    }
    TRACE(("-OI_CODEC_SBC_DecodeRaw: %d", status));
    return status;
}

OI_STATUS OI_CODEC_SBC_DecoderReset(OI_CODEC_SBC_DECODER_CONTEXT *context,
                                    OI_UINT32 *decoderData,
                                    OI_UINT32 decoderDataBytes,
                                    OI_UINT8 maxChannels,
                                    OI_UINT8 pcmStride,
                                    OI_BOOL enhanced,
                                    OI_BOOL msbc_enable)
{
    return internal_DecoderReset(context, decoderData, decoderDataBytes, maxChannels, pcmStride, enhanced, msbc_enable);
}

OI_STATUS OI_CODEC_SBC_DecodeFrame(OI_CODEC_SBC_DECODER_CONTEXT *context,
                                   const OI_BYTE **frameData,
                                   OI_UINT32 *frameBytes,
                                   OI_INT16 *pcmData,
                                   OI_UINT32 *pcmBytes)
{
    OI_STATUS status;
    OI_UINT framelen;
    OI_UINT8 crc;

    TRACE(("+OI_CODEC_SBC_DecodeFrame"));

    TRACE(("Finding syncword"));
    status = FindSyncword(context, frameData, frameBytes);
    if (!OI_SUCCESS(status)) {
        return status;
    }

    /* Make sure enough data remains to read the header. */
    if (*frameBytes < SBC_HEADER_LEN) {
        TRACE(("-OI_CODEC_SBC_DecodeFrame: OI_CODEC_SBC_NOT_ENOUGH_HEADER_DATA"));
        return OI_CODEC_SBC_NOT_ENOUGH_HEADER_DATA;
    }

    TRACE(("Reading Header"));
    OI_SBC_ReadHeader(&context->common, *frameData);

    /*
     * Some implementations load the decoder into RAM and use overlays for 4 vs 8 subbands. We need
     * to ensure that the SBC parameters for this frame are compatible with the restrictions imposed
     * by the loaded overlays.
     */
    if (context->limitFrameFormat && (context->common.frameInfo.subbands != context->restrictSubbands)) {
        ERROR(("SBC parameters incompatible with loaded overlay"));
        return OI_STATUS_INVALID_PARAMETERS;
    }

    if (context->common.frameInfo.nrof_channels > context->common.maxChannels) {
        ERROR(("SBC parameters incompatible with number of channels specified during reset"));
        return OI_STATUS_INVALID_PARAMETERS;
    }

    if (context->common.pcmStride < 1 || context->common.pcmStride > 2) {
        ERROR(("PCM stride not set correctly during reset"));
        return OI_STATUS_INVALID_PARAMETERS;
    }

    /*
     * At this point a header has been read. However, it's possible that we found a false syncword,
     * so the header data might be invalid. Make sure we have enough bytes to read in the
     * CRC-protected header, but don't require we have the whole frame. That way, if it turns out
     * that we're acting on bogus header data, we don't stall the decoding process by waiting for
     * data that we don't actually need.
     */
    framelen = OI_CODEC_SBC_CalculateFramelen(&context->common.frameInfo);
    if (*frameBytes < framelen) {
        TRACE(("-OI_CODEC_SBC_DecodeFrame: OI_CODEC_SBC_NOT_ENOUGH_BODY_DATA"));
        return OI_CODEC_SBC_NOT_ENOUGH_BODY_DATA;
    }

    TRACE(("Calculating checksum"));

    crc = OI_SBC_CalculateChecksum(&context->common.frameInfo, *frameData);
    if (crc != context->common.frameInfo.crc) {
        TRACE(("CRC Mismatch:  calc=%02x read=%02x\n", crc, context->common.frameInfo.crc));
        TRACE(("-OI_CODEC_SBC_DecodeFrame: OI_CODEC_SBC_CHECKSUM_MISMATCH"));
        return OI_CODEC_SBC_CHECKSUM_MISMATCH;
    }

#ifdef OI_DEBUG
    /*
     * Make sure the bitpool values are sane.
     */
    if ((context->common.frameInfo.bitpool < SBC_MIN_BITPOOL) && !context->common.frameInfo.enhanced) {
        ERROR(("Bitpool too small: %d (must be >= 2)", context->common.frameInfo.bitpool));
        return OI_STATUS_INVALID_PARAMETERS;
    }
    if (context->common.frameInfo.bitpool > OI_SBC_MaxBitpool(&context->common.frameInfo)) {
        ERROR(("Bitpool too large: %d (must be <= %ld)", context->common.frameInfo.bitpool, OI_SBC_MaxBitpool(&context->common.frameInfo)));
        return OI_STATUS_INVALID_PARAMETERS;
    }
#endif

    /*
     * Now decode the SBC data. Partial decode is not yet implemented for an SBC
     * stream, so pass FALSE to decode body to have it enforce the old rule that
     * you have to decode a whole packet at a time.
     */
    status = DecodeBody(context, *frameData + SBC_HEADER_LEN, pcmData, pcmBytes, FALSE);
    if (OI_SUCCESS(status)) {
        *frameData += framelen;
        *frameBytes -= framelen;
    }
    TRACE(("-OI_CODEC_SBC_DecodeFrame: %d", status));

    return status;
}

OI_STATUS OI_CODEC_SBC_SkipFrame(OI_CODEC_SBC_DECODER_CONTEXT *context,
                                 const OI_BYTE **frameData,
                                 OI_UINT32 *frameBytes)
{
    OI_STATUS status;
    OI_UINT framelen;
    OI_UINT headerlen;
    OI_UINT8 crc;

    status = FindSyncword(context, frameData, frameBytes);
    if (!OI_SUCCESS(status)) {
        return status;
    }
    if (*frameBytes < SBC_HEADER_LEN) {
        return OI_CODEC_SBC_NOT_ENOUGH_HEADER_DATA;
    }
    OI_SBC_ReadHeader(&context->common, *frameData);
    framelen = OI_SBC_CalculateFrameAndHeaderlen(&context->common.frameInfo, &headerlen);
    if (*frameBytes < headerlen) {
        return OI_CODEC_SBC_NOT_ENOUGH_HEADER_DATA;
    }
    crc = OI_SBC_CalculateChecksum(&context->common.frameInfo, *frameData);
    if (crc != context->common.frameInfo.crc) {
        return OI_CODEC_SBC_CHECKSUM_MISMATCH;
    }
    if (*frameBytes < framelen) {
        return OI_CODEC_SBC_NOT_ENOUGH_BODY_DATA;
    }
    context->bufferedBlocks = 0;
    *frameData += framelen;
    *frameBytes -= framelen;
    return OI_OK;
}

OI_UINT8 OI_CODEC_SBC_FrameCount(OI_BYTE  *frameData,
                                 OI_UINT32 frameBytes)
{
    OI_UINT8 mode;
    OI_UINT8 blocks;
    OI_UINT8 subbands;
    OI_UINT8 frameCount = 0;
    OI_UINT  frameLen;

    while (frameBytes) {
        while (frameBytes && ((frameData[0] & 0xFE) != 0x9C)) {
            frameData++;
            frameBytes--;
        }

        if (frameBytes < SBC_HEADER_LEN) {
            return frameCount;
        }

        /* Extract and translate required fields from Header */
        subbands = mode = blocks = frameData[1];;
        mode = (mode & (BIT3 | BIT2)) >> 2;
        blocks = block_values[(blocks & (BIT5 | BIT4)) >> 4];
        subbands = band_values[(subbands & BIT0)];

        /* Inline logic to avoid corrupting context */
        frameLen = blocks * frameData[2];
        switch (mode) {
        case SBC_JOINT_STEREO:
            frameLen += subbands + (8 * subbands);
            break;

        case SBC_DUAL_CHANNEL:
            frameLen *= 2;
        /* fall through */

        default:
            if (mode == SBC_MONO) {
                frameLen += 4 * subbands;
            } else {
                frameLen += 8 * subbands;
            }
        }

        frameCount++;
        frameLen = SBC_HEADER_LEN + (frameLen + 7) / 8;
        if (frameBytes > frameLen) {
            frameBytes -= frameLen;
            frameData += frameLen;
        } else {
            frameBytes = 0;
        }
    }
    return frameCount;
}

/** Read quantized subband samples from the input bitstream and expand them. */

#ifdef SPECIALIZE_READ_SAMPLES_JOINT

PRIVATE void OI_SBC_ReadSamplesJoint4(OI_CODEC_SBC_DECODER_CONTEXT *context, OI_BITSTREAM *global_bs)
{
#define NROF_SUBBANDS 4
#include "readsamplesjoint.inc"
#undef NROF_SUBBANDS
}

PRIVATE void OI_SBC_ReadSamplesJoint8(OI_CODEC_SBC_DECODER_CONTEXT *context, OI_BITSTREAM *global_bs)
{
#define NROF_SUBBANDS 8
#include "readsamplesjoint.inc"
#undef NROF_SUBBANDS
}

typedef void (*READ_SAMPLES)(OI_CODEC_SBC_DECODER_CONTEXT *context, OI_BITSTREAM *global_bs);

static const READ_SAMPLES SpecializedReadSamples[] = {
    OI_SBC_ReadSamplesJoint4,
    OI_SBC_ReadSamplesJoint8
};

#endif /* SPECIALIZE_READ_SAMPLES_JOINT */


PRIVATE void OI_SBC_ReadSamplesJoint(OI_CODEC_SBC_DECODER_CONTEXT *context, OI_BITSTREAM *global_bs)
{
    OI_CODEC_SBC_COMMON_CONTEXT *common = &context->common;
    OI_UINT nrof_subbands = common->frameInfo.nrof_subbands;
#ifdef SPECIALIZE_READ_SAMPLES_JOINT
    OI_ASSERT((nrof_subbands >> 3u) <= 1u);
    SpecializedReadSamples[nrof_subbands >> 3](context, global_bs);
#else

#define NROF_SUBBANDS nrof_subbands
#include "readsamplesjoint.inc"
#undef NROF_SUBBANDS
#endif /* SPECIALIZE_READ_SAMPLES_JOINT */
}

/**@}*/

#endif /* #if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE) */
