// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _ESPAUDIOCOM_H
#define _ESPAUDIOCOM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "audio_log.h"
#include "EspAudioCom.h"

#define ESP_AUDIO_RELEASE_MAJOR (0000)
#define ESP_AUDIO_RELEASE_MINOR (51)
#define ESP_AUDIO_RELEASE_VER   "0.51"


typedef struct
{
    uint32_t type;
    char   str[256];
} UrlInfo;


typedef enum {
    StreamType_Opus,
    StreamType_Pcm,  // it will be support
    StreamType_Max,
} StreamType;


typedef enum {
    StreamSampleRate_8k  = 8000,  
    StreamSampleRate_16k = 16000, // This is support
    StreamSampleRate_22K = 22050,
    StreamSampleRate_32K = 32000,
    StreamSampleRate_44k = 44100,
    StreamSampleRate_48k = 48000,
    StreamSampleRate_Max,
} StreamSampleRate;


typedef enum {
    StreamBitLen_8BIT = 8,
    StreamBitLen_16BIT = 16,
    StreamBitLen_24BIT = 24,
    StreamBitLen_32BIT = 32,
    StreamBitLen_Max,
} StreamBitLen;


typedef enum {
    StreamChannel_One, // it will be support
    StreamChannel_Two,
    StreamChannel_Max,
} StreamChannel;

#endif  /* _ESPAUDIOCOM_H */