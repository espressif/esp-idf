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

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "stdlib.h"
#include "stdio.h"
#include "DecoderCom.h"
#include "EspAudioCom.h"
#include "MediaHal.h"
#include "EspAudioRecorder.h"
#include "Ap80xx.h"

#define RECORD_TSK_PRIO            6
#define RECORDER_PKG_SIZE          1024

typedef enum {
    RecorderEventType_Unknown,
    RecorderEventType_Start,
    RecorderEventType_Stop,
    RecorderEventType_Pause,
    RecorderEventType_Resume,
    RecorderEventType_Quit,
} RecorderEventType;

typedef struct {
    RecorderEventType type;
    void *value;
} RecorderEventInfo;

static xTaskHandle recorderThread;
static xQueueHandle recorderEventQue;
static RecorderState recTaskState;
static MusicInfo streamInfo;

void MediaEncoderTask(void *pvParameters)
{
    LOGI("%s is running.%x %x", __func__);
/////////////////////////////
    RecorderEventInfo eventInfo ;
    static RecorderAttr s_attr;
    static int recordStartFlag = 0;
    eventInfo.type = RecorderEventType_Unknown;
    eventInfo.value = NULL;
    uint8_t *recvData = malloc(RECORDER_PKG_SIZE);
    if (NULL == recvData) {
        LOGE("malloc failed");
        return ;
    }
    memset(recvData, 0, RECORDER_PKG_SIZE);

    while (1) {
        if (xQueueReceive(recorderEventQue, &eventInfo, 10)) {
            switch (eventInfo.type) {
            case RecorderEventType_Start: {
                MediaState mediaState;
                mediaHalStatusGet(&mediaState);
                LOGD("Start recording ...%d", mediaState);
                if (MediaState_Initialized > mediaState) {
                    mediaHalInit();                    
                    LOGD("Need to init codec driver...[%d]", mediaState);
                }
                if (eventInfo.value) {
                    memcpy(&s_attr, eventInfo.value, sizeof(s_attr));
                    free(eventInfo.value);
                    eventInfo.value =  NULL;
                    LOGI("recv:channel=%d, rate=%d, encodeType=%d, func=%p",
                         s_attr.channel, s_attr.rate, s_attr.encodeType, s_attr.func);
                    streamInfo.sampling_rate =s_attr.rate;
                    streamInfo.num_channels = s_attr.channel;
                    streamInfo.stream_type = s_attr.encodeType;

                    mediaHalModeSet(MediaMode_Encode, &streamInfo);
                    recordStartFlag = 1;
                    recTaskState = RecorderState_Recording;
                    LOGI("Start recording ....");
                } else {
                    LOGE("eventInfo.value = NULL");
                }
                break;
            }
            case RecorderEventType_Pause: {
                if (RecorderState_Recording == recTaskState) {
                    recordStartFlag = 0;
                    recTaskState = RecorderState_Paused;
                    LOGI("Pause recording");
                }
                break;
            }
            case RecorderEventType_Resume: {
                if (RecorderState_Paused == recTaskState) {
                    recordStartFlag = 1;
                    recTaskState = RecorderState_Recording;
                    LOGI("Resume recording");
                }
                break;
            }
            case RecorderEventType_Stop: {
                recordStartFlag = 0;
                recTaskState = RecorderState_Stoped;
                
                apSetDecodeMode();
                memset(&streamInfo, 0, sizeof(streamInfo));
                LOGI("Stop recording");
                break;
            }
            case RecorderEventType_Quit: {
                recordStartFlag = 0;
                recTaskState = RecorderState_Unknown;
                LOGI("MediaEncoderTask will be quit");
                goto encoderTaskQuit;
            }
            default:
                vTaskDelay(250 / portTICK_RATE_MS);
                LOGE("Event type does not support[%d]", eventInfo.type);
                break;
            }

        }
        
//        LOGD("stack left %d byte\n", uxTaskGetStackHighWaterMark(NULL));
        if (recordStartFlag) {
            int len = 0;            
            int ret = mediaHalDataRead(recvData, &len);
            if ((ret != 0) || (len == 1)) {
                LOGE("mediaHalDataRead failed,ret=%d len=%d", ret, len);
                continue;
            } else {
//                LOGD("mediaHalDataRead ok,ret=%d len=%d", ret, len);
                if (len>0) {
                    s_attr.func(recvData, (uint32_t*)&len);
                }
            }
        }
    }
encoderTaskQuit:
    free(recvData);
    recvData = NULL;
    configASSERT(recorderEventQue);
    vQueueDelete(recorderEventQue);
    recorderEventQue = NULL;    
    recorderThread = NULL;
    LOGI("%s is ended...", __func__);
    vTaskDelete(NULL);
}

EspAudioErr EspAudioRecorderStateGet(RecorderState *state)
{
    if (NULL == state) {
        LOGE("Invalid para,%p", state);
        return EspAudioErr_InvalidPara;
    }
    *state = recTaskState;

    return EspAudioErr_NoErr;
}

