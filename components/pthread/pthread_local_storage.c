/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sys/lock.h"
#include "sys/queue.h"

#include "pthread_internal.h"

/* Sanity check to ensure that the number of FreeRTOS TLSPs is at least 1 */
#if (CONFIG_FREERTOS_THREAD_LOCAL_STORAGE_POINTERS < 1)
#error "CONFIG_FREERTOS_THREAD_LOCAL_STORAGE_POINTERS cannot be 0 for pthread TLS"
#endif

#define PTHREAD_TLS_INDEX 0

typedef void (*pthread_destructor_t)(void*);

/* This is a very naive implementation of key-indexed thread local storage, using two linked lists
   (one is a global list of registered keys, one per thread for thread local storage values).

   It won't work well if lots of keys & thread-local values are stored (O(n) lookup for both),
   but it should work for small amounts of data.
*/
typedef struct key_entry_t_ {
    pthread_key_t key;
    pthread_destructor_t destructor;
    SLIST_ENTRY(key_entry_t_) next;
} key_entry_t;

// List of all keys created with pthread_key_create()
SLIST_HEAD(key_list_t, key_entry_t_) s_keys = SLIST_HEAD_INITIALIZER(s_keys);

static portMUX_TYPE s_keys_lock = portMUX_INITIALIZER_UNLOCKED;

// List of all value entries associated with a thread via pthread_setspecific()
typedef struct value_entry_t_ {
    pthread_key_t key;
    void *value;
    SLIST_ENTRY(value_entry_t_) next;
} value_entry_t;

// Type for the head of the list, as saved as a FreeRTOS thread local storage pointer
SLIST_HEAD(values_list_t_, value_entry_t_);
typedef struct values_list_t_ values_list_t;

int pthread_key_create(pthread_key_t *key, pthread_destructor_t destructor)
{
    key_entry_t *new_key = malloc(sizeof(key_entry_t));
    if (new_key == NULL) {
        return ENOMEM;
    }

    portENTER_CRITICAL(&s_keys_lock);

    const key_entry_t *head = SLIST_FIRST(&s_keys);
    new_key->key = (head == NULL) ? 1 : (head->key + 1);
    new_key->destructor = destructor;
    *key = new_key->key;

    SLIST_INSERT_HEAD(&s_keys, new_key, next);

    portEXIT_CRITICAL(&s_keys_lock);
    return 0;
}

static key_entry_t *find_key(pthread_key_t key)
{
    portENTER_CRITICAL(&s_keys_lock);
    key_entry_t *result = NULL;;
    SLIST_FOREACH(result, &s_keys, next) {
        if (result->key == key) {
            break;
        }
    }
    portEXIT_CRITICAL(&s_keys_lock);
    return result;
}

int pthread_key_delete(pthread_key_t key)
{

    portENTER_CRITICAL(&s_keys_lock);

    /* Ideally, we would also walk all tasks' thread local storage value_list here
       and delete any values associated with this key. We do not do this...
    */

    key_entry_t *entry = find_key(key);
    if (entry != NULL) {
        SLIST_REMOVE(&s_keys, entry, key_entry_t_, next);
        free(entry);
    }

    portEXIT_CRITICAL(&s_keys_lock);

    return 0;
}

/* Clean up callback for deleted tasks.

   This is called from one of two places:

   If the thread was created via pthread_create() then it's called by pthread_task_func() when that thread ends,
   or calls pthread_exit(), and the FreeRTOS thread-local-storage is removed before the FreeRTOS task is deleted.

   For other tasks, this is called when the FreeRTOS idle task performs its task cleanup after the task is deleted.

   There are two reasons for calling it early for pthreads:

   - To keep the timing consistent with "normal" pthreads, so after pthread_join() the task's destructors have all
     been called even if the idle task hasn't run cleanup yet.

   - The destructor is always called in the context of the thread itself - which is important if the task then calls
     pthread_getspecific() or pthread_setspecific() to update the state further, as allowed for in the spec.
*/
static void pthread_cleanup_thread_specific_data_callback(int index, void *v_tls)
{
    values_list_t *tls = (values_list_t *)v_tls;
    assert(tls != NULL);

    /* Walk the list, freeing all entries and calling destructors if they are registered */
    while (1) {
        value_entry_t *entry = SLIST_FIRST(tls);
        if (entry == NULL) {
            break;
        }
        SLIST_REMOVE_HEAD(tls, next);

        // This is a little slow, walking the linked list of keys once per value,
        // but assumes that the thread's value list will have less entries
        // than the keys list
        key_entry_t *key = find_key(entry->key);
        if (key != NULL && key->destructor != NULL) {
            key->destructor(entry->value);
        }
        free(entry);
    }
    free(tls);
}

