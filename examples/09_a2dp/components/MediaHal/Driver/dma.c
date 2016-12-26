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

#include "dma.h"
#include "rom/ets_sys.h"
#include "string.h"
#include "stdlib.h"

#define QUEUE_BLOCK_LENGTH  (4096L)


#define DMA_DBG_WARING_ENABLE (0)
#define DMA_DBG_ERROR_ENABLE  (0)
#define DMA_INFO_ENABLE       (0)
#define DMA_DBG_ENABLE        (0)

//DBG INFOR
#if DMA_DBG_ENABLE
#define DMA_DBG(format,...) do{\
        ets_printf("[dbg][%s#%u]",__FUNCTION__,__LINE__);\
        ets_printf(format,##__VA_ARGS__);\
}while(0)
#else
#define DMA_DBG(...)
#endif
#if DMA_INFO_ENABLE
#define DMA_INFO(format,...) do{\
        ets_printf("[info][%s#%u]",__FUNCTION__,__LINE__);\
        ets_printf(format,##__VA_ARGS__);\
}while(0)
#else
#define DMA_INFO(...)
#endif

#if DMA_DBG_WARNING_ENABLE
#define DMA_WARNING(format,...) do{\
        ets_printf("[waring][%s#%u]",__FUNCTION__,__LINE__);\
        ets_printf(format,##__VA_ARGS__);\
}while(0)
#else
#define DMA_WARNING(...)
#endif
#if DMA_DBG_ERROR_ENABLE
#define DMA_ERROR(format,...) do{\
        ets_printf("[error][%s#%u]",__FUNCTION__,__LINE__);\
        ets_printf(format,##__VA_ARGS__);\
}while(0)
#else
#define DMA_ERROR(...)
#endif

void dma_show_queue(ping_pong_buf_t *pcfg)
{
    uint32_t i;
    DMA_INFO("obj=%x \r\n", pcfg);
    DMA_INFO("ping bck que=%x buf=%x,next_link_ptr=%x\r\n", pcfg->ping->backup_queue, pcfg->ping->buffer_addr, pcfg->ping->backup_queue.next_link_ptr);
    DMA_INFO("ping first_que=%x, %08x, buf=%x\r\n", pcfg->ping->first_queue,*pcfg->ping->first_queue, pcfg->ping->buffer_addr);
    DMA_INFO("ping last_que=%x, %08x, buf=%x\r\n", pcfg->ping->last_queue, *pcfg->ping->last_queue, pcfg->ping->buffer_addr);

    DMA_INFO("pong bck que=%x buf=%x,next_link_ptr=%x\r\n", pcfg->pong->backup_queue, pcfg->pong->buffer_addr, pcfg->pong->backup_queue.next_link_ptr);
    DMA_INFO("pong first_que=%x, %08x, buf=%x\r\n", pcfg->pong->first_queue, *pcfg->pong->first_queue, pcfg->pong->buffer_addr);
    DMA_INFO("pong last_que=%x, %08x, buf=%x\r\n", pcfg->pong->last_queue, *pcfg->pong->last_queue, pcfg->pong->buffer_addr);

    dma_queue_t *addr = (dma_queue_t*)pcfg->ping->first_queue;
    dma_queue_t *addr1 = (dma_queue_t*)pcfg->pong->first_queue;
    for (i = 0; i < pcfg->queue_cnt; ++i) {
        DMA_INFO("ping queue%d buf:%08x,len=%d,size=%d,cur_link:%08x,next_link:%08x\r\n", i,
                 addr->buf_ptr,addr->data_length, addr->block_size, addr, addr->next_link_ptr);
        addr = (dma_queue_t*)addr->next_link_ptr;
    }
    for (i = 0; i < pcfg->queue_cnt; ++i) {
        DMA_INFO("pong queue%d buf:%08x,len=%d,size=%d,cur_link:%08x,next_link:%08x\r\n", i,
                 addr1->buf_ptr,addr1->data_length, addr1->block_size, addr1, addr1->next_link_ptr);
        addr1 = (dma_queue_t*)addr1->next_link_ptr;
    }
}

