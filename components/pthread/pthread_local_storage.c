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
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sys/lock.h"
#include "rom/queue.h"

#include "pthread_internal.h"

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

static _lock_t s_keys_lock;

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

    _lock_acquire_recursive(&s_keys_lock);

    const key_entry_t *head = SLIST_FIRST(&s_keys);
    new_key->key = (head == NULL) ? 1 : (head->key + 1);
    new_key->destructor = destructor;
    *key = new_key->key;

    SLIST_INSERT_HEAD(&s_keys, new_key, next);

    _lock_release_recursive(&s_keys_lock);
    return 0;
}

static key_entry_t *find_key(pthread_key_t key)
{
    _lock_acquire_recursive(&s_keys_lock);
    key_entry_t *result = NULL;;
    SLIST_FOREACH(result, &s_keys, next) {
        if(result->key == key) {
            break;
        }
    }
    _lock_release_recursive(&s_keys_lock);
    return result;
}

int pthread_key_delete(pthread_key_t key)
{

    _lock_acquire_recursive(&s_keys_lock);

    /* Ideally, we would also walk all tasks' thread local storage value_list here
       and delete any values associated with this key. We do not do this...
    */

    key_entry_t *entry = find_key(key);
    if (entry != NULL) {
        SLIST_REMOVE(&s_keys, entry, key_entry_t_, next);
        free(entry);
    }

    _lock_release_recursive(&s_keys_lock);

    return 0;
}

/* Clean up callback for deleted tasks.

   This is called from one of two places:

   If the thread was created via pthread_create() then it's called by pthread_task_func() when that thread ends,
   and the FreeRTOS thread-local-storage is removed before the FreeRTOS task is deleted.

   For other tasks, this is called when the FreeRTOS idle task performs its task cleanup after the task is deleted.

   (The reason for calling it early for pthreads is to keep the timing consistent with "normal" pthreads, so after
   pthread_join() the task's destructors have all been called even if the idle task hasn't run cleanup yet.)
*/
static void pthread_local_storage_thread_deleted_callback(int index, void *v_tls)
{
    values_list_t *tls = (values_list_t *)v_tls;
    assert(tls != NULL);

    /* Walk the list, freeing all entries and calling destructors if they are registered */
    value_entry_t *entry = SLIST_FIRST(tls);
    while(entry != NULL) {
        // This is a little slow, walking the linked list of keys once per value,
        // but assumes that the thread's value list will have less entries
        // than the keys list
        key_entry_t *key = find_key(entry->key);
        if (key != NULL && key->destructor != NULL) {
            key->destructor(entry->value);
        }
        value_entry_t *next_entry = SLIST_NEXT(entry, next);
        free(entry);
        entry = next_entry;
    }
    free(tls);
}

/* this function called from pthread_task_func for "early" cleanup of TLS in a pthread */
void pthread_internal_local_storage_destructor_callback()
{
    void *tls = pvTaskGetThreadLocalStoragePointer(NULL, PTHREAD_TLS_INDEX);
    if (tls != NULL) {
        pthread_local_storage_thread_deleted_callback(PTHREAD_TLS_INDEX, tls);
        /* remove the thread-local-storage pointer to avoid the idle task cleanup
           calling it again...
        */
        vTaskSetThreadLocalStoragePointerAndDelCallback(NULL,
                                                        PTHREAD_TLS_INDEX,
                                                        NULL,
                                                        NULL);
    }
}

static value_entry_t *find_value(const values_list_t *list, pthread_key_t key)
{
    value_entry_t *result = NULL;;
    SLIST_FOREACH(result, list, next) {
        if(result->key == key) {
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
    if(entry != NULL) {
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
        vTaskSetThreadLocalStoragePointerAndDelCallback(NULL,
                                                        PTHREAD_TLS_INDEX,
                                                        tls,
                                                        pthread_local_storage_thread_deleted_callback);
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
        SLIST_INSERT_HEAD(tls, entry, next);
    }

    return 0;
}
