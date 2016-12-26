#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include "Utility.h"
#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include "MyString.h"

#ifdef _DEBUG
#define UTL_MALLOC  malloc
#define UTL_FREE    free

int utl_malloc_counter = 0;
void* utl_malloc (int size)
{
    ++utl_malloc_counter;
#ifdef TRACK_MALLOC_VERBOSE
    printf("utl_malloc_counter=%d\r\n", utl_malloc_counter);
#endif
    return malloc(size);
}

void utl_free (void *ptr)
{
    --utl_malloc_counter;
#ifdef TRACK_MALLOC_VERBOSE
    printf("utl_malloc_counter=%d\r\n", utl_malloc_counter);
#endif
    free(ptr);
}
#endif

#ifndef _DEBUG
#define UTL_MALLOC  malloc
#define UTL_FREE    free
#endif

void* EspCopyArray(int elementSize, int numElements, const void* data)
{
    int size;
    void* dataCopy = NULL;

    size =  elementSize * numElements;
    dataCopy = (void*) UTL_MALLOC (size);
    memcpy(dataCopy, data, size);

    return dataCopy;
}

void _SafeFree1 (void* freeThis)
{
    if (freeThis != NULL) {
        UTL_FREE (freeThis);
    }
}

void SafeFree1 (void** freeThis)
{
    _SafeFree1(*freeThis);

    *freeThis = NULL;
}

char* SafeStringCopy1 (char* storeHere, const char* str)
{
    char* retVal = storeHere;
    int size = 1;

    if (str != NULL) {
        if (storeHere == NULL) {
            size = (int) strlen(str) + 1;
            retVal = EspCopyArray(1, size, str);
        } else {
            strcpy(retVal, str);
        }
    } else {
        if (storeHere == NULL) {
            retVal = (char*) UTL_MALLOC(1);
        }
        retVal[0] = '\0';
    }

#ifdef _DEBUG
#ifdef WIN32
    /*
    if (!HeapValidate(GetProcessHeap(), 0, NULL))
    {
        foo = 3;
    }
    */
#endif
#endif

    return retVal;
}

/** Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return 0: successed;1:failed;
 */
int mutex_init(mutex_t *pxMutex)
{
    int xReturn = -1;
    *pxMutex = xSemaphoreCreateMutex();
    if (*pxMutex != NULL) {
        xReturn = 0;

    } else {
        ;
    }
    return xReturn;
}

/** Lock a mutex
 * @param mutex the mutex to lock
 */
void mutex_lock(mutex_t *pxMutex)
{
    while (xSemaphoreTake(*pxMutex, portMAX_DELAY) != pdPASS);
//    printf("[%08x]%s\r\n",pxMutex,__func__);
}

/** Unlock a mutex
 * @param mutex the mutex to unlock
 */
void mutex_unlock(mutex_t *pxMutex)
{
    xSemaphoreGive(*pxMutex);
//    printf("[%08x]%s\r\n",pxMutex,__func__);
}

/** Delete a semaphore
 * @param mutex the mutex to delete
 */
void mutex_destroy(mutex_t *pxMutex)
{
    vQueueDelete(*pxMutex);
}

/////-----------------------------------
//int sem_init_t (sem_t* sem, int pshared, unsigned int value)
//{
//    xSemaphoreHandle xSemaphore;
//    xSemaphore = xSemaphoreCreateCounting(10, value);
//    //printf("xSemaphore: 0x%08x\r\n",xSemaphore);
//    if (xSemaphore) {
//        *sem = (int)xSemaphore;
//        return 0;
//    } else {
//        return 1;
//    }
//}

//int sem_wait_t(sem_t *sem)
//{

//    xSemaphoreHandle xSemaphore = (xSemaphoreHandle)(*sem);
//    if (xSemaphore) {
//        while (1) {
//            if (pdTRUE == xSemaphoreTake(xSemaphore, 10)) {
//                break;
//            }
//        }
//        return 0;
//    } else {
//        return -1;
//    }
//}

//int sem_post_t(sem_t *sem)
//{
//    xSemaphoreHandle xSemaphore = (xSemaphoreHandle)(*sem);
//    if (xSemaphore) {
//        xSemaphoreGive(xSemaphore);
//        return 0;
//    } else {
//        return -1;
//    }
//}

//int sem_destroy_t(sem_t* sem)
//{
//    xSemaphoreHandle xSemaphore = (xSemaphoreHandle)(*sem);
//    if (xSemaphore) {
//        vSemaphoreDelete(xSemaphore);
//        *sem = 0;
//        return 0;
//    } else {
//        return -1;
//    }
//}

