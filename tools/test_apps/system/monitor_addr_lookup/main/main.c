/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static volatile bool s_initialization_done = false;

static void initialize(void)
{
    srand(time(0));
}

static int get_random_number(void)
{
    if (!s_initialization_done) {
        initialize();
        s_initialization_done = true;
    }
    return rand();
}

void app_main(void)
{
    volatile int number = get_random_number();
    int *n = malloc(sizeof(int));

    assert(n);

    *n = number;

    printf("app_main is running from 0x%x\n", (int) app_main);
    printf("Initializer function at 0x%x\n", (int) initialize);
    printf("Got %d stored at 0x%x and 0x%x from a function from 0x%x\n", *n, (int) n, (int) (&number), (int) get_random_number);
    printf("This is the end of the report\n");

    free(n);
}
