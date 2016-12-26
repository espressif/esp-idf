/*****************************************************************************
*
* File Name : wm_http_client.h
*
* Description: Http client header file.
*
* Copyright (c) 2014 Winner Microelectronics Co., Ltd.
* All rights reserved.
*
* Author : wanghf
*
* Date : 2014-6-6
*****************************************************************************/

#ifndef __PLAYERTASK_H__
#define __PLAYERTASK_H__
#include "esp_types.h"
#include "EspAudioCom.h"
#include "EspAudio.h"
#include "lwip/sys.h"
#include "MediaHal.h"

//Type Definitions

typedef struct {
    char                *dat;
    InputSrcType        srcType;
    EspAudioMeidaType   musicType;
    uint32_t            totalByteLength;
    uint32_t            bytePosition;
    int                 codecVer;
    int                 codecType;  // 0:hardware; 1:software
} DecoderObj;

typedef struct {
    uint32_t rate;
    uint32_t ch;
    uint32_t bitLen;
    uint32_t size; // 0xFFFFFFFF stream length is infinite 
} StreamInfo;

TaskState mediaDecoderTaskStateGet();
void mediaDecoderStreamInfoSet(StreamInfo *info);

void mediaDecoderTaskInfoGet(DecoderObj *obj);
void mediaDecoderTaskInfoSet(DecoderObj *obj);
void mediaDecoderTaskPause();
void mediaDecoderTaskResume();

int mediaDecoderTaskStart();
void mediaDecoderTaskStop();

void mediaDecoderTaskCreate();
void mediaDecoderTaskDestroy();

void mediaDecoderVolSet();
void mediaDecoderVolGet();
void mediaDecoderMusicInfoGet();
void mediaDecoderDecodeTimeGet();
void mediaDecoderModeSet();
void mediaDecoderStreamSizeUpdate(uint32_t *size);
void mediaDecoderTaskCb(void *func);



#endif //__PLAYERTASK_H__
