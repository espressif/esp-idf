/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
// Test pthread_create_key, pthread_delete_key, pthread_setspecific, pthread_getspecific
#include <pthread.h>
#include <inttypes.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "test_utils.h"
#include "esp_random.h"

TEST_CASE("pthread local storage basics", "[thread-specific]")
{
    pthread_key_t key;
    TEST_ASSERT_EQUAL(0, pthread_key_create(&key, NULL));

    TEST_ASSERT_NULL(pthread_getspecific(key));
    int val = 3;

    printf("Setting to %p...\n", &val);
    TEST_ASSERT_EQUAL(0, pthread_setspecific(key, &val));

    printf("Reading back...\n");
    TEST_ASSERT_EQUAL_PTR(&val, pthread_getspecific(key));

    printf("Setting to NULL...\n");
    TEST_ASSERT_EQUAL(0, pthread_setspecific(key, NULL));

    printf("Reading back...\n");
    TEST_ASSERT_NULL(pthread_getspecific(key));

    TEST_ASSERT_EQUAL(0, pthread_key_delete(key));
}

TEST_CASE("pthread local storage unique keys", "[thread-specific]")
{
    const int NUM_KEYS = 10;
    pthread_key_t keys[NUM_KEYS];

    for (int i = 0; i < NUM_KEYS; i++) {
        TEST_ASSERT_EQUAL(0, pthread_key_create(&keys[i], NULL));
        printf("New key %d = %"PRIu32"\n", i, keys[i]);
    }

    for (int i = 0; i < NUM_KEYS; i++) {
        for (int j = 0; j < NUM_KEYS; j++) {
            if (i != j) {
                TEST_ASSERT_NOT_EQUAL(keys[i], keys[j]);
            }
        }
    }

    for (int i = 0; i < NUM_KEYS; i++) {
        TEST_ASSERT_EQUAL(0, pthread_key_delete(keys[i]));
    }
}

static void test_pthread_destructor(void *);
static void *expected_destructor_ptr;
static void *actual_destructor_ptr;
static void *thread_test_pthread_destructor(void *);

TEST_CASE("pthread local storage destructor", "[thread-specific]")
{
    pthread_t thread;
    pthread_key_t key = -1;

    expected_destructor_ptr = NULL;
    actual_destructor_ptr = NULL;

    TEST_ASSERT_EQUAL(0, pthread_key_create(&key, test_pthread_destructor));

    TEST_ASSERT_EQUAL(0, pthread_create(&thread, NULL, thread_test_pthread_destructor, (void *)key));
    TEST_ASSERT_EQUAL(0, pthread_join(thread, NULL));

    printf("Joined...\n");
    TEST_ASSERT_NOT_NULL(expected_destructor_ptr);
    TEST_ASSERT_NOT_NULL(actual_destructor_ptr);
    TEST_ASSERT_EQUAL_PTR(expected_destructor_ptr, actual_destructor_ptr);

    TEST_ASSERT_EQUAL(0, pthread_key_delete(key));
}

static void *thread_test_pthread_destructor(void *v_key)
{
    printf("Local storage thread running...\n");
    pthread_key_t key = (pthread_key_t) v_key;
    expected_destructor_ptr = &key; // address of stack variable in the task...
    pthread_setspecific(key, expected_destructor_ptr);
    printf("Local storage thread done.\n");
    return NULL;
}

static void test_pthread_destructor(void *value)
{
    actual_destructor_ptr = value;
}

#if defined(CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS)
static void task_test_pthread_destructor(void *v_key);

TEST_CASE("pthread local storage destructor in FreeRTOS task", "[thread-specific]")
{
    // Same as previous test case, but doesn't use pthread APIs therefore must wait
    // for the idle task to call the destructor
    pthread_key_t key = -1;

    expected_destructor_ptr = NULL;
    actual_destructor_ptr = NULL;

    TEST_ASSERT_EQUAL(0, pthread_key_create(&key, test_pthread_destructor));

    xTaskCreate(task_test_pthread_destructor,
                "ptdest", 8192, (void *)key, UNITY_FREERTOS_PRIORITY+1,
                NULL);

    // Above task has higher priority to us, so should run immediately
    // but we need to wait for the idle task cleanup to run
    vTaskDelay(20);

    TEST_ASSERT_NOT_NULL(expected_destructor_ptr);
    TEST_ASSERT_NOT_NULL(actual_destructor_ptr);
    TEST_ASSERT_EQUAL_PTR(expected_destructor_ptr, actual_destructor_ptr);

    TEST_ASSERT_EQUAL(0, pthread_key_delete(key));
}

static void task_test_pthread_destructor(void *v_key)
{
    /* call the pthread main routine, then delete ourselves... */
    thread_test_pthread_destructor(v_key);
    vTaskDelete(NULL);
}
#endif /* CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS */

