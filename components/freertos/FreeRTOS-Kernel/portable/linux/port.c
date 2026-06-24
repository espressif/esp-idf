/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/queue.h>
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "utils/wait_for_event.h"
#include "esp_private/freertos_linux_coop_syscalls.h"
#include "utils/linux_port_utils.h"

#define FREERTOS_SIM_TICK_PERIOD_US (1000000 / CONFIG_FREERTOS_HZ)

typedef struct thread {
    const char *name;
    pthread_t pthread;
    TaskFunction_t pxCode;
    void *pvParams;
    bool is_dying;
    bool yield_needed;
    struct event *ev;
} thread_t;

typedef struct task_thread_node {
    TaskHandle_t handle;
    thread_t *thread;
    SLIST_ENTRY(task_thread_node) next;
} task_thread_node_t;


static SLIST_HEAD(task_thread_node_ll, task_thread_node) s_task_thread_list = SLIST_HEAD_INITIALIZER(task_thread_node);
static pthread_mutex_t s_thread_map_mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t s_port_mutex;
static pthread_t s_scheduler_thread;
static bool s_scheduler_started = false;
static int s_ux_critical_nesting = 0;

/* TLS flag: true only when inside a real FreeRTOS task pthread */
static __thread bool s_in_freertos_task = false;

bool linux_port_in_freertos_task(void)
{
    return s_in_freertos_task;
}

static void linux_port_initialize_mutexes(void)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&s_port_mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

static void linux_port_fatal_error(const char *msg, int err)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(err));
    abort();
}

static void linux_port_register_thread(TaskHandle_t handle, thread_t *thread)
{
    if (handle == NULL) {
        return;
    }

    task_thread_node_t *node = malloc(sizeof(task_thread_node_t));
    if (!node) {
        linux_port_fatal_error("Failed to allocate thread map node", -1);
    }

    node->handle = handle;
    node->thread = thread;

    pthread_mutex_lock(&s_thread_map_mutex);
    SLIST_INSERT_HEAD(&s_task_thread_list, node, next);
    pthread_mutex_unlock(&s_thread_map_mutex);
}

static void linux_port_unregister_thread(TaskHandle_t handle)
{
    if (handle == NULL) {
        return;
    }

    pthread_mutex_lock(&s_thread_map_mutex);

    task_thread_node_t *cur_node = SLIST_FIRST(&s_task_thread_list);
    task_thread_node_t *prev_node = NULL;

    while (cur_node) {
        if (cur_node->handle == handle) {
            if (prev_node) {
                prev_node->next.sle_next = SLIST_NEXT(cur_node, next);
            } else {
                SLIST_REMOVE_HEAD(&s_task_thread_list, next);
            }

            free(cur_node);
            pthread_mutex_unlock(&s_thread_map_mutex);
            return;
        }

        prev_node = cur_node;
        cur_node = SLIST_NEXT(cur_node, next);
    }

    pthread_mutex_unlock(&s_thread_map_mutex);
}

static thread_t *linux_port_get_thread_from_handle(TaskHandle_t handle)
{
    if (handle == NULL) {
        return NULL;
    }

    pthread_mutex_lock(&s_thread_map_mutex);
    task_thread_node_t *node = NULL;
    SLIST_FOREACH(node, &s_task_thread_list, next) {
        if (node->handle == (TaskHandle_t)(*(StackType_t **)(handle))) {
            thread_t *t = node->thread;
            pthread_mutex_unlock(&s_thread_map_mutex);
            return t;
        }
    }
    pthread_mutex_unlock(&s_thread_map_mutex);
    return NULL;
}

static thread_t *linux_port_get_calling_thread(void)
{
    pthread_t self = pthread_self();
    pthread_mutex_lock(&s_thread_map_mutex);
    task_thread_node_t *node = NULL;
    SLIST_FOREACH(node, &s_task_thread_list, next) {
        if (pthread_equal(node->thread->pthread, self)) {
            thread_t *thread = node->thread;
            pthread_mutex_unlock(&s_thread_map_mutex);
            return thread;
        }
    }
    pthread_mutex_unlock(&s_thread_map_mutex);
    return NULL;
}

pthread_t linux_port_get_scheduled_task_pthread(void)
{
    thread_t *thread = linux_port_get_thread_from_handle(xTaskGetCurrentTaskHandle());
    return thread ? thread->pthread : pthread_self();
}

