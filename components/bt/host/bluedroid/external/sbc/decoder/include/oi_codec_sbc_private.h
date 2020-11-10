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
#ifndef _OI_CODEC_SBC_PRIVATE_H
#define _OI_CODEC_SBC_PRIVATE_H

/**********************************************************************************
  $Revision: #1 $
***********************************************************************************/

/**
@file
Function prototypes and macro definitions used internally by the codec.

@ingroup codec_internal
*/

/**
@addtogroup codec_internal
@{
*/

#ifdef USE_RESTRICT_KEYWORD
#define RESTRICT restrict
#else
#define RESTRICT
#endif

#ifdef CODEC_DEBUG
#include <stdio.h>
#define ERROR(x) do { printf x; printf("\n"); } while (0)
#else
#define ERROR(x)
#endif

#ifdef TRACE_EXECUTION
#define TRACE(x) do { printf x; printf("\n"); } while (0)
#else
#define TRACE(x)
#endif

#ifndef PRIVATE
#define PRIVATE
#endif

#ifndef INLINE
#define INLINE
#endif

#include "oi_assert.h"
#include "oi_codec_sbc.h"

#ifndef OI_SBC_SYNCWORD
#define OI_SBC_SYNCWORD 0x9c
#endif

#ifndef DIVIDE
#define DIVIDE(a, b) ((a) / (b))
#endif

typedef union {
    OI_UINT8 uint8[SBC_MAX_BANDS];
    OI_UINT32 uint32[SBC_MAX_BANDS / 4];
} BITNEED_UNION1;

typedef union {
    OI_UINT8 uint8[2 * SBC_MAX_BANDS];
    OI_UINT32 uint32[2 * SBC_MAX_BANDS / 4];
} BITNEED_UNION2;

static const OI_UINT16 freq_values[] =    { 16000, 32000, 44100, 48000 };
static const OI_UINT8 block_values[] =    { 4, 8, 12, 16 };
static const OI_UINT8 channel_values[] =  { 1, 2, 2, 2 };
static const OI_UINT8 band_values[] =     { 4, 8 };


#define TEST_MODE_SENTINEL "OINA"
#define TEST_MODE_SENTINEL_LENGTH 4

/** Used internally. */
typedef struct {
    union {
        const OI_UINT8 *r;
        OI_UINT8 *w;
    } ptr;
    OI_UINT32 value;
    OI_UINT bitPtr;
} OI_BITSTREAM;


#define VALID_INT16(x) (((x) >= OI_INT16_MIN) && ((x) <= OI_INT16_MAX))
#define VALID_INT32(x) (((x) >= OI_INT32_MIN) && ((x) <= OI_INT32_MAX))

#define DCTII_8_SHIFT_IN 0
#define DCTII_8_SHIFT_OUT 16-DCTII_8_SHIFT_IN

#define DCTII_8_SHIFT_0 (DCTII_8_SHIFT_OUT)
#define DCTII_8_SHIFT_1 (DCTII_8_SHIFT_OUT)
#define DCTII_8_SHIFT_2 (DCTII_8_SHIFT_OUT)
#define DCTII_8_SHIFT_3 (DCTII_8_SHIFT_OUT)
#define DCTII_8_SHIFT_4 (DCTII_8_SHIFT_OUT)
#define DCTII_8_SHIFT_5 (DCTII_8_SHIFT_OUT)
#define DCTII_8_SHIFT_6 (DCTII_8_SHIFT_OUT-1)
#define DCTII_8_SHIFT_7 (DCTII_8_SHIFT_OUT-2)

#define DCT_SHIFT 15

#define DCTIII_4_SHIFT_IN 2
#define DCTIII_4_SHIFT_OUT 15

#define DCTIII_8_SHIFT_IN 3
#define DCTIII_8_SHIFT_OUT 14

OI_UINT computeBitneed(OI_CODEC_SBC_COMMON_CONTEXT *common,
                       OI_UINT8 *bitneeds,
                       OI_UINT ch,
                       OI_UINT *preferredBitpool);

void oneChannelBitAllocation(OI_CODEC_SBC_COMMON_CONTEXT *common,
                             BITNEED_UNION1 *bitneeds,
                             OI_UINT ch,
                             OI_UINT bitcount);


OI_INT adjustToFitBitpool(const OI_UINT bitpool,
                          OI_UINT32 *bitneeds,
                          const OI_UINT subbands,
                          OI_UINT bitcount,
                          OI_UINT *excess);

OI_INT allocAdjustedBits(OI_UINT8 *dest,
                                OI_INT bits,
                                OI_INT excess);

OI_INT allocExcessBits(OI_UINT8 *dest,
                              OI_INT excess);

PRIVATE OI_UINT32 internal_CalculateBitrate(OI_CODEC_SBC_FRAME_INFO *frame);

PRIVATE OI_UINT16 internal_CalculateFramelen(OI_CODEC_SBC_FRAME_INFO *frame);