/* this function called from pthread_task_func for "early" cleanup of TLS in a pthread
   and from pthread_join/pthread_detach for cleanup of TLS after pthread exit
*/
void pthread_internal_local_storage_destructor_callback(TaskHandle_t handle)
{
    void *tls = pvTaskGetThreadLocalStoragePointer(handle, PTHREAD_TLS_INDEX);
    if (tls != NULL) {
        pthread_cleanup_thread_specific_data_callback(PTHREAD_TLS_INDEX, tls);
        /* remove the thread-local-storage pointer to avoid the idle task cleanup
           calling it again...
        */
#if !defined(CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS)
        vTaskSetThreadLocalStoragePointer(handle, PTHREAD_TLS_INDEX, NULL);
#else
        vTaskSetThreadLocalStoragePointerAndDelCallback(handle,
                                                        PTHREAD_TLS_INDEX,
                                                        NULL,
                                                        NULL);
#endif /* CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS */
    }
}

static value_entry_t *find_value(const values_list_t *list, pthread_key_t key)
{
    value_entry_t *result = NULL;;
    SLIST_FOREACH(result, list, next) {
        if (result->key == key) {
            break;
        }
    }
    return result;
}

void *pthread_getspecific(pthread_key_t key)
{
    values_list_t *tls = (values_list_t *) pvTaskGetThreadLocalStoragePointer(NULL, PTHREAD_TLS_INDEX);
    if (tls == NULL) {
        return NULL;
    }

    value_entry_t *entry = find_value(tls, key);
    if (entry != NULL) {
        return entry->value;
    }
    return NULL;
}

int pthread_setspecific(pthread_key_t key, const void *value)
{
    key_entry_t *key_entry = find_key(key);
    if (key_entry == NULL) {
        return ENOENT; // this situation is undefined by pthreads standard
    }

    values_list_t *tls = pvTaskGetThreadLocalStoragePointer(NULL, PTHREAD_TLS_INDEX);
    if (tls == NULL) {
        tls = calloc(1, sizeof(values_list_t));
        if (tls == NULL) {
            return ENOMEM;
        }
#if !defined(CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS)
        vTaskSetThreadLocalStoragePointer(NULL, PTHREAD_TLS_INDEX, tls);
#else
        vTaskSetThreadLocalStoragePointerAndDelCallback(NULL,
                                                        PTHREAD_TLS_INDEX,
                                                        tls,
                                                        pthread_cleanup_thread_specific_data_callback);
#endif /* CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS */
    }

    value_entry_t *entry = find_value(tls, key);
    if (entry != NULL) {
        if (value != NULL) {
            // cast on next line is necessary as pthreads API uses
            // 'const void *' here but elsewhere uses 'void *'
            entry->value = (void *) value;
        } else { // value == NULL, remove the entry
            SLIST_REMOVE(tls, entry, value_entry_t_, next);
            free(entry);
        }
    } else if (value != NULL) {
        entry = malloc(sizeof(value_entry_t));
        if (entry == NULL) {
            return ENOMEM;
        }
        entry->key = key;
        entry->value = (void *) value; // see note above about cast

        // insert the new entry at the end of the list. this is important because
        // a destructor may call pthread_setspecific() to add a new non-NULL value
        // to the list, and this should be processed after all other entries.
        //
        // See pthread_cleanup_thread_specific_data_callback()
        value_entry_t *last_entry = NULL;
        value_entry_t *it;
        SLIST_FOREACH(it, tls, next) {
            last_entry = it;
        }
        if (last_entry == NULL) {
            SLIST_INSERT_HEAD(tls, entry, next);
        } else {
            SLIST_INSERT_AFTER(last_entry, entry, next);
        }
    }

    return 0;
}

/* Hook function to force linking this file */
void pthread_include_pthread_local_storage_impl(void)
{
}
