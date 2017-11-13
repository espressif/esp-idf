// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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
//
// This module implements pthread API on top of FreeRTOS. API is implemented to the level allowing
// libstdcxx threading framework to operate correctly. So not all original pthread routines are supported.
// Moreover some implemened functions do not provide full functionality, e.g. pthread_create does not support
// thread's attributes customization (prio, stack size and so on). So if you are not satisfied with default
// behavior use native FreeRTOS API.
//
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include "esp_err.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/list.h"

#include "pthread_internal.h"

#define LOG_LOCAL_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include "esp_log.h"
const static char *TAG = "esp_pthread";

/** task state */
enum esp_pthread_task_state {
    PTHREAD_TASK_STATE_RUN,
    PTHREAD_TASK_STATE_EXIT
};

/** pthread thread FreeRTOS wrapper */
typedef struct {
    ListItem_t                  list_item;  ///< Tasks list node struct. FreeRTOS task handle is kept as list_item.xItemValue
    TaskHandle_t                join_task;  ///< Handle of the task waiting to join
    enum esp_pthread_task_state state;      ///< pthread task state
    bool                        detached;   ///< True if pthread is detached
} esp_pthread_t;

/** pthread wrapper task arg */
typedef struct {
    void *(*func)(void *);  ///< user task entry
    void *arg;              ///< user task argument
} esp_pthread_task_arg_t;

/** pthread mutex FreeRTOS wrapper */
typedef struct {
    ListItem_t          list_item;  ///< mutexes list node struct
    SemaphoreHandle_t   sem;        ///< Handle of the task waiting to join
    int                 type;       ///< Mutex type. Currently supported PTHREAD_MUTEX_NORMAL and PTHREAD_MUTEX_RECURSIVE
} esp_pthread_mutex_t;


static SemaphoreHandle_t s_once_mux = NULL;
static SemaphoreHandle_t s_threads_mux = NULL;
static portMUX_TYPE s_mutex_init_lock = portMUX_INITIALIZER_UNLOCKED;

static List_t s_threads_list;


static int IRAM_ATTR pthread_mutex_lock_internal(esp_pthread_mutex_t *mux, TickType_t tmo);

