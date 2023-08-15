/*
 * SPDX-FileCopyrightText: 2001-2003 Swedish Institute of Computer Science
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2018-2022 Espressif Systems (Shanghai) CO LTD
 */

/* lwIP includes. */

#include <pthread.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"
#include "arch/sys_arch.h"
#include "arch/vfs_lwip.h"
#include "esp_log.h"
#include "esp_compiler.h"

static const char* TAG = "lwip_arch";

static sys_mutex_t g_lwip_protect_mutex = NULL;

static pthread_key_t sys_thread_sem_key;
static void sys_thread_sem_free(void* data);

#if !LWIP_COMPAT_MUTEX

/**
 * @brief Create a new mutex
 *
 * @param pxMutex pointer of the mutex to create
 * @return ERR_OK on success, ERR_MEM when out of memory
 */
err_t
sys_mutex_new(sys_mutex_t *pxMutex)
{
  *pxMutex = xSemaphoreCreateMutex();
  if (*pxMutex == NULL) {
    LWIP_DEBUGF(ESP_THREAD_SAFE_DEBUG, ("sys_mutex_new: out of mem\r\n"));
    return ERR_MEM;
  }

  LWIP_DEBUGF(ESP_THREAD_SAFE_DEBUG, ("sys_mutex_new: m=%p\n", *pxMutex));

  return ERR_OK;
}

/**
 * @brief Lock a mutex
 *
 * @param pxMutex pointer of mutex to lock
 */
void
sys_mutex_lock(sys_mutex_t *pxMutex)
{
  BaseType_t ret = xSemaphoreTake(*pxMutex, portMAX_DELAY);

  LWIP_ASSERT("failed to take the mutex", ret == pdTRUE);
  (void)ret;
}

/**
 * @brief Unlock a mutex
 *
 * @param pxMutex pointer of mutex to unlock
 */
void
sys_mutex_unlock(sys_mutex_t *pxMutex)
{
  BaseType_t ret = xSemaphoreGive(*pxMutex);

  LWIP_ASSERT("failed to give the mutex", ret == pdTRUE);
  (void)ret;
}

/**
 * @brief Delete a mutex
 *
 * @param pxMutex pointer of mutex to delete
 */
void
sys_mutex_free(sys_mutex_t *pxMutex)
{
  LWIP_DEBUGF(ESP_THREAD_SAFE_DEBUG, ("sys_mutex_free: m=%p\n", *pxMutex));
  vSemaphoreDelete(*pxMutex);
  *pxMutex = NULL;
}

#endif /* !LWIP_COMPAT_MUTEX */

/**
 * @brief Creates a new semaphore
 *
 * @param sem pointer of the semaphore
 * @param count initial state of the semaphore
 * @return err_t
 */
err_t
sys_sem_new(sys_sem_t *sem, u8_t count)
{
  LWIP_ASSERT("initial_count invalid (neither 0 nor 1)",
             (count == 0) || (count == 1));

  *sem = xSemaphoreCreateBinary();
  if (*sem == NULL) {
      LWIP_DEBUGF(ESP_THREAD_SAFE_DEBUG, ("sys_sem_new: out of mem\r\n"));
      return ERR_MEM;
  }

  if (count == 1) {
      BaseType_t ret = xSemaphoreGive(*sem);
      LWIP_ASSERT("sys_sem_new: initial give failed", ret == pdTRUE);
      (void)ret;
  }

  return ERR_OK;
}

/**
 * @brief Signals a semaphore
 *
 * @param sem pointer of the semaphore
 */
void
sys_sem_signal(sys_sem_t *sem)
{
  BaseType_t ret = xSemaphoreGive(*sem);
  /* queue full is OK, this is a signal only... */
  LWIP_ASSERT("sys_sem_signal: sane return value",
             (ret == pdTRUE) || (ret == errQUEUE_FULL));
  (void)ret;
}

/*-----------------------------------------------------------------------------------*/
// Signals a semaphore (from ISR)
int
sys_sem_signal_isr(sys_sem_t *sem)
{
    BaseType_t woken = pdFALSE;
    xSemaphoreGiveFromISR(*sem, &woken);
    return woken == pdTRUE;
}

/**
 * @brief Wait for a semaphore to be signaled
 *
 * @param sem pointer of the semaphore
 * @param timeout if zero, will wait infinitely, or will wait at least for milliseconds specified by this argument
 * @return SYS_ARCH_TIMEOUT when timeout, 0 otherwise
 */