static void *linux_port_task_runner(void *arg)
{
    /* Allow this thread to be cancelled */
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    /* set the flag showing that this is a freertos task */
    s_in_freertos_task = true;

    /* setup the backtrace signal. ONLY triggered before abort so
     * it will not interfere with the simulation while its running */
    linux_port_setup_backtrace_signal();

    thread_t *thread = arg;

    /* Block until scheduler signals first time, then run the task body. */
    event_wait(thread->ev);
    thread->pxCode(thread->pvParams);

    return NULL;
}

static void linux_port_unblock_thread(thread_t *thread)
{
    event_signal(thread->ev);
}

static void linux_port_block_thread(thread_t *thread)
{
    event_wait(thread->ev);
}

static void linux_port_increment_tick(void)
{
    (void)xTaskIncrementTick();
}

static void linux_port_switch_context(TaskHandle_t current_task_hdl)
{
    pthread_mutex_lock(&s_port_mutex);

    thread_t *current_thread = linux_port_get_thread_from_handle(current_task_hdl);

    /* get the task that should be scheduled next */
    vTaskSwitchContext();

    /* get the new task to schedule and the associated thread item */
    TaskHandle_t next_task_hdl = xTaskGetCurrentTaskHandle();
    thread_t *next_thread = linux_port_get_thread_from_handle(next_task_hdl);

    /* unblock the newly scheduled task if it is different from
     * the one already scheduled */
    if (next_thread) {
        /* Only unblock the thread if we are actually switching to a
         * different one. Signaling the already-running thread would
         * leave a stale event_triggered flag, causing its next
         * event_wait (e.g. in vPortYield) to return immediately
         * instead of blocking.
         *
         * Exception: on the very first switch, the task is still blocked
         * in its initial event_wait (linux_port_task_runner), so we must
         * signal it even though current_thread == next_thread. */
        if (next_thread != current_thread || !s_scheduler_started) {
            linux_port_unblock_thread(next_thread);
        }
    }

    if (!s_scheduler_started) {
        s_scheduler_started = true;
    }

    /* fill the name of the task in the thread item if not done already. */
    if (next_thread && next_thread->name == NULL) {
        /* fill the name of the thread now */
        next_thread->name = pcTaskGetName(next_task_hdl);
    }

    /* fill the name of the task in the thread item if not done already. */
    if (current_thread && current_thread->name == NULL) {
        /* fill the name of the thread now */
        current_thread->name = pcTaskGetName(current_task_hdl);
    }

    pthread_mutex_unlock(&s_port_mutex);
}

static void *linux_port_scheduler_runner(void *arg)
{
    (void)arg;

    while (1) {
        /* sleep for a period of 1 tick */
        usleep(FREERTOS_SIM_TICK_PERIOD_US);

        /* get the task that is currently scheduled */
        TaskHandle_t current_task_hdl = xTaskGetCurrentTaskHandle();

        /* Lock the port mutex. This will block while any task is in a
         * critical section, ensuring ticks don't preempt critical code. */
        pthread_mutex_lock(&s_port_mutex);

        /* increment the freertos tick */
        linux_port_increment_tick();

        /* schedule a new task, and schedule out the currently running one */
        linux_port_switch_context(current_task_hdl);

        pthread_mutex_unlock(&s_port_mutex);
    }
    return NULL;
}

StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack,
                                   StackType_t *pxEndOfStack,
                                   TaskFunction_t pxCode,
                                   void *pvParameters)
{
    pthread_attr_t thread_attr;
    size_t thread_stack_size;

    /* Store the thread data at the start of the stack. */
    thread_stack_size = (pxTopOfStack - pxEndOfStack) * sizeof(*pxTopOfStack);
    pthread_attr_init(&thread_attr);
    pthread_attr_setstack(&thread_attr, pxEndOfStack, thread_stack_size);

    thread_t *thread = malloc(sizeof(thread_t));
    if (!thread) {
        linux_port_fatal_error("Failed to allocate thread metadata", -1);
    }
    thread->name = NULL; // this will be filled later when we know about the task name
    thread->pxCode = pxCode;
    thread->pvParams = pvParameters;
    thread->is_dying = false;
    thread->yield_needed = false;
    thread->ev = event_create();

    linux_port_register_thread((TaskHandle_t)pxTopOfStack, thread);

    /* create the thread associated with the task being created */
    const int ret = pthread_create(&thread->pthread, &thread_attr, linux_port_task_runner, thread);
    if (ret != 0) {
        linux_port_fatal_error("pthread_create", ret);
    }
    return pxTopOfStack;
}

