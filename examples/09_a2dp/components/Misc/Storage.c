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

#include "audio_log.h"
#include "EspAudioCom.h"
#include "Storage.h"
#include "Utility.h"
#include "string.h"
#include "psramApi.h"

static char *audioBuf;
static char *audioBufWrPos;
static char *audioBufRdPos;

mutex_t audioBufMutex;
#ifdef AUDIO_BUFFER_ADDR
#define ISMALLOC 0
#else
#define ISMALLOC 1
#endif

uint32_t rdpos,wrpos;

static QueueHandle_t queWrQuit;
static QueueHandle_t queRdQuit;

/**
 * @brief Initialize buffer
 *
 */
int audioBufInit()
{
    int ret = 0;
#if (ISMALLOC == 1)
    audioBuf = malloc(AUDIO_BUFFER_SIZE);
    if (NULL == audioBuf) {
        LOGE("malloc is failed.");
    }

    LOGD("Audio buffer malloc is ok,%x.", audioBuf);
#else
    audioBuf = (char*)AUDIO_BUFFER_ADDR;
    LOGD("Audio buffer address is%x, size %d bytes.", audioBuf,AUDIO_BUFFER_SIZE);
#endif

    if (NULL == audioBuf) {
        LOGE("[audioBufInit]malloc failed!\r\n");
        ret = -1;
    }
    audioBufRdPos = audioBuf;
    audioBufWrPos = audioBuf;
    mutex_init(&audioBufMutex);
    rdpos = 0;
    wrpos = 0;
    LOGI("[%08x]%s++++%d\r\n", (uint32_t)audioBufMutex, __func__, __LINE__);
    return ret;
}

/**
 * @brief Uninitialize buffer
 *
 */
void audioBufUninit()
{
    configASSERT(audioBufMutex);
    mutex_lock(&audioBufMutex);
    audioBufRdPos = NULL;
    audioBufWrPos = NULL;
    if (audioBuf) {
#if (ISMALLOC == 1)
        free(audioBuf);
#endif
        audioBuf = NULL;
    }
    mutex_unlock(&audioBufMutex);
}

/**
 * @brief Retrieves the size of the specified buffer, in bytes.
 *
 */
void audioBufRdQuitCfg(void * que)
{
    configASSERT(que);
    mutex_lock(&audioBufMutex);  
    LOGD("que=%p",que);
    queRdQuit = (QueueHandle_t)que;
    mutex_unlock(&audioBufMutex);
}

/**
 * @brief Retrieves the size of the specified buffer, in bytes.
 *
 */
void audioBufWrQuitCfg(void * que)
{
    configASSERT(que);
    mutex_lock(&audioBufMutex);
    LOGD("que=%p",que);
    queWrQuit = (QueueHandle_t)que;
    mutex_unlock(&audioBufMutex);
}


/**
 * @brief Retrieves the size of the specified buffer, in bytes.
 *
 */
int audioBufFillGet()
{
    configASSERT(audioBufMutex);
    mutex_lock(&audioBufMutex);
    int fill = audioBufWrPos - audioBufRdPos;
    if (fill < 0) {
        fill += AUDIO_BUFFER_SIZE;
    }
    mutex_unlock(&audioBufMutex);
    return fill;
}


/**
 * @brief Reads data from the specified buffer.
 *
 */
int audioBufRead(char *dest, int n, uint32_t t)
{
    configASSERT(audioBufMutex);
    int ret = 0;
    char *outDest = dest;
    if (NULL == outDest) {
        LOGE(">>>>> audioBufRead dest is NULL\r\n");
        return -1;
    }
retryR:
    ret = audioBufFillGet();
    if (ret < n) {
        int flag = 0;
        LOGD("Waiting readable semaphore,wanted[%d] fill[%d] timeout[%d]",n, ret, t);
        if (xQueueReceive(queRdQuit, &flag, t)) {            
            LOGE("receive a read quit queue\r\n");
            return 0;
        }
        else {
            goto retryR;
        }
    }
    ret = n;
    mutex_lock(&audioBufMutex);
    #if (ISMALLOC == 1)
    while (n--) {
        *outDest++ = *audioBufRdPos++;
        rdpos++;
        if (audioBufRdPos == (audioBuf + AUDIO_BUFFER_SIZE)) {
            audioBufRdPos = audioBuf;
//            LOGD(">>>>> audioBuf+ ReadPos + AUDIO_BUFFER_SIZE\r\n");
        }
    }
    #else
    if(n + audioBufRdPos <= (audioBuf + AUDIO_BUFFER_SIZE))
    {
        psram_cache_memcpy((uint8_t*) outDest, (uint8_t*) audioBufRdPos, n);
        audioBufRdPos += n;
    } else {
        uint32_t len = audioBuf + AUDIO_BUFFER_SIZE - audioBufRdPos;        
        psram_cache_memcpy((uint8_t*) outDest, (uint8_t*) audioBufRdPos, len);
        
        outDest += len;
        len      = n - len;
        psram_cache_memcpy((uint8_t*) outDest, (uint8_t*) audioBuf, len);
        audioBufRdPos = audioBuf + len;
    }
    rdpos += n;
    #endif

    mutex_unlock(&audioBufMutex);
//    LOGD("r:%p w:%p,rdpos=%d,wrpos=%d,dlt=%d f=%d",audioBufRdPos,audioBufWrPos,rdpos,wrpos,(wrpos-rdpos),audioBufFillGet());
    return ret;
}

