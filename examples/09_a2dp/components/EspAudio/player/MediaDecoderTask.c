#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "esp_types.h"
#include "stdlib.h"
#include "stdio.h"
#include "audio_log.h"

#include "inc/DecoderCom.h"
#include "EspAudioCom.h"
#include "Storage.h"
#include "Utility.h"
#include "inc/MediaDecoderTask.h"


#define  DECODER_TSK_PRIO       8
#define  DATA_PKG_SIZE        (512*2)

#if 0
#define DEBUGPLY    printf
#else
#define DEBUGPLY
#endif

static xQueueHandle rdQue;

void mediaDecoderTask(void *pvParameters);

typedef struct {
    TaskState state;
    pthread_t thread;
    int threadQuit;
    TaskState lastState;
    DecoderObj player;
} MediaPlayerInstance;
extern uint32_t rdpos;
extern uint32_t wrpos;


MediaPlayerInstance decoderInstance = { 0 };
static StreamInfo streamInfo;

typedef void (*mediaTaskCallback)(uint32_t *state);
static mediaTaskCallback stateCB;


void mediaDecoderTask(void *pvParameters)
{
    MediaPlayerInstance *MPI = (MediaPlayerInstance *)pvParameters;
    LOGI("%s is running.%x %x", __func__, pvParameters, MPI->threadQuit);

/////////////////////////////
    MPI->threadQuit = 0;
    int newPos = 0;
    uint32_t wrPos, rdPos;
    int isLoopNum = 0;
    int readSize = DATA_PKG_SIZE;
    int writeDataFinished = 0;
    while (MPI->threadQuit == 0) {
        switch (MPI->state) {
        case TaskState_Ready:
            if (MPI->player.musicType == EspAudioMeidaType_Unknown) {
                vTaskDelay(100);
                continue;
            }
            LOGD("MPI is here,rate=%d,channel=%d,bitLen=%x",
                 streamInfo.rate,
                 streamInfo.ch,
                 streamInfo.bitLen);
            MusicInfo setInfo;
            setInfo.num_channels = streamInfo.ch;
            setInfo.sampling_rate = streamInfo.rate;
            mediaHalModeSet(MediaMode_Decode, &setInfo);
            mediaHalPlay(MPI->player.musicType);
            audioBufPosGet(&rdPos, &wrPos);
            if ((InputSrcType_NetSpec == MPI->player.srcType)
                || (InputSrcType_LocalFile == MPI->player.srcType)
                || ((InputSrcType_Stream == MPI->player.srcType) &&  (MPI->player.totalByteLength != 0))) {
                if ((MPI->player.totalByteLength - MPI->player.bytePosition) < DATA_PKG_SIZE) {
                    readSize = MPI->player.totalByteLength - MPI->player.bytePosition;
                } else {
                    readSize = DATA_PKG_SIZE;
                }
            } else {
                readSize = DATA_PKG_SIZE;
            }
            LOGD("rdPos=%x,wrPos=%x,readSize=%d, type=%d\r\n", rdPos, wrPos, readSize, MPI->player.srcType);
            MPI->state = TaskState_Running;
            writeDataFinished = 0;
            isLoopNum  = 0;

            MPI->lastState = TaskState_Ready;
            break;
        case TaskState_Pause:
            mediaHalPause();
            LOGI("Pause playing music....");
            MPI->state = TaskState_Idle;
            MPI->lastState = TaskState_Pause;
            break;
        case TaskState_Resume:
            mediaHalPlay(MPI->player.musicType);
            MPI->state = TaskState_Running;
            MPI->lastState = TaskState_Resume;
            LOGI("Resume playing music....");
            break;
        case TaskState_Running: {
            if (1 == writeDataFinished) {
                MusicInfo info;
                mediaHalMusicInfoGet(&info);
                if (1 == info.bufEmpty) {
                    MPI->state = TaskState_Stoped;
                    LOGI("Set state TaskState_Stoped");
                } else {
                    LOGI("Waiting for decoder empty...");
                }
                vTaskDelay(100 / portTICK_PERIOD_MS);
                break;
            }
            int res = audioBufRead(MPI->player.dat, readSize, 100);
            if (res == 0) {
                MPI->state = TaskState_Stoped;
                LOGD("audio buffer read quit");
                break;
            }
            MediaErr ret = mediaHalDataWrite((uint8_t*)MPI->player.dat, readSize);
            if (ret == MediaErr_NoError) {
                MPI->player.bytePosition += readSize;
                if ((InputSrcType_NetSpec == MPI->player.srcType)
                    || (InputSrcType_LocalFile == MPI->player.srcType)
                    || ((InputSrcType_Stream == MPI->player.srcType) && (MPI->player.totalByteLength != 0))) {
                    newPos = MPI->player.bytePosition + DATA_PKG_SIZE;
                    if (MPI->player.totalByteLength <= newPos) {
                        writeDataFinished = 1;
                        readSize = MPI->player.totalByteLength - MPI->player.bytePosition;

                        MPI->player.bytePosition = MPI->player.totalByteLength;
                        LOGD("Last byte Pos=%d total=%d", MPI->player.bytePosition, MPI->player.totalByteLength);
                        break;
                    } else {
                        readSize = DATA_PKG_SIZE;
                    }
                }
            } else if (MediaErr_CodecError == ret) {
                uint32_t rdPOS, wrPOS;
                audioBufPosGet(&rdPOS, &wrPOS);
                LOGI("ap8048 init failed ret=%x.Try again.rdPOS=%x,wrPOS=%x", ret, rdPOS, wrPOS);
                audioBufRdPosSet(&rdPos);
                mediaHalStop();
                MPI->state = TaskState_Ready;
            } else {
                LOGE("ap8048 ret error[%x]", ret);
            }
            MPI->lastState = TaskState_Running;
        }

        break;
        case TaskState_Stoped: {
            mediaHalStop();
            newPos = 0;
            if (MPI->player.bytePosition == MPI->player.totalByteLength) {
                MPI->state = TaskState_Finished;
                MPI->player.bytePosition = 0;
                MPI->player.totalByteLength = 0;
            } else {
                MPI->state = TaskState_Idle;
            }
            LOGI("Stop playing music....state[%d]", MPI->state);
            MPI->lastState = TaskState_Stoped;
            if (stateCB) stateCB(&MPI->state);
            break;
        }
        case TaskState_Finished:
        case TaskState_Idle:
        default:
            vTaskDelay(300 / portTICK_PERIOD_MS);
//            LOGD("MPI->state[%d]", MPI->state);
            break;
        }
    }
    MPI->threadQuit = -1;
    MPI->thread = NULL;
    MPI->state = TaskState_Unknown;
    MPI->lastState = TaskState_Unknown;

    LOGD("%s is ended...", __func__);

    vTaskDelete(NULL);
}