/**
 * @brief Fill a link
 *
 */
static void fill_one_link(uint8_t own, uint8_t eof, uint8_t sub_sof, uint16_t size, uint16_t length,
                          uint32_t *buf_ptr, dma_queue_t *nxt_ptr, dma_queue_t *i2s_queue)
{
    i2s_queue->owner = own;
    i2s_queue->eof = eof;
    i2s_queue->sub_sof = sub_sof;
    i2s_queue->data_length = 0x0FFF & length;
    i2s_queue->block_size = size ;
    i2s_queue->buf_ptr = (uint32_t)buf_ptr;
    i2s_queue->next_link_ptr = (uint32_t)nxt_ptr;
    i2s_queue->unused = 0;
}

/**
 * @brief Fill the queue
 *
 */
static int dma_queue_fill(uint32_t cnt, uint32_t len, ping_pong_buf_t *cfg)
{
    if (0 == cnt) {
        return -1;
    }
    // ping queue list
    dma_queue_t *pingAry[cnt];
    // pong queue list
    dma_queue_t *pongAry[cnt];
    uint32_t i, j;
    memset(&pingAry, 0, sizeof(pingAry));
    memset(&pongAry, 0, sizeof(pongAry));
    cnt = 1;
    for (i = 0; i < cnt; ++i) {
        pingAry[i] = (dma_queue_t*)malloc(sizeof(dma_queue_t));
        if (pingAry[i] == NULL) {
            for (j = 0; j < i; ++j) {
                free(pingAry[j]);
                pingAry[j] = NULL;
            }
            return -2;
        }
    }
    for (i = 0; i < cnt; ++i) {
        pongAry[i] = (dma_queue_t*)malloc(sizeof(dma_queue_t));
        if (NULL == pongAry[i]) {
            for (j = 0; j < cnt; ++j) {
                free(pingAry[j]);
                pingAry[j] = NULL;
            }
            for (j = 0; j < i; ++j) {
                free(pongAry[j]);
                pongAry[j] = NULL;
            }
            return -2;
        }
    }

    cfg->ping->first_queue = pingAry[0];
    cfg->pong->first_queue = pongAry[0];
    if (1 == cnt) {
        cfg->ping->last_queue = pingAry[0];
        cfg->pong->last_queue = pongAry[0];
    } else {
        cfg->ping->last_queue = pingAry[cnt - 1];
        cfg->pong->last_queue = pongAry[cnt - 1];
    }
    uint32_t remainSize = len;
    uint32_t bufSize = QUEUE_BLOCK_LENGTH;
    for (i = 0; i < cnt; ++i) {
        if (1 == cnt) {
            // Queue list include only one link, and set up eof bit.
            if (QUEUE_BLOCK_LENGTH == len) {
                bufSize = len - 1;
            } else {
                bufSize = len;
            }
            fill_one_link(1, 1, 0, bufSize, bufSize, cfg->ping->buffer_addr, pongAry[i], pingAry[i]);
            fill_one_link(1, 1, 0, bufSize, bufSize, cfg->pong->buffer_addr, pingAry[i], pongAry[i]);
        } else {
            if (i == (cnt - 1)) {
                // ping/pong queue list last link connect to the pong/ping first link, and set up eof bit.
                bufSize = remainSize;
                fill_one_link(1, 1, 0, bufSize, bufSize, cfg->ping->buffer_addr + ((QUEUE_BLOCK_LENGTH / sizeof(uint32_t)) * i),
                              pongAry[0], pingAry[i]);
                fill_one_link(1, 1, 0, bufSize, bufSize, cfg->pong->buffer_addr + ((QUEUE_BLOCK_LENGTH / sizeof(uint32_t)) * i),
                              pingAry[0], pongAry[i]);
            } else {
                // Conncet the next link.
                fill_one_link(1, 0, 0, bufSize - 1, bufSize - 1, cfg->ping->buffer_addr + ((QUEUE_BLOCK_LENGTH / sizeof(uint32_t)) * i), pingAry[i + 1], pingAry[i]);
                fill_one_link(1, 0, 0, bufSize - 1, bufSize - 1, cfg->pong->buffer_addr + ((QUEUE_BLOCK_LENGTH / sizeof(uint32_t)) * i), pongAry[i + 1], pongAry[i]);
            }
        }
        remainSize -= bufSize;
    }
    return 0;
}