BaseType_t xPortStartScheduler(void)
{
    /* set the port mutex to be recursive. Must be done before
     * vPortEnableInterrupts() which calls vPortExitCritical(). */
    linux_port_initialize_mutexes();

    /* enable interrupt that were disabled in vTaskStartScheduler */
    vPortEnableInterrupts();

    /* init the cooperative syscall layer (sets stdio non-blocking).
     * Provided by VFS component; weak no-op when VFS is not linked. */
    freertos_linux_coop_syscalls_init();

    /* Start scheduler thread */
    int ret = pthread_create(&s_scheduler_thread, NULL, linux_port_scheduler_runner, NULL);
    if (ret != 0) {
       linux_port_fatal_error("pthread_create", ret);
    }

    /* Should never return */
    pthread_join(s_scheduler_thread, NULL);

    return 0;
}

void vPortEndScheduler(void)
{
    exit(0);
}

void vPortEnterCritical(void)
{
    if (!s_scheduler_started) {
        return;
    }

    pthread_mutex_lock(&s_port_mutex);

    /* Non-FreeRTOS thread or recursive enter: just bump the counter.
     * The mutex is already held (recursive lock succeeds for same thread). */
    if (!linux_port_in_freertos_task() || s_ux_critical_nesting > 0) {
        s_ux_critical_nesting++;
        return;
    }

    /* First enter from a FreeRTOS task: ensure we're the scheduled task.
     * If not, release the mutex, block until the scheduler switches to us,
     * then re-acquire. */
    thread_t *calling_thread = linux_port_get_calling_thread();
    thread_t *scheduled_thread = linux_port_get_thread_from_handle(xTaskGetCurrentTaskHandle());

    while (calling_thread && !calling_thread->is_dying && calling_thread != scheduled_thread) {
        pthread_mutex_unlock(&s_port_mutex);
        linux_port_block_thread(calling_thread);
        pthread_mutex_lock(&s_port_mutex);
        calling_thread = linux_port_get_calling_thread();
        scheduled_thread = linux_port_get_thread_from_handle(xTaskGetCurrentTaskHandle());
    }

    if (!calling_thread || calling_thread->is_dying) {
        linux_port_switch_context(xTaskGetCurrentTaskHandle());
        pthread_mutex_unlock(&s_port_mutex);
        return;
    }

    s_ux_critical_nesting = 1;
}

void vPortExitCritical(void)
{
    if (!s_scheduler_started || s_ux_critical_nesting == 0) {
        return;
    }

    s_ux_critical_nesting--;

    /* Check for deferred yield on final exit from a FreeRTOS task */
    if (s_ux_critical_nesting == 0 && linux_port_in_freertos_task()) {
        thread_t *calling_thread = linux_port_get_calling_thread();
        if (calling_thread && calling_thread->yield_needed) {
            calling_thread->yield_needed = false;
            pthread_mutex_unlock(&s_port_mutex);
            vPortYield();
            return;
        }
    }

    pthread_mutex_unlock(&s_port_mutex);
}

/* Handle the case where the calling pthread is not a registered FreeRTOS task.
 * If the calling thread has been deleted but is still the scheduled task,
 * perform a context switch. Otherwise just release the mutex and return.
 * Returns true if the caller should return early. */
static bool linux_port_handle_deleted_task(TaskHandle_t scheduled_task_hdl,
                                           thread_t *calling_thread)
{
    if (calling_thread != NULL) {
        return false;
    }

    thread_t *scheduled_thread = linux_port_get_thread_from_handle(scheduled_task_hdl);
    if (scheduled_thread == NULL) {
        linux_port_switch_context(scheduled_task_hdl);
    }
    pthread_mutex_unlock(&s_port_mutex);
    return true;
}