//int sem_trywait_t(sem_t* sem)
//{
//    xSemaphoreHandle xSemaphore = (xSemaphoreHandle)(*sem);
//    if (xSemaphore) {
//        if (pdTRUE == xSemaphoreTake(xSemaphore, 0)) return 0;
//        else return -1;
//    } else {
//        return -1;
//    }
//}

#if  0


uint32 task_cnt = 0;
//int pthread_task_create(pthread_t *thread,pthread_attr_t *attr,void* function,void* param)
int pthread_task_create(xTaskHandle *thread_handle, void *attr, void* function, void* param)
{
    printf("task num: %d \r\n", ++task_cnt);
    xTaskCreate(function, "test", 512, param, 18, thread_handle);
    return 0;
}

int pthread_task_detach(xTaskHandle *thread_handle)
{
    printf("task num: %d \r\n", --task_cnt);
    vTaskDelete(thread_handle);
}

void gettimeofday_tt(struct timeval* t, void* timezone)
{
    /* wujg : pass compile first */
    if (timezone != NULL)
        t->tv_sec = *(time_t*)timezone + system_get_time() / 1000000;
    else
        t->tv_sec = system_get_time() / 1000000;
    //t->tv_sec = system_get_time() + system_get_time()/1000000;
    t->tv_usec = 0; /* 1ms precision */
}

#define MAX_LEN  1460
#define RETRY_NUM  3
int IRAM_ATTR send_t(int s, const void *data, int size, int flags)
{
#if 1
    int send_len = 0;
    int retry = 0;
    int res;
    printf("send_t ...,retry num: %d; send_len: %d;size:%d\r\n", retry, send_len, size);

    while (retry < RETRY_NUM && send_len < size) {
        printf("in while\r\n");
        if ((size - send_len) <= MAX_LEN) {
            res = write(s, (uint8*)data + send_len, size - send_len);
            send_len += res > 0 ? res : 0;

        } else {
            res = write(s, (uint8*)data + send_len, MAX_LEN);
            send_len += res > 0 ? res : 0;
        }
        printf("send data len: %d\r\n", send_len);
        res <= 0 ? (retry++) : (retry);
    }
    return send_len;

#else
    if (size <= 1460) {
        return lwip_send(s, data, size, flags);
    } else {
        int total_num = (size % MAX_LEN == 0) ? (size / MAX_LEN) : (size / MAX_LEN + 1);
        int i;
        for (i = 0; i < total_num; i++) {
            if (i == total_num - 1)
                lwip_send(s, (uint8*)data + MAX_LEN * i, size - MAX_LEN * i, flags);
            else
                lwip_send(s, (uint8*)data + MAX_LEN * i, MAX_LEN, flags);
            vTaskDelay(2);

        }

    }
    return size;
#endif
}

int IRAM_ATTR sendto_t(int s, const void *data, int size, int flags, const struct sockaddr *to, socklen_t tolen)
{
    printf("s1\r\n");
    if (size <= 1460) {
        printf("s2\r\n");
        return lwip_sendto(s, data, size, flags, to, tolen);
    } else {
        printf("s3\r\n");
        int total_num = (size % MAX_LEN == 0) ? (size / MAX_LEN) : (size / MAX_LEN + 1);
        int i;
        for (i = 0; i < total_num; i++) {
            if (i == total_num - 1) {
                printf("s4\r\n");
                lwip_sendto(s, (uint8*)data + MAX_LEN * i, size - MAX_LEN * i, flags, to, tolen);
                printf("s5\r\n");
            } else {
                printf("s6\r\n");
                lwip_sendto(s, (uint8*)data + MAX_LEN * i, MAX_LEN, flags, to, tolen);
                printf("s7\r\n");
            }
            vTaskDelay(2);

        }

    }
    printf("s8\r\n");
    return size;
}

int fd_rec = 0;
void update_fd_rec(int fd_num)
{
    if (fd_num > fd_rec) fd_rec = fd_num;
}
int get_fd_rec()
{
    return fd_rec;
}

int socket_t(int domain, int type, int protocol)
{
    int fd_num = lwip_socket(domain, type, protocol);
    printf("!!!!!!!!\r\n");
    printf("socket fd: %d\r\n", fd_num);
    printf("!!!!!!!!\r\n");
    update_fd_rec(fd_num);
    printf("fd rec::: %d\r\n", get_fd_rec());

    return fd_num;
}


#endif
//============================================

void debug_func()
{
   printf("debug_func...\r\n");
}

