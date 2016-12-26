/*
Copyright 2006 - 2011 Intel Corporation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


#ifndef UTILITY_H
#define UTILITY_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/igmp.h"

/*
 *  Defines an empty string.
 */
#define EMPTY_STRING ""

/*
 *  Used to prevent warnings on assinign NULL
 *  to a char*
 */
#define NULL_CHAR '\0'

/*
 *  Copies memory from one location to a new location
 *  and returns the pointer.
 */
void* CopyArray(int elementSize, int numElements, const void* data);

/*
 *  Does a normal free on freeThis, except
 *  that it checks for non-NULL value first.
 */
void _SafeFree1 (void* freeThis);

/*
 *  This macro calls _SafeFree and then assigns
 *  the pointer to NULL, for extra safety.
 */
//#define SafeFree(x) _SafeFree(x); x = NULL;
void SafeFree1(void** freeThis);

/*
 *  Copies a string safely.
 *  If str is NULL returned value is an empty string.
 *
 *  If storeHere is NULL, then memory is allocated
 *  by the method. Use SafeFree() to deallocate
 *  that memory.
 *
 *  Returns the copy of str.
 */
char* SafeStringCopy1 (char* storeHere, const char* str);

/*
 *  Spawns a normal thread that is detached
 *  from the calling thread.
 */
void SpawnDetachedThread(void* method, void* arg);


#define mutex_t           xSemaphoreHandle

/** Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return 0: successed;1:failed;
 */
int mutex_init(mutex_t *pxMutex);

/** Lock a mutex
 * @param mutex the mutex to lock
 */
void mutex_lock(mutex_t *pxMutex);

/** Unlock a mutex
 * @param mutex the mutex to unlock
 */
void mutex_unlock(mutex_t *pxMutex);

/** Delete a semaphore
 * @param mutex the mutex to delete
 */
void mutex_destroy(mutex_t *pxMutex);


#define TEST_OS_MODE  1

typedef int sem_t_t;
#if 1 //TEST_OS_MODE

//int sem_init_t (sem_t_t *sem, int pshared, unsigned int value);
//int sem_wait_t(sem_t_t *sem);
//int sem_post_t(sem_t_t *sem);
//int sem_destroy_t(sem_t_t* sem);
//int sem_trywait_t(sem_t_t* sem);

int pthread_task_create();
void gettimeofday_tt(struct timeval* t, void* timezone);

int send_t(int s, const void *data, int size, int flags);
int sendto_t(int s, const void *data, int size, int flags,const struct sockaddr *to, socklen_t tolen);
int get_fd_rec();
void update_fd_rec(int fd_num);
int socket_t(int domain, int type, int protocol);


#define pthread_t xTaskHandle
#define pthread_create    pthread_task_create
//#define pthread_detach    //pthread_task_detach


#if 0
#define pthread_t xTaskHandle
#define pthread_create    pthread_task_create
#define pthread_detach    //pthread_task_detach
#define gettimeofday  gettimeofday_tt
#define send(a,b,c,d)   lwip_send(a,b,c,d)//send_t(a,b,c,d)//write(a,b,c)//lwip_send(a,b,c,d) //send_t(a,b,c,d)
#define sendto(a,b,c,d,e,f) lwip_sendto(a,b,c,d,e,f)  // sendto_t(a,b,c,d,e,f)
#define socket(a,b,c)   lwip_socket(a,b,c) //socket_t(a,b,c)




//#define printf os_printf
//#define printf(fmt,...)   os_printf(fmt, ##__VA_ARGS__)
//#define ioctl   ioctlsocket
#define ASSERT(exp)    do{ \
                     if(!(exp)) {os_printf("ERROR IN RESERT IN %s:%d",__func__,__LINE__);} \
                     }while(0)

  #endif
//typedef int sem_t;
//#define sem_init(x,y,z)   sem_init_t(x,y,z)
//#define sem_destroy(x)    sem_destroy_t(x)
//#define sem_wait(x)       sem_wait_t(x)
//#define sem_trywait(x)    sem_trywait_t(x)
//#define sem_post(x)       sem_post_t(x)



void debug_func();

#endif




#endif