esp_err_t esp_pthread_init(void)
{
    vListInitialise((List_t *)&s_threads_list);
    s_once_mux = xSemaphoreCreateRecursiveMutex();
    if (s_once_mux == NULL) {
        return ESP_ERR_NO_MEM;
    }
    s_threads_mux = xSemaphoreCreateMutex();
    if (s_threads_mux == NULL) {
        vSemaphoreDelete(s_once_mux);
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

static void *pthread_find_list_item(void *(*item_check)(ListItem_t *, void *arg), void *check_arg)
{
    ListItem_t const *list_end = listGET_END_MARKER(&s_threads_list);
    ListItem_t *list_item = listGET_HEAD_ENTRY(&s_threads_list);
    while (list_item != list_end) {
        void *val = item_check(list_item, check_arg);
        if (val) {
            return val;
        }
        list_item = listGET_NEXT(list_item);
    }
    return NULL;
}

static void *pthread_get_handle_by_desc(ListItem_t *item, void *arg)
{
    esp_pthread_t *pthread = listGET_LIST_ITEM_OWNER(item);
    if (pthread == arg) {
        return (void *)listGET_LIST_ITEM_VALUE(item);
    }
    return NULL;
}
static inline TaskHandle_t pthread_find_handle(pthread_t thread)
{
    return pthread_find_list_item(pthread_get_handle_by_desc, (void *)thread);
}

static void *pthread_get_desc_by_handle(ListItem_t *item, void *arg)
{
    TaskHandle_t task_handle = arg;
    TaskHandle_t cur_handle = (TaskHandle_t)listGET_LIST_ITEM_VALUE(item);
    if (task_handle == cur_handle) {
        return (esp_pthread_t *)listGET_LIST_ITEM_OWNER(item);
    }
    return NULL;
}
static esp_pthread_t *pthread_find(TaskHandle_t task_handle)
{
    return pthread_find_list_item(pthread_get_desc_by_handle, task_handle);
}

static void pthread_delete(esp_pthread_t *pthread)
{
    uxListRemove(&pthread->list_item);
    free(pthread);
}

static void pthread_task_func(void *arg)
{
    esp_pthread_task_arg_t *task_arg = (esp_pthread_task_arg_t *)arg;

    ESP_LOGV(TAG, "%s ENTER %p", __FUNCTION__, task_arg->func);

    // wait for start
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

    ESP_LOGV(TAG, "%s START %p", __FUNCTION__, task_arg->func);
    task_arg->func(task_arg->arg);
    ESP_LOGV(TAG, "%s END %p", __FUNCTION__, task_arg->func);
    free(task_arg);

    /* preemptively clean up thread local storage, rather than
       waiting for the idle task to clean up the thread */
    pthread_internal_local_storage_destructor_callback();

    if (xSemaphoreTake(s_threads_mux, portMAX_DELAY) != pdTRUE) {
        assert(false && "Failed to lock threads list!");
    }
    esp_pthread_t *pthread = pthread_find(xTaskGetCurrentTaskHandle());
    if (!pthread) {
        assert(false && "Failed to find pthread for current task!");
    }
    if (pthread->detached) {
        // auto-free for detached threads
        pthread_delete(pthread);
    } else {
        // Remove from list, it indicates that task has exited
        if (pthread->join_task) {
            // notify join
            xTaskNotify(pthread->join_task, 0, eNoAction);
        } else {
            pthread->state = PTHREAD_TASK_STATE_EXIT;
        }
    }
    xSemaphoreGive(s_threads_mux);

    vTaskDelete(NULL);

    ESP_LOGV(TAG, "%s EXIT", __FUNCTION__);
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine) (void *), void *arg)
{
    TaskHandle_t xHandle = NULL;

    ESP_LOGV(TAG, "%s", __FUNCTION__);
    if (attr) {
        ESP_LOGE(TAG, "%s: attrs not supported!", __FUNCTION__);
        return ENOSYS;
    }
    esp_pthread_task_arg_t *task_arg = malloc(sizeof(esp_pthread_task_arg_t));
    if (task_arg == NULL) {
        ESP_LOGE(TAG, "Failed to allocate task args!");
        return ENOMEM;
    }
    memset(task_arg, 0, sizeof(esp_pthread_task_arg_t));
    esp_pthread_t *pthread = malloc(sizeof(esp_pthread_t));
    if (pthread == NULL) {
        ESP_LOGE(TAG, "Failed to allocate pthread data!");
        free(task_arg);
        return ENOMEM;
    }
    memset(pthread, 0, sizeof(esp_pthread_t));
    task_arg->func = start_routine;
    task_arg->arg = arg;
    BaseType_t res = xTaskCreate(&pthread_task_func, "pthread", CONFIG_ESP32_PTHREAD_TASK_STACK_SIZE_DEFAULT,
        task_arg, CONFIG_ESP32_PTHREAD_TASK_PRIO_DEFAULT, &xHandle);
    if(res != pdPASS) {
        ESP_LOGE(TAG, "Failed to create task!");
        free(pthread);
        free(task_arg);
        if (res == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
            return ENOMEM;
        } else {
            return EAGAIN;
        }
    }
    vListInitialiseItem((ListItem_t *)&pthread->list_item);
    listSET_LIST_ITEM_OWNER((ListItem_t *)&pthread->list_item, pthread);
    listSET_LIST_ITEM_VALUE((ListItem_t *)&pthread->list_item, (TickType_t)xHandle);

    if (xSemaphoreTake(s_threads_mux, portMAX_DELAY) != pdTRUE) {
        assert(false && "Failed to lock threads list!");
    }
    vListInsertEnd((List_t *)&s_threads_list, (ListItem_t *)&pthread->list_item);
    xSemaphoreGive(s_threads_mux);

    // start task
    xTaskNotify(xHandle, 0, eNoAction);

    *thread = (pthread_t)pthread; // pointer value fit into pthread_t (uint32_t)

    ESP_LOGV(TAG, "Created task %x", (uint32_t)xHandle);

    return 0;
}

int pthread_join(pthread_t thread, void **retval)
{
    esp_pthread_t *pthread = (esp_pthread_t *)thread;
    int ret = 0;
    bool wait = false;

    ESP_LOGV(TAG, "%s %p", __FUNCTION__, pthread);

    // find task
    if (xSemaphoreTake(s_threads_mux, portMAX_DELAY) != pdTRUE) {
        assert(false && "Failed to lock threads list!");
    }
    TaskHandle_t handle = pthread_find_handle(thread);
    if (!handle) {
        // not found
        ret = ESRCH;
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
            } else {
                pthread_delete(pthread);
            }
        }
    }
    xSemaphoreGive(s_threads_mux);

    if (ret == 0 && wait) {
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
        if (xSemaphoreTake(s_threads_mux, portMAX_DELAY) != pdTRUE) {
            assert(false && "Failed to lock threads list!");
        }
        pthread_delete(pthread);
        xSemaphoreGive(s_threads_mux);
    }

    if (retval) {
        *retval = 0; // no exit code in FreeRTOS
    }

    ESP_LOGV(TAG, "%s %p EXIT %d", __FUNCTION__, pthread, ret);
    return ret;
}

