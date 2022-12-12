/* Pthread Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_pthread.h"

static void *example_thread(void * arg);

void app_main(void)
{
    pthread_attr_t attr;
    pthread_t thread1, thread2;
    esp_pthread_cfg_t esp_pthread_cfg;
    int res;

    // Create a pthread with the default parameters
    res = pthread_create(&thread1, NULL, example_thread, NULL);
    assert(res == 0);
    printf("Created thread 0x%"PRIx32"\n", thread1);

    // Create a pthread with a larger stack size using the standard API
    res = pthread_attr_init(&attr);
    assert(res == 0);
    pthread_attr_setstacksize(&attr, 16384);
    res = pthread_create(&thread2, &attr, example_thread, NULL);
    assert(res == 0);
    printf("Created larger stack thread 0x%"PRIx32"\n", thread2);

    res = pthread_join(thread1, NULL);
    assert(res == 0);
    res = pthread_join(thread2, NULL);
    assert(res == 0);
    printf("Threads have exited\n\n");

    // Use the ESP-IDF API to change the default thread attributes
    esp_pthread_cfg = esp_pthread_get_default_config();
    esp_pthread_cfg.stack_size = 32768;
    esp_pthread_cfg.prio += 2;
    ESP_ERROR_CHECK( esp_pthread_set_cfg(&esp_pthread_cfg) );

    res = pthread_create(&thread1, NULL, example_thread, NULL);
    assert(res == 0);
    printf("Created thread 0x%"PRIx32" with new default config\n", thread1);
    res = pthread_join(thread1, NULL);
    assert(res == 0);
    printf("Thread has exited\n\n");
}

static void *example_thread(void * arg)
{
    usleep(250 * 1000);
    printf("This thread has ID 0x%"PRIx32" and %u bytes free stack\n", pthread_self(), uxTaskGetStackHighWaterMark(NULL));

    sleep(1);
    printf("Thread 0x%"PRIx32" exiting\n", pthread_self());

    return NULL;
}