/**
 * @brief Writes data to the specified buffer.
 *
 */
int audioBufWrite(char *src, int n, uint32_t t)
{
    configASSERT(audioBufMutex);
    char *inSrc = src;
    int ret = n;
    if (NULL == inSrc) {
        ret = -1;
        LOGE("Source is NULL\r\n");
        return ret;
    }
    int empty;
retryW:
    empty = (AUDIO_BUFFER_SIZE - audioBufFillGet());
    if (empty < n) {
        int flag = 0;        
        LOGD("Waiting writeable  semaphore,wanted[%d] empty[%d] timeout[%d]",n, empty, t);
        if (xQueueReceive(queWrQuit, &flag, t)) {            
            LOGE("receive a write quit queue\r\n");
            return 0;
        }
        else {
            goto retryW;
       }
    }
    mutex_lock(&audioBufMutex);
    #if (ISMALLOC == 1)
    while (n--) {
        *audioBufWrPos++ = *inSrc++;
        wrpos++;
        if (audioBufWrPos == (audioBuf + AUDIO_BUFFER_SIZE)) {
            audioBufWrPos = audioBuf;
            //LOGD(">>>>> audioBuf+ WritePos + AUDIO_BUFFER_SIZE\r\n");
        }
    }
    #else
    if(n + audioBufWrPos <= (audioBuf + AUDIO_BUFFER_SIZE))
    {
        psram_cache_memcpy((uint8_t*) audioBufWrPos, (uint8_t*) src, n);
        audioBufWrPos += n;
    } else {        
        uint32_t len = audioBuf + AUDIO_BUFFER_SIZE - audioBufWrPos;
        psram_cache_memcpy((uint8_t*) audioBufWrPos, (uint8_t*) inSrc, len);
        inSrc += len;        
        len = n - len;
        psram_cache_memcpy((uint8_t*) audioBuf, (uint8_t*) inSrc, len);
        audioBufWrPos = audioBuf + len;
    }
    wrpos += n;
    #endif    
    mutex_unlock(&audioBufMutex);
//    LOGD("r:%p w:%p,rdpos=%d,wrpos=%d,dlt=%d f=%d",audioBufRdPos,audioBufWrPos,rdpos,wrpos,(wrpos-rdpos),audioBufFillGet());
    return ret;
}

/**
 * @brief Reset the write and read buffer pointer.
 *
 */
void audioBufRestAll()
{
    configASSERT(audioBufMutex);
    mutex_lock(&audioBufMutex);

    audioBufWrPos = audioBuf;
    audioBufRdPos = audioBuf;
    mutex_unlock(&audioBufMutex);
}

/**
 * @brief Reset the read buffer address.
 *
 */
void audioBufRdRest()
{
    configASSERT(audioBufMutex);
    mutex_lock(&audioBufMutex);
    audioBufRdPos = audioBuf;
    mutex_unlock(&audioBufMutex);
}

/**
 * @brief Reset the write buffer address.
 *
 */
void audioBufWrRest()
{
    configASSERT(audioBufMutex);
    mutex_lock(&audioBufMutex);
    audioBufWrPos = audioBuf;
    mutex_unlock(&audioBufMutex);
}

/**
 * @brief Get the buffer read and write position.
 *
 */
void audioBufPosGet(uint32_t *rdPos, uint32_t *wrPos)
{
    configASSERT(audioBufMutex);
    mutex_lock(&audioBufMutex);
    *wrPos = (uint32_t)audioBufWrPos;
    *rdPos = (uint32_t)audioBufRdPos;
    mutex_unlock(&audioBufMutex);
}

/**
 * @brief Set the buffer read and write position.
 *
 */
void audioBufRdPosSet(uint32_t *rdPos)
{
    audioBufRdPos = (char*)*rdPos;
}


// end
