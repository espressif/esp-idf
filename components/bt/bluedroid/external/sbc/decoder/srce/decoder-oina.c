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

/**
@file
This file exposes OINA-specific interfaces to decoder functions.

@ingroup codec_internal
*/

/**
@addtogroup codec_internal
@{
*/

#include "bt_target.h"
#include <oi_codec_sbc_private.h>

#if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE)

OI_STATUS OI_CODEC_SBC_DecoderConfigureRaw(OI_CODEC_SBC_DECODER_CONTEXT *context,
        OI_BOOL enhanced,
        OI_UINT8 frequency,
        OI_UINT8 mode,
        OI_UINT8 subbands,
        OI_UINT8 blocks,
        OI_UINT8 alloc,
        OI_UINT8 maxBitpool)
{
    if (frequency > SBC_FREQ_48000) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    if (enhanced) {
#ifdef SBC_ENHANCED
        if (subbands != SBC_SUBBANDS_8) {
            return OI_STATUS_INVALID_PARAMETERS;
        }
#else
        return OI_STATUS_INVALID_PARAMETERS;
#endif
    }

    if (mode > SBC_JOINT_STEREO) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    if (subbands > SBC_SUBBANDS_8) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    if (blocks > SBC_BLOCKS_16) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    if (alloc > SBC_SNR) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

#ifdef SBC_ENHANCED
    context->common.frameInfo.enhanced = enhanced;
#else
    context->common.frameInfo.enhanced = FALSE;
#endif
    context->common.frameInfo.freqIndex = frequency;
    context->common.frameInfo.mode = mode;
    context->common.frameInfo.subbands = subbands;
    context->common.frameInfo.blocks = blocks;
    context->common.frameInfo.alloc = alloc;
    context->common.frameInfo.bitpool = maxBitpool;

    OI_SBC_ExpandFrameFields(&context->common.frameInfo);

    if (context->common.frameInfo.nrof_channels >= context->common.pcmStride) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    return OI_OK;
}



OI_STATUS OI_CODEC_SBC_DecodeRaw(OI_CODEC_SBC_DECODER_CONTEXT *context,
                                 OI_UINT8 bitpool,
                                 const OI_BYTE **frameData,
                                 OI_UINT32 *frameBytes,
                                 OI_INT16 *pcmData,
                                 OI_UINT32 *pcmBytes)
{
    return internal_DecodeRaw(context,
                              bitpool,
                              frameData,
                              frameBytes,
                              pcmData,
                              pcmBytes);
}

OI_STATUS OI_CODEC_SBC_DecoderLimit(OI_CODEC_SBC_DECODER_CONTEXT *context,
                                    OI_BOOL                       enhanced,
                                    OI_UINT8                      subbands)
{
    if (enhanced) {
#ifdef SBC_ENHANCED
        context->enhancedEnabled = TRUE;
#else
        context->enhancedEnabled = FALSE;
#endif
    } else {
        context->enhancedEnabled = FALSE;
    }
    context->restrictSubbands = subbands;
    context->limitFrameFormat = TRUE;
    return OI_OK;
}


/**
@}
*/

#endif /* #if (defined(SBC_DEC_INCLUDED) && SBC_DEC_INCLUDED == TRUE) */