u32_t
sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  BaseType_t ret;

  if (!timeout) {
    /* wait infinite */
    ret = xSemaphoreTake(*sem, portMAX_DELAY);
    LWIP_ASSERT("taking semaphore failed", ret == pdTRUE);
  } else {
    /* Round up the number of ticks.
     * Not only we need to round up the number of ticks, but we also need to add 1.
     * Indeed, this function shall wait for AT LEAST timeout, but on FreeRTOS,
     * if we specify a timeout of 1 tick to `xSemaphoreTake`, it will take AT MOST
     * 1 tick before triggering a timeout. Thus, we need to pass 2 ticks as a timeout
     * to `xSemaphoreTake`. */
    TickType_t timeout_ticks = ((timeout + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS) + 1;
    ret = xSemaphoreTake(*sem, timeout_ticks);
    if (ret == errQUEUE_EMPTY) {
      /* timed out */
      return SYS_ARCH_TIMEOUT;
    }
    LWIP_ASSERT("taking semaphore failed", ret == pdTRUE);
  }

  return 0;
}

/**
 * @brief Delete a semaphore
 *
 * @param sem pointer of the semaphore to delete
 */
void
sys_sem_free(sys_sem_t *sem)
{
  vSemaphoreDelete(*sem);
  *sem = NULL;
}

/**
 * @brief Create an empty mailbox.
 *
 * @param mbox pointer of the mailbox
 * @param size size of the mailbox
 * @return ERR_OK on success, ERR_MEM when out of memory
 */
err_t
sys_mbox_new(sys_mbox_t *mbox, int size)
{
  *mbox = mem_malloc(sizeof(struct sys_mbox_s));
  if (*mbox == NULL){
    LWIP_DEBUGF(ESP_THREAD_SAFE_DEBUG, ("fail to new *mbox\n"));
    return ERR_MEM;
  }

  (*mbox)->os_mbox = xQueueCreate(size, sizeof(void *));

  if ((*mbox)->os_mbox == NULL) {
    LWIP_DEBUGF(ESP_THREAD_SAFE_DEBUG, ("fail to new (*mbox)->os_mbox\n"));
    free(*mbox);
    return ERR_MEM;
  }

#if ESP_THREAD_SAFE
  (*mbox)->owner = NULL;
#endif

  LWIP_DEBUGF(ESP_THREAD_SAFE_DEBUG, ("new *mbox ok mbox=%p os_mbox=%p\n", *mbox, (*mbox)->os_mbox));
  return ERR_OK;
}

/**
 * @brief Send message to mailbox
 *
 * @param mbox pointer of the mailbox
 * @param msg pointer of the message to send
 */
void
sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
  BaseType_t ret = xQueueSendToBack((*mbox)->os_mbox, &msg, portMAX_DELAY);
  LWIP_ASSERT("mbox post failed", ret == pdTRUE);
  (void)ret;
}

/**
 * @brief Try to post a message to mailbox
 *
 * @param mbox pointer of the mailbox
 * @param msg pointer of the message to send
 * @return ERR_OK on success, ERR_MEM when mailbox is full
 */
err_t
sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  err_t xReturn;

  if (xQueueSend((*mbox)->os_mbox, &msg, 0) == pdTRUE) {
    xReturn = ERR_OK;
  } else {
    LWIP_DEBUGF(ESP_THREAD_SAFE_DEBUG, ("trypost mbox=%p fail\n", (*mbox)->os_mbox));
    xReturn = ERR_MEM;
  }

  return xReturn;
}

/**
 * @brief Try to post a message to mailbox from ISR
 *
 * @param mbox pointer of the mailbox
 * @param msg pointer of the message to send
 * @return  ERR_OK on success
 *          ERR_MEM when mailbox is full
 *          ERR_NEED_SCHED when high priority task wakes up
 */
err_t
sys_mbox_trypost_fromisr(sys_mbox_t *mbox, void *msg)
{
  BaseType_t ret;
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  ret = xQueueSendFromISR((*mbox)->os_mbox, &msg, &xHigherPriorityTaskWoken);
  if (ret == pdTRUE) {
    if (xHigherPriorityTaskWoken == pdTRUE) {
      return ERR_NEED_SCHED;
    }
    return ERR_OK;
  } else {
    LWIP_ASSERT("mbox trypost failed", ret == errQUEUE_FULL);
    return ERR_MEM;
  }
}

