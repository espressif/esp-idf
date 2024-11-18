/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/lock.h>
#include "esp_err.h"
#include "esp_attr.h"
#include "esp_cpu.h"
#include "sys/queue.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_private/startup_internal.h"
#if CONFIG_SPIRAM
#include "esp_private/freertos_idf_additions_priv.h"
#endif
#include "esp_heap_caps.h"
#include "soc/soc_memory_layout.h"

#include "pthread_internal.h"
#include "esp_pthread.h"
#include "esp_compiler.h"
#include "esp_check.h"

#include "esp_log.h"
const static char *TAG = "pthread";

/** task state */
enum esp_pthread_task_state {
    PTHREAD_TASK_STATE_RUN,
    PTHREAD_TASK_STATE_EXIT
};

/** pthread thread FreeRTOS wrapper */
typedef struct esp_pthread_entry {
    SLIST_ENTRY(esp_pthread_entry)  list_node;  ///< Tasks list node struct.
    TaskHandle_t                handle;         ///< FreeRTOS task handle
    TaskHandle_t                join_task;      ///< Handle of the task waiting to join
    enum esp_pthread_task_state state;          ///< pthread task state
    bool                        detached;       ///< True if pthread is detached
    void                       *retval;         ///< Value supplied to calling thread during join
    void                       *task_arg;       ///< Task arguments
} esp_pthread_t;

/** pthread wrapper task arg */
typedef struct {
    void *(*func)(void *);  ///< user task entry
    void *arg;              ///< user task argument
    esp_pthread_cfg_t cfg;  ///< pthread configuration
} esp_pthread_task_arg_t;

/** pthread mutex FreeRTOS wrapper */
typedef struct {
    SemaphoreHandle_t   sem;        ///< Handle of the task waiting to join
    int                 type;       ///< Mutex type. Currently supported PTHREAD_MUTEX_NORMAL and PTHREAD_MUTEX_RECURSIVE
} esp_pthread_mutex_t;

static _lock_t s_threads_lock;
portMUX_TYPE pthread_lazy_init_lock  = portMUX_INITIALIZER_UNLOCKED; // Used for mutexes and cond vars and rwlocks
static SLIST_HEAD(esp_thread_list_head, esp_pthread_entry) s_threads_list
    = SLIST_HEAD_INITIALIZER(s_threads_list);
static pthread_key_t s_pthread_cfg_key;

static int pthread_mutex_lock_internal(esp_pthread_mutex_t *mux, TickType_t tmo);

static void esp_pthread_cfg_key_destructor(void *value)
{
    free(value);
}

