#ifndef __os_ARCH_H__
#define __os_ARCH_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#define OSI_ARCH_TIMEOUT 0xffffffffUL

typedef xSemaphoreHandle osi_sem_t;
typedef xSemaphoreHandle osi_mutex_t;

#define osi_mutex_valid( x ) ( ( ( *x ) == NULL) ? pdFALSE : pdTRUE )
#define osi_mutex_set_invalid( x ) ( ( *x ) = NULL )
#define osi_sem_valid( x ) ( ( ( *x ) == NULL) ? pdFALSE : pdTRUE )
#define osi_sem_set_invalid( x ) ( ( *x ) = NULL )

int osi_mutex_new(osi_mutex_t *pxMutex);

void osi_mutex_lock(osi_mutex_t *pxMutex);

int osi_mutex_trylock(osi_mutex_t *pxMutex);

void osi_mutex_unlock(osi_mutex_t *pxMutex);

void osi_mutex_free(osi_mutex_t *pxMutex);

int osi_sem_new(osi_sem_t *sem, uint32_t max_count, uint32_t init_count);

void osi_sem_signal(osi_sem_t *sem);

uint32_t osi_sem_wait(osi_sem_t *sem, uint32_t timeout);

void osi_sem_free(osi_sem_t *sem);

void osi_arch_init(void);

uint32_t osi_now(void);

void osi_delay_ms(uint32_t ms);


#endif /* __os_ARCH_H__ */

