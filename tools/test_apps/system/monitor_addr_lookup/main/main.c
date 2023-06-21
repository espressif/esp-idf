/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#if CONFIG_TEST_ADDR_LOOKUP_IN_APP
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
#endif // CONFIG_TEST_ADDR_LOOKUP_IN_APP

void app_main(void)
{
    printf("app_main is running from 0x%x\n", (int) app_main);

#if CONFIG_TEST_ADDR_LOOKUP_IN_APP
    volatile int number = get_random_number();
    int *n = malloc(sizeof(int));

    assert(n);

    *n = number;

    printf("Initializer function at 0x%x\n", (int) initialize);
    printf("Got %d stored at 0x%x and 0x%x from a function from 0x%x\n", *n, (int) n, (int) (&number), (int) get_random_number);
    printf("This is the end of the report\n");

    free(n);
#endif // CONFIG_TEST_ADDR_LOOKUP_IN_APP

#if CONFIG_TEST_ADDR_LOOKUP_IN_ROM
    printf("Crashing now!\n");

    esp_rom_install_channel_putc(1, (void (*)(char)) abort);
    esp_rom_printf("a");
#endif // CONFIG_TEST_ADDR_LOOKUP_IN_ROM
}