static esp_err_t lazy_init_pthread_cfg_key(void)
{
    if (s_pthread_cfg_key != 0) {
        return ESP_OK;
    }

    if (pthread_key_create(&s_pthread_cfg_key, esp_pthread_cfg_key_destructor) != 0) {
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}

esp_err_t esp_pthread_init(void)
{
    lazy_init_pthread_cfg_key();

    return ESP_OK;
}

static void *pthread_list_find_item(void *(*item_check)(esp_pthread_t *, void *arg), void *check_arg)
{
    esp_pthread_t *it;
    SLIST_FOREACH(it, &s_threads_list, list_node) {
        void *val = item_check(it, check_arg);
        if (val) {
            return val;
        }
    }
    return NULL;
}

static void *pthread_get_handle_by_desc(esp_pthread_t *item, void *desc)
{
    if (item == desc) {
        return item->handle;
    }
    return NULL;
}

static void *pthread_get_desc_by_handle(esp_pthread_t *item, void *hnd)
{
    if (hnd == item->handle) {
        return item;
    }
    return NULL;
}

static inline TaskHandle_t pthread_find_handle(pthread_t thread)
{
    return pthread_list_find_item(pthread_get_handle_by_desc, (void *)thread);
}

static esp_pthread_t *pthread_find(TaskHandle_t task_handle)
{
    return pthread_list_find_item(pthread_get_desc_by_handle, task_handle);
}

static void pthread_delete(esp_pthread_t *pthread)
{
    SLIST_REMOVE(&s_threads_list, pthread, esp_pthread_entry, list_node);
    free(pthread);
}

/* Call this function to configure pthread stacks in Pthreads */
esp_err_t esp_pthread_set_cfg(const esp_pthread_cfg_t *cfg)
{
    if (cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (cfg->stack_size < PTHREAD_STACK_MIN) {
        return ESP_ERR_INVALID_ARG;
    }

    // 0 is treated as default value, hence change caps to MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL in that case
    int heap_caps;
    if (cfg->stack_alloc_caps == 0) {
        heap_caps = MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL;
    } else {
        // Check that memory is 8-bit capable
        if (!(cfg->stack_alloc_caps & MALLOC_CAP_8BIT)) {
            return ESP_ERR_INVALID_ARG;
        }

        heap_caps = cfg->stack_alloc_caps;
    }

    ESP_RETURN_ON_ERROR(lazy_init_pthread_cfg_key(), TAG, "Failed to initialize pthread key");

    /* If a value is already set, update that value */
    esp_pthread_cfg_t *p = pthread_getspecific(s_pthread_cfg_key);
    ESP_COMPILER_DIAGNOSTIC_PUSH_IGNORE("-Wanalyzer-malloc-leak") // ignore leak of 'p'
    if (!p) {
        p = malloc(sizeof(esp_pthread_cfg_t));
        if (!p) {
            return ESP_ERR_NO_MEM;
        }
    }
    *p = *cfg;
    p->stack_alloc_caps = heap_caps;
    pthread_setspecific(s_pthread_cfg_key, p);

    return ESP_OK;
    ESP_COMPILER_DIAGNOSTIC_POP("-Wanalyzer-malloc-leak")
}

esp_err_t esp_pthread_get_cfg(esp_pthread_cfg_t *p)
{
    if (p == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_RETURN_ON_ERROR(lazy_init_pthread_cfg_key(), TAG, "Failed to initialize pthread key");

    esp_pthread_cfg_t *cfg = pthread_getspecific(s_pthread_cfg_key);
    if (cfg) {
        *p = *cfg;
        return ESP_OK;
    }
    memset(p, 0, sizeof(*p));
    return ESP_ERR_NOT_FOUND;
}

static int get_default_pthread_core(void)
{
    return CONFIG_PTHREAD_TASK_CORE_DEFAULT == -1 ? tskNO_AFFINITY : CONFIG_PTHREAD_TASK_CORE_DEFAULT;
}

esp_pthread_cfg_t esp_pthread_get_default_config(void)
{
    esp_pthread_cfg_t cfg = {
        .stack_size = CONFIG_PTHREAD_TASK_STACK_SIZE_DEFAULT,
        .prio = CONFIG_PTHREAD_TASK_PRIO_DEFAULT,
        .inherit_cfg = false,
        .thread_name = NULL,
        .pin_to_core = get_default_pthread_core(),
        .stack_alloc_caps = MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT,
    };

    return cfg;
}

static void pthread_task_func(void *arg)
{
    void *rval = NULL;
    esp_pthread_task_arg_t *task_arg = (esp_pthread_task_arg_t *)arg;

    ESP_LOGV(TAG, "%s ENTER %p", __FUNCTION__, task_arg->func);

    // wait for start
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

    if (task_arg->cfg.inherit_cfg) {
        /* If inherit option is set, then do a set_cfg() ourselves for future forks,
        but first set thread_name to NULL to enable inheritance of the name too.
        (This also to prevents dangling pointers to name of tasks that might
        possibly have been deleted when we use the configuration).*/
        esp_pthread_cfg_t *cfg = &task_arg->cfg;
        cfg->thread_name = NULL;
        esp_pthread_set_cfg(cfg);
    }
    ESP_LOGV(TAG, "%s START %p", __FUNCTION__, task_arg->func);
    rval = task_arg->func(task_arg->arg);
    ESP_LOGV(TAG, "%s END %p", __FUNCTION__, task_arg->func);

    pthread_exit(rval);

    ESP_LOGV(TAG, "%s EXIT", __FUNCTION__);
}

#if CONFIG_SPIRAM && CONFIG_FREERTOS_SMP
static UBaseType_t coreID_to_AffinityMask(BaseType_t core_id)
{
    UBaseType_t affinity_mask = tskNO_AFFINITY;
    if (core_id != tskNO_AFFINITY) {
        affinity_mask = 1 << core_id;
    }
    return affinity_mask;
}
#endif

static BaseType_t pthread_create_freertos_task_with_caps(TaskFunction_t pxTaskCode,
                                                         const char * const pcName,
                                                         const configSTACK_DEPTH_TYPE usStackDepth,
                                                         void * const pvParameters,
                                                         UBaseType_t uxPriority,
                                                         BaseType_t core_id,
                                                         UBaseType_t uxStackMemoryCaps,
                                                         TaskHandle_t * const pxCreatedTask)
{
#if CONFIG_SPIRAM
#if CONFIG_FREERTOS_SMP
    return prvTaskCreateDynamicAffinitySetWithCaps(pxTaskCode,
                                                   pcName,
                                                   usStackDepth,
                                                   pvParameters,
                                                   uxPriority,
                                                   coreID_to_AffinityMask(core_id),
                                                   uxStackMemoryCaps,
                                                   pxCreatedTask);
#else
    return prvTaskCreateDynamicPinnedToCoreWithCaps(pxTaskCode,
                                                    pcName,
                                                    usStackDepth,
                                                    pvParameters,
                                                    uxPriority,
                                                    core_id,
                                                    uxStackMemoryCaps,
                                                    pxCreatedTask);
#endif
#else
    return xTaskCreatePinnedToCore(pxTaskCode,
                                   pcName,
                                   usStackDepth,
                                   pvParameters,
                                   uxPriority,
                                   pxCreatedTask,
                                   core_id);
#endif
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg)
{
    TaskHandle_t xHandle = NULL;

    ESP_LOGV(TAG, "%s", __FUNCTION__);

    if (lazy_init_pthread_cfg_key() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to allocate pthread cfg key!");
        return ENOMEM;
    }

    esp_pthread_task_arg_t *task_arg = calloc(1, sizeof(esp_pthread_task_arg_t));
    if (task_arg == NULL) {
        ESP_LOGE(TAG, "Failed to allocate task args!");
        return ENOMEM;
    }

    esp_pthread_t *pthread = calloc(1, sizeof(esp_pthread_t));
    if (pthread == NULL) {
        ESP_LOGE(TAG, "Failed to allocate pthread data!");
        free(task_arg);
        return ENOMEM;
    }

    uint32_t stack_size = CONFIG_PTHREAD_TASK_STACK_SIZE_DEFAULT;
    BaseType_t prio = CONFIG_PTHREAD_TASK_PRIO_DEFAULT;
    BaseType_t core_id = get_default_pthread_core();
    const char *task_name = CONFIG_PTHREAD_TASK_NAME_DEFAULT;
    uint32_t stack_alloc_caps = MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT;

    esp_pthread_cfg_t *pthread_cfg = pthread_getspecific(s_pthread_cfg_key);
    if (pthread_cfg) {
        if (pthread_cfg->stack_size) {
            stack_size = pthread_cfg->stack_size;
        }
        if (pthread_cfg->prio && pthread_cfg->prio < configMAX_PRIORITIES) {
            prio = pthread_cfg->prio;
        }

        if (pthread_cfg->inherit_cfg) {
            if (pthread_cfg->thread_name == NULL) {
                // Inherit task name from current task.
                task_name = pcTaskGetName(NULL);
            } else {
                // Inheriting, but new task name.
                task_name = pthread_cfg->thread_name;
            }
        } else if (pthread_cfg->thread_name == NULL) {
            task_name = CONFIG_PTHREAD_TASK_NAME_DEFAULT;
        } else {
            task_name = pthread_cfg->thread_name;
        }

        if (pthread_cfg->pin_to_core >= 0 && pthread_cfg->pin_to_core < CONFIG_FREERTOS_NUMBER_OF_CORES) {
            core_id = pthread_cfg->pin_to_core;
        }

        // Note: validity has been checked during esp_pthread_set_cfg()
        stack_alloc_caps = pthread_cfg->stack_alloc_caps;

        task_arg->cfg = *pthread_cfg;
    }

    if (attr) {
        /* Overwrite attributes */
        stack_size = attr->stacksize;

        switch (attr->detachstate) {
        case PTHREAD_CREATE_DETACHED:
            pthread->detached = true;
            break;
        case PTHREAD_CREATE_JOINABLE:
        default:
            pthread->detached = false;
        }
    }

    // stack_size is in bytes. This transformation ensures that the units are
    // transformed to the units used in FreeRTOS.
    // Note: float division of ceil(m / n) ==
    //       integer division of (m + n - 1) / n
    stack_size = (stack_size + sizeof(StackType_t) - 1) / sizeof(StackType_t);
    task_arg->func = start_routine;
    task_arg->arg = arg;
    pthread->task_arg = task_arg;

    BaseType_t res = pthread_create_freertos_task_with_caps(&pthread_task_func,
                                                            task_name,
                                                            stack_size,
                                                            task_arg,
                                                            prio,
                                                            core_id,
                                                            stack_alloc_caps,
                                                            &xHandle);

    if (res != pdPASS) {
        ESP_LOGE(TAG, "Failed to create task!");
        free(pthread);
        free(task_arg);
        if (res == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
            return ENOMEM;
        } else {
            return EAGAIN;
        }
    }
    pthread->handle = xHandle;

    _lock_acquire(&s_threads_lock);

    SLIST_INSERT_HEAD(&s_threads_list, pthread, list_node);
    _lock_release(&s_threads_lock);

    // start task
    xTaskNotify(xHandle, 0, eNoAction);

    *thread = (pthread_t)pthread; // pointer value fit into pthread_t (uint32_t)

    ESP_LOGV(TAG, "Created task %"PRIx32, (uint32_t)xHandle);

    return 0;
}

int pthread_join(pthread_t thread, void **retval)
{
    esp_pthread_t *pthread = (esp_pthread_t *)thread;
    int ret = 0;
    bool wait = false;
    void *child_task_retval = 0;

    ESP_LOGV(TAG, "%s %p", __FUNCTION__, pthread);

    // find task
    _lock_acquire(&s_threads_lock);

    TaskHandle_t handle = pthread_find_handle(thread);
    if (!handle) {
        // not found
        ret = ESRCH;
    } else if (pthread->detached) {
        // Thread is detached
        ret = EDEADLK;
    } else if (pthread->join_task) {
        // already have waiting task to join
        ret = EINVAL;
    } else if (handle == xTaskGetCurrentTaskHandle()) {
        // join to self not allowed
        ret = EDEADLK;
    } else {
        esp_pthread_t *cur_pthread = pthread_find(xTaskGetCurrentTaskHandle());
        if (cur_pthread && cur_pthread->join_task == handle) {
            // join to each other not allowed
            ret = EDEADLK;
        } else {
            if (pthread->state == PTHREAD_TASK_STATE_RUN) {
                pthread->join_task = xTaskGetCurrentTaskHandle();
                wait = true;
            } else { // thread has exited and task is already suspended, or about to be suspended
                child_task_retval = pthread->retval;
                pthread_delete(pthread);
            }
        }
    }
    _lock_release(&s_threads_lock);

    if (ret == 0) {
        if (wait) {
            xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
            _lock_acquire(&s_threads_lock);
            child_task_retval = pthread->retval;
            pthread_delete(pthread);
            _lock_release(&s_threads_lock);
        }
        /* clean up thread local storage before task deletion */
        pthread_internal_local_storage_destructor_callback(handle);
        vTaskDelete(handle);
    }

    if (retval) {
        *retval = child_task_retval;
    }

    ESP_LOGV(TAG, "%s %p EXIT %d", __FUNCTION__, pthread, ret);
    return ret;
}

int pthread_detach(pthread_t thread)
{
    esp_pthread_t *pthread = (esp_pthread_t *)thread;
    int ret = 0;

    _lock_acquire(&s_threads_lock);

    TaskHandle_t handle = pthread_find_handle(thread);
    if (!handle) {
        ret = ESRCH;
    } else if (pthread->detached) {
        // already detached
        ret = EINVAL;
    } else if (pthread->join_task) {
        // already have waiting task to join
        ret = EINVAL;
    } else if (pthread->state == PTHREAD_TASK_STATE_RUN) {
        // pthread still running
        pthread->detached = true;
    } else {
        // pthread already stopped
        pthread_delete(pthread);
        /* clean up thread local storage before task deletion */
        pthread_internal_local_storage_destructor_callback(handle);
        vTaskDelete(handle);
    }
    _lock_release(&s_threads_lock);
    ESP_LOGV(TAG, "%s %p EXIT %d", __FUNCTION__, pthread, ret);
    return ret;
}

void pthread_exit(void *value_ptr)
{
    bool detached = false;
    /* clean up thread local storage before task deletion */
    pthread_internal_local_storage_destructor_callback(NULL);

    _lock_acquire(&s_threads_lock);

    esp_pthread_t *pthread = pthread_find(xTaskGetCurrentTaskHandle());
    if (!pthread) {
        assert(false && "Failed to find pthread for current task!");
    }
    if (pthread->task_arg) {
        free(pthread->task_arg);
    }
    if (pthread->detached) {
        // auto-free for detached threads
        pthread_delete(pthread);
        detached = true;
    } else {
        // Set return value
        pthread->retval = value_ptr;
        // Remove from list, it indicates that task has exited
        if (pthread->join_task) {
            // notify join
            xTaskNotify(pthread->join_task, 0, eNoAction);
        } else {
            pthread->state = PTHREAD_TASK_STATE_EXIT;
        }
    }

    ESP_LOGD(TAG, "Task stk_wm = %d", (int)uxTaskGetStackHighWaterMark(NULL));

    _lock_release(&s_threads_lock);
    // note: if this thread is joinable then after giving back s_threads_mux
    // this task could be deleted at any time, so don't take another lock or
    // do anything that might lock (such as printing to stdout)

    if (detached) {
        vTaskDelete(NULL);
    } else {
        vTaskSuspend(NULL);
    }

    // Should never be reached
    abort();
}

int pthread_cancel(pthread_t thread)
{
    ESP_LOGE(TAG, "%s: not supported!", __FUNCTION__);
    return ENOSYS;
}

int sched_yield(void)
{
    vTaskDelay(0);
    return 0;
}

pthread_t pthread_self(void)
{
    _lock_acquire(&s_threads_lock);

    esp_pthread_t *pthread = pthread_find(xTaskGetCurrentTaskHandle());
    if (!pthread) {
        assert(false && "Failed to find current thread ID!");
    }
    _lock_release(&s_threads_lock);
    return (pthread_t)pthread;
}

int pthread_equal(pthread_t t1, pthread_t t2)
{
    return t1 == t2 ? 1 : 0;
}

/***************** ONCE ******************/
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void))
{
    if (once_control == NULL || init_routine == NULL || !once_control->is_initialized) {
        ESP_LOGE(TAG, "%s: Invalid args!", __FUNCTION__);
        return EINVAL;
    }

    // Check if compare and set was successful
    if (esp_cpu_compare_and_set((volatile uint32_t *)&once_control->init_executed, 0, 1)) {
        ESP_LOGV(TAG, "%s: call init_routine %p", __FUNCTION__, once_control);
        init_routine();
    }

    return 0;
}

/***************** MUTEX ******************/
static int mutexattr_check(const pthread_mutexattr_t *attr)
{
    if (attr->type != PTHREAD_MUTEX_NORMAL &&
            attr->type != PTHREAD_MUTEX_RECURSIVE &&
            attr->type != PTHREAD_MUTEX_ERRORCHECK) {
        return EINVAL;
    }
    return 0;
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
    int type = PTHREAD_MUTEX_NORMAL;

    if (!mutex) {
        return EINVAL;
    }

    if (attr) {
        if (!attr->is_initialized) {
            return EINVAL;
        }
        int res = mutexattr_check(attr);
        if (res) {
            return res;
        }
        type = attr->type;
    }

    esp_pthread_mutex_t *mux = (esp_pthread_mutex_t *)malloc(sizeof(esp_pthread_mutex_t));
    if (!mux) {
        return ENOMEM;
    }
    mux->type = type;

    if (mux->type == PTHREAD_MUTEX_RECURSIVE) {
        mux->sem = xSemaphoreCreateRecursiveMutex();
    } else {
        mux->sem = xSemaphoreCreateMutex();
    }
    if (!mux->sem) {
        free(mux);
        return EAGAIN;
    }

    *mutex = (pthread_mutex_t)mux; // pointer value fit into pthread_mutex_t (uint32_t)

    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    esp_pthread_mutex_t *mux;

    ESP_LOGV(TAG, "%s %p", __FUNCTION__, mutex);

    if (!mutex) {
        return EINVAL;
    }
    if ((intptr_t) *mutex == PTHREAD_MUTEX_INITIALIZER) {
        return 0; // Static mutex was never initialized
    }

    mux = (esp_pthread_mutex_t *)*mutex;
    if (!mux) {
        return EINVAL;
    }

    // check if mux is busy
    int res = pthread_mutex_lock_internal(mux, 0);
    if (res == EBUSY) {
        return EBUSY;
    }

    if (mux->type == PTHREAD_MUTEX_RECURSIVE) {
        res = xSemaphoreGiveRecursive(mux->sem);
    } else {
        res = xSemaphoreGive(mux->sem);
    }
    if (res != pdTRUE) {
        assert(false && "Failed to release mutex!");
    }
    vSemaphoreDelete(mux->sem);
    free(mux);

    return 0;
}

static int pthread_mutex_lock_internal(esp_pthread_mutex_t *mux, TickType_t tmo)
{
    if (!mux) {
        return EINVAL;
    }

    if ((mux->type == PTHREAD_MUTEX_ERRORCHECK) &&
            (xSemaphoreGetMutexHolder(mux->sem) == xTaskGetCurrentTaskHandle())) {
        return EDEADLK;
    }

    if (mux->type == PTHREAD_MUTEX_RECURSIVE) {
        if (xSemaphoreTakeRecursive(mux->sem, tmo) != pdTRUE) {
            return EBUSY;
        }
    } else {
        if (xSemaphoreTake(mux->sem, tmo) != pdTRUE) {
            return EBUSY;
        }
    }

    return 0;
}

static int pthread_mutex_init_if_static(pthread_mutex_t *mutex)
{
    int res = 0;
    if ((intptr_t) *mutex == PTHREAD_MUTEX_INITIALIZER) {
        portENTER_CRITICAL(&pthread_lazy_init_lock);
        if ((intptr_t) *mutex == PTHREAD_MUTEX_INITIALIZER) {
            res = pthread_mutex_init(mutex, NULL);
        }
        portEXIT_CRITICAL(&pthread_lazy_init_lock);
    }
    return res;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    if (!mutex) {
        return EINVAL;
    }
    int res = pthread_mutex_init_if_static(mutex);
    if (res != 0) {
        return res;
    }
    return pthread_mutex_lock_internal((esp_pthread_mutex_t *)*mutex, portMAX_DELAY);
}

int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *timeout)
{
    if (!mutex) {
        return EINVAL;
    }
    int res = pthread_mutex_init_if_static(mutex);
    if (res != 0) {
        return res;
    }

    struct timespec currtime;
    clock_gettime(CLOCK_REALTIME, &currtime);
    TickType_t tmo = ((timeout->tv_sec - currtime.tv_sec) * 1000 +
                      (timeout->tv_nsec - currtime.tv_nsec) / 1000000) / portTICK_PERIOD_MS;

    res = pthread_mutex_lock_internal((esp_pthread_mutex_t *)*mutex, tmo);
    if (res == EBUSY) {
        return ETIMEDOUT;
    }
    return res;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    if (!mutex) {
        return EINVAL;
    }
    int res = pthread_mutex_init_if_static(mutex);
    if (res != 0) {
        return res;
    }
    return pthread_mutex_lock_internal((esp_pthread_mutex_t *)*mutex, 0);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    esp_pthread_mutex_t *mux;

    if (!mutex) {
        return EINVAL;
    }
    mux = (esp_pthread_mutex_t *)*mutex;
    if (!mux) {
        return EINVAL;
    }

    if (((mux->type == PTHREAD_MUTEX_RECURSIVE) ||
            (mux->type == PTHREAD_MUTEX_ERRORCHECK)) &&
            (xSemaphoreGetMutexHolder(mux->sem) != xTaskGetCurrentTaskHandle())) {
        return EPERM;
    }

    int ret;
    if (mux->type == PTHREAD_MUTEX_RECURSIVE) {
        ret = xSemaphoreGiveRecursive(mux->sem);
    } else {
        ret = xSemaphoreGive(mux->sem);
    }
    if (ret != pdTRUE) {
        assert(false && "Failed to unlock mutex!");
    }
    return 0;
}

int pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
    if (!attr) {
        return EINVAL;
    }
    memset(attr, 0, sizeof(*attr));
    attr->type = PTHREAD_MUTEX_NORMAL;
    attr->is_initialized = 1;
    return 0;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr)
{
    if (!attr) {
        return EINVAL;
    }
    attr->is_initialized = 0;
    return 0;
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type)
{
    if (!attr) {
        return EINVAL;
    }
    *type = attr->type;
    return 0;
}

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type)
{
    if (!attr) {
        return EINVAL;
    }
    pthread_mutexattr_t tmp_attr = {.type = type};
    int res = mutexattr_check(&tmp_attr);
    if (!res) {
        attr->type = type;
    }
    return res;
}

/***************** ATTRIBUTES ******************/
int pthread_attr_init(pthread_attr_t *attr)
{
    if (attr) {
        /* Nothing to allocate. Set everything to default */
        memset(attr, 0, sizeof(*attr));
        attr->stacksize   = CONFIG_PTHREAD_TASK_STACK_SIZE_DEFAULT;
        attr->detachstate = PTHREAD_CREATE_JOINABLE;
        return 0;
    }
    return EINVAL;
}

int pthread_attr_destroy(pthread_attr_t *attr)
{
    /* Nothing to deallocate. Reset everything to default */
    return pthread_attr_init(attr);
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize)
{
    if (attr) {
        *stacksize = attr->stacksize;
        return 0;
    }
    return EINVAL;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
    if (attr && !(stacksize < PTHREAD_STACK_MIN)) {
        attr->stacksize = stacksize;
        return 0;
    }
    return EINVAL;
}

