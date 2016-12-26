/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
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

#include "MediaHal.h"
#include "audio_log.h"
#include "driver/gpio.h"
#include "esp_types.h"
//#include "inc/Vs10xx.h"
#include "AP80/Ap80xx.h"
#include "EspAudioCom.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "MyString.h"
#include "ES8388.h"

// 2---ES8388
// 1---AP8048
// 0---VS1053

#define PA_POINT_PWR    25      // GPIO25
#define AP_POINT_PWR    26      // GPIO26

#define CODECTYPE  1
static char* LOGPRE = "[mediaHal Err]";
static char* LOGPRD = "[mediaHal Info]";


static MediaState mediaStatus = 0;
static uint32_t mediaVol = 50;

void mediaHalInit(void)
{
#if CODECTYPE==0
    VS_Init();
    VS_Sine_Test();
    VS_HD_Reset();
    VS_Soft_Reset();
    VS_Set_All();
#elif CODECTYPE==1
    // AP8048 PWRKEY
    gpio_config_t  io_conf;
    memset(&io_conf, 0, sizeof(io_conf));
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO26_U, FUNC_GPIO26_GPIO26);
    SET_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO26_U, FUN_PU);

    io_conf.pin_bit_mask     = GPIO_SEL_26;
    io_conf.mode             = GPIO_MODE_OUTPUT;
    io_conf.intr_type        = GPIO_PIN_INTR_DISABLE ;
    gpio_config(&io_conf);
    gpio_set_level(AP_POINT_PWR, 0x01);  // Enable the AP80 powerkey
    
    // PA PWR
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO25_U, FUNC_GPIO25_GPIO25);
    SET_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO25_U, FUN_PU);
    gpio_set_level(PA_POINT_PWR, 0x01);

    apInit();
#elif CODECTYPE==2
    ES8388_init();

#endif
    mediaStatus = MediaState_Initialized;
    LOGI("mediaHalInit ok");
}

MediaErr mediaHalDataRead(uint8_t *dat, int *len)
{
    if ((NULL == dat)
        /*|| (MediaState_Playing != mediaStatus)*/) {
        printf("%s,[func]%s,MediaState is not playing[%x]\r\n", LOGPRE, __func__, mediaStatus);
        return MediaErr_ParaError;
    }
    uint32_t ret = 0;
#if CODECTYPE==0
    ret = VS_Send_MusicData(dat);
#elif CODECTYPE==1
    ret = apMicReadData(dat, (uint16_t*)len);
#endif
    if (ret != MediaErr_NoError) {
        printf("%s,[func]%s,Send data to codec failed,ret=%d\r\n", LOGPRE, __func__, ret);
        return MediaErr_CodecError;
    } else {
        return MediaErr_NoError;
    }
}

MediaErr mediaHalDataWrite(uint8_t *dat, int len)
{
    if ((NULL == dat)
        || (MediaState_Playing != mediaStatus)) {
        printf("%s,[func]%s,MediaState is not playing[%x]\r\n", LOGPRE, __func__, mediaStatus);
        return MediaErr_ParaError;
    }
    uint32_t ret = 0;
#if CODECTYPE==0
    ret = VS_Send_MusicData(dat);
#elif CODECTYPE==1
    ret = apSendData(dat, len);
#endif
    if (ret != MediaErr_NoError) {
        printf("%s,[func]%s,Send data to codec failed,ret=%d\r\n", LOGPRE, __func__, ret);
        return MediaErr_CodecError;
    } else {
        return MediaErr_NoError;
    }
}

void mediaHalReset(void)
{
  //  VS_HD_Reset();
    //VS_Soft_Reset();
}

