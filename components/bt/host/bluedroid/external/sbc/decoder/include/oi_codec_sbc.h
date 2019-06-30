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

#ifndef _OI_CODEC_SBC_CORE_H
#define _OI_CODEC_SBC_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
@file
Declarations of codec functions, data types, and macros.

@ingroup codec_lib
*/

/**
@addtogroup codec_lib
@{
*/

/* Non-BM3 users of of the codec must include oi_codec_sbc_bm3defs.h prior to
 * including this file, or else these includes will fail because the BM3 SDK is
 * not in the include path */
#ifndef _OI_CODEC_SBC_BM3DEFS_H
#include "oi_stddefs.h"
#include "oi_status.h"
#endif

#include <stdint.h>

#define SBC_MAX_CHANNELS 2
#define SBC_MAX_BANDS 8
#define SBC_MAX_BLOCKS 16
#define SBC_MIN_BITPOOL 2   /**< Minimum size of the bit allocation pool used to encode the stream */
#define SBC_MAX_BITPOOL 250 /**< Maximum size of the bit allocation pool used to encode the stream */
#define SBC_MAX_ONE_CHANNEL_BPS 320000
#define SBC_MAX_TWO_CHANNEL_BPS 512000


#define SBC_WBS_BITRATE 62000
#define SBC_WBS_BITPOOL 27
#define SBC_WBS_NROF_BLOCKS 16
#define SBC_WBS_FRAME_LEN 62
#define SBC_WBS_SAMPLES_PER_FRAME 128


#define SBC_HEADER_LEN 4
#define SBC_MAX_FRAME_LEN (SBC_HEADER_LEN + \
                             ((SBC_MAX_BANDS * SBC_MAX_CHANNELS / 2) + \
                              (SBC_MAX_BANDS + SBC_MAX_BLOCKS * SBC_MAX_BITPOOL + 7)/8))
#define SBC_MAX_SAMPLES_PER_FRAME   (SBC_MAX_BANDS * SBC_MAX_BLOCKS)

#define SBC_MAX_SCALEFACTOR_BYTES ((4*(SBC_MAX_CHANNELS * SBC_MAX_BANDS) + 7)/8)

#define OI_SBC_SYNCWORD 0x9c
#define OI_SBC_ENHANCED_SYNCWORD 0x9d
#define OI_mSBC_SYNCWORD 0xad

#define OI_SBC_MODE_STD      0
#define OI_SBC_MODE_MSBC     1

/**@name Sampling frequencies */
/**@{*/
#define SBC_FREQ_16000 0 /**< The sampling frequency is 16 kHz. One possible value for the @a frequency parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_FREQ_32000 1 /**< The sampling frequency is 32 kHz. One possible value for the @a frequency parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_FREQ_44100 2 /**< The sampling frequency is 44.1 kHz. One possible value for the @a frequency parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_FREQ_48000 3 /**< The sampling frequency is 48 kHz. One possible value for the @a frequency parameter of OI_CODEC_SBC_EncoderConfigure() */
/**@}*/

/**@name Channel modes */
/**@{*/
#define SBC_MONO 0         /**< The mode of the encoded channel is mono. One possible value for the @a mode parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_DUAL_CHANNEL 1 /**< The mode of the encoded channel is dual-channel. One possible value for the @a mode parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_STEREO 2       /**< The mode of the encoded channel is stereo. One possible value for the @a mode parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_JOINT_STEREO 3 /**< The mode of the encoded channel is joint stereo. One possible value for the @a mode parameter of OI_CODEC_SBC_EncoderConfigure() */
/**@}*/

/**@name Subbands */
/**@{*/
#define SBC_SUBBANDS_4  0 /**< The encoded stream has 4 subbands. One possible value for the @a subbands parameter of OI_CODEC_SBC_EncoderConfigure()*/
#define SBC_SUBBANDS_8  1 /**< The encoded stream has 8 subbands. One possible value for the @a subbands parameter of OI_CODEC_SBC_EncoderConfigure() */
/**@}*/