int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate)
{
    if (attr) {
        *detachstate = attr->detachstate;
        return 0;
    }
    return EINVAL;
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate)
{
    if (attr) {
        switch (detachstate) {
        case PTHREAD_CREATE_DETACHED:
            attr->detachstate = PTHREAD_CREATE_DETACHED;
            break;
        case PTHREAD_CREATE_JOINABLE:
            attr->detachstate = PTHREAD_CREATE_JOINABLE;
            break;
        default:
            return EINVAL;
        }
        return 0;
    }
    return EINVAL;
}

int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param)
{
    int ret;
    if (!policy || !param) {
        return EINVAL;
    }

    _lock_acquire(&s_threads_lock);
    TaskHandle_t handle = pthread_find_handle(thread);
    if (!handle) {
        ret = ESRCH;
    } else {
        *policy = SCHED_OTHER;
        param->sched_priority = uxTaskPriorityGet(handle);
        ret = 0;
    }
    _lock_release(&s_threads_lock);

    return ret;
}

static int set_prio(pthread_t thread, int policy, int prio)
{
    int ret;
    if (prio < sched_get_priority_min(policy) || sched_get_priority_max(policy) < prio) {
        return EINVAL;
    }

    _lock_acquire(&s_threads_lock);
    TaskHandle_t handle = pthread_find_handle(thread);
    if (!handle) {
        ret = ESRCH;
    } else {
        vTaskPrioritySet(handle, prio);
        ret = 0;
    }
    _lock_release(&s_threads_lock);

    return ret;
}

int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param)
{
    // the policy does not change anything for the FreeRTOS kernel, ignore it.
    int ret;
    if (!param) {
        return EINVAL;
    }

    ret = set_prio(thread, policy, param->sched_priority);

    return ret;
}

int pthread_setschedprio(pthread_t thread, int prio)
{
    // the policy does not change anything for the FreeRTOS kernel, ignore it.
    int policy = SCHED_OTHER;
    return set_prio(thread, policy, prio);
}

int sched_get_priority_min(int policy)
{
    // the policy does not change anything for the FreeRTOS kernel, ignore it.
    (void) policy;
    return tskIDLE_PRIORITY;
}

int sched_get_priority_max(int policy)
{
    // the policy does not change anything for the FreeRTOS kernel, ignore it.
    (void) policy;
    return configMAX_PRIORITIES - 1;
}

/* Hook function to force linking this file */
void pthread_include_pthread_impl(void)
{
}