void vPortYield(void)
{
    pthread_mutex_lock(&s_port_mutex);

    thread_t *calling_thread = linux_port_get_calling_thread();
    TaskHandle_t scheduled_task_hdl = xTaskGetCurrentTaskHandle();

    if (linux_port_handle_deleted_task(scheduled_task_hdl, calling_thread)) {
        return;
    }

    /* If in a critical section, defer the yield until the section exits. */
    if (s_ux_critical_nesting != 0) {
        calling_thread->yield_needed = true;
        pthread_mutex_unlock(&s_port_mutex);
        return;
    }

    /* Hand the CPU to the next ready task right now (mimics PendSV on real
     * hardware). */
    linux_port_switch_context(scheduled_task_hdl);
    pthread_mutex_unlock(&s_port_mutex);

    /* If the newly scheduled task is different from the calling thread,
     * block until the scheduler resumes this task. */
    TaskHandle_t next_task_hdl = xTaskGetCurrentTaskHandle();
    thread_t *next_thread = linux_port_get_thread_from_handle(next_task_hdl);
    if (calling_thread != next_thread) {
        linux_port_block_thread(calling_thread);
    }
}

void vPortYieldWithinApi(void)
{
    vPortYield();
}

void vPortSuspendScheduler(void)
{
    /* scheduled out task trying to suspend the scheduler should get blocked here */
    pthread_mutex_lock(&s_port_mutex);

   /* get the metadata of the pthread calling this function */
    thread_t *calling_thread = linux_port_get_calling_thread();

    /* get the thread metadata from the scheduled task */
    TaskHandle_t scheduled_task_hdl = xTaskGetCurrentTaskHandle();

    if (linux_port_handle_deleted_task(scheduled_task_hdl, calling_thread)) {
        return;
    }

    thread_t *scheduled_thread = linux_port_get_thread_from_handle(scheduled_task_hdl);
    if (calling_thread != scheduled_thread) {
        pthread_mutex_unlock(&s_port_mutex);
        vPortYield();
        return;
    }
    pthread_mutex_unlock(&s_port_mutex);
}

void vPortDisableInterrupts(void)
{
    vPortEnterCritical();
}

void vPortEnableInterrupts(void)
{
    vPortExitCritical();
}

BaseType_t xPortSetInterruptMask(void)
{
    vPortEnterCritical();
    return pdTRUE;
}

void vPortClearInterruptMask(BaseType_t xMask)
{
    vPortExitCritical();
}

void vPortThreadDying(void *pxTaskToDelete, volatile BaseType_t *pxPendYield)
{
    pthread_mutex_lock(&s_port_mutex);

    thread_t *thread = linux_port_get_thread_from_handle((TaskHandle_t)pxTaskToDelete);
    if (thread == NULL) {
        pthread_mutex_unlock(&s_port_mutex);
        return;
    }

    /* Mark the thread as dying, cancel the thread. the pthread
     * will be stopped on next cancellation point. Do not remove the
     * thread item from the list since it will be done in vPortCancelThread */
    thread->is_dying = true;
    pthread_cancel(thread->pthread);

    pthread_mutex_unlock(&s_port_mutex);
}

#if CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS
static void vPortTLSPointersDelCb(void *pxTCB)
{
    StaticTask_t *tcb = (StaticTask_t *)pxTCB;
    TlsDeleteCallbackFunction_t *pvDelCbs = (TlsDeleteCallbackFunction_t *)(&tcb->pvDummy15[configNUM_THREAD_LOCAL_STORAGE_POINTERS / 2]);

    for (int x = 0; x < (configNUM_THREAD_LOCAL_STORAGE_POINTERS / 2); x++) {
        if (pvDelCbs[x] != NULL) {
            pvDelCbs[x](x, tcb->pvDummy15[x]);
        }
    }
}
#endif /* CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS */

void vPortCancelThread(void *pxTaskToDelete)
{
    pthread_mutex_lock(&s_port_mutex);

#if CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS
    vPortTLSPointersDelCb(pxTaskToDelete);
#endif

    thread_t *thread = linux_port_get_thread_from_handle((TaskHandle_t)pxTaskToDelete);
    if (!thread) {
        pthread_mutex_unlock(&s_port_mutex);
        return;
    }

    if (thread->is_dying) {
        /* vPortThreadDying already called */
    } else {
        thread->is_dying = true;
        pthread_cancel(thread->pthread);
    }

    /* Save fields and unregister while holding the lock. */
    pthread_t pt = thread->pthread;
    event_t *ev = thread->ev;
    linux_port_unregister_thread((TaskHandle_t)pxTaskToDelete);

    /* Release the mutex before joining – the dying thread may need the
     * scheduler (which also takes s_port_mutex) to reach a cancellation
     * point. */
    pthread_mutex_unlock(&s_port_mutex);

    pthread_join(pt, NULL);
    event_delete(ev);
    free(thread);
}

void vPortSetStackWatchpoint(void *pxStackStart)
{
}