/**@name Block lengths */
/**@{*/
#define SBC_BLOCKS_4    0 /**< A block size of 4 blocks was used to encode the stream. One possible value for the @a blocks parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_BLOCKS_8    1 /**< A block size of 8 blocks was used to encode the stream is. One possible value for the @a blocks parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_BLOCKS_12   2 /**< A block size of 12 blocks was used to encode the stream. One possible value for the @a blocks parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_BLOCKS_16   3 /**< A block size of 16 blocks was used to encode the stream. One possible value for the @a blocks parameter of OI_CODEC_SBC_EncoderConfigure() */
/**@}*/

/**@name Bit allocation methods */
/**@{*/
#define SBC_LOUDNESS 0    /**< The bit allocation method. One possible value for the @a loudness parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_SNR 1         /**< The bit allocation method. One possible value for the @a loudness parameter of OI_CODEC_SBC_EncoderConfigure() */
/**@}*/

/**
@}

@addtogroup codec_internal
@{
*/

typedef OI_INT16 SBC_BUFFER_T;


/** Used internally. */
typedef struct {
    OI_UINT16 frequency;    /**< The sampling frequency. Input parameter. */
    OI_UINT8 freqIndex;

    OI_UINT8 nrof_blocks;   /**< The block size used to encode the stream. Input parameter. */
    OI_UINT8 blocks;


    OI_UINT8 nrof_subbands; /**< The number of subbands of the encoded stream. Input parameter. */
    OI_UINT8 subbands;

    OI_UINT8 mode;          /**< The mode of the encoded channel. Input parameter. */
    OI_UINT8 nrof_channels; /**< The number of channels of the encoded stream. */

    OI_UINT8 alloc;         /**< The bit allocation method. Input parameter. */
    OI_UINT8 bitpool;       /**< Size of the bit allocation pool used to encode the stream. Input parameter. */
    OI_UINT8 crc;           /**< Parity check byte used for error detection. */
    OI_UINT8 join;          /**< Whether joint stereo has been used. */
    OI_UINT8 enhanced;
    OI_UINT8 min_bitpool;   /**< This value is only used when encoding. SBC_MAX_BITPOOL if variable
                                 bitpools are disallowed, otherwise the minimum bitpool size that will
                                 be used by the bit allocator.  */

    OI_UINT8 cachedInfo;    /**< Information about the previous frame */
} OI_CODEC_SBC_FRAME_INFO;

/** Used internally. */
typedef struct {
    const OI_CHAR *codecInfo;
    OI_CODEC_SBC_FRAME_INFO frameInfo;
    OI_INT8 scale_factor[SBC_MAX_CHANNELS * SBC_MAX_BANDS];
    OI_UINT32 frameCount;
    OI_INT32 *subdata;

    SBC_BUFFER_T *filterBuffer[SBC_MAX_CHANNELS];
    OI_INT32 filterBufferLen;
    OI_UINT filterBufferOffset;

    union {
        OI_UINT8 uint8[SBC_MAX_CHANNELS * SBC_MAX_BANDS];
        OI_UINT32 uint32[SBC_MAX_CHANNELS * SBC_MAX_BANDS / 4];
    } bits;
    OI_UINT8 maxBitneed;    /**< Running maximum bitneed */
    OI_BYTE formatByte;
    OI_UINT8 pcmStride;
    OI_UINT8 maxChannels;
} OI_CODEC_SBC_COMMON_CONTEXT;


/*
 * A smaller value reduces RAM usage at the expense of increased CPU usage. Values in the range
 * 27..50 are recommended, beyond 50 there is a diminishing return on reduced CPU usage.
 */
#define SBC_CODEC_MIN_FILTER_BUFFERS 16
#define SBC_CODEC_FAST_FILTER_BUFFERS 27

