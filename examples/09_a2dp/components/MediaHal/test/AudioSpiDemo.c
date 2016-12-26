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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "esp_timer.h"
#include "esp_types.h"
#include "esp_system.h"
#include "Utility.h"


#include "math.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"


#include "driver/gpio.h"
#include "driver/spi.h"
#include "stdarg.h"
#include "freertos/task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/sys.h"
#include "tcpip_adapter.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

#include "Ap80xx.h"

#include "inc/AudioSpiDemo.h"

#define WAV_DECODER_H   2
#define FLAC_DECODER_H  3
#define AAC_DECODER_H   4
#define AIF_DECODER_H   5
#define PCM_DECODER_H   8
#define MP3_DECODER_H   129
#define WMA_DECODER_H   130
#define SBC_DECODER_H   131

#define DECODELTYPE AAC_DECODER_H

#define CHIPTYPE    32


#define	require_action( X, LABEL, fmt, ... )															\
	do 																									\
	{																									\
		if( (X) ) 																			            \
		{																								\
			{ printf(fmt, ##__VA_ARGS__); printf("\n");}												\
			goto LABEL;																					\
		}                                                                                               \
																										\
	}	while( 0 )

#if CHIPTYPE == 32
//#define os_printf                   ets_printf
#define os_delay_us                 ets_delay_us

#elif CHIPTYPE == 31

#endif

const char *gLongFileName  = {"Test Audio"};

#define FRAMELEN    (0x400)

void delays(uint8_t timeS)
{
    int i = 0;
    for (i = 0; i < timeS * 20; i++)
        os_delay_us(50000);    
}

void getinfo_test_1()
{
    SongInformation songInfo;
    if(apGetInfo(&songInfo) == OK_RESPONSE)
    {
        printf("\nSongInformation:\n");
        printf("stream_type      %d\n",songInfo.stream_type);
        printf("num_channels     %d\n",songInfo.num_channels);
        printf("sampling_rate    %dHZ\n",songInfo.sampling_rate);
        printf("bitrate          %dbps\n",songInfo.bitrate);
        printf("duration         %dms\n",songInfo.duration);        
        printf("file_size        %dbyte\n",songInfo.file_size);
        printf("firmware_ver     %03X\n",songInfo.firmware_ver);
        printf("firmware_num     %d\n",songInfo.firmware_num);
        printf("Stream_buf       %d\n",songInfo.Stream_buf_is_Empty);
    } else {
        printf("get Info         error\n");
    }
}

void getinfo_ver()
{
    SongInformation songInfo;
    if(apGetInfo(&songInfo) == OK_RESPONSE)
    { 
        printf("\nSongInformation:\n");
        printf("stream_type      %d\n",songInfo.stream_type);
        printf("num_channels     %d\n",songInfo.num_channels);
        printf("sampling_rate    %dHZ\n",songInfo.sampling_rate);
        printf("bitrate          %dbps\n",songInfo.bitrate);
        printf("duration         %dms\n",songInfo.duration);        
        printf("file_size        %dbyte\n",songInfo.file_size);
    } else {
        printf("get Info         error\n");
    }
}
void getdecodetime_test()
{
    DecodeTime time;
    if(apGetDecodeTime(&time) == OK_RESPONSE)
    {
        printf("DecodeTime      %dms\n",time);
    } else {
        printf("DecodeTime      error\n");
    }
}

//uint8_t      micData[1024*20];
uint32_t     pos    = 0;
uint32_t     oldpos = 0;
uint8_t BccCheckSum(uint8_t *buf, uint16_t len)
{
    uint16_t i;
    uint8_t checksum = 0;

    for(i = 0; i < len; i++) {
        checksum ^= *buf++;
    }
    return checksum;
}

#if MICTEST
void mic_recode_opus_16K_01_test()
{
    uint8_t      recvData[128];
    uint8_t      sendData[128];
    uint16_t     result      = 0;    
    uint8_t      ret         = 0;
    uint16_t     len         = 0;    
    uint32_t     fileSize    = 1024*10;
    uint32_t     opusPos     = 0;
    uint32_t     opusPlan    = 0;

    printf("****************************MIC      demo****************************\n");
    
    ret = apSetEncodeMode();
    require_action(ret != OK_RESPONSE, error, "[ERROR] set encode mode fail %d",ret);
    
    setEnccodeInfo tmpInfo;
    memset(&tmpInfo,0,sizeof(tmpInfo));
    
    tmpInfo.streamDirection         = 1;
    tmpInfo.numChannels             = 1;
    tmpInfo.sampleRate              = 16000;
    tmpInfo.encoderType             = OPUS_ENCODER_EX;

    ret = apSetEncodeInfo(&tmpInfo);
    require_action(ret != OK_RESPONSE, error, "[ERROR] set encode info fail %d",ret);    
    
    ret = apEncodeStart();
    require_action(ret != OK_RESPONSE, error, "[ERROR] set encode start fail %d",ret);    

    pos                         = 0;

    #define HEADLEN             (0)
    do
    {
        ret = apMicReadData(recvData,&len);
        require_action(ret != OK_RESPONSE, error, "[ERROR] read data fail %d",ret);
        
        if(pos % 1024 > 1024-16)
            printf("[INFO] recode recv data %d %d\n",len,pos);
        *(micData + pos++) = len;        
        MEMCPY(micData + pos, recvData, len);        
        pos += len;
        
        if(!len)
            printf("[ERROR] recode recv data len error %d %d\n",len,pos);
    }while(pos + 128 < fileSize);
    

    printf("[INFO] start back play\n");
    
    ret = apSetDecodeMode();
    require_action(ret != OK_RESPONSE, error, "[ERROR] start decoderet mode fail %d",ret);
    
    apVolSet(70);
    
    setDecodeInfo tmpDecodeInfo;
    memset(&tmpDecodeInfo,0,sizeof(tmpDecodeInfo));   
   
    tmpDecodeInfo.streamDirection       = 0;
    tmpDecodeInfo.numChannels           = 1;
    tmpDecodeInfo.sampleRate            = 16000;
    tmpDecodeInfo.decoderType           = OPUS_DECODER;
    apSetDecodeInfo(&tmpDecodeInfo);
    
    ret = apPlayStart(gLongFileName, strlen(gLongFileName), OPUS_DECODER);
    require_action(ret != OK_RESPONSE, error, "[ERROR] start play  fail %d",ret);
    
    for(opusPos = 0;opusPos < fileSize;)
    {
        MEMCPY(sendData, micData + opusPos, micData[opusPos] + 1);

        if(micData[opusPos] > 48)
        {
            printf("[WARN] opus len %d\n",micData[opusPos]);
        }
        
        ret = apSendData(sendData, micData[opusPos] + 1);
        require_action(ret != OK_RESPONSE, error, "[ERROR] read data fail %d",ret);
        
        if(opusPlan > 1024)
        {
            printf("[INFO] send data %d %d\n",len,opusPos);
            opusPlan = 0;
        }
        else
        {
            opusPlan    += micData[opusPos];
        }
        
        opusPos     += micData[opusPos] + 1;
    }
    ret = apPlayStop();
    require_action(ret != OK_RESPONSE, error, "[ERROR] start play  fail %d",ret);
    printf("[INFO] mic test end\n");
    return ;
error:
    printf("[ERROR] mic test Abnormal exit\n");
}
#endif

#if MICTEST
void mic_recode_pcm_16K_01_test()
{
    uint8_t      recvData[1024];
    uint8_t      sendData[1024];
    uint16_t     result      = 0;
    uint8_t      ret         = 0;
    uint16_t     len         = 0;
    uint32_t     fileSize    = 0;
    uint32_t     opusPos     = 0;
    uint32_t     opusPlan    = 0;

    printf("****************************MIC(PCM 16K)   demo****************************\n");
    
    ret = apSetEncodeMode();
    require_action(ret != OK_RESPONSE, error, "[ERROR] set encode mode fail %d",ret);
    
    setEnccodeInfo tmpInfo;
    memset(&tmpInfo,0,sizeof(tmpInfo));
    
    tmpInfo.streamDirection         = 1;
    tmpInfo.numChannels             = 1;
    tmpInfo.sampleRate              = 16000;
    tmpInfo.encoderType             = PCM_ENCODER_EX;
    
    ret = apEncodeStart();
    require_action(ret != OK_RESPONSE, error, "[ERROR] set encode start fail %d",ret);
    
    ret = apSetEncodeInfo(&tmpInfo);
    require_action(ret != OK_RESPONSE, error, "[ERROR] set encode info fail %d",ret);
    pos                         = 0;
    oldpos                      = 0;

    do
    {
        ret = apMicReadData(recvData,&len);
        require_action(ret != OK_RESPONSE, error, "[ERROR] read data fail %d",ret);
        if(!len)
        {
            printf("[ERROR] recode recv data len error %d %d\n",len,pos);
            continue;
        } //else
          //  len --;
        
        if(pos - oldpos > 1024 * 5)
        {
            printf("[INFO] recode recv data %d %d\n",len,pos);
            oldpos = pos;
        }
        MEMCPY(micData + pos, recvData + 0, len);
        pos += len;
    }while(pos + 128 < sizeof(micData));
    
    //printfByteS(micData, 0x800);
    printf("[INFO] start back play\n");
    ret = apSetDecodeMode();
    require_action(ret != OK_RESPONSE, error, "[ERROR] start decoderet mode fail %d",ret);
    
    setDecodeInfo tmpDecodeInfo;
    memset(&tmpDecodeInfo,0,sizeof(tmpDecodeInfo));   
   
    tmpDecodeInfo.streamDirection       = 0;
    tmpDecodeInfo.numChannels           = 1;
    tmpDecodeInfo.sampleRate            = 16000;
    tmpDecodeInfo.decoderType           = PCM_DECODER;
    apSetDecodeInfo(&tmpDecodeInfo);
    
    apVolSet(80);
    
    ret = apPlayStart(gLongFileName, strlen(gLongFileName), PCM_DECODER);
    require_action(ret != OK_RESPONSE, error, "[ERROR] start play  fail %d",ret);
    
    for(opusPos = 0;opusPos < sizeof(micData);)
    {
        MEMCPY(sendData, micData + opusPos, FRAMELEN);
        ret = apSendData(sendData, FRAMELEN);
        require_action(ret != OK_RESPONSE, error, "[ERROR] read data fail %d",ret);
        opusPos     += FRAMELEN;
    }
    ret = apPlayStop();
    require_action(ret != OK_RESPONSE, error, "[ERROR] start play  fail %d",ret);
    printf("[INFO] mic test end\n");
    return ;
error:    
    printf("[ERROR] mic test Abnormal exit\n");
}
#endif

#if MICTEST
void mic_block_test()
{
    uint8_t      recvData[128];
    uint8_t      sendData[128];
    uint16_t     result      = 0;    
    uint8_t      ret         = 0;
    uint16_t     len         = 0;    
    uint32_t     fileSize    = 0;
    uint32_t     opusPos     = 0;
    uint32_t     opusPlan    = 0;
    
    printf("[INFO] CASE START MIC\n");
    
    ret = apSetEncodeMode();
    require_action(ret != OK_RESPONSE, error, "[ERROR] set encode mode fail %d",ret);
    
    setEnccodeInfo tmpEncodeInfo;
    memset(&tmpEncodeInfo,0,sizeof(tmpEncodeInfo));
    
    tmpEncodeInfo.streamDirection         = 1;
    tmpEncodeInfo.numChannels             = 1;
    tmpEncodeInfo.sampleRate              = 16000;
    tmpEncodeInfo.encoderType             = OPUS_ENCODER_EX;
    
    ret = apEncodeStart();
    require_action(ret != OK_RESPONSE, error, "[ERROR] set encode start fail %d",ret);
    
    ret = apSetEncodeInfo(&tmpEncodeInfo);
    require_action(ret != OK_RESPONSE, error, "[ERROR] set encode info fail %d",ret);
    
    pos                         = 0;
    do
    {
        ret = apMicReadData(recvData,&len);    
        require_action(ret != OK_RESPONSE, error, "[ERROR] read data fail %d",ret);
        
        if(pos % 1024 > 1024-16)
            printf("[INFO] recode recv data %d %d\n",len,pos);
        
        MEMCPY(micData + pos, recvData, len);
        pos += len;
        os_delay_us(30000);
    }while(pos + 128 < sizeof(micData));
    
    //printfByteS(micData, 1*1024);    
    
    printf("[INFO] start back play\n");
    
    ret = apSetDecodeMode();
    require_action(ret != OK_RESPONSE, error, "[ERROR] start decoderet mode fail %d",ret);
    
    setDecodeInfo tmpDecInfo;
    memset(&tmpDecInfo,0,sizeof(tmpDecInfo));
    
    tmpDecInfo.streamDirection         = 0;
    tmpDecInfo.numChannels             = 1;
    tmpDecInfo.sampleRate              = 16000;
    tmpDecInfo.decoderType             = OPUS_DECODER;
    
    ret = apPlayStart(gLongFileName, strlen(gLongFileName), OPUS_DECODER);
    require_action(ret != OK_RESPONSE, error, "[ERROR] start play  fail %d",ret);
    
    for(opusPos = 0;opusPos < sizeof(micData);)
    {
        MEMCPY(sendData, micData + opusPos, micData[opusPos] + 1);

        if(micData[opusPos] > 48)
        {
            printf("[WARN] opus len %d\n",micData[opusPos]);
        }
        
        ret = apSendData(sendData, micData[opusPos] + 1);
        
        require_action(ret != OK_RESPONSE, error, "[ERROR] read data fail %d",ret);
        
        if(opusPlan > 1024)
        {
            printf("[INFO] send data %d %d\n",len,opusPos);
            opusPlan = 0;
        }
        else
        {
            opusPlan    += micData[opusPos];
        }
        
        opusPos     += micData[opusPos] + 1;
    }
    printf("[INFO] mic test end\n");
    return ;
error:    
    printf("[ERROR] mic test Abnormal exit\n");
}
#endif

#if AACTEST
void aac_play_test()
{    
    uint8_t      sendData[FRAMELEN];
    uint16_t     ret            = 0;
    uint16_t     count          = 0;
    uint32_t     filePos        = 0;
    uint32_t     decodeType     = MP3_DECODER;
    uint32_t     fileSize       = 0;  

    int percentage  = 0;
    fileSize   = sizeof(aacData);
    printf("****************************AAC play demo****************************\n");
    
    decodeType = AAC_DECODER_H;
    
    ret = apSetDecodeMode();    
    require_action(ret != OK_RESPONSE && ret != ERROR_OTHER, error, "[ERROR] start decoderet mode fail %d",ret);
    
    do{
        ret = apPlayStart(gLongFileName, strlen(gLongFileName), decodeType);
        if(ret) {
            printf("[ERROR] start decoderet mode fail %d", ret);
        } else {
            break;
        }
        os_delay_us(5000);
    }while(++count < 50000);

    require_action(ret == 50000, error, "[ERROR] start play fail %d",ret);
    printf("[INFO] play start success\n");    

    uint32_t currentPlay = 0;
    for(filePos = 0;filePos < fileSize;)
    {   
        if((int)(((float)filePos / (float)fileSize) * 100) > currentPlay)
        {
            currentPlay = (int)(((float)filePos / (float)fileSize) * 100);
            printf("[INFO] play %2d ",currentPlay);
            getdecodetime_test();
        }
        
        MEMCPY(sendData, aacData + filePos, FRAMELEN);
        ret = apSendData(sendData, FRAMELEN);
        
        require_action(ret != OK_RESPONSE, error, "[ERROR] send data fail %d",ret);
        
        filePos += FRAMELEN;
    }
    getinfo_test_1();
    
    apPlayStop();

    printf("[INFO] aac play end\n");
    return;    
error:    
    printf("[ERROR] aac play test abnormal exit\n");
}
#endif

#if MP3TEST
void mp3_play_test()
{    
    uint8_t      sendData[FRAMELEN];
    uint16_t     ret        = 0;
    uint16_t     count      = 0;
    uint32_t     filePos    = 0;
    uint32_t     decodeType = MP3_DECODER;
    uint32_t     fileSize   = 0;
    
    int percentage  = 0;
    fileSize        = sizeof(mp3Data);
    const uint8_t *songDemoData = mp3Data;
    
    printf("****************************MP3 play demo****************************\n");
    
    ret = apSetDecodeMode();
    require_action(ret != OK_RESPONSE && ret != ERROR_OTHER, error, "[ERROR] set decoderet mode fail %d",ret);
    
    decodeType = MP3_DECODER_H;       
    
    do{
        ret = apPlayStart(gLongFileName, strlen(gLongFileName), decodeType);
        if(ret) {
            printf("[ERROR] start decoderet mode fail %d", ret);
        } else {
            break;
        }
        os_delay_us(1000);
    }while(++count < 50000);
    
    require_action(ret == 50000, error, "[ERROR] start play fail %d",ret);
    printf("[INFO] play start success\n");
    
#if 0    
    for(filePos = 0;filePos < fileSize;)
    {        
        MEMCPY(sendData, mp3Data + filePos, FRAMELEN);
        result = apSendData(sendData, FRAMELEN);
        
        if(result) {
            printf("send data %d\n", result);
        }
        filePos += FRAMELEN;
    }
    printf("[TEST] CASE 01:PLAY  END\n");

    printf("[TEST] CASE 02:STOP  START\n");
    for(filePos = 0;filePos < fileSize;)
    {        
        MEMCPY(sendData, mp3Data + filePos, FRAMELEN);
        result = apSendData(sendData, FRAMELEN);
        
        if(filePos >= (200 * 1024) &&  filePos < (200 * 1024 + FRAMELEN))
        {
            int i = 0;
            apPlayStop();
            printf("delays satrt\n");
            for(i = 0;i < 5;i++)
            {
                delays(1);
                printf("delays time %ds\n",i);
            }
            printf("delays end\n");        
        }
        
        if(result) {
            printf("send data %d\n", result);
        }
        filePos += FRAMELEN;
    }       
    printf("[TEST] CASE 02:STOP END\n");


    printf("[TEST] CASE 03:MUTE AND UNMUTE START\n");
    for(filePos = 0;filePos < fileSize;)
    {
        MEMCPY(sendData, songDemoData + filePos, FRAMELEN);
        result = apSendData(sendData, FRAMELEN);
        
        if(result == ERROR_START) {
            printf("send data %d\n", result);
            result = apPlayResume();
            printf("result = %d\n",result);
        }
        
        //if(result != OK_RESPONSE) {
        //    printf("send data %d\n", result);
        //}
        
        if(filePos  ==  100*1024)
        {
            printf("audio Pause\n");
            result = apPlayPause();
            printf("Pause = %d\n",result);
            int i = 0;
            for(i = 0;i < 2;i++)
            {
                delays(1);
                printf("Pause time %ds\n",i);
            }
            
            //printf("audio Resume\n");
            //result = apPlayResume();
            //printf("apPlayResume = %d\n",result);
        }
        
        filePos += FRAMELEN;
    }       
    printf("[TEST] CASE 03:MUTE AND UNMUTE END\n");

    printf("[TEST] CASE 04:VOL SET START\n");
    for(filePos = 0;filePos < fileSize;)
    {
        MEMCPY(sendData, songDemoData + filePos, FRAMELEN);
        result = apSendData(sendData, FRAMELEN);

        
        if(filePos  ==  100*1024)
        {
            apVolSet(50);
        }
        
        if(filePos  ==  200*1024)
        {
            apVolSet(100);
        }
        
        if(result) {
            printf("send data %d\n", result);
        }
        filePos += FRAMELEN;
    }
    printf("[TEST] CASE 04:VOL SET END\n");
#endif    

    uint32_t currentPlay = 0;
    for(filePos = 0;filePos < fileSize;)
    {   
        if((int)(((float)filePos / (float)fileSize) * 100) > currentPlay)
        {
            currentPlay = (int)(((float)filePos / (float)fileSize) * 100);
            printf("[INFO] play %2d ",currentPlay);
            getdecodetime_test();
        }
        
        MEMCPY(sendData, songDemoData + filePos, FRAMELEN);
        
        ret = apSendData(sendData, FRAMELEN);        
        require_action(ret != OK_RESPONSE, error, "[ERROR] send data fail %d",ret);             
        
        filePos += FRAMELEN;        
    }
    getinfo_test_1();    
    apPlayStop();
    
    printf("[INFO] mp3 play end\n");
    return;    
error:    
    printf("[ERROR] mp3 play test abnormal exit\n");
    
}
#endif

#if AUOXTEST
void auox_test()
{
    uint8_t      recvData[128];
    uint8_t      sendData[128];  
    uint8_t      ret    = 0;
    
    printf("****************************AUX IN   demo****************************\n");
    
    ret = apSetEncodeMode();
    require_action(ret != OK_RESPONSE, error, "[ERROR] set encode mode fail %d",ret);
    
    ret = apLinSwitch();
    require_action(ret != OK_RESPONSE, error, "[ERROR] switch line fail %d",ret);
    
    int i = 0;
    for(i = 0;i < 10;i++)
    {
        delays(1);
        printf("[INFO] auox time %ds\n",i);
    }
    ret = apLinSwitch();
    require_action(ret != OK_RESPONSE, error, "[ERROR] switch line fail %d",ret);
    
    printf("[INFO] auox play end\n");
    return;    
error:    
    printf("[ERROR] auox play test abnormal exit\n");
        
}
#endif

#if PCM4402TEST
void pcm4402_play_test()
{   
    #define PCM4402FRAMELEN    (352 * 4)
    
    uint8_t      sendData[PCM4402FRAMELEN];
    uint16_t     ret        = 0;
    uint16_t     count      = 0;
    uint32_t     filePos    = 0;
    uint32_t     decodeType = PCM_DECODER;
    uint32_t     fileSize   = 0;

    int percentage  = 0;
    fileSize   = sizeof(pcmData_44K_2);
    printf("****************************PCM PLAY demo****************************\n");
   
    ret = apSetDecodeMode();
    require_action(ret != OK_RESPONSE && ret != ERROR_OTHER, error, "[ERROR] start decoderet mode fail %d",ret);
    
    setDecodeInfo tmpInfo;
    memset(&tmpInfo,0,sizeof(tmpInfo));
    
    tmpInfo.streamDirection     = 0;
    tmpInfo.numChannels         = 2;
    tmpInfo.decoderType         = decodeType;
    tmpInfo.sampleRate          = 44100;    
    apSetDecodeInfo(&tmpInfo);    

    do{
        ret = apPlayStart(gLongFileName, strlen(gLongFileName), decodeType);
        if(ret) {
            printf("[ERROR] start decoderet mode fail %d", ret);
        } else {
            break;
        }
        os_delay_us(5000);
    }while(++count < 50000);
    
    require_action(ret == 50000, error, "[ERROR] start play fail %d",ret);
    printf("[INFO] play start success\n");
    
    uint32_t currentPlay = 0;    
    for(filePos = 0;filePos < fileSize - 1024 * 2;)
    {   
        MEMCPY(sendData, pcmData_44K_2 + filePos, PCM4402FRAMELEN);
        ret = apSendData(sendData, PCM4402FRAMELEN);
        
        if(ret) {
            printf("send data %d\n", ret);
        }        
        filePos += PCM4402FRAMELEN;
    }
    
#if 0
    {
    setDecodeInfo tmpInfo;
    memset(&tmpInfo,0,sizeof(tmpInfo));
    
    tmpInfo.stream_direction    = 0;
    tmpInfo.num_channels        = 2;
    tmpInfo.bitrate             = 44100;
    apSetDecodeInfo(&tmpInfo);
    
    os_printf("[TEST] CASE 01:PLAY\n");
    do{
        result = apPlayStart(gLongFileName, strlen(gLongFileName), decodeType);
        if(result) {
            os_printf("apPlayStart Fail %d\n", result);
        } else {
            break;
        }
        os_delay_us(5000);
    }while(++result < 50000);
    
    if(result == 50000) {
        os_printf("apPlayStart fail\n");
        return;
    }
    else
    {
        os_printf("apPlayStart ok\n");
    }
    
    os_printf("[TEST] CASE 05:GET INFO START\n");
    uint32_t currentPlay = 0;
    for(filePos = 0;filePos < fileSize;)
    {   
        MEMCPY(sendData, pcmData_44K_2 + filePos, FRAMELEN);
        result = apSendData(sendData, FRAMELEN);
        
        if(result) {
            os_printf("send data %d\n", result);
        }
        filePos += FRAMELEN;
    }

    }
#endif
    apPlayStop();
    
    ret = apSetDecodeMode();
    require_action(ret != OK_RESPONSE && ret != ERROR_OTHER, error, "[ERROR] start decoderet mode fail %d",ret);

    printf("[INFO] pcm play end\n");
    return;    
error:    
    printf("[ERROR] pcm play test abnormal exit\n");
}
#endif

#if PCM1601TEST
void pcm1601_play_test()
{    
    uint8_t      sendData[FRAMELEN];
    uint16_t     ret        = 0;
    uint16_t     count      = 0;
    uint32_t     filePos    = 0;
    uint32_t     decodeType = PCM_DECODER;
    uint32_t     fileSize   = 0;

    int percentage  = 0;
    fileSize   = sizeof(pcmData_16K_1);
    printf("****************************PCM PLAY demo****************************\n");    
   
    ret = apSetDecodeMode();
    require_action(ret != OK_RESPONSE && ret != ERROR_OTHER, error, "[ERROR] start decoderet mode fail %d",ret);
    
    setDecodeInfo tmpInfo;
    memset(&tmpInfo,0,sizeof(tmpInfo));
    
    tmpInfo.streamDirection     = 0;
    tmpInfo.numChannels         = 1;
    tmpInfo.decoderType         = decodeType;
    tmpInfo.sampleRate          = 16000;
    apSetDecodeInfo(&tmpInfo);

    do{
        ret = apPlayStart(gLongFileName, strlen(gLongFileName), decodeType);
        if(ret) {
            printf("[ERROR] start decoderet mode fail %d", ret);
        } else {
            break;
        }
        os_delay_us(5000);
    }while(++count < 50000);
    
    require_action(ret == 50000, error, "[ERROR] start play fail %d",ret);
    printf("[INFO] play start success\n");
    
    uint32_t currentPlay = 0;    
    for(filePos = 0;filePos < fileSize - 1024 * 2;)
    {   
        MEMCPY(sendData, pcmData_16K_1 + filePos, FRAMELEN);
        ret = apSendData(sendData, FRAMELEN);
        
        if(ret) {
            printf("send data %d\n", ret);
        }
        filePos += FRAMELEN;
    }
    
    apPlayStop();
    
    ret = apSetDecodeMode();
    require_action(ret != OK_RESPONSE && ret != ERROR_OTHER, error, "[ERROR] start decoderet mode fail %d",ret);

    printf("[INFO] pcm play end\n");
    return;    
error:    
    printf("[ERROR] pcm play test abnormal exit\n");
}
#endif

extern uint8_t g_enableDma;

void audio_test_main(void *pvParameters)
{    
    // AP8048 PWRKEY
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO26_U, FUNC_GPIO26_GPIO26);
    SET_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO26_U, FUN_PU);   
    gpio_set_level(26, 0x01);
    
    delays(1);
    g_enableDma = 1;
    apInit();
    apSetResponse(true);
    
    apVolSet(80);
    
    #if MP3TEST
    mp3_play_test();
    #endif

    #if MICTEST
    mic_recode_opus_16K_01_test();
    mic_recode_pcm_16K_01_test();
    #endif
    
    #if AACTEST
    aac_play_test();
    #endif
    
    #if AUOXTEST
    auox_test();
    #endif
    
    #if PCM1601TEST
    pcm1601_play_test();
    #endif

    #if PCM4402TEST
    pcm4402_play_test();
    #endif
    
    printf("****************************demo     end*****************************\n");
    
#if 0    
    printf("****************************encode/decode change test****************\n");    
    uint8_t count = 0;
    for(count = 0;count < 3;count++)
    {
        printf("count %d\n",count);
        aac_play_test();
        mic_test();        
    }
#endif

    while(1)
    {
        vTaskDelay(100);
    }
}  
void printfByteS(uint8_t *indata, uint16_t len)
{
    int i = 0;
    for(; i < len ; i++)
    {
        printf("%02X", indata[i]);
    }
    printf("\n");
}

static esp_err_t eventHandler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:        
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        esp_wifi_connect();        
        break;
    default:
        break;
    }
    return ESP_OK;
}
void openWifi()
{
    #define WIFI_SSID "steven"
    #define WIFI_PASS "hello5617"
    
    nvs_flash_init();
    system_init();
    tcpip_adapter_init();    

    ESP_ERROR_CHECK( esp_event_loop_init(eventHandler, NULL) );
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t sta_config = {
        .sta = {
            .ssid       = WIFI_SSID,
            .password   = WIFI_PASS,
            .bssid_set  = false
        }
    };
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );
}

void audio_test()
{
    openWifi();
    printf("%s is running %d system heap size is %d\r\n",__func__,system_get_time(),system_get_free_heap_size());
    if (xTaskCreate(audio_test_main, "audio_test_main", 10 * 1024, NULL, 7, NULL) != pdPASS) {
        printf("Error create audio_test_main\n");
    }    
}