int pthread_detach(pthread_t thread)
{
    esp_pthread_t *pthread = (esp_pthread_t *)thread;
    int ret = 0;

    if (xSemaphoreTake(s_threads_mux, portMAX_DELAY) != pdTRUE) {
        assert(false && "Failed to lock threads list!");
    }
    TaskHandle_t handle = pthread_find_handle(thread);
    if (!handle) {
        ret = ESRCH;
    } else {
        pthread->detached = true;
    }
    xSemaphoreGive(s_threads_mux);
    ESP_LOGV(TAG, "%s %p EXIT %d", __FUNCTION__, pthread, ret);
    return ret;
}

int pthread_cancel(pthread_t thread)
{
    ESP_LOGE(TAG, "%s: not supported!", __FUNCTION__);
    return ENOSYS;
}

int sched_yield( void )
{
    vTaskDelay(0);
    return 0;
}

pthread_t pthread_self(void)
{
    if (xSemaphoreTake(s_threads_mux, portMAX_DELAY) != pdTRUE) {
        assert(false && "Failed to lock threads list!");
    }
    esp_pthread_t *pthread = pthread_find(xTaskGetCurrentTaskHandle());
    if (!pthread) {
        assert(false && "Failed to find current thread ID!");
    }
    xSemaphoreGive(s_threads_mux);
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

    TaskHandle_t cur_task = xTaskGetCurrentTaskHandle();
    uint8_t do_execute = 0;
    // do not take mutex if OS is not running yet
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED ||
            // init_routine can call pthread_once for another objects, so use recursive mutex
            // FIXME: behaviour is undefined if init_routine calls pthread_once for the same object in the current context
            !cur_task || xSemaphoreTakeRecursive(s_once_mux, portMAX_DELAY) == pdTRUE)
    {
        if (!once_control->init_executed) {
            do_execute = 1;
            once_control->init_executed = 1;
        }
        if (cur_task) {
            xSemaphoreGiveRecursive(s_once_mux);
        }
        if (do_execute) {
            ESP_LOGV(TAG, "%s: call init_routine %p", __FUNCTION__, once_control);
            init_routine();
        }
    }
    else
    {
        ESP_LOGE(TAG, "%s: Failed to lock!", __FUNCTION__);
        return EBUSY;
    }

    return 0;
}

/***************** MUTEX ******************/
static int mutexattr_check(const pthread_mutexattr_t *attr)
{
    if (attr->type < PTHREAD_MUTEX_NORMAL || attr->type > PTHREAD_MUTEX_RECURSIVE) {
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
    mux = (esp_pthread_mutex_t *)*mutex;

    // check if mux is busy
    int res = pthread_mutex_lock_internal(mux, 0);
    if (res == EBUSY) {
        return EBUSY;
    }

    vSemaphoreDelete(mux->sem);
    free(mux);

    return 0;
}

static int IRAM_ATTR pthread_mutex_lock_internal(esp_pthread_mutex_t *mux, TickType_t tmo)
{
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

static int pthread_mutex_init_if_static(pthread_mutex_t *mutex) {
    int res = 0;
    if ((intptr_t) *mutex == PTHREAD_MUTEX_INITIALIZER) {
        portENTER_CRITICAL(&s_mutex_init_lock);
        if ((intptr_t) *mutex == PTHREAD_MUTEX_INITIALIZER) {
            res = pthread_mutex_init(mutex, NULL);
        }
        portEXIT_CRITICAL(&s_mutex_init_lock);
    }
    return res;
}

int IRAM_ATTR pthread_mutex_lock(pthread_mutex_t *mutex)
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

int IRAM_ATTR pthread_mutex_trylock(pthread_mutex_t *mutex)
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

int IRAM_ATTR pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    esp_pthread_mutex_t *mux;

    if (!mutex) {
        return EINVAL;
    }
    mux = (esp_pthread_mutex_t *)*mutex;

    if (mux->type == PTHREAD_MUTEX_RECURSIVE) {
        xSemaphoreGiveRecursive(mux->sem);
    } else {
        xSemaphoreGive(mux->sem);
    }
    return 0;
}

int pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
    if (!attr) {
        return EINVAL;
    }
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
    ESP_LOGE(TAG, "%s: not supported!", __FUNCTION__);
    return ENOSYS;
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