/**
 * @brief Fetch message from mailbox
 *
 * @param mbox pointer of mailbox
 * @param msg pointer of the received message, could be NULL to indicate the message should be dropped
 * @param timeout if zero, will wait infinitely; or will wait milliseconds specify by this argument
 * @return SYS_ARCH_TIMEOUT when timeout, 0 otherwise
 */
u32_t
sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
  BaseType_t ret;
  void *msg_dummy;

  if (msg == NULL) {
    msg = &msg_dummy;
  }

  if (timeout == 0) {
    /* wait infinite */
    ret = xQueueReceive((*mbox)->os_mbox, &(*msg), portMAX_DELAY);
    LWIP_ASSERT("mbox fetch failed", ret == pdTRUE);
  } else {
    TickType_t timeout_ticks = timeout / portTICK_PERIOD_MS;
    ret = xQueueReceive((*mbox)->os_mbox, &(*msg), timeout_ticks);
    if (ret == errQUEUE_EMPTY) {
      /* timed out */
      *msg = NULL;
      return SYS_ARCH_TIMEOUT;
    }
    LWIP_ASSERT("mbox fetch failed", ret == pdTRUE);
  }

  return 0;
}

/**
 * @brief try to fetch message from mailbox
 *
 * @param mbox pointer of mailbox
 * @param msg pointer of the received message
 * @return SYS_MBOX_EMPTY if mailbox is empty, 1 otherwise
 */
u32_t
sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
  BaseType_t ret;
  void *msg_dummy;

  if (msg == NULL) {
    msg = &msg_dummy;
  }
  ret = xQueueReceive((*mbox)->os_mbox, &(*msg), 0);
  if (ret == errQUEUE_EMPTY) {
    *msg = NULL;
    return SYS_MBOX_EMPTY;
  }
  LWIP_ASSERT("mbox fetch failed", ret == pdTRUE);

  return 0;
}

void
sys_mbox_set_owner(sys_mbox_t *mbox, void* owner)
{
  if (mbox && *mbox) {
    (*mbox)->owner = owner;
    LWIP_DEBUGF(ESP_THREAD_SAFE_DEBUG, ("set mbox=%p owner=%p", *mbox, owner));
  }
}

/**
 * @brief Delete a mailbox
 *
 * @param mbox pointer of the mailbox to delete
 */
void
sys_mbox_free(sys_mbox_t *mbox)
{
  if ((NULL == mbox) || (NULL == *mbox)) {
    return;
  }
  UBaseType_t msgs_waiting = uxQueueMessagesWaiting((*mbox)->os_mbox);
  LWIP_ASSERT("mbox quence not empty", msgs_waiting == 0);

  vQueueDelete((*mbox)->os_mbox);
  free(*mbox);
  *mbox = NULL;

  (void)msgs_waiting;
}

/**
 * @brief Create a new thread
 *
 * @param name thread name
 * @param thread thread function
 * @param arg thread arguments
 * @param stacksize stacksize of the thread
 * @param prio priority of the thread
 * @return thread ID
 */
sys_thread_t
sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
  TaskHandle_t rtos_task;
  BaseType_t ret;

  /* LwIP's lwip_thread_fn matches FreeRTOS' TaskFunction_t, so we can pass the
     thread function without adaption here. */
  ret = xTaskCreatePinnedToCore(thread, name, stacksize, arg, prio, &rtos_task,
          CONFIG_LWIP_TCPIP_TASK_AFFINITY);

  LWIP_DEBUGF(TCPIP_DEBUG, ("new lwip task : %" U32_F ", prio:%d,stack:%d\n",
             (u32_t)rtos_task, prio, stacksize));

  if (ret != pdTRUE) {
    return NULL;
  }

  return (sys_thread_t)rtos_task;
}

/**
 * @brief Initialize the sys_arch layer
 *
 */
void
sys_init(void)
{
  if (!g_lwip_protect_mutex) {
    if (ERR_OK != sys_mutex_new(&g_lwip_protect_mutex)) {
      ESP_LOGE(TAG, "sys_init: failed to init lwip protect mutex");
    }
  }

  // Create the pthreads key for the per-thread semaphore storage
  pthread_key_create(&sys_thread_sem_key, sys_thread_sem_free);

  esp_vfs_lwip_sockets_register();
}

/**
 * @brief Get system ticks
 *
 * @return system tick counts
 */