/**
 * @brief Create a ping-pong buffer object used by DMA.
 *
 */
ping_pong_buf_t* dma_buf_create(uint32_t bufLen)
{
    if (0 == bufLen) {
        return NULL;
    }
    uint32_t i, j;
    uint32_t queue_cnt ;
    uint8_t * pBuf = NULL;
    i = bufLen / QUEUE_BLOCK_LENGTH;
    j = bufLen % QUEUE_BLOCK_LENGTH;
    if (0 == j) {
        queue_cnt = i;
    } else {
        queue_cnt = i + 1;
    }
    DMA_INFO("\r\nbufLen=%d queue_cnt=%d\r\n", bufLen, queue_cnt);

    ping_pong_buf_t* pcfg;
    pcfg = (ping_pong_buf_t*)malloc(sizeof(ping_pong_buf_t));
    if (NULL == pcfg) {
        return NULL;
    }
    pBuf = ((uint8_t*)malloc(bufLen * 2)); // buflen is number of bytes buffer.malloc ping and pong buffer.
    pcfg->ping = (dma_element_t*)malloc(sizeof(dma_element_t));
    pcfg->pong = (dma_element_t*)malloc(sizeof(dma_element_t));
    if ((NULL == pBuf)
        || (NULL == pcfg->pong)
        || (NULL == pcfg->ping)) {
        free(pBuf);
        pBuf = NULL;
        free(pcfg->pong);
        pcfg->pong = NULL;
        free(pcfg->ping);
        pcfg->ping = NULL;
        free(pcfg);
        pcfg = NULL;
        DMA_INFO("Malloc ping->buffer_addr failed");
        return NULL;
    }
    memset(pBuf, 0, (bufLen * 2));    
    memset(pcfg->ping, 0, sizeof(dma_element_t));
    memset(pcfg->pong, 0, sizeof(dma_element_t));
    pcfg->ping->buffer_addr = (uint32_t*)pBuf;
    pcfg->pong->buffer_addr = (uint32_t*)(pBuf + bufLen);
    pcfg->queue_cnt = queue_cnt;  // Number of queue
    if (dma_queue_fill(queue_cnt, bufLen, pcfg) < 0) {
        free(pcfg->ping->buffer_addr);
        pcfg->ping->buffer_addr = NULL;
        free(pcfg->pong->buffer_addr);
        pcfg->pong->buffer_addr = NULL;
        free(pBuf);
        pBuf = NULL;
        free(pcfg->pong);
        pcfg->pong = NULL;
        free(pcfg->ping);
        pcfg->ping = NULL;
        free(pcfg);
        pcfg = NULL;
        return NULL;
    }
    pcfg->len = bufLen;         // Buffer length
    dma_show_queue(pcfg);
    return pcfg;
}
static esp_err_t dma_queue_reset(int32_t que_size, dma_element_t *obj)
{
    if (NULL == obj) {
        return ESP_FAIL;
    }
    // No need reset;
    if (0 == obj->backup_queue.next_link_ptr) {
        return ESP_OK;
    }
    dma_queue_t *dmaQueCur = obj->first_queue;
    dma_queue_t *dmaQueNext = NULL;
    if (que_size > 1) {
        while (dmaQueNext != obj->first_queue) {
            dmaQueNext = (dma_queue_t*)dmaQueCur->next_link_ptr;
            if ((dma_queue_t*)obj->backup_queue.next_link_ptr == dmaQueNext) {
                DMA_INFO("find next_link_ptr=%x \r\n", dmaQueNext);
                break;
            }
            dmaQueCur = dmaQueNext;
        }
    }
    memcpy(dmaQueCur, &obj->backup_queue, sizeof(obj->backup_queue));
    memset(&obj->backup_queue, 0, sizeof(obj->backup_queue));

    return ESP_OK;
}

