/* Example application which uses testable component.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include "esp_random.h"
#include "testable.h"

/* This application has a test subproject in 'test' directory, all the
 * interesting things happen there. See ../test/main/example_idf_test_runner_test.c
 * and the makefiles in ../test/ directory.
 *
 * This specific app_main function is provided only to illustrate the layout
 * of a project.
 */

void app_main(void)
{
    const int count = 32;
    const int max = 100;

    printf("In main application. Collecting %d random numbers from 1 to %d:\n", count, max);
    int *numbers = calloc(count, sizeof(numbers[0]));
    for (int i = 0; i < count; ++i) {
        numbers[i] = 1 + esp_random() % (max - 1);
        printf("%4d ", numbers[i]);
        if ((i + 1) % 10 == 0) {
            printf("\n");
        }
    }

    int mean = testable_mean(numbers, count);
    printf("\nMean: %d\n", mean);
    free(numbers);
}