u32_t
sys_jiffies(void)
{
  return xTaskGetTickCount();
}

/**
 * @brief Get current time, in miliseconds
 *
 * @return current time
 */
u32_t
sys_now(void)
{
  return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

/**
 * @brief Protect critical region
 *
 * @note This function is only called during very short critical regions.
 *
 * @return previous protection level
 */
sys_prot_t
sys_arch_protect(void)
{
  if (unlikely(!g_lwip_protect_mutex)) {
    sys_mutex_new(&g_lwip_protect_mutex);
  }
  sys_mutex_lock(&g_lwip_protect_mutex);
  return (sys_prot_t) 1;
}

/**
 * @brief Unprotect critical region
 *
 * @param pval protection level
 */
void
sys_arch_unprotect(sys_prot_t pval)
{
  LWIP_UNUSED_ARG(pval);
  sys_mutex_unlock(&g_lwip_protect_mutex);
}

/*
 * get per thread semaphore
 */
sys_sem_t*
sys_thread_sem_get(void)
{
  sys_sem_t *sem = pthread_getspecific(sys_thread_sem_key);

  if (!sem) {
      sem = sys_thread_sem_init();
  }
  LWIP_DEBUGF(ESP_THREAD_SAFE_DEBUG, ("sem_get s=%p\n", sem));
  return sem;
}

static void
sys_thread_sem_free(void* data) // destructor for TLS semaphore
{
  sys_sem_t *sem = (sys_sem_t*)(data);

  if (sem && *sem){
    LWIP_DEBUGF(ESP_THREAD_SAFE_DEBUG, ("sem del, sem=%p\n", *sem));
    vSemaphoreDelete(*sem);
  }

  if (sem) {
    LWIP_DEBUGF(ESP_THREAD_SAFE_DEBUG, ("sem pointer del, sem_p=%p\n", sem));
    free(sem);
  }
}

sys_sem_t*
sys_thread_sem_init(void)
{
  sys_sem_t *sem = (sys_sem_t*)mem_malloc(sizeof(sys_sem_t*));

  if (!sem){
    ESP_LOGE(TAG, "thread_sem_init: out of memory");
    return 0;
  }

  *sem = xSemaphoreCreateBinary();
  if (!(*sem)){
    free(sem);
    ESP_LOGE(TAG, "thread_sem_init: out of memory");
    return 0;
  }

  pthread_setspecific(sys_thread_sem_key, sem);
  return sem;
}

void
sys_thread_sem_deinit(void)
{
  sys_sem_t *sem = pthread_getspecific(sys_thread_sem_key);
  if (sem != NULL) {
    sys_thread_sem_free(sem);
    pthread_setspecific(sys_thread_sem_key, NULL);
  }
}

void
sys_delay_ms(uint32_t ms)
{
  vTaskDelay(ms / portTICK_PERIOD_MS);
}

bool
sys_thread_tcpip(sys_thread_core_lock_t type)
{
    static sys_thread_t lwip_task = NULL;
#if LWIP_TCPIP_CORE_LOCKING
    static sys_thread_t core_lock_holder = NULL;
#endif
    switch (type) {
        default:
            return false;
        case LWIP_CORE_IS_TCPIP_INITIALIZED:
            return lwip_task != NULL;
        case LWIP_CORE_MARK_TCPIP_TASK:
            LWIP_ASSERT("LWIP_CORE_MARK_TCPIP_TASK: lwip_task == NULL", (lwip_task == NULL));
            lwip_task = (sys_thread_t) xTaskGetCurrentTaskHandle();
            return true;
#if LWIP_TCPIP_CORE_LOCKING
        case LWIP_CORE_LOCK_QUERY_HOLDER:
            return lwip_task ? core_lock_holder == (sys_thread_t) xTaskGetCurrentTaskHandle() : true;
        case LWIP_CORE_LOCK_MARK_HOLDER:
            core_lock_holder = (sys_thread_t) xTaskGetCurrentTaskHandle();
            return true;
        case LWIP_CORE_LOCK_UNMARK_HOLDER:
            core_lock_holder = NULL;
            return true;
#else
        case LWIP_CORE_LOCK_QUERY_HOLDER:
            return lwip_task == NULL || lwip_task == (sys_thread_t) xTaskGetCurrentTaskHandle();
#endif /* LWIP_TCPIP_CORE_LOCKING */
    }
    return true;
}