/**
 * @brief Reset the dma buffer length.
 *
 */
esp_err_t dma_buf_len_reset(ping_pong_buf_t *obj)
{
    if (NULL == obj) {
        return ESP_FAIL;
    }
    dma_queue_t *dmaQueCur = obj->ping->first_queue;
    dma_queue_t *dmaQueNext = NULL;
    esp_err_t ret = ESP_OK;
    
    DMA_INFO("next_link_ptr=%x lenght=%d\r\n", obj->ping->backup_queue.next_link_ptr, obj->ping->first_queue->data_length);
    
    obj->ping->first_queue->owner = 1;
    obj->ping->last_queue->owner = 1;
    obj->pong->first_queue->owner = 1;
    obj->pong->last_queue->owner = 1;
    
    obj->pong->first_queue->data_length = 32;
    obj->pong->last_queue->data_length = 32;
    obj->ping->first_queue->data_length = 32;
    obj->ping->last_queue->data_length = 32;
    
    ret = dma_queue_reset(obj->queue_cnt, obj->ping);
    ret += dma_queue_reset(obj->queue_cnt, obj->pong);
    
    //dma_show_queue(obj);
    DMA_INFO("[%s # %u lenght=%d]\r\n",__FUNCTION__,__LINE__, obj->ping->first_queue->data_length);

    return ret;
}

/**
 * @brief Set the buffer length before the start.
 *
 */
esp_err_t dma_buf_len_set(ping_pong_buf_t *obj, dma_element_t *element, uint32_t len)
{
    if (NULL == obj) {
        return ESP_FAIL;
    }
    if (len < obj->len) {
        int i, k, cnt;
        i = len / QUEUE_BLOCK_LENGTH;
        k = len % QUEUE_BLOCK_LENGTH;
        if (0 == k) {
            cnt = i;
        } else {
            cnt = i + 1;
        }
        dma_queue_t *dmaQueCur = element->first_queue;
        dma_queue_t *dmaQueNext = NULL;
        while (--cnt) {
            dmaQueNext = (dma_queue_t*)dmaQueCur->next_link_ptr;
            dmaQueCur = dmaQueNext;
        }
        memcpy(&element->backup_queue, (dma_queue_t*)dmaQueCur, sizeof(element->backup_queue));
        
        dmaQueCur->next_link_ptr = 0;
        dmaQueCur->data_length = k;
    }
    return ESP_OK;
}

/**
 * @brief Destroy the ping-pong buffer instance.
 *
 */
void dma_buf_destroy(ping_pong_buf_t *obj)
{
    ping_pong_buf_t *temp = obj;
    dma_show_queue(temp);
    if (NULL != temp) {
        // Free the link list
        uint32_t i = 0;
        dma_queue_t *curtCfg = temp->ping->first_queue;
        dma_queue_t *nextCfg = NULL;
        for (i = 0; i < temp->queue_cnt; ++i) {
            nextCfg = (dma_queue_t*)curtCfg->next_link_ptr;
            free(curtCfg);
            curtCfg =  NULL;
            curtCfg = nextCfg;
        }
        curtCfg = temp->pong->first_queue;
        nextCfg = NULL;
        for (i = 0; i < temp->queue_cnt; ++i) {
            nextCfg = (dma_queue_t*)curtCfg->next_link_ptr;
            free(curtCfg);
            curtCfg =  NULL;
            curtCfg = nextCfg;
        }
        // Free the buffer
        free(temp->ping->buffer_addr);
        temp->ping->buffer_addr = NULL;
        free(temp->ping);
        temp->ping = NULL;
        free(temp->pong);
        temp->pong = NULL;
        free(temp);
        temp = NULL;
    }
}
