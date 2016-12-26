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

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H
#include "esp_types.h"
#include "lwip/sys.h"
#include "DecoderCom.h"

#define ESP_URI_LEN                 1024

typedef struct {
    char   *str;
    int    srcType;       // 0:specific net audio; 1:m3u8 files; 2:audio data files
//    int    setStartBytePos;
} UriSrcAttr;

TaskState downloadTaskStateGet();

void downloadTaskContentSizeGet(uint32_t *size);

void downloadTaskInfoSet(UriSrcAttr *obj);
int downloadTaskHTTPStateGet();

void downloadTaskPause();

void downloadTaskResume();

int downloadTaskStart();
void downloadTaskStop();

void downloadTaskCreate(int taskType);
void downloadTaskDestroy();

#endif //
