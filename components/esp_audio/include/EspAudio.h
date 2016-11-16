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

#ifndef ESP_AUDIO
#define ESP_AUDIO
#include "esp_types.h"
#include "EspAudioCom.h"



/*
 *  Maximum size of the URI, including null character.
 */
#define MAX_URI_SIZE 1024

/*
 *  Defines various states that the ESP player can be.
 */
enum EspAudioState
{
    AUDIOSTATE_UNINITIALIZED = 0,               /*  player  is not initialized */
    AUDIOSTATE_IDLE,                            /*  player is idle */
    AUDIOSTATE_STOPPED,                         /*  player is stopled */
    AUDIOSTATE_TRANSIT,                         /*  player is busy in a transition */
    AUDIOSTATE_PLAYING,                         /*  player is currently playing */
    AUDIOSTATE_PAUSED,                          /*  player is in the paused state for playback */
    AUDIOSTATE_ENDED,                           /*  player finished decoding player, without user request. */
    AUDIOSTATE_ERROR,                           /*  player was given a URI that could not be played */

    AUDIOSTATE_UNKNOWN                          /* unknown playstate */
};


typedef enum
{
    InputSrcType_NetSpec    = 0,    ///< Specify audio file from internet.
    InputSrcType_NetLiving  = 1,    ///< Living audio stream from internet,e.g.m3u8
    InputSrcType_LocalFile  = 2,    ///< Local file which store in the SD card
    InputSrcType_Stream     = 3,    ///< Only audio stream,e.g.PCM
    InputSrcType_Max
} InputSrcType;


typedef enum
{
    EspAudioErr_NoErr                   =0,     ///< No error
    EspAudioErr_InvalidPara,                    ///< Invalid parameters
    EspAudioErr_NoMem,                          ///< Malloc failed
    EspAudioErr_HardwareErr,                    ///< Hardware error
    EspAudioErr_OutOfRange,                     ///< length of URI is too long.
    EspAudioErr_NotSupport,                     ///< Not support the

} EspAudioErr;


/*
 *  Initializes the ESP player library.
 */
void EspAudio_Init(void);

/*
 *  Cleans up the ESP player library.
 */
void EspAudio_UnInit(void);

/*
 *  Play, stop, pause the streaming and output of player
 *  EspAudio_Init must be called before these methods work.
 */
int EspAudio_SetupStream(const char* URI, InputSrcType UriType);
/*
 *  Call this method to transition to playing state.
 *
 *  Returns nonzero if an error occurred.
 */
int EspAudio_Play(void);

/*
 *  Call this method to stop playing ESP player.
 *
 *  Returns nonzero if an error occurred.
 */
int EspAudio_Stop(void);

/*
 *  Call this method to pause the playing ESP player.
 *
 *  Returns nonzero if an error occurred.
 */
int EspAudio_Pause(void);

/*
 *  Returns the playstate of the ESP player library.
 *  Returns MEDIA_UNKNOWN if the stream could not be found.
 */
enum EspAudioState EspAudio_GetPlayState(void);

/*
 *  Get the player volume level . Volume level is from 0 to 100.
 *  Returns a negative value if the stream could not be found or the value is unknown.
 */
int EspAudio_GetVolume(int *level);

/*
 *  Set the player volume level . Volume level is from 0 to 100.
 *  Returns a negative value if the stream could not be found or the value is unknown.
 */
int EspAudio_SetVolume(int level);

/*
 *  Call this method to configrate stream information.
 *
 *  Returns nonzero if an error occurred.
 */
EspAudioErr EspAudioPlayerStreamCfg(uint32_t rate, uint32_t channel,uint32_t bitLen);

/*
 *  Call this method to flush stream data to driver.
 *
 *  Returns nonzero if an error occurred.
 */
EspAudioErr EspAudioPlayerStreamWrite(uint8_t *inData, uint32_t inSize);


#endif
