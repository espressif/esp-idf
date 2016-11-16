// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __ESPAUDIORECORDER_H__
#define __ESPAUDIORECORDER_H__
#include "esp_types.h"
#include "EspAudio.h"


//Type Definitions
typedef void (*Callback_RecordData)(void *recBuf, uint32_t *recLen);

typedef enum {
    RecorderState_Unknown = 0,
    RecorderState_Init,
    RecorderState_Stoped,
    RecorderState_Recording,
    RecorderState_Paused,
} RecorderState;

typedef enum {
    EncodeType_Opus = 1,
    EncodeType_Pcm,  // it will be support
    EncodeType_Max,
} EncodeType;


typedef enum {
    RecorderSampleRate_8k  = 8000,  // it will be support
    RecorderSampleRate_16k = 16000,
    RecorderSampleRate_Max,
} RecorderSampleRate;


typedef enum {
    RecorderChannel_One = 1, // it will be support
    RecorderChannel_Two,
    RecorderChannel_Max,
} RecorderChannel;


typedef struct {
    RecorderSampleRate rate;
    RecorderChannel channel;
    EncodeType encodeType;
    Callback_RecordData func;
} RecorderAttr;


EspAudioErr EspAudioRecorderStart(RecorderAttr *recorderAttr);
EspAudioErr EspAudioRecorderStop(void);
EspAudioErr EspAudioRecorderPause(void);
EspAudioErr EspAudioRecorderResume(void);
EspAudioErr EspAudioRecorderStateGet(RecorderState *state);
EspAudioErr EspAudioRecorderInit(void);
EspAudioErr EspAudioRecorderUninit(void);


#endif //__ESPAUDIORECORDER_H__