/* Expands to the number of OI_UINT32s needed to ensure enough memory to encode
 * or decode streams of numChannels channels, using numBuffers buffers.
 * Example:
 * OI_UINT32 decoderData[CODEC_DATA_WORDS(SBC_MAX_CHANNELS, SBC_DECODER_FAST_SYNTHESIS_BUFFERS)];
 * */
#define CODEC_DATA_WORDS(numChannels, numBuffers) \
    ((\
        (sizeof(OI_INT32) * SBC_MAX_BLOCKS * numChannels * SBC_MAX_BANDS) \
         + (sizeof(SBC_BUFFER_T) * SBC_MAX_CHANNELS * SBC_MAX_BANDS * numBuffers) \
         + (sizeof (OI_UINT32) - 1) \
    ) / sizeof(OI_UINT32))

/** Opaque parameter to decoding functions; maintains decoder context. */
typedef struct {
    OI_CODEC_SBC_COMMON_CONTEXT common;
    OI_UINT8 limitFrameFormat;              /* Boolean, set by OI_CODEC_SBC_DecoderLimit() */
    OI_UINT8 restrictSubbands;
    OI_UINT8 enhancedEnabled;
    OI_UINT8 bufferedBlocks;
    OI_UINT8 sbc_mode;                      /* OI_SBC_MODE_STD or OI_SBC_MODE_MSBC */
} OI_CODEC_SBC_DECODER_CONTEXT;

typedef struct {
    OI_UINT32 data[CODEC_DATA_WORDS(1, SBC_CODEC_FAST_FILTER_BUFFERS)];
} OI_CODEC_SBC_CODEC_DATA_MONO;

typedef struct {
    OI_UINT32 data[CODEC_DATA_WORDS(2, SBC_CODEC_FAST_FILTER_BUFFERS)];
} OI_CODEC_SBC_CODEC_DATA_STEREO;

/**
@}

@addtogroup codec_lib
@{
*/

/**
 * This function resets the decoder. The context must be reset when
 * changing streams, or if the following stream parameters change:
 * number of subbands, stereo mode, or frequency.
 *
 * @param context   Pointer to the decoder context structure to be reset.
 *
 * @param enhanced  If true, enhanced SBC operation is enabled. If enabled,
 *                  the codec will recognize the alternative syncword for
 *                  decoding an enhanced SBC stream. Enhancements should not
 *                  be enabled unless the stream is known to be generated
 *                  by an enhanced encoder, or there is a small possibility
 *                  for decoding glitches if synchronization were to be lost.
 */
OI_STATUS OI_CODEC_SBC_DecoderReset(OI_CODEC_SBC_DECODER_CONTEXT *context,
                                    OI_UINT32 *decoderData,
                                    OI_UINT32 decoderDataBytes,
                                    OI_UINT8 maxChannels,
                                    OI_UINT8 pcmStride,
                                    OI_BOOL enhanced,
                                    OI_BOOL msbc_enable);

/**
 * This function restricts the kind of SBC frames that the Decoder will
 * process.  Its use is optional.  If used, it must be called after
 * calling OI_CODEC_SBC_DecoderReset(). After it is called, any calls
 * to OI_CODEC_SBC_DecodeFrame() with SBC frames that do not conform
 * to the Subband and Enhanced SBC setting will be rejected with an
 * OI_STATUS_INVALID_PARAMETERS return.
 *
 * @param context   Pointer to the decoder context structure to be limited.
 *
 * @param enhanced  If true, all frames passed to the decoder must be
 *                  Enhanced SBC frames. If false, all frames must be
 *                  standard SBC frames.
 *
 * @param subbands  May be set to SBC_SUBBANDS_4 or SBC_SUBBANDS_8. All
 *                  frames passed to the decoder must be encoded with
 *                  the requested number of subbands.
 *
 */
OI_STATUS OI_CODEC_SBC_DecoderLimit(OI_CODEC_SBC_DECODER_CONTEXT *context,
                                    OI_BOOL enhanced,
                                    OI_UINT8 subbands);

