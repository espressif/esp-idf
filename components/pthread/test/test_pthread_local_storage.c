// Test pthread_create_key, pthread_delete_key, pthread_setspecific, pthread_getspecific
#include <pthread.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TEST_CASE("pthread local storage basics", "[pthread]")
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

TEST_CASE("pthread local storage unique keys", "[pthread]")
{
    const int NUM_KEYS = 10;
    pthread_key_t keys[NUM_KEYS];

    for (int i = 0; i < NUM_KEYS; i++) {
        TEST_ASSERT_EQUAL(0, pthread_key_create(&keys[i], NULL));
        printf("New key %d = %d\n", i, keys[i]);
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

TEST_CASE("pthread local storage destructor", "[pthread]")
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

static void task_test_pthread_destructor(void *v_key);

TEST_CASE("pthread local storage destructor in FreeRTOS task", "[pthread]")
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
    printf("Destructor called...\n");
    actual_destructor_ptr = value;
}

static void task_test_pthread_destructor(void *v_key)
{
    /* call the pthread main routine, then delete ourselves... */
    thread_test_pthread_destructor(v_key);
    vTaskDelete(NULL);
}