MediaErr mediaHalPlay(EspAudioMeidaType type)
{
    uint32_t ret = 0;
#if CODECTYPE==0
    VS_Restart_Play();                      //
    VS_Reset_DecodeTime();                  //
    VS_SPI_SpeedHigh();
#else
    if ((MediaState_Initialized == mediaStatus)
        || (MediaState_Stoped == mediaStatus)) {
        if (EspAudioMeidaType_Aac == type) {
            ret = apPlayStart("radio", sizeof("radio"), AAC_DECODER);
        } else if (EspAudioMeidaType_Mp3 == type) {
            ret = apPlayStart("radio", sizeof("radio"), MP3_DECODER);
        } else if (EspAudioMeidaType_Wav == type) {
            ret = apPlayStart("radio", sizeof("radio"), WAV_DECODER);
        } else if (EspAudioMeidaType_Flac == type) {
            ret = apPlayStart("radio", sizeof("radio"), FLAC_DECODER);
        } else if (EspAudioMeidaType_Opus == type) {
            ret = apPlayStart("radio", sizeof("radio"), OPUS_DECODER);
        } else if (EspAudioMeidaType_Pcm == type) {          
            ret = apPlayStart("radio", sizeof("radio"), PCM_DECODER);
        }
        else {
            printf("%s,[func]%s,Music type[%d] does not support\r\n", LOGPRE, __func__, type);
        }
    } else {
        apPlayResume();
    }

    printf("%s,[func]%s, mediaStatus=%d\r\n", LOGPRD, __func__, mediaStatus);
#endif
    mediaStatus = MediaState_Playing;
    if (ret != OK_RESPONSE) {
        printf("%s,[func]%s,ret=%d\r\n", LOGPRE, __func__, ret);
        ret = MediaErr_CodecError;
    }
    return ret;

}

MediaErr mediaHalPause(void)
{
    uint32_t ret = 0;
    if (MediaState_Playing == mediaStatus) {
        apPlayPause();
        mediaStatus = MediaState_Paused;
    }
    if (ret != OK_RESPONSE) {
        printf("%s,[func]%s,ret=%d\r\n", LOGPRE, __func__, ret);
        ret = MediaErr_CodecError;
    }
    printf("%s,[func]%s,mediaStatus=%d\r\n", LOGPRD, __func__, mediaStatus);
    return ret;
}

MediaErr mediaHalStop(void)
{

    uint32_t ret = 0;
#if CODECTYPE==0
    VS_Cancel();
#elif CODECTYPE==1

    apPlayStop();
#endif
    mediaStatus = MediaState_Stoped;
    if (ret != OK_RESPONSE) {
        printf("%s,[func]%s,ret=%d\r\n", LOGPRE, __func__, ret);
        ret = MediaErr_CodecError;
    }
    return ret;
}

MediaErr mediaHalVolumeSet(uint8_t value)
{
    uint32_t ret = 0;
    uint32_t tmp = value;
    LOGD("volmue=%d", tmp);

#if CODECTYPE==0
    vsset.mvol = value;
    mediaVol = vsset.mvol;
    tmp = (value * 200) / 100 + 50;
    mediaVol = tmp;
    VS_Set_Vol(tmp); // when vs1053 does not init,this call will be hang.
#elif CODECTYPE==1

    ret = apVolSet(tmp);
#endif
    if (ret != OK_RESPONSE) {
        printf("%s,[func]%s,ret=%d\r\n", LOGPRE, __func__, ret);
        ret = MediaErr_CodecError;
    }
    return ret;
}

MediaErr mediaHalMute(void)
{
    uint32_t ret = 0;
#if CODECTYPE==0
    VS_Set_Vol(0);
#elif CODECTYPE==1

    ret = apVolMuteEnable();
#endif
    mediaVol = 0;
    if (ret != OK_RESPONSE) {
        printf("%s,[func]%s,ret=%d\r\n", LOGPRE, __func__, ret);
        ret = MediaErr_CodecError;
    }
    return ret;
}

MediaErr mediaHalStatusGet(MediaState *staus)
{
    if (NULL == staus) {
        return MediaErr_ParaError;
    }
    *staus = mediaStatus;
    return MediaErr_NoError;
}

void mediaHalSleep(void)
{
  //  GPIO_OUTPUT_SET(PA_POINT_PWR, 0);
  //  GPIO_OUTPUT_SET(AP_POINT_PWR, 0);
    printf("%s mediaHal sleep\r\n", LOGPRE);
}

void mediaHalWakeup(void)
{
  //  GPIO_OUTPUT_SET(PA_POINT_PWR, 1);
  //  GPIO_OUTPUT_SET(AP_POINT_PWR, 1);
    printf("%s Codec wakeup\r\n", LOGPRE);
}

MediaErr mediaHalDecodeTimeRest()
{
    uint32_t ret = apRestDecodeTime();
    if (ret != OK_RESPONSE) {
        printf("%s,[func]%s,ret=%d\r\n", LOGPRE, __func__, ret);
        ret = MediaErr_CodecError;
    }
    return ret;
}