/**
 * This function sets the decoder parameters for a raw decode where the decoder parameters are not
 * available in the sbc data stream. OI_CODEC_SBC_DecoderReset must be called
 * prior to calling this function.
 *
 * @param context        Decoder context structure. This must be the context must be
 *                       used each time a frame is decoded.
 *
 * @param enhanced       Set to TRUE to enable Qualcomm proprietary
 *                       quality enhancements.
 *
 * @param frequency      One of SBC_FREQ_16000, SBC_FREQ_32000, SBC_FREQ_44100,
 *                       SBC_FREQ_48000
 *
 * @param mode           One of SBC_MONO, SBC_DUAL_CHANNEL, SBC_STEREO,
 *                       SBC_JOINT_STEREO
 *
 * @param subbands       One of SBC_SUBBANDS_4, SBC_SUBBANDS_8
 *
 * @param blocks         One of SBC_BLOCKS_4, SBC_BLOCKS_8, SBC_BLOCKS_12,
 *                       SBC_BLOCKS_16
 *
 * @param alloc          One of SBC_LOUDNESS, SBC_SNR
 *
 * @param maxBitpool     The maximum bitpool size for this context
 */
OI_STATUS OI_CODEC_SBC_DecoderConfigureRaw(OI_CODEC_SBC_DECODER_CONTEXT *context,
        OI_BOOL enhanced,
        OI_UINT8 frequency,
        OI_UINT8 mode,
        OI_UINT8 subbands,
        OI_UINT8 blocks,
        OI_UINT8 alloc,
        OI_UINT8 maxBitpool);

/**
 * Decode one SBC frame. The frame has no header bytes. The context must have been previously
 * initialized by calling  OI_CODEC_SBC_DecoderConfigureRaw().
 *
 * @param context       Pointer to a decoder context structure. The same context
 *                      must be used each time when decoding from the same stream.
 *
 * @param bitpool       The actual bitpool size for this frame. Must be <= the maxbitpool specified
 *                      in the call to OI_CODEC_SBC_DecoderConfigureRaw(),
 *
 * @param frameData     Address of a pointer to the SBC data to decode. This
 *                      value will be updated to point to the next frame after
 *                      successful decoding.
 *
 * @param frameBytes    Pointer to a UINT32 containing the number of available
 *                      bytes of frame data. This value will be updated to reflect
 *                      the number of bytes remaining after a decoding operation.
 *
 * @param pcmData       Address of an array of OI_INT16 pairs, which will be
 *                      populated with the decoded audio data. This address
 *                      is not updated.
 *
 * @param pcmBytes      Pointer to a UINT32 in/out parameter. On input, it
 *                      should contain the number of bytes available for pcm
 *                      data. On output, it will contain the number of bytes
 *                      written. Note that this differs from the semantics of
 *                      frameBytes.
 */
OI_STATUS OI_CODEC_SBC_DecodeRaw(OI_CODEC_SBC_DECODER_CONTEXT *context,
                                 OI_UINT8 bitpool,
                                 const OI_BYTE **frameData,
                                 OI_UINT32 *frameBytes,
                                 OI_INT16 *pcmData,
                                 OI_UINT32 *pcmBytes);

/**
 * Decode one SBC frame.
 *
 * @param context       Pointer to a decoder context structure. The same context
 *                      must be used each time when decoding from the same stream.
 *
 * @param frameData     Address of a pointer to the SBC data to decode. This
 *                      value will be updated to point to the next frame after
 *                      successful decoding.
 *
 * @param frameBytes    Pointer to a UINT32 containing the number of available
 *                      bytes of frame data. This value will be updated to reflect
 *                      the number of bytes remaining after a decoding operation.
 *
 * @param pcmData       Address of an array of OI_INT16 pairs, which will be
 *                      populated with the decoded audio data. This address
 *                      is not updated.
 *
 * @param pcmBytes      Pointer to a UINT32 in/out parameter. On input, it
 *                      should contain the number of bytes available for pcm
 *                      data. On output, it will contain the number of bytes
 *                      written. Note that this differs from the semantics of
 *                      frameBytes.
 */