#define STRESS_NUMITER 2000000
#define STRESS_NUMTASKS 16

static void *thread_stress_test(void *v_key)
{
    pthread_key_t key = (pthread_key_t) v_key;
    void *tls_value = (void *)esp_random();

    pthread_setspecific(key, tls_value);

    for(int i = 0; i < STRESS_NUMITER; i++) {
        TEST_ASSERT_EQUAL_HEX32(pthread_getspecific(key), tls_value);
    }

    return NULL;
}


// This test case added to reproduce issues with unpinned tasks and TLS
TEST_CASE("pthread local storage stress test", "[thread-specific]")
{
    pthread_key_t key = -1;
    pthread_t threads[STRESS_NUMTASKS] = { 0 };
    TEST_ASSERT_EQUAL(0, pthread_key_create(&key, test_pthread_destructor));

    for (int i = 0; i < STRESS_NUMTASKS; i++) {
        TEST_ASSERT_EQUAL(0, pthread_create(&threads[i], NULL, thread_stress_test, (void *)key));
    }
    for (int i = 0; i < STRESS_NUMTASKS; i++) {
        TEST_ASSERT_EQUAL(0, pthread_join(threads[i], NULL));
    }
}


#define NUM_KEYS 4 // number of keys used in repeat destructor test
#define NUM_REPEATS 17 // number of times we re-set a key to a non-NULL value to re-trigger destructor

typedef struct {
    pthread_key_t keys[NUM_KEYS]; // pthread local storage keys used in test
    unsigned count; // number of times the destructor has been called
    int last_idx; // index of last key where destructor was called
} destr_test_state_t;


static void s_test_repeat_destructor(void *vp_state);
static void *s_test_repeat_destructor_thread(void *vp_state);

// Test the correct behaviour of a pthread destructor function that uses
// pthread_setspecific() to set another value when it runs, and also
//
// As described in https://pubs.opengroup.org/onlinepubs/009695399/functions/pthread_key_create.html
TEST_CASE("pthread local storage 'repeat' destructor test", "[thread-specific]")
{
    int r;
    destr_test_state_t state = { .last_idx = -1 };
    pthread_t thread;

    for (int i = 0; i < NUM_KEYS; i++) {
        r = pthread_key_create(&state.keys[i], s_test_repeat_destructor);
        TEST_ASSERT_EQUAL(0, r);
    }

    r = pthread_create(&thread, NULL, s_test_repeat_destructor_thread, &state);
    TEST_ASSERT_EQUAL(0, r);

    r = pthread_join(thread, NULL);
    TEST_ASSERT_EQUAL(0 ,r);

    // Cheating here to make sure compiler reads the value of 'count' from memory not from a register
    //
    // We expect the destructor was called NUM_REPEATS times when it repeated, then NUM_KEYS times when it didn't
    TEST_ASSERT_EQUAL(NUM_REPEATS + NUM_KEYS, ((volatile destr_test_state_t)state).count);

    // cleanup
    for (int i = 0; i < NUM_KEYS; i++) {
        r = pthread_key_delete(state.keys[i]);
        TEST_ASSERT_EQUAL(0, r);
    }
}

static void s_test_repeat_destructor(void *vp_state)
{
    destr_test_state_t *state = vp_state;

    state->count++;
    printf("Destructor! Arg %p Count %d\n", state, state->count);
    if (state->count > NUM_REPEATS) {
        return; // Stop replacing values after NUM_REPEATS destructors have been called, they will be NULLed out now
    }

    // Find the key which has a NULL value, this is the key for this destructor. We will set it back to 'state' to repeat later.
    // At this point only one key should have a NULL value
    int null_idx = -1;
    for (int i = 0; i < NUM_KEYS; i++) {
        if (pthread_getspecific(state->keys[i]) == NULL) {
            TEST_ASSERT_EQUAL(-1, null_idx); // If more than one key has a NULL value, something has gone wrong
            null_idx = i;
            // don't break, verify the other keys have non-NULL values
        }
    }

    TEST_ASSERT_NOT_EQUAL(-1, null_idx); // One key should have a NULL value

    // The same key shouldn't be destroyed twice in a row, as new non-NULL values should be destroyed
    // after existing non-NULL values (to match spec behaviour)
    TEST_ASSERT_NOT_EQUAL(null_idx, state->last_idx);

    printf("Re-setting index %d\n", null_idx);
    pthread_setspecific(state->keys[null_idx], state);
    state->last_idx = null_idx;
}

static void *s_test_repeat_destructor_thread(void *vp_state)
{
    destr_test_state_t *state = vp_state;
    for (int i = 0; i < NUM_KEYS; i++) {
        pthread_setspecific(state->keys[i], state);
    }
    pthread_exit(NULL);
}