MediaErr mediaHalDecodeTimeGet(uint32_t *time)
{
    if (NULL == time) {
        return MediaErr_ParaError;
    }
    uint32_t ret = apGetDecodeTime((DecodeTime*)time);
    if (ret != OK_RESPONSE) {
        printf("%s,[func]%s,ret=%d\r\n", LOGPRE, __func__, ret);
        ret = MediaErr_CodecError;
    }
    return ret;
}

MediaErr mediaHalMusicInfoGet(MusicInfo *info)
{
    if (NULL == info) {
        return MediaErr_ParaError;
    }
    uint32_t ret = apGetInfo((SongInformation*)info);
    if (ret != OK_RESPONSE) {
        printf("%s,[func]%s,ret=%d\r\n", LOGPRE, __func__, ret);
        ret = MediaErr_CodecError;
    }
    return ret;
}

MediaErr mediaHalModeSet(MediaMode mode, MusicInfo* info)
{
    MediaErr err = MediaErr_NoError;
    uint8_t ret = OK_RESPONSE;
   

    switch (mode) {
    case MediaMode_Encode:
    {
         setEnccodeInfo tmpInfo;
        memset(&tmpInfo, 0, sizeof(tmpInfo));
        ret = apSetEncodeMode();
        tmpInfo.streamDirection    = 1;
        tmpInfo.numChannels        = info->num_channels;
        tmpInfo.sampleRate         = info->sampling_rate;
        tmpInfo.encoderType        = info->stream_type; //OPUS_ENCODER;
        ret = apEncodeStart();
        ret += apSetEncodeInfo(&tmpInfo);
        LOGD("apSetEncodeInfo ret = %d %d\n", ret, tmpInfo.encoderType, 
            tmpInfo.sampleRate, tmpInfo.numChannels);
        break;
    }
    case MediaMode_Decode:
    {
        setDecodeInfo tmpInfo;
        memset(&tmpInfo, 0, sizeof(tmpInfo));
        ret = apSetDecodeMode();
        tmpInfo.streamDirection = 0;
        tmpInfo.numChannels    = info->num_channels;
        tmpInfo.sampleRate     = info->sampling_rate;
        
        ret += apSetDecodeInfo(&tmpInfo);
        LOGD("apSetDecodeInfo ret=%d\n", ret);

        break;
    }
    case MediaMode_AuxIn:
        ret = apSetDecodeMode();
        break;
    default:
        err = MediaErr_NotSuport;
        break;
    }
    if ((ret != OK_RESPONSE) && (ERROR_OTHER!= ret)) {
        printf("%s,[func]%s,ret=%d\r\n", LOGPRE, __func__, ret);
        err = MediaErr_CodecError;
    }
    return err;
}

EspAudioMeidaType checkMediaType(const char* targetUri)
{
    EspAudioMeidaType type;
    if (EndsWith(targetUri, ".wav", 1) != 0) {
        type = EspAudioMeidaType_Wav;
    } else if (EndsWith(targetUri, ".aac", 1) != 0) {
        type = EspAudioMeidaType_Aac;
    } else if (EndsWith(targetUri, ".mp3", 1) != 0) {
        type = EspAudioMeidaType_Mp3;
    } else if (EndsWith(targetUri, ".amr", 1) != 0) {
        type = EspAudioMeidaType_Amr;
    } else if (EndsWith(targetUri, ".alac", 1) != 0) {
        type = EspAudioMeidaType_Alac;
    } else if (EndsWith(targetUri, ".flac", 1) != 0) {
        type = EspAudioMeidaType_Flac;
    } else if (EndsWith(targetUri, ".ape", 1) != 0) {
        type = EspAudioMeidaType_Ape;
    } else if (EndsWith(targetUri, ".m3u8", 1) != 0) {
        type = EspAudioMeidaType_M3u;
    } else if (EndsWith(targetUri, ".opu", 1) != 0) {
        type = EspAudioMeidaType_Opus;
    } else if (EndsWith(targetUri, ".pcm", 1) != 0) {
        type = EspAudioMeidaType_Pcm;
    } else {
        type = EspAudioMeidaType_Unknown;
    }
    return type;
}

// end
