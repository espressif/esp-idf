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

#include <stdlib.h>
#include <oi_codec_sbc_private.h>

/**********************************************************************************
  $Revision: #1 $
***********************************************************************************/

PRIVATE OI_STATUS OI_CODEC_SBC_Alloc(OI_CODEC_SBC_COMMON_CONTEXT *common,
                                     OI_UINT32 *codecDataAligned,
                                     OI_UINT32 codecDataBytes,
                                     OI_UINT8 maxChannels,
                                     OI_UINT8 pcmStride)
{
    int i;
    size_t filterBufferCount;
    size_t subdataSize;
    OI_BYTE *codecData = (OI_BYTE*)codecDataAligned;

    if (maxChannels < 1 || maxChannels > 2) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    if (pcmStride < 1 || pcmStride > maxChannels) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    common->maxChannels = maxChannels;
    common->pcmStride = pcmStride;

    /* Compute sizes needed for the memory regions, and bail if we don't have
     * enough memory for them. */
    subdataSize = maxChannels * sizeof(common->subdata[0]) * SBC_MAX_BANDS * SBC_MAX_BLOCKS;
    if (subdataSize > codecDataBytes) {
        return OI_STATUS_OUT_OF_MEMORY;
    }

    filterBufferCount = (codecDataBytes - subdataSize) / (sizeof(common->filterBuffer[0][0]) * SBC_MAX_BANDS * maxChannels);
    if (filterBufferCount < SBC_CODEC_MIN_FILTER_BUFFERS) {
        return OI_STATUS_OUT_OF_MEMORY;
    }
    common->filterBufferLen = filterBufferCount * SBC_MAX_BANDS;

    /* Allocate memory for the subband data */
    common->subdata = (OI_INT32*)codecData;
    codecData += subdataSize;
    OI_ASSERT(codecDataBytes >= subdataSize);
    codecDataBytes -= subdataSize;

    /* Allocate memory for the synthesis buffers */
    for (i = 0; i < maxChannels; ++i) {
        size_t allocSize = common->filterBufferLen * sizeof(common->filterBuffer[0][0]);
        common->filterBuffer[i] = (SBC_BUFFER_T*)codecData;
        OI_ASSERT(codecDataBytes >= allocSize);
        codecData += allocSize;
        codecDataBytes -= allocSize;
    }

    return OI_OK;
}