void mediaDecoderTaskCb(void *func)
{
    stateCB = func;
}
TaskState mediaDecoderTaskStateGet()
{
    return decoderInstance.lastState;
}

void mediaDecoderTaskInfoGet(DecoderObj *obj)
{
    if (NULL == obj) {
        LOGE("Invalid para");
        return ;
    }
    memcpy(obj, &decoderInstance.player, sizeof(*obj));
}

void mediaDecoderStreamInfoSet(StreamInfo *info)
{
    if (NULL == info) {
        LOGE("Invalid para");
        return ;
    }
    memcpy(&streamInfo, info, sizeof(streamInfo));
    decoderInstance.player.totalByteLength = 0;
    LOGD("Set Stream info ok,rate=%d,channel=%d,bitLen=%x",
         streamInfo.rate,
         streamInfo.ch,
         streamInfo.bitLen);
}

void mediaDecoderStreamSizeUpdate(uint32_t *size)
{
    if (NULL == *size) {
        LOGE("Invalid para");
        return ;
    }
    decoderInstance.player.totalByteLength = *size;
    LOGD("Stream total size=%d", decoderInstance.player.totalByteLength);
}


void mediaDecoderTaskInfoSet(DecoderObj *obj)
{
    if (NULL == obj) {
        LOGE("Invalid para");
        return ;
    }
    memcpy(&decoderInstance.player, obj, sizeof(*obj));
    
    LOGD("Set DecoderObj ok,total=%d,type=%d,dat=%x music=%d",
         decoderInstance.player.totalByteLength,
         decoderInstance.player.srcType,
         decoderInstance.player.dat,
         decoderInstance.player.musicType);
}

