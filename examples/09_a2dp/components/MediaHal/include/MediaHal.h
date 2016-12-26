/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP32 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#ifndef __MEDIAHAL_H__
#define __MEDIAHAL_H__
#include <stdint.h>


//*****************************************************************************
//
// Make sure all of the definitions in this header have a C binding.
//
//*****************************************************************************

#ifdef __cplusplus
extern "C"
{
#endif
    
#define AP_POINT_PWR    26      // GPIO26 AP8048
#define PA_POINT_PWR    25      // GPIO25

typedef enum {
    MediaState_Initialized = 0,        /* codec is initialized */
    MediaState_Stoped,                  /* codec is stoped */
    MediaState_Transit,                  /* codec is busy in a transition */
    MediaState_Playing,                 /* codec is currently playing */
    MediaState_Paused,                 /* codec is in the paused state for playback */
    MediaState_Ended,                   /* codec finished decoding media, media has stopped without user request. */
    MediaState_Error,                     /* codec was given a URI that could not be played */

    MediaState_Unknow                 /* unknown playstate */
}MediaState;


typedef enum
{
    EspAudioMeidaType_Unknown = 0,
    EspAudioMeidaType_Wav,
    EspAudioMeidaType_Aac,
    EspAudioMeidaType_Mp3,
    EspAudioMeidaType_Amr,
    EspAudioMeidaType_Alac,
    EspAudioMeidaType_Flac,
    EspAudioMeidaType_Ape,
    EspAudioMeidaType_M3u,
    EspAudioMeidaType_Opus,
    EspAudioMeidaType_Pcm,
    EspAudioMeidaType_Max,
}EspAudioMeidaType;


typedef enum {
    MediaErr_NoError    = 0,
    MediaErr_ParaError,
    MediaErr_CodecInitFailed,
    MediaErr_CodecError,
    MediaErr_NotSuport,
} MediaErr;

typedef enum {
    MediaMode_Decode    = 0,
    MediaMode_Encode    = 1,
    MediaMode_AuxIn     = 2,
    MediaMode_Max       = 3,
} MediaMode;

typedef struct
{
    int32_t     stream_type;            /**< Stream type, must be in @code StreamTypeSet    */
    uint32_t    num_channels;           /**< Number of channels                             */
    uint32_t    sampling_rate;          /**< Sampling rate, unit: Hz                        */
    uint32_t    bitrate;                /**< Bit rate, unit:bps                             */
    uint32_t    duration;               /**< Total play time, unit:ms                       */
    uint32_t    file_size;              /**< Song file size in bytes .                      */
    uint16_t    firmware_ver;           /**< AP80xx firmware version .                      */
    uint16_t    firmware_num;           /**< AP80xx firmware number  .                      */
    uint8_t     bufEmpty;               /**< Stream_buf_is_Empty                     .      */
    uint8_t     RFU;                    /**< RFU                     .                      */

} MusicInfo;

void mediaHalInit();
MediaErr mediaHalDataWrite(uint8_t *dat, int len);
MediaErr mediaHalDataRead(uint8_t *dat, int *len);

MediaErr mediaHalPlay(EspAudioMeidaType type);
MediaErr mediaHalPause(void);
MediaErr mediaHalStop(void);
void     mediaHalReset(void);
MediaErr mediaHalVolumeSet(uint8_t value);
MediaErr mediaHalMute(void);
MediaErr mediaHalStatusGet(MediaState *staus);
void mediaHalSleep(void);
void mediaHalWakeup(void);
MediaErr mediaHalModeSet(MediaMode mode, MusicInfo* info);


MediaErr mediaHalDecodeTimeRest(void);
MediaErr mediaHalDecodeTimeGet(uint32_t *time);
MediaErr mediaHalMusicInfoGet(MusicInfo *info);

EspAudioMeidaType checkMediaType(const char* targetUri);


#ifdef __cplusplus
}
#endif

#endif //  __MEDIAHAL_H__