void monoBitAllocation(OI_CODEC_SBC_COMMON_CONTEXT *common);

typedef void (*BIT_ALLOC)(OI_CODEC_SBC_COMMON_CONTEXT *common);

PRIVATE OI_STATUS internal_DecodeRaw(OI_CODEC_SBC_DECODER_CONTEXT *context,
                                     OI_UINT8 bitpool,
                                     const OI_BYTE **frameData,
                                     OI_UINT32 *frameBytes,
                                     OI_INT16 *pcmData,
                                     OI_UINT32 *pcmBytes);

OI_STATUS internal_DecoderReset(OI_CODEC_SBC_DECODER_CONTEXT *context,
                                       OI_UINT32 *decoderData,
                                       OI_UINT32 decoderDataBytes,
                                       OI_BYTE maxChannels,
                                       OI_BYTE pcmStride,
                                       OI_BOOL enhanced,
                                       OI_BOOL msbc_enable);

OI_UINT16 OI_SBC_CalculateFrameAndHeaderlen(OI_CODEC_SBC_FRAME_INFO *frame, OI_UINT *headerLen_);

PRIVATE OI_UINT32 OI_SBC_MaxBitpool(OI_CODEC_SBC_FRAME_INFO *frame);

PRIVATE void OI_SBC_ComputeBitAllocation(OI_CODEC_SBC_COMMON_CONTEXT *frame);
PRIVATE OI_UINT8 OI_SBC_CalculateChecksum(OI_CODEC_SBC_FRAME_INFO *frame, OI_BYTE const *data);

/* Transform functions */
PRIVATE void shift_buffer(SBC_BUFFER_T *dest, SBC_BUFFER_T *src, OI_UINT wordCount);
PRIVATE void cosineModulateSynth4(SBC_BUFFER_T *RESTRICT out, OI_INT32 const *RESTRICT in);
PRIVATE void SynthWindow40_int32_int32_symmetry_with_sum(OI_INT16 *pcm, SBC_BUFFER_T buffer[80], OI_UINT strideShift);

void dct3_4(OI_INT32 *RESTRICT out, OI_INT32 const *RESTRICT in);
PRIVATE void analyze4_generated(SBC_BUFFER_T analysisBuffer[RESTRICT 40],
                                OI_INT16 *pcm,
                                OI_UINT strideShift,
                                OI_INT32 subband[4]);

void dct3_8(OI_INT32 *RESTRICT out, OI_INT32 const *RESTRICT in);

PRIVATE void analyze8_generated(SBC_BUFFER_T analysisBuffer[RESTRICT 80],
                                OI_INT16 *pcm,
                                OI_UINT strideShift,
                                OI_INT32 subband[8]);

#ifdef SBC_ENHANCED
PRIVATE void analyze8_enhanced_generated(SBC_BUFFER_T analysisBuffer[RESTRICT 112],
        OI_INT16 *pcm,
        OI_UINT strideShift,
        OI_INT32 subband[8]);
#endif

/* Decoder functions */

void OI_SBC_ReadHeader(OI_CODEC_SBC_COMMON_CONTEXT *common, const OI_BYTE *data);
PRIVATE void OI_SBC_ReadScalefactors(OI_CODEC_SBC_COMMON_CONTEXT *common, const OI_BYTE *b, OI_BITSTREAM *bs);
PRIVATE void OI_SBC_ReadSamples(OI_CODEC_SBC_DECODER_CONTEXT *common, OI_BITSTREAM *ob);
PRIVATE void OI_SBC_ReadSamplesJoint(OI_CODEC_SBC_DECODER_CONTEXT *common, OI_BITSTREAM *global_bs);
PRIVATE void OI_SBC_SynthFrame(OI_CODEC_SBC_DECODER_CONTEXT *context, OI_INT16 *pcm, OI_UINT start_block, OI_UINT nrof_blocks);
OI_INT32 OI_SBC_Dequant(OI_UINT32 raw, OI_UINT scale_factor, OI_UINT bits);
PRIVATE OI_BOOL OI_SBC_ExamineCommandPacket(OI_CODEC_SBC_DECODER_CONTEXT *context, const OI_BYTE *data, OI_UINT32 len);
PRIVATE void OI_SBC_GenerateTestSignal(OI_INT16 pcmData[][2], OI_UINT32 sampleCount);

PRIVATE void OI_SBC_ExpandFrameFields(OI_CODEC_SBC_FRAME_INFO *frame);
PRIVATE OI_STATUS OI_CODEC_SBC_Alloc(OI_CODEC_SBC_COMMON_CONTEXT *common,
                                     OI_UINT32 *codecDataAligned,
                                     OI_UINT32 codecDataBytes,
                                     OI_UINT8 maxChannels,
                                     OI_UINT8 pcmStride);
/**
@}
*/

#endif /* _OI_CODEC_SBC_PRIVATE_H */