OI_STATUS OI_CODEC_SBC_DecodeFrame(OI_CODEC_SBC_DECODER_CONTEXT *context,
                                   const OI_BYTE **frameData,
                                   OI_UINT32 *frameBytes,
                                   OI_INT16 *pcmData,
                                   OI_UINT32 *pcmBytes);

/**
 * Calculate the number of SBC frames but don't decode. CRC's are not checked,
 * but the Sync word is found prior to count calculation.
 *
 * @param frameData     Pointer to the SBC data.
 *
 * @param frameBytes    Number of bytes avaiable in the frameData buffer
 *
 */
OI_UINT8 OI_CODEC_SBC_FrameCount(OI_BYTE  *frameData,
                                 OI_UINT32 frameBytes);

/**
 * Analyze an SBC frame but don't do the decode.
 *
 * @param context       Pointer to a decoder context structure. The same context
 *                      must be used each time when decoding from the same stream.
 *
 * @param frameData     Address of a pointer to the SBC data to decode. This
 *                      value will be updated to point to the next frame after
 *                      successful decoding.
 *
 * @param frameBytes    Pointer to a UINT32 containing the number of available
 *                      bytes of frame data. This value will be updated to reflect
 *                      the number of bytes remaining after a decoding operation.
 *
 */
OI_STATUS OI_CODEC_SBC_SkipFrame(OI_CODEC_SBC_DECODER_CONTEXT *context,
                                 const OI_BYTE **frameData,
                                 OI_UINT32 *frameBytes);

/* Common functions */

/**
  Calculate the frame length.

  @param frame The frame whose length to calculate

  @return the length of an individual encoded frame in
  bytes
  */
OI_UINT16 OI_CODEC_SBC_CalculateFramelen(OI_CODEC_SBC_FRAME_INFO *frame);


/**
 * Calculate the maximum bitpool size that fits within a given frame length.
 *
 * @param frame     The frame to calculate the bitpool size for
 * @param frameLen  The frame length to fit the bitpool to
 *
 * @return the maximum bitpool that will fit in the specified frame length
 */
OI_UINT16 OI_CODEC_SBC_CalculateBitpool(OI_CODEC_SBC_FRAME_INFO *frame,
                                        OI_UINT16 frameLen);

/**
  Calculate the bit rate.

  @param frame The frame whose bit rate to calculate

  @return the approximate bit rate in bits per second,
  assuming that stream parameters are constant
  */
OI_UINT32 OI_CODEC_SBC_CalculateBitrate(OI_CODEC_SBC_FRAME_INFO *frame);

/**
  Calculate decoded audio data length for one frame.

  @param frame The frame whose audio data length to calculate

  @return length of decoded audio data for a
  single frame, in bytes
  */
OI_UINT16 OI_CODEC_SBC_CalculatePcmBytes(OI_CODEC_SBC_COMMON_CONTEXT *common);

/**
 * Get the codec version text.
 *
 * @return  pointer to text string containing codec version text
 *
 */
OI_CHAR *OI_CODEC_Version(void);


/**
@}

@addtogroup codec_internal
@{
*/

extern const OI_CHAR *const OI_CODEC_SBC_FreqText[];
extern const OI_CHAR *const OI_CODEC_SBC_ModeText[];
extern const OI_CHAR *const OI_CODEC_SBC_SubbandsText[];
extern const OI_CHAR *const OI_CODEC_SBC_BlocksText[];
extern const OI_CHAR *const OI_CODEC_SBC_AllocText[];

/**
@}

@addtogroup codec_lib
@{
*/

#ifdef OI_DEBUG
void OI_CODEC_SBC_DumpConfig(OI_CODEC_SBC_FRAME_INFO *frameInfo);
#else
#define OI_CODEC_SBC_DumpConfig(f)
#endif

/**
@}
*/

#ifdef __cplusplus
}
#endif


#endif /* _OI_CODEC_SBC_CORE_H */