void mediaDecoderTaskPause()
{
    if (0 == decoderInstance.thread) {
        decoderInstance.state = TaskState_Unknown;
        LOGE("decoderInstance.thread is NULL");
        return;
    }
    if (TaskState_Running == decoderInstance.state) {
        decoderInstance.state = TaskState_Pause;
    }
    LOGD("Task state=%d", decoderInstance.state);
}

void mediaDecoderTaskResume()
{
    if (0 == decoderInstance.thread) {
        decoderInstance.state = TaskState_Unknown;
        LOGE("decoderInstance.thread is NULL");
        return;
    }
    decoderInstance.state = TaskState_Resume;
    LOGD("Task state=%d", decoderInstance.state);
}

void mediaDecoderTaskCreate()
{
    if (NULL != decoderInstance.player.dat) {
        free(decoderInstance.player.dat);
    }
    memset(&decoderInstance, 0, sizeof(decoderInstance));
    memset(&streamInfo, 0, sizeof(streamInfo));
    decoderInstance.player.dat = calloc(1, DATA_PKG_SIZE);
    if (NULL == decoderInstance.player.dat) {
        LOGE("decoderInstance.player.dat malloc error");
        return;
    }
    xTaskCreate(mediaDecoderTask, "mediaDecoderTask", (5 * 1024), &decoderInstance, DECODER_TSK_PRIO, &decoderInstance.thread);
    configASSERT(decoderInstance.thread);
    LOGD("Task state=%d,dat:%x", decoderInstance.state, decoderInstance.player.dat);
    rdQue = xQueueCreate(2, 1);
    configASSERT(rdQue);
    audioBufRdQuitCfg(rdQue);
}


int mediaDecoderTaskStart()
{
    if (0 == decoderInstance.thread) {
        decoderInstance.state = TaskState_Unknown;
        LOGE("decoderInstance.thread is NULL");
        return -1;
    }
    if ((decoderInstance.player.srcType == InputSrcType_NetSpec)
        || (decoderInstance.player.srcType == InputSrcType_LocalFile)) {
        if (decoderInstance.player.totalByteLength == 0) {
            LOGE("Player parameter is not ready");
            decoderInstance.state = TaskState_Idle;
            return -1;
        }
    }

    decoderInstance.state = TaskState_Ready;
    LOGD("Task state=%d", decoderInstance.state);
    audioBufRdRest();
    return 0;
}

void mediaDecoderTaskStop()
{
    if (0 == decoderInstance.thread) {
        decoderInstance.state = TaskState_Unknown;
        LOGE("decoderInstance.thread is NULL");
        return;
    }
    if ((decoderInstance.lastState == TaskState_Unknown)
        || (decoderInstance.lastState == TaskState_Stoped)) {
        LOGW("dlInstance.state is [%d]", decoderInstance.state);
        return;
    }
    decoderInstance.state = TaskState_Stoped;
    configASSERT(rdQue);
    xQueueSend(rdQue, &decoderInstance.state, 0);
    LOGD("Task state=%d", decoderInstance.state);
    while ((TaskState_Finished != decoderInstance.state)
           && (TaskState_Idle != decoderInstance.state)) {
        if ((TaskState_Idle == decoderInstance.state)) {
            break;
        }
        vTaskDelay(10 / portTICK_RATE_MS);
    }
    audioBufRdRest();
    int flag;
    xQueueReceive(rdQue, &flag, 0);
    LOGD("Task state=%d", decoderInstance.state);
}

void mediaDecoderTaskDestroy()
{
    if (0 == decoderInstance.threadQuit) {
        decoderInstance.state = TaskState_Stoped;
        decoderInstance.threadQuit = 1;
        while (1) {
            if ((decoderInstance.threadQuit == -1)) {
                break;
            }
            vTaskDelay(100 / portTICK_RATE_MS);
        }
    }

    if (NULL != decoderInstance.player.dat) {
        free(decoderInstance.player.dat);
    }
    memset(&decoderInstance.player, 0, sizeof(decoderInstance.player));
    memset(&streamInfo, 0, sizeof(streamInfo));
    decoderInstance.player.dat = NULL;
    decoderInstance.state = TaskState_Unknown;
    vQueueDelete(rdQue);
    rdQue =  NULL;
    LOGD("Task state=%d", decoderInstance.state);
}