EspAudioErr EspAudioRecorderInit(void)
{
    if (NULL == recorderThread) {
        xTaskCreate(MediaEncoderTask, "MediaEncoderTask", (5*1024), NULL, RECORD_TSK_PRIO, &recorderThread);
        configASSERT(recorderThread);
    }
    if (NULL == recorderEventQue) {
        recorderEventQue = xQueueCreate(4, sizeof(RecorderEventInfo));
        configASSERT(recorderEventQue);
    }

    LOGD("recorder has been created, %p,%p", recorderThread, recorderEventQue);
    recTaskState = RecorderState_Init;
    return EspAudioErr_NoErr;
}

EspAudioErr EspAudioRecorderStart(RecorderAttr *recorderAttr)
{
    if ((NULL == recorderAttr)
        || (NULL == recorderThread)
        || (NULL == recorderEventQue)) {
        LOGE("Invalid para, %p,%p,%p", recorderAttr, recorderThread, recorderEventQue);
        return EspAudioErr_InvalidPara;
    }
    if (NULL  == recorderAttr->func) {
        LOGE("Invalid para, %p", recorderAttr->func);
        return EspAudioErr_InvalidPara;
    }
    RecorderAttr *attr = malloc(sizeof(RecorderAttr));
    if (NULL  == attr) {
        LOGE("Malloc failed");
        return EspAudioErr_NoMem;
    }
    attr->channel = recorderAttr->channel;
    attr->rate = recorderAttr->rate;
    attr->encodeType = recorderAttr->encodeType;
    attr->func = recorderAttr->func;

    RecorderEventInfo eventInfo ;
    eventInfo.type = RecorderEventType_Start;
    eventInfo.value = attr;
    LOGD("channel=%d, rate=%d, encodeType=%d, func=%p",
         attr->channel, attr->rate, attr->encodeType, attr->func);
    configASSERT(recorderEventQue);
    if (pdTRUE != xQueueSend(recorderEventQue, &eventInfo, 0)) {
        LOGE("EspAudioRecorderStart, queue send failed");
    } else {
        LOGD("EspAudioRecorderStart, send sucessed");
    }
    return EspAudioErr_NoErr;
}

EspAudioErr EspAudioRecorderPause(void)
{
    if ((NULL == recorderThread)
        || (NULL == recorderEventQue)) {
        LOGE("Invalid para, %p,%p", recorderThread, recorderEventQue);
        return EspAudioErr_InvalidPara;
    }
    RecorderEventInfo eventInfo ;
    eventInfo.type = RecorderEventType_Pause;
    eventInfo.value = NULL;
    configASSERT(recorderEventQue);
    if (pdTRUE != xQueueSend(recorderEventQue, &eventInfo, 0)) {
        LOGE("RecorderEventType_Pause, queue send failed");
    } else {
        LOGD("RecorderEventType_Pause, send sucessed");
    }
    return EspAudioErr_NoErr;
}

EspAudioErr EspAudioRecorderResume(void)
{
    if ((NULL == recorderThread)
        || (NULL == recorderEventQue)) {
        LOGE("Invalid para, %p,%p", recorderThread, recorderEventQue);
        return EspAudioErr_InvalidPara;
    }
    RecorderEventInfo eventInfo ;
    eventInfo.type = RecorderEventType_Resume;
    eventInfo.value = NULL;
    configASSERT(recorderEventQue);
    if (pdTRUE != xQueueSend(recorderEventQue, &eventInfo, 0)) {
        LOGE("RecorderEventType_Resume, queue send failed");
    } else {
        LOGD("RecorderEventType_Resume, send sucessed");
    }
    return EspAudioErr_NoErr;

}

EspAudioErr EspAudioRecorderStop(void)
{
    if ((NULL == recorderThread)
        || (NULL == recorderEventQue)) {
        LOGE("Invalid para, %p,%p", recorderThread, recorderEventQue);
        return EspAudioErr_InvalidPara;
    }
    RecorderEventInfo eventInfo ;
    eventInfo.type = RecorderEventType_Stop;
    eventInfo.value = NULL;
    configASSERT(recorderEventQue);
    if (pdTRUE != xQueueSend(recorderEventQue, &eventInfo, 0)) {
        LOGE("RecorderEventType_Stop, queue send failed");
    } else {
        LOGD("RecorderEventType_Stop, send sucessed");
    }
    return EspAudioErr_NoErr;
}

EspAudioErr EspAudioRecorderUninit(void)
{
    if ((NULL == recorderThread)
        || (NULL == recorderEventQue)) {
        LOGE("recorderThread=%p, recorderEventQue=%p", recorderThread, recorderEventQue);
        return EspAudioErr_InvalidPara;
    }
    RecorderEventInfo eventInfo ;
    eventInfo.type = RecorderEventType_Quit;
    eventInfo.value = NULL;
    configASSERT(recorderEventQue);
    if (pdTRUE != xQueueSend(recorderEventQue, &eventInfo, 0)) {
        LOGE("RecorderEventType_Quit, queue send failed");
    } else {
        LOGD("RecorderEventType_Quit, send sucessed");
    }
    return EspAudioErr_NoErr;
}
